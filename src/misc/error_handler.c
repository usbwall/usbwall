#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "error_handler.h"

static const char* err_array[] =
{
  "Success",
  "Error memory allocation",
  "Error Input/Output",
  "Ressource temporary unavailable",
  "Operation not permitted",
  "Other"
};

int32_t devidd_log(char *module, int32_t err_val, int32_t level)
{
  char *buf = NULL;

  buf = malloc(ERR_MAX_LEN * sizeof(char));
  if (!buf)
    return ENOMEM;

  strcpy(buf, module);

  strncat(buf, " : ", ERR_MAX_LEN);
  strncat(buf, err_array[err_val], ERR_MAX_LEN);

  syslog(level, "%s", buf);

  return err_val;
}
