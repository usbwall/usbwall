#pragma once

#include <stdio.h>

#include "ldap_config.h"

/**
 * \brief parse a configuration file and return configuration structure.
 *
 * \param istream  the stream to the file to be parsed
 *
 * \return structure filled with the parsed values
 */
struct ldap_cfg *parse_config(FILE *istream);
