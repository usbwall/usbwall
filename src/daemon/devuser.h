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
 * \brief maximum possible size of device id (devid)
 */
# define DEVID_MAX_LEN 1024

/**
 * \brief index of field horary in devusb->complete_id
 */
# define I_HORARY 7

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
