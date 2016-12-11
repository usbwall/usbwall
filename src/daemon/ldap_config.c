#include "ldap_config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/**
 * \brief maximum size of a configuration line, in characters
 */
#define MAX_LINE_LEN 256

/**
 * \brief Internal ldap_config function that take a string and delete comments
 * from it
 *
 * \param line  line to be modified
 */
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

/**
 * \brief Internal ldap_config function to parse a line for a string and
 * allocated the destination to the good
 * size and copy the string in the destination.
 *
 * \param line  The line to be parsed
 * \param format  the format string used to detect the substring to copy
 * \param destination  the pointer to malloc and to fill with the substring copy
 *
 * \return non zero value in case of error of unsuccesful parsing.
 *
 * The function is used to replace the use of sscanf with the m flag, that
 * produced warnings. The function will just call sscanf with the given format
 * string, allocated the destination with the good size and put the parsed
 * substring in destination, if one is found.
 */
static int scanstr(const char *line, const char *format, char **destination)
{
  assert(line && format && destination);

  char buffer[MAX_LINE_LEN] = { '\0' };

  int parse_success = sscanf(line, format, buffer);
  if (parse_success != 1)
    return 0;

  /* We need to check if the value was duplicated. If it is, then we free it
   * and rewrite it = the last given value prevale! */
  if (*destination)
  {
    syslog(LOG_NOTICE, "Value %s is shadowed!", *destination);
    free(*destination);
  }

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
  assert(cfg_file);

  FILE *stream = fopen(cfg_file, "r");
  if (!stream)
  {
    syslog(LOG_ERR, "Configuration file not accessible : %s", cfg_file);

    return NULL;
  }
  syslog(LOG_INFO, "Found configuration file at %s", cfg_file);

  struct ldap_cfg *config = calloc(1, sizeof(struct ldap_cfg));
  if (!config)
  {
    fclose(stream);

    return NULL;
  }

  /* parsing configurations from the file */
  syslog(LOG_DEBUG, "Parsing configuration file");
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
  syslog(LOG_DEBUG, "Configuration parsing finished");

  free(buffer);
  fclose(stream);

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
