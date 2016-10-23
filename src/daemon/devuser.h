/**
 * \file devuser.h
 * \brief Contain user management functions
 * \author Arthur d'Avray
 **/

#pragma once

#include "ldap_config.h"

/**
 * \brief Wait for a user to loggin and return his name
 * \return current user name
 *
 * The function wait for an event from the pam module,
 * it will block until something is received.
 * Then, the user name is extracted from utmp get.
 */
char *wait_for_logging(void);

/**
 * \brief Extract the list of usb serial ids allowed by
 *    the user.
 * \param username name of the checked user
 * \param cfg current ldap configuration
 * \return Allowed serial ids. NULL if an error occured
 *   or if none are allowed.
 *
 * The function will try to bind to the ldap server and
 * retrieved the devids from the given username. In the
 * case of a connection problem, or if no entry are found,
 * NULL is returned.
 */
char *devids_get(char *username, struct ldap_cfg *cfg);
