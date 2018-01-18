/**
 * \file ipc_pam.h
 * \brief Containing function to do IPC between PAM and daemon
 */
#ifndef IPC_PAM_H
# define IPC_PAM_H

#pragma once

#include "event.h"

/**
 * \brief Initialize the connection with PAM. Should be called before all others
 * functions of ipc_pam
 * \return Return 1 on error, 0 otherwhise
 */
int init_ipc_pam(void);

/**
 * \brief destroy and free every thing allocated by ipc_pam.
 * Should be called last
 */
void destroy_ipc_pam(void);

/**
 * \brief close all connection with the PAM.
 */
void close_ipc_pam(void);

/**
 * \brief Block until PAM notify the daemon.
 * \return USER_CONNECT when a user just connected.
 * Return ERROR on error.
 *
 * The function wait for an event from the pam module.
 * The daemon is notified when a user connect to the machine.
 */
enum event accept_user(void);

#endif /* ! IPC_PAM_H  */
