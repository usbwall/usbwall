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
 */
int init_devusb(void);

/**
 * \brief free everything related to devusb. Must be called last.
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
 * \return return non 0 value in case of error */
int update_devices(struct devusb **devices);
