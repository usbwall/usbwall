#pragma once

struct ldap_cfg
{
  char *uri;
  char *basedn;
  char *binddn;
  char *bindpw;
  int version;
};

char *cfg_file_find(void);
struct ldap_cfg *make_ldap_cfg(char *cfg_file);
void destroy_ldap_cfg(struct ldap_cfg *cfg);
