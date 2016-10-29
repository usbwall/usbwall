#include "devusb.h"

#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#define SERIAL_MAX_SIZE 64

static struct devusb *device_to_devusb(struct libusb_device *device);

int init_devusb(void)
{
  if (libusb_init(NULL) != LIBUSB_SUCCESS)
  {
    syslog(LOG_ERR,
           "Initialization error - libusb can not be initiated");
    return 1;
  }

  if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
  {
    syslog(LOG_ERR,
           "Initialization error - your system does not support hotplug");
    return 1;
  }

  libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_WARNING);

  return 0;
}

struct devusb **devices_get(void)
{
  struct devusb **devusb_list = calloc(16, sizeof (struct devusb *));
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
   * TODO : get, open, and update authorized files for
   * given devices
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
  libusb_exit(NULL);
}

/************************************
 * Static functions implementations *
 ************************************/

static struct devusb *device_to_devusb(struct libusb_device *device)
{
  struct devusb *result = calloc(1, sizeof (struct devusb));
  if (!result)
    return NULL;

  struct libusb_device_descriptor usb_infos;
  libusb_device_handle *udev = NULL;

  if (libusb_get_device_descriptor(device, &usb_infos) != LIBUSB_SUCCESS)
  {
    free(result);
    return NULL;
  }

  if (libusb_open(device, &udev) != LIBUSB_SUCCESS)
  {
    free(result);
    return NULL;
  }

  if (usb_infos.iSerialNumber) // the device does have an unique identifier
  {
    unsigned char tmp_dev_serial[SERIAL_MAX_SIZE] = { '\0' };
    const int ret = libusb_get_string_descriptor_ascii(udev,
                                                       usb_infos.iSerialNumber,
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
      result->serial = malloc(len + 1);
      if (result->serial)
        memcpy(result->serial, tmp_dev_serial, len + 1);
    }
    else
      syslog(LOG_WARNING, "Unable to extract usb string descriptor");

  }

  result->bus = libusb_get_bus_number(device);
  result->port = libusb_get_port_number(device);
  libusb_close(udev);

  return result;
}
