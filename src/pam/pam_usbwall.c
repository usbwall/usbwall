#include <security/pam_ext.h>
#include <security/pam_modules.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include <linux/types.h>
#include <linux/un.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *socket_path = "\0usbwall";

static int fetch_debug(int argc, const char **argv);
static void notify_daemon(int netlink_fd);

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh __attribute__((unused)),
                                   int flags          __attribute__((unused)),
                                   int argc,
                                   const char **argv)
{
  // Unix Domain Socket
  struct sockaddr_un addr;
  int debug = fetch_debug(argc, argv);

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
    return PAM_ABORT;

  memset(&addr, 0, sizeof (addr));
  addr.sun_family = AF_UNIX;

  if (*socket_path == '\0')
  {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path + 1, socket_path + 1, sizeof (addr.sun_path) - 2);
  }
  else
    strncpy(addr.sun_path, socket_path, sizeof (addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof (addr)) == -1)
  {
    if (debug)
      syslog(LOG_ERR, "Initialization error - can not connect to daemon");
    return PAM_ABORT;
  }

  notify_daemon(fd);
  close(fd);

  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh __attribute__((unused)),
                                    int flags          __attribute__((unused)),
                                    int argc           __attribute__((unused)),
                                    const char **argv  __attribute__((unused)))
{
  return PAM_SUCCESS;
}

/************************************
 * Static functions implementations *
 ************************************/

static int fetch_debug(int argc          __attribute__((unused)),
                       const char **argv __attribute__((unused)))
{
  return 1;
}

static void notify_daemon(int fd)
{
  char buffer[] = "";
  send(fd, buffer, sizeof (buffer), 0);
}
