#include "ldap_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void skip_comments(char *line);

char *cfg_file_find(void)
{
  /**
   * \todo
   * TODO: search for more than one place for the config file
   * It's optional but conveniant for the user.
   */

  return "/etc/usbwall.cfg";
}

struct ldap_cfg *make_ldap_cfg(const char *cfg_file)
{
  FILE *stream = fopen(cfg_file, "r");
  if (!stream)
    return NULL;

  struct ldap_cfg *config = calloc(1, sizeof (struct ldap_cfg));

  /* parsing configurations from the file */
  char *buffer = NULL;
  size_t buff_size = 0;
  while (getline(&buffer, &buff_size, stream) != -1)
  {
    skip_comments(buffer);

    /* store attributes to config */
    if (!sscanf(buffer, " uri %ms ", &config->uri)
        && !sscanf(buffer, " basedn %ms ", &config->basedn)
        && !sscanf(buffer, " binddn %ms ", &config->binddn)
        && !sscanf(buffer, " bindpw %ms ", &config->bindpw)
        && !sscanf(buffer, " version %hd ", &config->version))
      puts("ERROR ??");
    /**
     * \todo
     * FIXME: Change the behavior for the case of an unknown
     * attribute in the configuration file. A more precise message
     * is better. Using the syslog could also be a good idea.
     */
  }

  free(buffer);
  fclose(stream);

  return config;
}

void destroy_ldap_cfg(struct ldap_cfg *cfg)
{
  free(cfg->uri);
  free(cfg->basedn);
  free(cfg->binddn);
  free(cfg->bindpw);
  free(cfg);
}

/************************************
 * Static functions implementations *
 ************************************/

static void skip_comments(char *line)
{
  char *comment_start = strchr(line, '#');

  if (comment_start)
    *comment_start = '\0';
}
