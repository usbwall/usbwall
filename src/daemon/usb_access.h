/**
 * \file devusb.h
 * \brief Containing functions to manipulate the accessibility of usb devices,
 * on the system.
 */
#pragma once

#include "../misc/linked_list.h"

#include "devusb.h"

/**
 * \brief update the access status for a given device
 * \param device  Struct containing all informations on the device to manage
 * \param value  boolean value to authorize or not the access of the device
 * \return return non 0 value in case of error
 *
 * The function will search for the file that controle acces for the given
 * device. The path format is /sys/bus/usb/[bus]-[port]/authorized. Then,
 * value will be written in the file to permit the creation of a virtual device
 * block in /dev by udev.
 */
int update_device_access(struct devusb *device, int value);

/**
 * \brief same as authorize_device but for multiple elements
 * \param authorized  list of the devices to authorize
 * \param forbidden  list of the devices to forbid
 */
void update_devices_access(struct linked_list *authorized,
                           struct linked_list *forbidden);

/**
 * \brief set the accessibility default value for all usb devices
 * for usb devices.
 *
 * \param value  0 if all devices must be blocked by default, 1 otherwhise.
 */
void set_usb_default_access(int value);

/**
 * \brief check the possibility to manipulate the given device in the sysfs.
 *
 * \param device  the device to check
 *
 * \return 0 if not valid, 1 otherwhise
 */
int device_is_valid(struct devusb *device);
