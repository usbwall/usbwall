/**
 * \file config.h
 * \brief handle for ldap configuration parsing and storage
 */
#ifndef CONFIG_H
# define CONFIG_H

#pragma once

/**
 * \brief contain every needed informations about the ldap
 * configuration
 */
struct config
{
  char *backend;	/**< backend to use */
  /* FILE configuration */
  char *config_file;    /**< path to the configuration file */
  /* LDAP configuration */
  char *uri;		/**< uri of the ldap */
  char *basedn;		/**< base domain name to access users on ldap */
  char *binddn;		/**< bind domain name to connect to ldap */
  char *bindpw;		/**< bind password to connect to ldap */
  short version;	/**< ldap version (2 and 3 are supported) */
};

/**
 * \brief access to the global configuration state of the project. Do not
 * call this function before the initialization of the configuration.
 *
 * \return structure containing configurations variables of the project.
 */
__attribute__((pure))
const struct config *configuration_get(void);

/**
 * \brief find the path where file containing the ldap configuration should be.
 * \return path to the configuration file
 */
__attribute__((const))
const char *cfg_file_find(void);

/**
 * \brief parse, and extract information from the configuration file and update
 * the project configuration. The first call to this function initialize the
 * configuration.
 * \param cfg_file path of the configuration file
 * \return 1 if an error prevented the configuration to be updated.
 * 0 otherwhise.
 */
int update_configuration(const char *cfg_file);

/**
 * \brief destroy and free project configuration. If the configuration has not
 * been set yet, the fonction will simply return.
 */
void destroy_configuration(void);

#endif /* ! CONFIG_H  */
