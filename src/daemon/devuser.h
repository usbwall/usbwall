#pragma once

#include "ldap_config.h"

char *wait_for_logging(void);
char *devids_get(char *username, struct ldap_cfg *cfg);
