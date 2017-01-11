#ifndef COMMON_H
# define COMMON_H

#include <stdint.h>
#include <syslog.h>

# define ERR_MAX_LEN 1024

enum devidd_err
{
  DEVIDD_SUCCESS, /* Success */
  DEVIDD_ERR_MEM, /* Memory error */
  DEVIDD_ERR_IO, /* Input/Output error */
  DEVIDD_ERR_EAGAIN, /* Ressource temporarily unavailable */
  DEVIDD_ERR_PERM, /* Operation not permitted */
  DEVIDD_ERR /* Generic error */
};

int32_t devidd_log(char *module, int32_t err_val, int32_t level);
  

#endif /* !COMMON_H */
