#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "parser.h"

static struct config *g_configuration = NULL;

/**
 * \brief config internal function checking if a config structure is correctly
 * field.
 *
 * \param cfg  config to be checked
 *
 * \return 0 if the structure is correct, 1 in the case of an incomplete or
 * invalid config.
 */
static int check_cfg(const struct config *cfg)
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

/**
 * \brief config internal function that free the given configuration
 * structure. The cfg pointer should not be NULL.
 *
 * \param cfg  the structure to be freed.
 */
static void destroy_config(struct config *cfg)
{
  assert(cfg);

  free(cfg->uri);
  free(cfg->basedn);
  free(cfg->binddn);
  free(cfg->bindpw);
  free(cfg);
}

const struct config *configuration_get(void)
{
  assert(g_configuration);

  return g_configuration;
}

const char *cfg_file_find(void)
{
  /**
   * \todo
   * TODO: search for more than one place for the config file
   * It's optional but conveniant for the user.
   */

  return "/etc/usbwall.cfg";
}

int update_configuration(const char *cfg_file)
{
  assert(cfg_file);

  FILE *stream = fopen(cfg_file, "r");
  if (!stream)
  {
    syslog(LOG_ERR, "Configuration file not accessible : %s", cfg_file);

    return 1;
  }
  syslog(LOG_INFO, "Found configuration file at %s", cfg_file);

  struct config *config = parse_config(stream);
  fclose(stream);

  if (!config)
  {
    syslog(LOG_ERR, "Configuration is not valid!");

    return 1;
  }

  if (check_cfg(config))
  {
    destroy_config(config);

    return  1;
  }

  destroy_configuration();
  g_configuration = config;

  return 0;
}

void destroy_configuration(void)
{
  if (!g_configuration)
    return;

  destroy_config(g_configuration);
  g_configuration = NULL;
}
