#include "core.h"

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <misc/linked_list.h>

#include "devuser.h"
#include "ipc_pam.h"
#include "config.h"
#include "usb_access.h"

/* Globals */
/**
 * \brief core internal global used to trigger the terminaison of the main
 * process.
 */
static int g_terminaison = 0;
/**
 * \brief core internal global used to notify the main process that a cfg update
 * is needed.
 */
static int g_cfgupdate = 0;
/* ****** */

/**
 * \brief core internal function to filter for allowed devices.
 * \param allowed_devices  The list containing devices to filter.
 * \param devids  The list containing authorized serial ids.
 * \return Return a list of the devices that are not allowed for the
 * current user.
 *
 * The function will check all the devusb structures in allowed_devices,
 * and compare them to the serials (char *) contained in devids. For each
 * devices, if it is allowed it stay in the given allowed_devices linked list,
 * if it is not, then it is moved to the returned linked_list. The function will
 * also check for invalid devices (devices that can't be acceded or manipulated
 * by the program), and simply delete them (removed from the list, and freed).
 */
static struct linked_list *filter_devices(struct linked_list *allowed_devices,
                                          struct linked_list *devids)
{
  assert(allowed_devices && devids);

  struct linked_list *corrupted_devices = list_make();
  if (!corrupted_devices)
    return NULL;

  struct linked_list *forbidden_devices = list_make();
  if (!forbidden_devices)
    return NULL;

  list_for_each(device_ptr, allowed_devices)
  {
    struct devusb *device = device_ptr->data;
    if (!device_is_valid(device))
    {
      /* Corrupted device... Store it in a list for delayed destruction */
      list_add_back(corrupted_devices, device);
      list_remove(allowed_devices, device_ptr, 0);

      continue;
    }

    if (!check_devid(device->serial, devids))
    {
      /* We can't find the serial in the devid list. The device is then not
       * allowed. Just push it in the forbidden list and remove it from the
       * authorized ones. */
      list_add_back(forbidden_devices, device);
      list_remove(allowed_devices, device_ptr, 0);
    }
  }
  list_destroy(corrupted_devices, 1);

  return forbidden_devices;
}

/**
 * \brief core internal function to handle globals lookup.
 * notification is received
 * \return 1 if the program need to terminate, 0 else way.
 *
 * The function will lookup globals, usually setted by signal handler and
 * show that a change need to be made to the program state.
 */
static int notifs_lookup(void)
{
  if (g_terminaison)
  {
    syslog(LOG_INFO, "Terminaison notif received, terminating program");

    return 1;
  }

  if (g_cfgupdate)
  {
    syslog(LOG_INFO, "Update notif received, updating config");
    if (update_configuration(cfg_file_find()))
    {
      syslog(LOG_WARNING, "Configuration update failed, terminating program");

      return 1; // no configs found
    }
    g_cfgupdate = 0;
  }

  return 0;
}

/**
 * \brief core internal function to handle a user connection
 * \param username the login of the connected user.
 *
 * The function will get plugged device list from devusb, it will get the list
 * of authorized devices from devuser, and update the access of devices in the
 * sysfs.
 */
static void handle_login(const char *username)
{
  assert(username);

  struct linked_list *device_list = devices_get();
  if (!device_list)
    return;

  struct linked_list *devids = devids_get(username);
  if (!devids)
    return;

  struct linked_list *forbid = filter_devices(device_list, devids);
  if (!forbid)
    return;

  update_devices_access(device_list, forbid);

  list_destroy(devids, 1);
  list_destroy(device_list, 1);
}

/**
 * \brief core internal function handling signal when received.
 * \param signo  id number of the signal received
 */
static void signal_handler(int signo)
{
  syslog(LOG_DEBUG, "Received signal no %d", signo);

  if (signo == SIGTERM)
  {
    syslog(LOG_INFO, "SIGTERM received");
    close_ipc_pam(); // it will close all connections with pam
    g_terminaison = 1;
  }
  else if (signo == SIGHUP)
  {
    syslog(LOG_INFO, "SIGHUP received");
    g_cfgupdate = 1;
  }
}

/**
 * \brief core internal function used to isolate the main loop.
 * This function should be call only after every modules is initialized
 * correctly. When this function return, the program should start its
 * terminaison procedure
 */
static void core_loop(void)
{
  struct linked_list *usernames = usernames_get();
  do
  {
    if (notifs_lookup())
      break; // program terminaison requested

    if (!usernames)
      continue;

    list_for_each(node_ptr, usernames)
      handle_login(node_ptr->data);

    list_destroy(usernames, 1);
  } while ((usernames = wait_for_logging()));
}

int usbwall_run(void)
{
  /* Initialization of the IPC_PAM module */
  if (init_ipc_pam())
    return 1;
  /* *** */

  /* Creation of the configuration structure */
  if (update_configuration(cfg_file_find()))
  {
    destroy_ipc_pam();

    return 1;
  }
  /* *** */

  /* Initial check for the ldap server */
  if (devids_check())
  {
    destroy_ipc_pam();
    destroy_configuration();

    return 1;
  }
  /* *** */

  /* Initialization of the Devusb module */
  if (init_devusb())
  {
    destroy_ipc_pam();
    destroy_configuration();

    return 1;
  }
  /* *** */

  core_loop();

  /* terminaison of modules */
  close_devusb();
  destroy_configuration();
  destroy_ipc_pam();
  /* *** */

  return 0;
}

int signal_config(void)
{
  struct sigaction action;
  action.sa_handler = signal_handler;
  sigfillset(&action.sa_mask);
  action.sa_flags = SA_RESTART;

  if (sigaction(SIGTERM, &action, NULL) == -1)
    return 1;
  if (sigaction(SIGHUP, &action, NULL) == -1)
    return 1;

  return 0;
}
