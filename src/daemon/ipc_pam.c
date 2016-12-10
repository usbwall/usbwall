#include "ipc_pam.h"
#include "uds.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <syslog.h>
#include <unistd.h>

/**
 * \brief Simple ipc_pam internal function to log an error and return -1
 *
 * \param err_msg  error message to be displayed in the syslog
 *
 * \return allways -1
 */
static int die(const char *err_msg)
{
  syslog(LOG_ERR, "%s\n", err_msg);

  return -1;
}

enum event accept_user(int socket_fd)
{
  enum event message_event;
  int client_fd = 0;

  syslog(LOG_DEBUG, "Waiting for a user from pam module ...");
  if ((client_fd = accept(socket_fd, NULL, NULL)) == -1)
    return ERROR;

  if (recv(client_fd, &message_event, sizeof(enum event), 0) == -1)
    return ERROR;

  return message_event;
}

int init_socket(void)
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

  syslog(LOG_DEBUG, "Unix Domain Socket succefully initialized.");

  return fd;
}
