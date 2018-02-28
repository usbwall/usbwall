#ifndef ERROR_HANDLER_H
# define ERROR_HANDLER_H

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
  DEVIDD_ERR_CONFIG, /* Problem in daemon configuration */
  DEVIDD_ERR_OTHER /* Generic error */
};

/* static const char* err_array[] = */
/* { */
/*   "Success", */
/*   "Error memory allocation", */
/*   "Error Input/Output", */
/*   "Ressource temporary unavailable", */
/*   "Operation not permitted", */
/*   "Other" */
/* }; */

__attribute__((const))
int32_t devidd_log(const char *module __attribute__((unused)),
		   int32_t err_val __attribute__((unused)),
		   int32_t level __attribute__((unused)));

#endif /* !ERROR_HANDLER_H */
