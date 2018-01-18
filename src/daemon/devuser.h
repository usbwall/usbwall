/**
 * \file devuser.h
 * \brief Contain user management functions
 **/
#ifndef DEVUSER_H
# define DEVUSER_H

#pragma once

#include <stdint.h>

#include <misc/linked_list.h>
#include "misc/error_handler.h"

/**
 * \brief Retrieve the current user name.
 * \return users names connected on the system. Return NULL on error.
 *
 * usernames is extracted from utmpx (Standard for accessing usernames on Unix systems).
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
 * \brief Take devid and a rule, and check if the devid match the given rule.
 *
 * \param not_parsed_rule the rule to parse
 * \param not_parsed_devid the devid to parse and match with the given rule
 *
 * \return match which is set to DEVIDD_SUCCESS if devid matched the given
 *  rule, DEVIDD_ERR otherwise
 */
int32_t check_one_rule(char **not_parsed_rule, char **not_parsed_devid);

/**
 * \brief Take a devid and a list of rules and check if the given devid
 * match at least one rule. That said, it checks if the devid is allowed
 * for the current user.
 *
 * \param devid  the devid to be checked
 * \param rules  list of rules (char *) for the current user
 *
 * \return is_auth, local variable set to DEVIDD_SUCCESS in case of
 * device authorization
 */
int32_t check_devid(char *devid, struct linked_list *rules);

#endif /* ! DEVUSER_H  */
