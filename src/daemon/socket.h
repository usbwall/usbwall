/*
 * \file socket.h
 * \brief Containing function to do IPC between PAM and daemon
 * \Author Damien Pradier
 */

#pragma once

/**
 * \brief Initialize a Unix Domain Socket to receive notifications from PAM.
 * \return socket file descriptor. Return -1 on error.
 */
int init_socket(void);

/**
 * \brief Block until PAM notify the daemon.
 * The function wait for an event from the pam module.
 * The daemon is notified when a user connect to the machine.
 * \return 0 when a user just connected. Return -1 on error.
 */
int accept_user(int netlink_fd);
