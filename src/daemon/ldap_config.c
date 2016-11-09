#include "ldap_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define MAX_LINE_LEN 256

static void skip_comments(char *line)
{
  /**
   * \todo
   * FIXME: Bug if a sharp is used as a full character, like in
   * a password. Thanks to Sylvain for reporting this bug.
   */
  char *comment_start = strchr(line, '#');

  if (comment_start)
    *comment_start = '\0';
}

static int scanstr(const char *line, const char *format, char **destination)
{
  char buffer[MAX_LINE_LEN] = { '\0' };

  int parse_success = sscanf(line, format, buffer);
  if (parse_success != 1)
    return 0;

  /* We need to check if the value was duplicated. If it is, then we free it
   * and rewrite it = the last given value prevale! */
  if (*destination)
    free(*destination);

  const size_t len = strlen(buffer);
  *destination = malloc(len + 1);
  if (!*destination)
    return 0;

  memcpy(*destination, buffer, len + 1);

  return 1;
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

struct ldap_cfg *make_ldap_cfg(const char *cfg_file)
{
  FILE *stream = fopen(cfg_file, "r");
  if (!stream)
  {
    syslog(LOG_ERR, "Configuration file not accessible : %s", cfg_file);
    return NULL;
  }

  struct ldap_cfg *config = calloc(1, sizeof(struct ldap_cfg));

  /* parsing configurations from the file */
  char *buffer = NULL;
  size_t buff_size = 0;
  while (getline(&buffer, &buff_size, stream) != -1)
  {
    skip_comments(buffer);

    /* store attributes to config */
    if (!scanstr(buffer, " uri %s ", &config->uri)
        && !scanstr(buffer, " basedn %s ", &config->basedn)
        && !scanstr(buffer, " binddn %s ", &config->binddn)
        && !scanstr(buffer, " bindpw %s ", &config->bindpw)
        && !sscanf(buffer, " version %hd ", &config->version))
      syslog(LOG_WARNING,
             "config syntax error, this line is invalid: %s",
             buffer);
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
