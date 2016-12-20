/**
 * \file devuser.h
 * \brief Contain user management functions
 **/

#pragma once

#include <misc/linked_list.h>

/**
 * \brief Retrieve the current user name.
 * \return users names connected on the system. Return NULL on error.
 *
 * usernames is extracted from utmp.
 */
struct linked_list *usernames_get(void);

/**
 * \brief Wait for a user to login and return his name.
 * \return name of users connected on the system. Return NULL on error.
 *
 * The function wait for an event from the pam module,
 * it will block until something is received.
 * Then, usernames_get is called and users names are returned.
 */
struct linked_list *wait_for_logging(void);

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
 * \brief Take a devid and a list of devids and check if the given devid
 * is contained in devids. That said, it check if the devid is allowed
 * for the current user.
 *
 * \param devid  the devid to be checked
 * \param devids  list of devids (char *) allowed for the current user
 *
 * \return 1 if the device is allowed, 0 otherwhise
 */
int check_devid(const char *devid, struct linked_list *devids);
