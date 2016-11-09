#include "devusb.h"

#ifdef _FREEBSD
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

/**
 * \brief maximum possible size of a device serial id.
 */
#define SERIAL_MAX_SIZE 64
/**
 * \brief interval time in second where usb plugging event are checked
 */
#define HOTPLUG_CHECK_INTV 3

/* Globals */
/**
 * \brief devusb internal global used to register/deregister the hotplug
 * callback.
 */
static libusb_hotplug_callback_handle g_callback_handler;
/**
 * \brief devusb internal global to define if the wait_for_hotplug thread
 * need to continue its checks or exit (1 to continue, 0 for exit).
 */
static int g_wait_for_hotplugs;
/**
 * \brief devusb internal global used to reference the wait_for_hotplug thread.
 * It is started in the module init and exited in the module exit.
 */
static pthread_t g_hotplug_thread;
/* ****** */

/**
 * \brief devusb internal function that will extract the serial from a device.
 *
 * \param device  the libusb object associated with the device
 * \param usb_infos  the libusb descriptor object associated with the device
 *
 * \return the device serial id. NULL if an error occured.
 *
 * The function will access to the device string descriptor and extract the
 * serial.
 */
static char *device_serial_get(struct libusb_device *device,
                               struct libusb_device_descriptor *usb_infos)
{
  libusb_device_handle *udev = NULL;
  int rcode = libusb_open(device, &udev);
  if (rcode != LIBUSB_SUCCESS)
  {
    syslog(LOG_WARNING, "Failed to open device : %s", libusb_strerror(rcode));

    return NULL;
  }

  char *serial = NULL;
  if (usb_infos->iSerialNumber) // the device does have an unique identifier
  {
    unsigned char tmp_dev_serial[SERIAL_MAX_SIZE] = { '\0' };
    const int ret = libusb_get_string_descriptor_ascii(udev,
                                                       usb_infos->iSerialNumber,
                                                       tmp_dev_serial,
                                                       SERIAL_MAX_SIZE);
    if (ret > 0) // ret < 0 is a LIBUSB_ERROR
    {
      /**
       * \todo
       * TODO : Some serials extracted from descriptors may not be unique, and
       * some are just useless garbage. We need to find a way to identify those
       * bad serial ids.
       */
      const size_t len = (size_t)ret;
      serial = malloc(len + 1);
      if (serial)
        memcpy(serial, tmp_dev_serial, len + 1);
    }
    else
      syslog(LOG_WARNING,
             "Unable to extract usb string descriptor : %s",
             libusb_strerror(ret));
  }
  libusb_close(udev);

  return serial;
}

/**
 * \brief devusb internal function that convert a libusb device object to a
 * devusb structure
 *
 * \param device  the structure to convert
 *
 * \return the converted devusb structure. NULL if an error occured
 *
 * The device will extract all the information from the device
 * and store it in a devusb structure.
 */
static struct devusb *device_to_devusb(struct libusb_device *device)
{
  struct devusb *result = calloc(1, sizeof(struct devusb));
  if (!result)
    return NULL;

  struct libusb_device_descriptor usb_infos;
  if (libusb_get_device_descriptor(device, &usb_infos) != LIBUSB_SUCCESS)
  {
    free(result);
    return NULL;
  }

  result->serial = device_serial_get(device, &usb_infos);
  result->bus = libusb_get_bus_number(device);
  result->port = libusb_get_port_number(device);

  return result;
}

/**
 * \brief devusb internal function that check if the device is accessible by the
 * current user and
 *
 * \param ctx  unused
 * \param dev  the libusb device object associated with the plugged device
 * \param event  unused
 * \param user_data  unused
 *
 * \return non zero value if an error occured
 *
 * The callback is called by libusb when a usb device is plugged (see libusb
 * documentation for more informations). It will update the device
 * accessibility.
 */
static int hotplug_callback(struct libusb_context *ctx __attribute__((unused)),
                            struct libusb_device *dev,
                            libusb_hotplug_event event __attribute__((unused)),
                            void *user_data __attribute__((unused)))
{
  struct devusb *device = device_to_devusb(dev);
  if (!device)
  {
    syslog(LOG_WARNING, "corrupted hotplug devusb");

    return 1;
  }

  /**
   * \todo
   * TODO: test if the device is authorized.
   * Then, modify the sysfs to authorize it in the case where it is.
   */

  printf("Device plugged : %s\n", device->serial);

  return 0;
}

/**
 * \brief devusb internal function that will check every HOTPLUG_CHECK_INTV
 * seconds if an usb has been plugged.
 *
 * \param arg  unused
 *
 * \return NULL
 *
 * The function must be launched in a separated thread, where it will check
 * every HOTPLUG_CHECK_INTV seconds for an usb event. If an event come up, it
 * will make libusb call the hotplug callback.
 */
__attribute__((noreturn)) static void *wait_for_hotplug(void *arg
                                                        __attribute__((unused)))
{
  struct timeval tv = { 0, 0 };
  while (g_wait_for_hotplugs)
  {
    libusb_handle_events_timeout_completed(NULL, &tv, NULL);
    sleep(HOTPLUG_CHECK_INTV);
  }

  pthread_exit(NULL);
}

int init_devusb(void)
{
  if (libusb_init(NULL) != LIBUSB_SUCCESS)
  {
    syslog(LOG_ERR, "Init error - libusb can not be initiated");

    return 1;
  }

  if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
  {
    syslog(LOG_ERR, "Init error - your system does not support hotplug");

    return 1;
  }

  libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_WARNING);

  g_wait_for_hotplugs = 1;
  int rcode =
    libusb_hotplug_register_callback(NULL,
                                     LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
                                     /**
                                      * \todo
                                      * FIXME: LIBUSB_HOTPLUG_NO_FLAGS should be
                                      * used instead of 0, but the enum value is
                                      * not recognized on the all OS
                                      **/
                                     0,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     hotplug_callback,
                                     NULL,
                                     &g_callback_handler);

  if (rcode != LIBUSB_SUCCESS)
  {
    libusb_exit(NULL);
    return 1;
  }

  /* start the wait_for_hotplug thread */
  pthread_create(&g_hotplug_thread, NULL, wait_for_hotplug, NULL);

  return 0;
}

struct devusb **devices_get(void)
{
  struct devusb **devusb_list = calloc(16, sizeof(struct devusb *));
  if (!devusb_list)
    return NULL;
  /**
   * \todo
   * FIXME: devusb_list should not be limited by 16 devices.
   * That's just an hotfix because I was lazy.
   */

  libusb_device **device_list = NULL;
  const ssize_t device_list_size = libusb_get_device_list(NULL, &device_list);

  /* iterate over all devices and extract needed infos */
  for (int i = 0; i < device_list_size; ++i)
  {
    devusb_list[i] = device_to_devusb(device_list[i]);
    if (!devusb_list[i])
    {
      syslog(LOG_WARNING, "corrupted devusb no %d", i);
      break;
    }
  }
  libusb_free_device_list(device_list, 1);

  return devusb_list;
}

void free_devices(struct devusb **devices)
{
  if (!devices)
    return;

  for (int i = 0; devices[i]; ++i)
  {
    free(devices[i]->serial);
    free(devices[i]);
  }
  free(devices);
}

int update_devices(struct devusb **devices)
{
  if (!devices)
    return 1;

  /**
   * \todo
   * TODO : get, open, and update authorized files for given devices
   */

  /**
   * \remark
   * the update_devices code is for now only for debug purpose.
   */
  for (int i = 0; devices[i]; ++i)
  {
    if (!devices[i]->serial) // not an identifiable device
      printf("unknown device ");
    else
      printf("recognized device %s ", devices[i]->serial);

    printf("on %d:%d\n", devices[i]->bus, devices[i]->port);
  }

  return 0;
}

void close_devusb(void)
{
  g_wait_for_hotplugs = 0;
  pthread_join(g_hotplug_thread, NULL);
  libusb_hotplug_deregister_callback(NULL, g_callback_handler);
  libusb_exit(NULL);
}
