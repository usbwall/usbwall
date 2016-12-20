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

#include "core.h"

/**
 * \brief create the pidfile associated with the usbwall daemon.
 *
 * \param pidfile  path of the pidfile to create
 *
 * \return the file descriptor corresponding to the pidfile, -1
 * if an error occured.
 */
static int create_pidfile(const char *pidfile)
{
  int fd = open(pidfile,
                O_CREAT | O_TRUNC | O_WRONLY,
                0644);
  if (fd == -1)
  {
    syslog(LOG_ERR, "Failed to create new pidfile at %s", pidfile);

    return -1;
  }

  if (flock(fd, LOCK_EX | LOCK_NB))
  {
    if (errno == EWOULDBLOCK)
      syslog(LOG_ERR, "An instance of usbwalld is already running!");
    else
      syslog(LOG_ERR, "Failed to lock pidfile");
    close(fd);

    return -1;
  }

  if (dprintf(fd, "%zd", (ssize_t)getpid()) <= 0)
  {
    syslog(LOG_ERR, "Failed to write to pidfile at %s", pidfile);
    close(fd);

    return -1;
  }

  syslog(LOG_INFO, "Pidfile created at %s", pidfile);

  return fd;
}

/**
 * \brief remove the pidfile associated with the usbwall daemon.
 *
 * \param pidfile  path of the pidfile to delete.
 * \param pidfile_fd  filedescriptor of the pidfile
 *
 * The function remove the pidfile from the filesystem. But, it
 * will also unlock it and close the associated filedescriptor
 */
static void remove_pidfile(const char *pidfile, int pidfile_fd)
{
  assert(pidfile && pidfile_fd >= 3);

  if (flock(pidfile_fd, LOCK_UN | LOCK_NB))
    syslog(LOG_WARNING, "Failed to unlock the pidfile");
  close(pidfile_fd);

  if (remove(pidfile))
    syslog(LOG_WARNING, "Failed to remove the pidfile");
}

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
  if (argc >= 2)
  {
    const char *help_msg =
      "usage :"
      "\n\t-h [--help] : print the usage help message"
      "\n\t-d [--daemonize]: start the program as a daemon";

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

  const char *pidfile = "/var/run/usbwall.pid";
  int pidfile_fd = -1;
  if ((pidfile_fd = create_pidfile(pidfile)) == -1)
    return 1; // pidfile is mandatory to ensure unique instance

  syslog(LOG_INFO, "Usbwall started");
  int rcode = usbwall_run();
  syslog(LOG_INFO, "Usbwall terminated");

  remove_pidfile(pidfile, pidfile_fd);

  closelog();

  return !!rcode;
}
