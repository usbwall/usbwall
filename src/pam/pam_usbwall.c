#include "uds.h"
#include "event.h"

#include <security/pam_ext.h>
#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <syslog.h>
#include <unistd.h>


/**
 * \brief pam usbwall module internal function to check if the debug mode has
 * been
 * enabled
 *
 * \param argc  number of arguments
 * \param argv  arguments array
 *
 * \return non zero value if the debug mode is activated
 */
static int fetch_debug(int argc, const char **argv)
{
  for (int i = 0; i < argc; ++i)
    if (!strcmp(argv[i], "debug"))
      return 1;

  return 0;
}

/**
 * \brief pam usbwall module internal function used to send an event to the
 * daemon.
 *
 * \param evt  The event to be sended to the daemon
 * \param debug  1 if debug is activated
 *
 * \return PAM_ABORT if an error occured. PAM_SUCCESS otherwhise.
 */
static int notify_daemon(enum event evt, int debug)
{
  struct sockaddr_un addr;

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
  {
    if (debug)
      syslog(LOG_ERR, "Unix socket can not be instanciated");

    return PAM_ABORT;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;

  strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    if (debug)
      syslog(LOG_ERR, "Initialization error - can not connect to daemon");

    return PAM_ABORT;
  }

  send(fd, &evt, sizeof(enum event), 0);

  close(fd);

  if (debug)
    syslog(LOG_DEBUG, "Send notification to the daemon of a new connection");

  return PAM_SUCCESS;

}

/**
 * \brief function called when an user connect
 *
 * \param pamh  unused
 * \param flags  unused
 * \param argc  number of arguments
 * \param argv  arguments array
 *
 * \return PAM_SUCCESS if no error occured.
*/
PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh __attribute__((unused)),
                                   int flags __attribute__((unused)),
                                   int argc,
                                   const char **argv)
{
  return notify_daemon(USER_CONNECT, fetch_debug(argc, argv));
}

/**
 * \brief function called when an user disconnect
 *
 * \param pamh  unused
 * \param flags  unused
 * \param argc  number of arguments
 * \param argv  arguments array
 *
 * \return PAM_SUCCESS if no error occured.
 */
PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh __attribute__((unused)),
                                    int flags __attribute__((unused)),
                                    int argc,
                                    const char **argv)
{
  return notify_daemon(USER_DISCONNECT, fetch_debug(argc, argv));
}
