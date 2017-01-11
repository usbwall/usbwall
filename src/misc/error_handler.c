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
