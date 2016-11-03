/*
 * \file socket.h
 * \brief Containing function to do IPC between PAM and daemon
 */

#pragma once

/**
 * \brief Initialize a Unix Domain Socket to receive notifications from PAM.
 * \return socket file descriptor. Return -1 on error.
 */
int init_socket(void);

/**
 * \brief Block until PAM notify the daemon.
 * \param Unix Domain Socket used to receive notifications
 * \return 0 when a user just connected. Return -1 on error.
 *
 * The function wait for an event from the pam module.
 * The daemon is notified when a user connect to the machine.
 */
int accept_user(int socket_fd);
