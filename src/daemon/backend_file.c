#include <assert.h>
/* #include <errno.h> */
/* #include <fcntl.h> */
#include <stdio.h>
/* #include <string.h> */
/* #include <sys/file.h> */
/* #include <sys/resource.h> */
/* #include <sys/stat.h> */
/* #include <sys/types.h> */
#include <syslog.h>
/* #include <unistd.h> */

#include "misc/error_handler.h"
/* #include "uw_pid.h" */
/* #include "uw_signal.h" */
/* #include "config.h" */
#include "backend_file.h"

int uw_open_file(const char *filename)
{
  FILE *file = NULL;

  file = fopen(filename, "r");
  if (file == NULL)
    {
      syslog(LOG_WARNING, "Can't open database %s opened", filename);
      return DEVIDD_ERR_IO;
    }

  syslog(LOG_INFO, "Locale database %s opened", filename);

  fclose(file);
  return DEVIDD_SUCCESS;
}

char *uw_check_rule()
{

  return DEVIDD_SUCCESS;
}

struct linked_list *uw_check_rules()
{

  return DEVIDD_SUCCESS;
}

/* int uw_read_file(int argc, char *argv[]) */
/* { */

/*   return DEVIDD_SUCCESS; */
/* } */

/* int uw_write_file(int argc, char *argv[]) */
/* { */

/*   return rcode; */
/* } */

/* int uw_close_file(int argc, char *argv[]) */
/* { */

/*   return rcode; */
/* } */
