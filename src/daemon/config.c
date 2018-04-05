#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
  #include <sys/param.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "parser.h"
#include "config.h"

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

  //check if the config file contain the URI field
  if (!cfg->uri)
  {
    syslog(LOG_ERR, "URI field is missing in configuration");
    result = 1;
  }
    //check if the config file contain the basedn field
  if (!cfg->basedn)
  {
    syslog(LOG_ERR, "Basedn field is missing in configuration");
    result = 1;
  }
    //check if the config file contain the binddn field
  if (!cfg->binddn)
  {
    syslog(LOG_ERR, "Binddn field is missing in configuration");
    result = 1;
  }
    //check if the config file contain the bindpw field
  if (!cfg->bindpw)
  {
    syslog(LOG_ERR, "Bindpw field is missing in configuration");
    result = 1;
  }
    //check if version field is correct
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

__attribute__((pure))
const struct config *configuration_get(void)
{
  assert(g_configuration);

  return g_configuration;
}

__attribute__((const))
const char *cfg_file_find(void)
{
  /**
   * \todo
   * TODO: search for more than one place for the config file
   * It's optional but conveniant for the user.
   */

  /**
   * \todo
   * FIXME : This value MUST be taken from a config file, NOT hardcoded
   */
#if defined(BSD)
  return "/usr/local/etc/usbwall.cfg";
#else
  return "/etc/usbwall.cfg";
#endif
}

int update_configuration(const char *cfg_file)
{
  assert(cfg_file);

  FILE *stream = fopen(cfg_file, "r");
  //check if we can open the configuration file
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
