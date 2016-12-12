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

#include "ipc_pam.h"

/**
 * \brief maximum possible size of a user login
 */
#define LOGIN_MAX_LEN 32

/**
 * \brief internal devuser function that start and configure a connection with
 * the ldap server
 *
 * \param cfg the configuration structure containing needed information to setup
 * the connection with the ldap
 *
 * \return the LDAP connection handler pointer. NULL if an error occured
 */
static LDAP *setup_ldap(const struct ldap_cfg *cfg)
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
                                      const struct ldap_cfg *cfg)
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

char *username_get(void)
{
  int utmp_fd = open("/var/run/utmp", O_RDONLY);
  if (utmp_fd != -1)
  {
    struct utmp log;
    while (read(utmp_fd, &log, sizeof(struct utmp)) == sizeof(struct utmp))
      if (log.ut_type == USER_PROCESS)
      {
        close(utmp_fd);
        char *username = strdup(log.ut_name);
        syslog(LOG_DEBUG, "Fetched current username : %s", username);

        return username;
      }
    close(utmp_fd);
  }
  syslog(LOG_WARNING, "Current username can't be fetched!");

  return NULL;
}

char *wait_for_logging(void)
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
    case ERROR:
      syslog(LOG_ERR, "Error from accept_user() function.");
      error = 1;
      break;
    case UNKNOWN:
      syslog(LOG_WARNING, "Unknown event from PAM module.");
      error = 1;
      break;
  }

  return error ? NULL : username_get();
}

struct linked_list *devids_get(const char *username,
                               const struct ldap_cfg *cfg)
{
  assert(username && cfg);

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

int check_devid(const char *devid, struct linked_list *devids)
{
  assert(devid && devids);

  syslog(LOG_DEBUG, "Devusb initialized sucessfully");

  int (*compare_function)(const void *, const void *) =
    (int (*)(const void *, const void *))strcmp;

  return !!list_extract(devids, devid, compare_function);
}
