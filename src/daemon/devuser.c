#include "devuser.h"

#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <ldap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utmp.h>

#define LOGIN_MAX_LEN 32

char *wait_for_logging(void)
{
  /**
   * \todo
   *  TODO : wait for the event from PAM
   */

  int utmp_fd = open("/var/run/utmp", O_RDONLY);
  if (utmp_fd != -1)
  {
    struct utmp log;
    while (read(utmp_fd, &log, sizeof (struct utmp)) == sizeof (struct utmp))
      if (log.ut_type == USER_PROCESS)
        return strdup(log.ut_name);
  }

  return strdup("unknown"); // for testing purpose
}

char *devids_get(char *username, struct ldap_cfg *cfg)
{
  LDAP *ldap_ptr = NULL;

  /* ldap library initialization */
  if (ldap_initialize(&ldap_ptr, cfg->uri) != LDAP_SUCCESS)
    return NULL;
  if (ldap_set_option(ldap_ptr, LDAP_OPT_PROTOCOL_VERSION, &cfg->version)
      != LDAP_OPT_SUCCESS)
    return NULL;
  if (ldap_sasl_bind_s(ldap_ptr,
                       cfg->binddn,
                       NULL,
                       &(struct berval) { strlen(cfg->bindpw), cfg->bindpw },
                       NULL,
                       NULL,
                       NULL)
      != LDAP_SUCCESS)
    return NULL;

  /* extract needed informations from the ldap */
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
    return NULL;
  msg_ptr = ldap_first_entry(ldap_ptr, msg_ptr);
  /**
   * \todo
   * FIXME : If everything is okay, msg_ptr should already contain only
   * one entry. If that assertion is true, is the above line really needed ?
   */

  /* extract devids */
  struct berval **values = ldap_get_values_len(ldap_ptr, msg_ptr, "devid");

  return values[0]->bv_val;
}
