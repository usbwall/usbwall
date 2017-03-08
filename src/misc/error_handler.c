#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "error_handler.h"

__attribute__((const))
int32_t devidd_log(const char *module __attribute__((unused)), int32_t err_val __attribute__((unused)), int32_t level __attribute__((unused)))
{
  return 0;
}
