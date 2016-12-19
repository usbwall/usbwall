#include "ldap_config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "parser.h"

const char *cfg_file_find(void)
{
  /**
   * \todo
   * TODO: search for more than one place for the config file
   * It's optional but conveniant for the user.
   */

  return "/etc/usbwall.cfg";
}

static int check_cfg(const struct ldap_cfg *cfg)
{
  assert(cfg);

  int result = 0;

  if (!cfg->uri)
  {
    syslog(LOG_ERR, "URI field is missing in configuration");
    result = 1;
  }
  if (!cfg->basedn)
  {
    syslog(LOG_ERR, "Basedn field is missing in configuration");
    result = 1;
  }
  if (!cfg->binddn)
  {
    syslog(LOG_ERR, "Binddn field is missing in configuration");
    result = 1;
  }
  if (!cfg->bindpw)
  {
    syslog(LOG_ERR, "Bindpw field is missing in configuration");
    result = 1;
  }
  if (cfg->version == 0)
  {
    syslog(LOG_ERR, "Version field is invalid in configuration");
    result = 1;
  }

  return result;
}

struct ldap_cfg *make_ldap_cfg(const char *cfg_file)
{
  assert(cfg_file);

  FILE *stream = fopen(cfg_file, "r");
  if (!stream)
  {
    syslog(LOG_ERR, "Configuration file not accessible : %s", cfg_file);

    return NULL;
  }
  syslog(LOG_INFO, "Found configuration file at %s", cfg_file);

  struct ldap_cfg *config = parse_config(stream);
  fclose(stream);

  if (!config)
  {
    syslog(LOG_ERR, "Configuration is not valid!");

    return NULL;
  }

  if (check_cfg(config))
  {
    destroy_ldap_cfg(config);

    return  NULL;
  }

  return config;
}

void destroy_ldap_cfg(struct ldap_cfg *cfg)
{
  assert(cfg);

  free(cfg->uri);
  free(cfg->basedn);
  free(cfg->binddn);
  free(cfg->bindpw);
  free(cfg);
}
