/**
 * \file config.h
 * \brief handle for ldap configuration parsing and storage
 */
#pragma once

/**
 * \brief contain every needed informations about the ldap
 * configuration
 */
struct config
{
  /* LDAP configuration */
  char *uri;     /**< uri of the ldap */
  char *basedn;  /**< base domain name to access users on ldap */
  char *binddn;  /**< bind domain name to connect to ldap */
  char *bindpw;  /**< bind password to connect to ldap */
  short version; /**< ldap version (2 and 3 are supported) */
};

/**
 * \brief find the path of the file containing the ldap configuration
 * \return path to the configuration file
 */
const char *cfg_file_find(void);

/**
 * \brief parse, and extract information from the configuration file
 * \param cfg_file path of the configuration file
 * \return config struct containing ldap configurations, NULL in case of error
 */
struct config *make_config(const char *cfg_file);

/**
 * \brief destroy and free config struct
 * \param cfg config to be destroyed
 */
void destroy_config(struct config *cfg);
