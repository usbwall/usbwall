/**
 * \file devusb.h
 * \brief Containing functions to manage plugged devices
 */
#pragma once

#include <stdint.h>

#include <misc/linked_list.h>

/**
 * \brief structure containing information on a plugged usb device.
 */
struct devusb
{
  char *serial; /**< device unique serial id,
                 * NULL in the case of non existance */
  uint8_t *ports; /**< port numbers from the root on which
                   * the device is attached. */
  uint8_t bus; /**< bus number where the device is attached */
  uint8_t ports_nb; /**< number of elements in the ports array */
  char padding32bits[2];
};

/**
 * \brief initialize devusb. Must be called before every other functions.
 * \return return non 0 value in case of error
 *
 * The function will initialize devusb, that is try to load libusb and
 * configure it. It will also start a second thread to listen for device
 * hotplug. The return value should be verified before calling any other
 * function from devusb.
 */
int init_devusb(void);

/**
 * \brief free everything related to devusb. Must be called last.
 *
 * The function will close devusb. That is, it will exit libusb.
 * Calling an other function of devusb after close_devusb is an
 * unspecified behavior.
 */
void close_devusb(void);

/**
 * \brief Scan for plugged usb devices and return informations on devices
 * \return linked list of devusb struct for each plugged usb device. NULL
 * is return if an error occured.
 */
struct linked_list *devices_get(void);
