#include <stdio.h>
#include <stdlib.h>

#include "devusb.h"
#include "devuser.h"
#include "ldap_config.h"

int main(void)
{
  init_devusb();

  struct devusb **device_list = devices_get();
  update_devices(device_list);
  close_devusb();
  free_devices(device_list);

  destroy_ldap_cfg(make_ldap_cfg(cfg_file_find()));

  char *username = wait_for_logging();

  puts(username);
  free(username);

  return 0;
}
