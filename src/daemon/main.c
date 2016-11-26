#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "core.h"

/**
 * \brief daemonize the process
 *
 * \return 1 if the process must stop, 0 else way
 */
static int daemonize(void)
{
  pid_t pid = fork();

  if (pid < 0)
    return 1; // fork error
  if (pid > 0)
    return 1; // terminate parent;
  if (setsid() < 0)
    return 1; // create a new session

  pid = fork();
  if (pid < 0)
    return 1; // fork error
  if (pid > 0)
    return 1; // terminate session leader process

  umask(0);           // new file permissions
  if (chdir("/") < 0) // change working directory
    return 1;

  close(STDOUT_FILENO);
  close(STDIN_FILENO);
  close(STDERR_FILENO);

  openlog("usbwall", LOG_PID, LOG_DAEMON);

  return 0;
}

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
  const char *help_msg = "usage :"
                         "\n\t-h [--help] : print the usage help message"
                         "\n\t-d [--daemonize]: start the program as a daemon";

  if (argc >= 2)
  {
    if (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--daemonize"))
      return daemonize();

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
      puts(help_msg);

      return 1;
    }

    fprintf(stderr, "Unknown argument %s\n", argv[1]);
    fputs(help_msg, stderr);

    return 1;
  }

  openlog("usbwall", LOG_CONS | LOG_PID, LOG_USER);

  return 0;
}

int main(int argc, char *argv[])
{
  if (parse_args(argc, argv))
    return 0;

  if (signal_config())
  {
    syslog(LOG_ERR, "Signal handling init failed.");

    return 1; // a valid signal handling is mandatory
  }

  int rcode = usbwall_run();

  closelog();

  return !!rcode;
}
