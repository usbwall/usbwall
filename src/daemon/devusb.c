#include "devusb.h"

#ifdef _FREEBSD
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "config.h"
#include "devuser.h"
#include "usb_access.h"

/**
 * \brief maximum possible size of a device serial id.
 */
#define SERIAL_MAX_SIZE 64
/**
 * \brief interval time in second where usb plugging event are checked
 */
#define HOTPLUG_CHECK_INTV 3
/**
 * \brief Maximum port numbers for a device. The USB3 standard say 7
 * is the current max depth. It should never be set above 255!
 */
#define MAX_PORTS_NB 7

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
  assert(device && usb_infos);

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
 * \brief devusb internal function that will extract ports from a device.
 *
 * \param device  the libusb object associated with the device
 * \param ports  pointer to the array of ports. The pointed pointer
 * must be NULL! The function will be in charge of allocating the array.
 *
 * \return the number of device ports. 0 is returned in case of error.
 */
static uint8_t device_ports_get(struct libusb_device *device, uint8_t **ports)
{
  assert(device && ports);

  uint8_t result[MAX_PORTS_NB];
  int res = libusb_get_port_numbers(device, result, MAX_PORTS_NB);
  if (res < 0 || res > 255)
  {
      syslog(LOG_WARNING, "Too many ports for the device!");

      return 0;
  }

  uint8_t ports_nb = (uint8_t)res;

  *ports = malloc(sizeof (uint8_t) * ports_nb);
  if (!(*ports))
    return 0;

  memcpy(*ports, result, ports_nb);

  return ports_nb;
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
  assert(device);

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
  result->ports_nb = device_ports_get(device, &result->ports);

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
  const struct config *cfg = configuration_get();
  syslog(LOG_DEBUG, "TEST %hd", cfg->version);
  struct devusb *device = device_to_devusb(dev);
  if (!device)
  {
    syslog(LOG_WARNING, "Corrupted hotplug devusb");

    return 1;
  }
  syslog(LOG_INFO, "New device detected : %s\n", device->serial);

  if (!device_is_valid(device))
  {
    syslog(LOG_WARNING, "Invalid device plugged.");
    free(device);

    return 1;
  }

  int authorized_status = check_devid(device->serial, devices_get());

  int rcode = 0;
  if ((rcode = update_device_access(device, authorized_status)))
    syslog(LOG_WARNING, "Device access update error");

  free(device);

  return !!rcode;
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
__attribute__((noreturn))
static void *wait_for_hotplug(void *arg __attribute__((unused)))
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
  assert(configuration_get());

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
                                     (libusb_hotplug_flag)0,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     hotplug_callback,
                                     NULL,
                                     &g_callback_handler);

  if (rcode != LIBUSB_SUCCESS)
  {
    libusb_exit(NULL);
    syslog(LOG_DEBUG, "Hotplug callback failed to register");

    return 1;
  }

  set_usb_default_access(0);

  /* start the wait_for_hotplug thread */
  pthread_create(&g_hotplug_thread, NULL, wait_for_hotplug, NULL);

  syslog(LOG_DEBUG, "Devusb initialized sucessfully");

  return 0;
}

struct linked_list *devices_get(void)
{
  struct linked_list *devusb_list = list_make();
  if (!devusb_list)
    return NULL;

  libusb_device **device_list = NULL;
  const ssize_t device_list_size = libusb_get_device_list(NULL, &device_list);
  if (device_list_size < 0)
    syslog(LOG_WARNING,
           "Failed to retrieve device list : %s",
           libusb_strerror((enum libusb_error)device_list_size));

  /* iterate over all devices and extract needed infos */
  for (ssize_t i = 0; i < device_list_size; ++i)
  {
    struct devusb *element = device_to_devusb(device_list[i]);
    if (!element)
    {
      syslog(LOG_DEBUG, "Corrupted devusb on iteration %zd", i);
      break;
    }
    list_add_back(devusb_list, element);
  }
  libusb_free_device_list(device_list, 1);

  return devusb_list;
}

void close_devusb(void)
{
  g_wait_for_hotplugs = 0;
  pthread_join(g_hotplug_thread, NULL);
  libusb_hotplug_deregister_callback(NULL, g_callback_handler);
  libusb_exit(NULL);
  syslog(LOG_DEBUG, "Devusb closed");
}
