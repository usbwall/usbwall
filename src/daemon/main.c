#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "devusb.h"
#include "devuser.h"
#include "ipc_pam.h"
#include "ldap_config.h"

int main(void)
{
  openlog("usbwall", LOG_CONS | LOG_PID, LOG_USER);
  struct ldap_cfg *cfg = make_ldap_cfg(cfg_file_find());
  int domain_socket_fd = -1;

  if (!cfg)
    return 1; // no configs found

  if (init_devusb())
    return 1; // devusb initialization error

  if ((domain_socket_fd = init_socket()) == -1)
    return 1; // Unix Domain Socket initialization error

  char *username = NULL;
  while ((username = wait_for_logging(domain_socket_fd)))
  {
    struct devusb **device_list = devices_get();
    char **devids = devids_get(username, cfg);

    /**
     * \todo
     * TODO: update devices_list depending of devids.
     * Only authorized devices should be kept
     */

    if (update_devices(device_list))
      syslog(LOG_WARNING, "Device update failed");

    free_devids(devids);
    free_devices(device_list);
    free(username);

    break; // for debug purpose
  }
  close_devusb();
  destroy_ldap_cfg(cfg);

  closelog();

  return 0;
}
