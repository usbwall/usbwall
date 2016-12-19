#include "parser.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/**
 * \brief maximum size of a configuration line, in characters
 */
#define MAX_LINE_LEN 256

static const char *fields[] =
{
  "uri",
  "basedn",
  "binddn",
  "bindpw",
  "version",
  NULL
};

/**
 * \brief Internal parser function that take a string and delete comments from
 * it
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
 * \brief check if a line is empty, i.e. the line contain only spaces or tabs
 *
 * \param line  the line to check
 *
 * \return 1 if the line is empty, 0 elseways.
 */
static int line_is_empty(const char *line)
{
  while (isspace(*line))
    line++;

  return !(*line);
}

/**
 * \brief Internal parser function to parse a line of the configuration file to
 * extract the value of a given field and return it
 *
 * \param line  The line to be parsed
 * \param field  The name of the field that should appeair on the line
 *
 * \return the value of the given field if the grammar is respected.
 * NULL otherwhise.
 */
static char *parse_line(const char *line, const char *field)
{
  assert(line && field);

  /* create the format to give to sscanf :
   * format = field %s %s
   * The first %s correspond to the value of the field
   * The second %s is just a check to make sure that only one value
   * is given to the field */
  const char *format_part = " %s %s ";
  size_t format_size = strlen(field) + strlen(format_part) + 2;
  char format[format_size];
  snprintf(format, format_size, " %s%s", field, format_part);
  /* *** */

  /* call to sscanf to parse the line and extract the value for the field */
  char buffer[MAX_LINE_LEN] = { '\0' };
  char aux_buffer[MAX_LINE_LEN] = { '\0' };

  int parse_success = sscanf(line, format, buffer, aux_buffer);
  /* parse_success can have 3 values :
   * - 0 : field pattern was not found
   * - 1 : field was found and the first argument was copied in buffer
   * - 2 : like before but there is more than one argument */
  if (parse_success != 1)
    return NULL;
  /* *** */

  /* We have the value, now let's allocate just the right size for it, copy the
   * value in the heap, and return a pointer to it */
  const size_t len = strlen(buffer);
  char *value = malloc(len + 1);
  if (!value)
    return NULL;

  memcpy(value, buffer, len + 1);

  return value;
}

/**
 * \brief store in a configuration structure the given value, in the field
 * correspondig to the given string.
 *
 * \param config  configuration structure in which to store the value
 * \param field  the name of the field in the structure, in which the value
 * should be stored
 * \param value  the value to store
 */
static void store_cfg_value(struct ldap_cfg *config,
                            const char *field,
                            char *value)
{
  if (field == fields[4]) // version
  {
    /* special case, the field should be an integer */
    char *err_ptr = NULL;
    long int convv = strtol(value, &err_ptr, 10);
    if (*err_ptr || convv < 1 || convv > 3)
      syslog(LOG_WARNING, "LDAP version invalid. Version should be 1, 2 or 3");
    else
      config->version = (short)convv;

    return;
  }

  /* the field is a string, select the pointer corresponding to the structure
   * field given in argument */
  char **destination = NULL;
  if (field == fields[0]) // uri
    destination = &config->uri;
  else if (field == fields[1]) // basedn
    destination = &config->basedn;
  else if (field == fields[2]) // binddn
    destination = &config->binddn;
  else if (field == fields[3]) // bindpw
    destination = &config->bindpw;

  /* check if the field was already given a value */
  if (*destination)
  {
    syslog(LOG_NOTICE, "Value %s is shadowed by %s", *destination, value);
    free(*destination);
  }

  *destination = value;
}

struct ldap_cfg *parse_config(FILE *istream)
{
  assert(istream);

  struct ldap_cfg *config = calloc(1, sizeof(struct ldap_cfg));
  if (!config)
    return NULL;

  syslog(LOG_DEBUG, "Parsing configuration file");
  char *buffer = NULL;
  size_t buff_size = 0;
  while (getline(&buffer, &buff_size, istream) != -1)
  {
    skip_comments(buffer);
    if (line_is_empty(buffer))
      continue;

    char *value = NULL;
    /* iterate over each field name of the config structure */
    for (int field_idx = 0; fields[field_idx]; ++field_idx)
    {
      const char *field = fields[field_idx];
      if ((value = parse_line(buffer, field)))
      {
        /* store attributes to config */
        store_cfg_value(config, field, value);
        break;
      }
    }
    if (!value)
      syslog(LOG_WARNING,
             "config syntax error, this line is invalid: %s",
             buffer);
  }
  free(buffer);
  syslog(LOG_DEBUG, "Configuration parsing finished");

  return config;
}
