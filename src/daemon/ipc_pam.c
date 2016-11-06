#include "ipc_pam.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <syslog.h>
#include <unistd.h>

/* Unique Domain Socket name, must be the same one in PAM
 * and daemon */
const char *socket_path = "\0usbwall";

static int die(const char *s)
{
  syslog(LOG_ERR, s);

  return -1;
}

int accept_user(int socket_fd)
{
  char buffer[1];
  int client_fd = 0;

  if ((client_fd = accept(socket_fd, NULL, NULL)) == -1)
    return -1;

  if (recv(client_fd, buffer, sizeof(buffer), 0) == -1)
    return -1;

  return 0;
}

int init_socket(void)
{
  // Unix Domain Socket
  struct sockaddr_un addr;

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
    return die("Initialization error - can not create socket");

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '\0')
  {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
  }
  else
  {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    unlink(socket_path);
  }

  // Unlink the socket if it already exist
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    return die("Initialization error - bind Unix Domain Socket failed");

  // Listen to Unix Domain socket
  if (listen(fd, 5) == -1)
    return die("Initialization error - listen Unix Domain Socket failed");

  return fd;
}
