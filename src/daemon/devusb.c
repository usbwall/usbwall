#include "devusb.h"

#define _XOPEN_SOURCE 600

#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SERIAL_MAX_SIZE 64

int init_devusb(void)
{
  libusb_init(NULL);
  if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
    return 1;

  libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_WARNING);

  return 0;
}

struct devusb **devices_get(void)
{
  struct devusb **devusb_list = calloc(16, sizeof (struct devusb *)); // FIXME
  /**
   * \todo
   * FIXME: devusb_list should not be limited by 16 devices.
   * That's just an hotfix because I was lazy.
   */

  libusb_device **device_list = NULL;
  const size_t device_list_size = libusb_get_device_list(NULL, &device_list);

  /* iterate over all devices and get their serial */
  struct libusb_device *device = NULL;
  struct libusb_device_descriptor *usb_infos =
    malloc(sizeof (struct libusb_device_descriptor));
  libusb_device_handle *udev = NULL;
  for (unsigned i = 0; i < device_list_size; ++i)
  {
    devusb_list[i] = calloc(1, sizeof (struct devusb));
    device = device_list[i];
    libusb_get_device_descriptor(device, usb_infos);
    libusb_open(device, &udev);

    if (usb_infos->iSerialNumber) // the device does have an unique identifier
    {
      unsigned char tmp_dev_serial[SERIAL_MAX_SIZE] = { '\0' };
      size_t len = libusb_get_string_descriptor_ascii(udev,
                                                      usb_infos->iSerialNumber,
                                                      tmp_dev_serial,
                                                      SERIAL_MAX_SIZE);

      /**
       * \todo
       * TODO : check the size of the serial id extracted from descriptors
       */
      devusb_list[i]->serial = malloc(len + 1);
      memcpy(devusb_list[i]->serial, tmp_dev_serial, len + 1);
    }

    devusb_list[i]->bus = libusb_get_bus_number(device);
    devusb_list[i]->port = libusb_get_port_number(device);
    libusb_close(udev);
  }

  free(usb_infos);
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
