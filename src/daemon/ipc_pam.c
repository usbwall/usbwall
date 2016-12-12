#include "ipc_pam.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <syslog.h>
#include <unistd.h>

#include "uds.h"

/**
 * \brief internal global variable corresponding to the file descriptor
 * associated with the UDS connection
 */
static int uds_fd;

/**
 * \brief Simple ipc_pam internal function to log an error and return -1
 *
 * \param err_msg  error message to be displayed in the syslog
 *
 * \return allways 1
 */
static int die(const char *err_msg)
{
  syslog(LOG_ERR, "%s\n", err_msg);

  return 1;
}

enum event accept_user(void)
{
  assert(uds_fd > 0);

  enum event message_event;
  int client_fd = 0;

  syslog(LOG_DEBUG, "Waiting for a user from pam module ...");
  if ((client_fd = accept(uds_fd, NULL, NULL)) == -1)
  {
    if (errno == EINVAL)
      return CLOSED;

    return ERROR;
  }

  if (recv(client_fd, &message_event, sizeof(enum event), 0) == -1)
    return ERROR;

  return message_event;
}

int init_ipc_pam(void)
{
  /* Unix Domain Socket */
  struct sockaddr_un addr;

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
    return die("Initialization error - can not create socket");

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 1);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    return die("Initialization error - bind Unix Domain Socket failed");

  /* Listen to Unix Domain socket */
  if (listen(fd, 5) == -1)
    return die("Initialization error - listen Unix Domain Socket failed");

  /* set the global variable*/
  uds_fd = fd;

  syslog(LOG_DEBUG, "Unix Domain Socket succefully initialized");

  return 0;
}

void close_ipc_pam(void)
{
  if (uds_fd > 0)
    shutdown(uds_fd, SHUT_RDWR);
}

void destroy_ipc_pam(void)
{
  if (uds_fd > 0)
    uds_fd = close(uds_fd);
}
