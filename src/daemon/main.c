#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "devusb.h"
#include "devuser.h"
#include "ldap_config.h"

int main(void)
{
  openlog("usbwall", LOG_CONS | LOG_PID, LOG_USER);

  if (init_devusb())
    return 1; // devusb initialization error

  struct ldap_cfg *cfg = make_ldap_cfg(cfg_file_find());
  if (!cfg)
    return 1; // no configs found

  char *username = NULL;
  while ((username = wait_for_logging()))
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
