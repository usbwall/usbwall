/*
 * \file netlink.h
 * \brief Containing function to link PAM and daemon
 */

#ifndef NETLINK_H_
# define NETLINK_H_

/*!
 ** @brief list of possible actions
 **
 ** supporting various device actions:
 ** add (when adding a new device)
 ** change (when a connected device changes its behavior)
 ** remove (wen a device is being disconnected)
 */
enum action
{
  ACTION_ADD = 1,
  ACTION_CHANGE,
  ACTION_REMOVE,
  ACTION_MAX
};

typedef enum action action_t;

/*!
 ** @brief list of device types
 **
 ** There is various device types depending on what is hotplugged
 ** device: basic client controler (such as usb client controler)
 ** disk: mass storage disk, associated to various filesystem(s)
 */
enum dev_type
{
  DEVTYPE_USBDEVICE = 1,
  DEVTYPE_DISK,
  DEVTYPE_MAX
};

typedef enum dev_type devtype_t;

/*!
 ** @brief the bus identifier, depending on which bus the device is connected
 **
 ** usb: USB bus
 ** pci: PCI bus
 ** firewire: FIREWIRE bus
 */
enum id_bus
{
  IDBUS_USB = 1,
  IDBUS_PCI,
  IDBUS_FIREWIRE,
  IDBUS_MAX
};

typedef enum id_bus idbus_t;

/*!
 ** @brief the udev informational environment structure, created from the data given by the kernel
 */
struct udev_env
{
  char *serial;
  char *devpath;
  action_t action;
  devtype_t idbus;
  devtype_t devtype;
};

/*!
 **
 */
int init_socket(void);

int accept_user(int netlink_fd);

#endif /* !NETLINK_H_ */
