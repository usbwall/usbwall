#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/stat.h>

#include "server.h"
#include "../misc/error_handler.h"


/**
 * \brief Create server socket
 *
 *
 */

int32_t serv_socket(int32_t *sock_fd)
{
  *sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (*sock_fd < 0)
  {
    syslog(LOG_ERR, "%s, %d: Cannot create socket for server",
           basename(__FILE__), __LINE__);
    return DEVIDD_ERR;
  }

  return DEVIDD_SUCCESS;
}


int32_t serv_bind(int32_t *sock_fd, struct sockaddr_in *serv_addr)
{
  int32_t b = 0; /* Return value for bind() */

  /* Declare struct serv_addr which will contain the sock_fd address */
  memset(serv_addr, 0, sizeof(*serv_addr)); /* FIXME: BSD: bzero */
  serv_addr->sin_family = AF_INET;
  serv_addr->sin_port = htons(SERV_PORT);
  serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);

  /* Bind sock_fd with this address */
  b = bind(*sock_fd, (struct sockaddr*) serv_addr, sizeof(*serv_addr));

  if (b < 0)
  {
    printf("[serv_bind]:%s\n", strerror(errno));
    syslog(LOG_ERR, "%s, %d: Cannot bind server socket",
           basename(__FILE__), __LINE__);
    return DEVIDD_ERR;
  }

  return DEVIDD_SUCCESS;
}

int32_t serv_recv(int32_t *sock_fd, char **buf,
                  struct sockaddr_in *serv_addr)
{

  int32_t r = 0; /* Return value for recvfrom() */
  uint32_t size_serv = sizeof (serv_addr); /* Size of serv_addr */

  /* Receive data from client
     - blocks until datagram received from the client */

  r = recvfrom(*sock_fd, *buf, BUF_LEN, 0,
      (struct sockaddr *) serv_addr,
      &size_serv);

  /* Handle recvfrom() failure */
  if (r < 0)
  {
    syslog(LOG_ERR, "%s, %d: Cannot receive from client to server",
           basename(__FILE__), __LINE__);
    return DEVIDD_ERR;
  }
  
  /* buf[r] = '\0'; */

  return DEVIDD_SUCCESS;

}

int32_t serv_send(int32_t *sock_fd, char **buf,
                  struct sockaddr_in *serv_addr)
{  
  int32_t s = 0; /* Return value for sendto() */
  uint32_t size_serv = sizeof (*serv_addr);
  /* Send data to the client */
  s = sendto(*sock_fd, *buf, BUF_LEN, 0,
      (struct sockaddr *) serv_addr,
      size_serv);

  /* Handle sendto() failure */
  if (s < 0)
  {
    printf("[serv_send]:%s\n", strerror(errno));
    syslog(LOG_ERR, "%s, %d: Cannot send from server to client",
           basename(__FILE__), __LINE__);
    return DEVIDD_ERR;
  }

  return DEVIDD_SUCCESS;
}


int32_t serv_core(void)
{
  int32_t sock_fd = 0; /* Server socket */
  char *buf; /* Buffer received from the client */
  struct sockaddr_in serv_addr; /* Server address */

  /* Create socket for server and bind it */
  if ((serv_socket(&sock_fd) != DEVIDD_SUCCESS)
      || (serv_bind(&sock_fd, &serv_addr) != DEVIDD_SUCCESS))
  {
    return DEVIDD_ERR;
  }

  buf = calloc(1, BUF_LEN);
  if (!buf)
  {
    syslog(LOG_ERR, "%s, %d: Memory allocation error",
           basename(__FILE__), __LINE__);

    return DEVIDD_ERR_MEM;
  }

  while(1)
  {
    /* syslog(LOG_ERR, "[SERVER] Buffer before reception: %s", buf);*/
    if (serv_recv(&sock_fd, &buf, &serv_addr) != DEVIDD_SUCCESS)
    {
      free(buf);
      return DEVIDD_ERR;
    }

    /* FIXME: treatment */
    printf("Client: %s\n", buf);
    buf = strdup("Because.");
    
    //syslog(LOG_ERR, "[SERVER] Buffer sent to client: %s", buf);

    if (serv_send(&sock_fd, &buf, &serv_addr) != DEVIDD_SUCCESS)
    {
      free(buf);
      return DEVIDD_ERR;
    }

  }

  free(buf);

  if (close(sock_fd) < 0)
  {
    syslog(LOG_ERR, "%s, %d: Cannot close server socket",
           basename(__FILE__), __LINE__);
    return DEVIDD_ERR;
  }

  return DEVIDD_SUCCESS;
}


int main(void) /* FIXME */
{
  if (serv_core() != DEVIDD_SUCCESS)
  {
    return DEVIDD_ERR;
  }

  return 0;
}


