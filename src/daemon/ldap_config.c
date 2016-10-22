#include "ldap_config.h"

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *cfg_file_find(void)
{
  /* TODO */

  return "/etc/usbwall.cfg";
}

struct ldap_cfg *make_ldap_cfg(char *cfg_file)
{
  struct ldap_cfg *config = calloc(1, sizeof (struct ldap_cfg));
  FILE *stream = fopen(cfg_file, "r");

  if (!stream)
    return NULL;

  /* parsing configurations from the file */
  char *buffer = NULL;
  size_t buff_size = 0;
  while (getline(&buffer, &buff_size, stream) != -1)
  {
    /* delete comments */
    char *comment_start = strchr(buffer, '#');
    if (comment_start)
      *comment_start = '\0';

    /* store attributes to config */
    if (!sscanf(buffer, " uri %ms ", &config->uri)
        && !sscanf(buffer, " basedn %ms ", &config->basedn)
        && !sscanf(buffer, " binddn %ms ", &config->binddn)
        && !sscanf(buffer, " bindpw %ms ", &config->bindpw)
        && !sscanf(buffer, " version %hd ", &config->version))
      puts("ERROR ??"); // FIXME
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
  free(cfg);
}
