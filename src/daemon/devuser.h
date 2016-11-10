/**
 * \file devuser.h
 * \brief Contain user management functions
 **/

#pragma once

#include "ldap_config.h"

/**
 * \brief Wait for a user to login and return his name.
 * \param socket_fd Unix Domain Socket used to receive notifications
 * \return current user name. Return NULL on error.
 *
 * The function wait for an event from the pam module,
 * it will block until something is received.
 * Then, the user name is extracted from utmp get.
 */
char *wait_for_logging(int socket_fd);

/**
 * \brief Extract the list of usb serial ids allowed by
 *    the user.
 * \param username name of the checked user
 * \param cfg current ldap configuration
 * \return Null terminated array of allowed serial ids.
 *  NULL if an error occured or if none are allowed.
 *
 * The function will try to bind to the ldap server and
 * retrieved the devids from the given username. In the
 * case of a connection problem, or if no entry are found,
 * NULL is returned.
 */
char **devids_get(const char *username, const struct ldap_cfg *cfg);

/**
 * \brief Destroy and free the serial array returned by devids_get.
 * \param devids array of string to destroy
 */
void free_devids(char **devids);
