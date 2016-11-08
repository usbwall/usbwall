#include "core.h"

#include <stdlib.h>
#include <syslog.h>

#include "devusb.h"
#include "devuser.h"
#include "ipc_pam.h"
#include "ldap_config.h"

static void handle_login(struct ldap_cfg *cfg, const char *username)
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
}

int usbwall_run(void)
{
  if (init_devusb())
    return 1; // devusb initialization error

  int domain_socket_fd = init_socket();
  if (domain_socket_fd == -1)
    return 1; // Unix Domain Socket initialization error

  struct ldap_cfg *cfg = make_ldap_cfg(cfg_file_find());
  if (!cfg)
    return 1; // no configs found

  char *username = NULL;
  while ((username = wait_for_logging(domain_socket_fd)))
  {
    handle_login(cfg, username);
    free(username);

    /**
     * \todo
     * TODO: The admin need to get a way to finish this loop.
     */
  }

  close_devusb();
  destroy_ldap_cfg(cfg);

  return 0;
}
