#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <syslog.h>

#include "uw_pid.h"

int uw_create_pidfile(const char *pidfile)
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
      syslog(LOG_ERR, "An instance of usbwall is already running!");
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

void uw_remove_pidfile(const char *pidfile, int pidfile_fd)
{
  assert(pidfile && pidfile_fd >= 3);

  if (flock(pidfile_fd, LOCK_UN | LOCK_NB))
    syslog(LOG_WARNING, "Failed to unlock the pidfile");
  close(pidfile_fd);

  if (remove(pidfile))
    syslog(LOG_WARNING, "Failed to remove the pidfile");
}
