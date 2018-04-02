#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "misc/error_handler.h"
#include "uw_signal.h"
#include "core.h"
#include "main.h"

/********************************************************************************/
/**
 * \brief daemonize the process
 *
 * \return 1 if the process must stop, 0 else way
 */
static int daemonize(void)
{
  pid_t pid = fork();

  if (pid < 0)
    return DEVIDD_ERR_OTHER; // fork error
  if (pid > 0)
    return DEVIDD_ERR_OTHER; // terminate parent;
  if (setsid() < 0)
    return DEVIDD_ERR_OTHER; // create a new session

  pid = fork();
  if (pid < 0)
    return DEVIDD_ERR_OTHER; // fork error
  if (pid > 0)
    return DEVIDD_ERR_OTHER; // terminate session leader process

  umask(0);           // new file permissions

  if (chdir("/") < 0) // change working directory
    return DEVIDD_ERR_OTHER;

  close(STDOUT_FILENO);
  close(STDIN_FILENO);
  close(STDERR_FILENO);

  openlog("usbwall", LOG_PID, LOG_DAEMON);

  return DEVIDD_SUCCESS;
}

/********************************************************************************/
/**
 * \brief parse arguments and execute the associated action. The function will
 * also appropriatly open the syslog
 *
 * \param argc  number of arguments
 * \param argv  program arguments
 *
 * \return 1 if the process must exit, 0 if it must continue
 */
static int parse_args(int argc, char *argv[])
{
  if (argc >= 2)
  {
    const char *help_msg =
      "usage :\n"
      "\t-h [--help] : print the usage help message\n"
      "\t-d [--daemonize]: start the program as a daemon\n";

    if (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--daemonize"))
      return daemonize();

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
      fprintf(stderr, "%s", help_msg);

      return DEVIDD_ERR_OTHER;
    }

    fprintf(stderr, "Unknown argument %s\n%s", argv[1], help_msg);

    return DEVIDD_ERR_OTHER;
  }

  openlog("usbwall", LOG_CONS | LOG_PID, LOG_USER);
  fprintf(stdout, "Check your syslog files for startup information about usbwall.\n");

  return DEVIDD_SUCCESS;
}
