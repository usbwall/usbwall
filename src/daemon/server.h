/**
 * \file server.h
 * \brief FIXME
 */
#ifndef SERVER_H
# define SERVER_H

#pragma once

# define SERV_PORT 4196
# define BUF_LEN 1024

int32_t serv_socket(int32_t *sock_fd);

int32_t serv_bind(int32_t *sock_fd, struct sockaddr_in *serv_addr);

int32_t serv_recv(int32_t *sock_fd, char **buf,
                  struct sockaddr_in *serv_addr);

int32_t serv_send(int32_t *sock_fd, char **buf,
                  struct sockaddr_in *serv_addr);

void *serv_core(void *arg __attribute__((unused)));


#endif /* !SERVER_H */
