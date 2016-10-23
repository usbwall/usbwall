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

static LDAP *setup_ldap(struct ldap_cfg *cfg);
static struct berval **extract_devids(LDAP *ldap_ptr,
                                      char *username,
                                      struct ldap_cfg *cfg);

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
      {
        close(utmp_fd);
        return strdup(log.ut_name);
      }
  }
  close(utmp_fd);

  return NULL;
}

char **devids_get(char *username, struct ldap_cfg *cfg)
{
  LDAP *ldap_ptr = setup_ldap(cfg);
  struct berval **values = extract_devids(ldap_ptr, username, cfg);
  size_t values_count = ldap_count_values_len(values);

  /* convert berval array to string array */
  char **devids = malloc(sizeof (char *) * values_count + 1);
  for (unsigned i = 0; i < values_count; ++i)
    devids[i] = values[i]->bv_val;
  devids[values_count] = NULL;

  return devids;
}

void free_devids(char **devids)
{
  for (int i = 0; devids[i]; ++i)
    free(devids[i]);
  free(devids);
}

/************************************
 * Static functions implementations *
 ************************************/

static LDAP *setup_ldap(struct ldap_cfg *cfg)
{
  LDAP *ldap_ptr = NULL;

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

  return ldap_ptr;
}

static struct berval **extract_devids(LDAP *ldap_ptr,
                                      char *username,
                                      struct ldap_cfg *cfg)
{
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

  return ldap_get_values_len(ldap_ptr, msg_ptr, "devid");
}
