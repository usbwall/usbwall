/**
 * \file ldap_config.h
 * \brief handle for ldap configuration parsing and storage
 */
#pragma once

/**
 * \brief contain every needed informations about the ldap
 * configuration
 */
struct ldap_cfg
{
  char *uri;     /**< uri of the ldap */
  char *basedn;  /**< base domain name to access users on ldap */
  char *binddn;  /**< bind domain name to connect to ldap */
  char *bindpw;  /**< bind password to connect to ldap */
  short version;   /**< ldap version (2 and 3 are supported) */
};

/**
 * \brief find the path of the file containing the ldap configuration
 * \return path to the configuration file
 */
char *cfg_file_find(void);

/**
 * \brief parse, and extract information from the configuration file
 * \param cfg_file path of the configuration file
 * \return ldap_cfg struct containing ldap configurations, NULL in case of error
 */
struct ldap_cfg *make_ldap_cfg(char *cfg_file);

/**
 * \brief destroy and free ldap_cfg struct
 * \param cfg ldap_cfg to be destroyed
 */
void destroy_ldap_cfg(struct ldap_cfg *cfg);
