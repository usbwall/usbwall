#include "devusb.h"

#include <libusb-1.0/libusb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#define SERIAL_MAX_SIZE 64

static char *device_serial_get(struct libusb_device *device,
                               struct libusb_device_descriptor *usb_infos)
{
  libusb_device_handle *udev = NULL;
  int ret = libusb_open(device, &udev);
  if (ret != LIBUSB_SUCCESS)
  {
    syslog(LOG_WARNING, "Failed to open device : %s", libusb_strerror(ret));

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

static libusb_hotplug_callback_handle handle;
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

  printf("Device plugged : %s", device->serial);

  return 0;
}

static void *wait_for_hotplug(void *arg __attribute__((unused)))
{
  struct timeval tv = { 0, 0 };
  while (1)
  {
    libusb_handle_events_timeout_completed(NULL, &tv, NULL);
    sleep(3);
  }

  return NULL;
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

  int rcode =
    libusb_hotplug_register_callback(NULL,
                                     LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
                                     0,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     hotplug_callback,
                                     NULL,
                                     &handle);

  if (rcode != LIBUSB_SUCCESS)
  {
    libusb_exit(NULL);
    return 1;
  }

  pthread_t hotplug_thread;
  pthread_create(&hotplug_thread, NULL, wait_for_hotplug, NULL);

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
  libusb_hotplug_deregister_callback(NULL, handle);
  libusb_exit(NULL);
}
