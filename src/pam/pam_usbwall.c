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
 * \param argc  unused
 * \param argv  unused
 *
 * \return non zero value if the debug mode is activated
 */
static int fetch_debug(int argc __attribute__((unused)),
                       const char **argv __attribute__((unused)))
{
  /**
   * \todo
   * TODO: The function will allways return 1
   **/

  return 1;
}

/**
 * \brief pam usbwall module internal function used to send an event to the
 * daemon.
 *
 * \param fd  The filedescriptor of the socket used for IPC
 */
static void notify_daemon(int fd, enum event evt)
{
  send(fd, &evt, sizeof(enum event), 0);
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
  // Unix Domain Socket
  struct sockaddr_un addr;
  int debug = fetch_debug(argc, argv);

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
    return PAM_ABORT;

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;

  strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    if (debug)
      syslog(LOG_ERR, "Initialization error - can not connect to daemon");
    return PAM_ABORT;
  }

  notify_daemon(fd, USER_CONNECT);
  close(fd);

  return PAM_SUCCESS;
}

/**
 * \brief function called when an user disconnect
 *
 * \param pamh  unused
 * \param flags  unused
 * \param argc  unused
 * \param argv  unused
 *
 * \return PAM_SUCCESS if no error occured.
 */
PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh __attribute__((unused)),
                                    int flags __attribute__((unused)),
                                    int argc,
                                    const char **argv)
{
  // Unix Domain Socket
  struct sockaddr_un addr;
  int debug = fetch_debug(argc, argv);

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
    return PAM_ABORT;

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;

  strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    if (debug)
      syslog(LOG_ERR, "Initialization error - can not connect to daemon");
    return PAM_ABORT;
  }

  notify_daemon(fd, USER_DISCONNECT);
  close(fd);

  return PAM_SUCCESS;
}
