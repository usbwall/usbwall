/**
 * \file devusb.h
 * \brief Containing functions to manage plugged devices
 */
#pragma once

/**
 * \brief structure containing information on a plugged usb device.
 */
struct devusb
{
  char *serial; /**< device unique serial id,
                     NULL in the case of non existance */
  int bus;      /**< bus number where the device is attached */
  int port;     /**< port number on which the device is attached */
};


/**
 * \brief initialize devusb. Must be called before every other functions.
 * \return return non 0 value in case of error
 *
 * The function will initialize devusb, that is try to load libusb and
 * configure it. The return value should be verified before calling
 * any other function from devusb.
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
 * \return list of devusb struct for each plugged usb device
 */
struct devusb **devices_get(void);

/**
 * \brief Destroy and free the list of devusb structs that can be returned
 *  by devices_get function.
 * \param devices list of devusb struct to destroy
 */
void free_devices(struct devusb **devices);

/**
 * \brief update the access status for given devices
 * \param devices list of devusb struct corresponding to device to authorize
 * \return return non 0 value in case of error
 *
 * The function will search for the file that controle acces for each given
 * devices. The path format is /sys/bus/usb/[bus]-[port]/authorized. Then,
 * 1 will be written in the file to permit the creation of a virtual device
 * block in /dev by udev.
 */
int update_devices(struct devusb **devices);
