#include "devuser.h"

#include <assert.h>
#include <fcntl.h>
#include <ldap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include <utmp.h>

#include "config.h"
#include "ipc_pam.h"

/**
 * \brief maximum possible size of a user login
 */
# define LOGIN_MAX_LEN 32


/**
 * \brief maximum possible size of device id (devid)
 */
# define DEVID_MAX_LEN 1024



/**
 * \brief index of field horary in devusb->complete_id
 */
# define I_HORARY 7




/**
 * \brief internal devuser function that start and configure a connection with
 * the ldap server
 *
 * \param cfg the configuration structure containing needed information to setup
 * the connection with the ldap
 *
 * \return the LDAP connection handler pointer. NULL if an error occured
 */
static LDAP *setup_ldap(const struct config *cfg)
{
  assert(cfg);

  LDAP *ldap_ptr = NULL;

  if (ldap_initialize(&ldap_ptr, cfg->uri) != LDAP_SUCCESS)
  {
    syslog(LOG_WARNING, "Ldap initialization failed");

    return NULL;
  }

  if (ldap_set_option(ldap_ptr, LDAP_OPT_PROTOCOL_VERSION, &cfg->version)
      != LDAP_OPT_SUCCESS)
  {
    syslog(LOG_WARNING,
        "Ldap does not support the protocol version %hd",
        cfg->version);
    ldap_unbind_ext(ldap_ptr, NULL, NULL);

    return NULL;
  }

  if (ldap_sasl_bind_s(ldap_ptr,
        cfg->binddn,
        NULL,
        &(struct berval){ strlen(cfg->bindpw), cfg->bindpw },
        NULL,
        NULL,
        NULL)
      != LDAP_SUCCESS)
  {
    syslog(LOG_WARNING, "Ldap sasl binding failed");
    ldap_unbind_ext(ldap_ptr, NULL, NULL);

    return NULL;
  }

  syslog(LOG_DEBUG, "Ldap initialization succeded");

  return ldap_ptr;
}

/**
 * \brief internal devuser function that ask the LDAP all the allowed device ids
 * for the given user.
 *
 * \param ldap_ptr  Ldap connection handler pointer
 * \param username  The uid that will be searched in the LDAP
 * \param cfg  the ldap configuration structure
 *
 * \return NULL terminated array of ber values containing the allowed devices
 * for the given user. NULL if an error occured
 *
 * The function will request the devids from the LDAP for the given uid
 * (username). The returned values are bervals to be coherant with the LDAP API.
 */
static struct berval **extract_devids(LDAP *ldap_ptr,
    const char *username,
    const struct config *cfg)
{
  assert(ldap_ptr && username && cfg);

  LDAPMessage *msg_ptr = NULL;
  char filter[LOGIN_MAX_LEN + 1] = { '\0' };
  snprintf(filter, LOGIN_MAX_LEN, "(uid=%s)", username);

  if (ldap_search_ext_s(ldap_ptr,
        cfg->basedn,
        LDAP_SCOPE_SUB,
        filter,
        NULL,
        /**
         * \todo
         * FIXME : a valid attribute list would
         * be better than NULL for the searching
         * function.
         */
        0,
        NULL, /* no timeout */
        NULL,
        NULL,
        12,
        /**
         * \remark
         *  searching for 1 entry should be valid,
         *  12 may be overkill...
         */
        &msg_ptr)
          != LDAP_SUCCESS)
          return NULL;
  if (!ldap_count_entries(ldap_ptr, msg_ptr))
  {
    syslog(LOG_WARNING,
        "Ldap research failed. No entry found for user %s",
        username);

    return NULL;
  }
  msg_ptr = ldap_first_entry(ldap_ptr, msg_ptr);
  /**
   * \todo
   * FIXME : If everything is okay, msg_ptr should already contain only
   * one entry. If that assertion is true, is the above line really needed ?
   */

  struct berval **res = ldap_get_values_len(ldap_ptr, msg_ptr, "devid");
  ldap_msgfree(msg_ptr);

  syslog(LOG_DEBUG, "Extracted devids from ldap");

  return res;
}

struct linked_list *usernames_get(void)
{
  int utmp_fd = open("/var/run/utmp", O_RDONLY);
  if (utmp_fd != -1)
  {
    struct linked_list *usernames = list_make();
    struct utmp log;
    while (read(utmp_fd, &log, sizeof(struct utmp)) == sizeof(struct utmp))
      if (log.ut_type == USER_PROCESS)
      {
        close(utmp_fd);
        char *username = strdup(log.ut_name);
        list_add_back(usernames, username);
        syslog(LOG_DEBUG, "Fetched username : %s", username);
      }
    close(utmp_fd);

    if (!usernames->first)
      syslog(LOG_WARNING, "User not found!");

    return usernames;
  }
  syslog(LOG_WARNING,
      "Current username can't be fetched! : utmp not available");

  return NULL;
}

struct linked_list *wait_for_logging()
{
  /* Wait for the event from PAM */
  enum event message_event = accept_user();

  /* Handle enum here */
  int error = 0;
  switch (message_event)
  {
    case USER_CONNECT:
      syslog(LOG_INFO, "New user just connected.");
      break;
    case USER_DISCONNECT:
      syslog(LOG_INFO, "User just disconnected.");
      break;
    case CLOSED:
      syslog(LOG_INFO, "Connection with PAM closed.");
      break;
    case ERROR:
      syslog(LOG_ERR, "Error from accept_user() function.");
      error = 1;
      break;
    case UNKNOWN:
      syslog(LOG_WARNING, "Unknown event from PAM module.");
      error = 1;
      break;
  }

  return error ? NULL : usernames_get();
}

int devids_check(void)
{
  const struct config *cfg = configuration_get();
  LDAP *ldap_ptr = setup_ldap(cfg);
  if (!ldap_ptr)
  {
    syslog(LOG_ERR, "Initial LDAP connection can't be established.");

    return 1;
  }

  ldap_unbind_ext(ldap_ptr, NULL, NULL);
  syslog(LOG_INFO, "LDAP connection verified");

  return 0;
}

struct linked_list *devids_get(const char *username)
{
  assert(username);
  const struct config *cfg = configuration_get();

  LDAP *ldap_ptr = setup_ldap(cfg); // init the connection
  if (!ldap_ptr)
    return NULL;

  struct berval **values = extract_devids(ldap_ptr, username, cfg);
  ldap_unbind_ext(ldap_ptr, NULL, NULL); // close the connection

  struct linked_list *devids = list_make();
  const int ret = ldap_count_values_len(values);
  if (ret > 0)
  {
    const size_t values_count = (size_t)ret;

    if (!devids)
      return NULL;

    for (unsigned i = 0; i < values_count; ++i)
      list_add_back(devids, strdup(values[i]->bv_val));
  }
  ldap_value_free_len(values);

  return devids;
}

/*int check_devid(const char *devid, struct linked_list *devids)
  {
  assert(devid && devids);

  syslog(LOG_DEBUG, "Devusb initialized sucessfully");

  int (*compare_function)(const void *, const void *) =
  (int (*)(const void *, const void *))strcmp;

  return !!list_extract(devids, devid, compare_function);
  }*/


int32_t check_horary(char)
{
  
}
int32_t check_one_rule(char **not_parsed_rule, char **not_parsed_devid)
{
  char *field_rule; 
  char *field_devid;
  char *begin;
  char *end;
  int32_t match = DEVIDD_ERR_OTHER;
  int32_t i = 0;

  /* FIXME: sizes are too big */
  field_rule = calloc(1, DEVID_MAX_LEN);
  field_devid = calloc(1, DEVID_MAX_LEN);

  if (!field_rule || !field_devid)
  {
    return DEVIDD_ERR_MEM;
  }

  for (i = 0; (i < I_HORARY) && *not_parsed_rule; i++)
  {
    field_rule = strtok(*not_parsed_rule, ":");
    field_devid = strtok(*not_parsed_devid, ":");


    /* Offset = length of last token + separator ":" */
    *not_parsed_rule += strlen(field_rule) + 1;
    *not_parsed_devid += strlen(field_devid) + 1;

    /* If rule's field is not equal to "*" nor to devid field,
       we don't need to continue: the entire rule cannot match */
    if (strcmp(field_rule, "*") && strcmp(field_rule, field_devid))
      break;
  }

  if (i == I_HORARY)
  {
    begin = strtok(*not_parsed_rule, "-");
    end = not_parsad_rule + strlen(begin) + 1;
    field_devid = strtok(*not_parsed_devid, ":");

    if ((atoi(begin) <= atoi(field_devid))
        && (atoi(field_devid) <= atoi(end)))
    {
      match = DEVIDD_SUCCESS;
    }
  }

  free(field_rule);
  free(field_devid);

  return matched;
}

int32_t check_devid(const char * const devid, struct linked_list *rules)
{
  int32_t i = 0;
  char *not_parsed_devid;
  char *not_parsed_rule;
  /* Equal to DEVIDD_SUCCESS if at least one rule matched 
     => device is then authorized */
  int32_t is_auth = DEVIDD_ERR_OTHER;
  struct ll_node *rule = rules->first;

  /* TODO: check if devid format is valid */

  /* Allocation of 2 strings which will store the tokens that remain
     unparsed */
  not_parsed_devid = malloc(strlen(devid));
  not_parsed_rule = malloc(DEVID_MAX_LEN);

  if (!not_parsed_devid || !not_parsed_rule)
  { 
    free(not_parsed_devid);
    free(not_parsed_rule); 

    return DEVIDD_ERR_MEM; 
  }

  while (rule)
  {
    not_parsed_rule = strdup(rule->data); 
    not_parsed_devid = strdup(devid);

    is_auth = check_one_rule(&not_parsed_rule, &not_parsed_devid); 

    if (is_auth == DEVIDD_SUCCESS)
      break;

    rule = rule->next; 
  }

  free(not_parsed_devid);
  free(not_parsed_rule); 

  /* is_auth was set to DEVIDD_SUCCESS if one rule matched  */
  return is_auth; 
}
