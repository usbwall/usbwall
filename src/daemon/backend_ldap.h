/**
 * \ldap backend_ldap.h
 * \Brief FIXME
 */
#ifndef BACKEND_LDAP_H
# define BACKEND_LDAP_H

#pragma once

#include <ldap.h>

/**
 * \brief maximum possible size of a user login
 */
# define LOGIN_MAX_LEN 32


/**
 * \brief internal devuser function that start and configure a connection with
 * the ldap server
 *
 * \param cfg the configuration structure containing needed information to setup
 * the connection with the ldap
 *
 * \return the LDAP connection handler pointer. NULL if an error occured
 */
LDAP *setup_ldap(const struct config *cfg);

/**
 * \brief check if the ldap is accessible and usable. Be
 * sure that the project configuration has been initialized
 * before a call to this function.
 *  Making devids accessible.
 * \return 1 if an error occured, 0 otherwhise.
 */
int devids_check(void);

/**
 * \brief Extract the list of usb serial ids allowed by
 *  the user. Be sure that the project configuration has
 *  been initialized before a call to this function.
 * \param username name of the checked user
 * \return Linked list containing serial ids as char *.
 *  NULL if an error occured.
 *
 * The function will try to bind to the ldap server and
 * retrieved the devids from the given username. In the
 * case of a connection problem, NULL is returned.
 */
struct linked_list *devids_get(const char *username);

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
/* int uw_open_ldap(void); */

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
/* int uw_read_ldap(int argc, char *argv[]); */

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
/* int uw_write_ldap(int argc, char *argv[]); */

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
/* int uw_close_ldap(int argc, char *argv[]); */

#endif /* !BACKEND_LDAP_H */
