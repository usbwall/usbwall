/**
 * \file parser.h
 * \brief FIXME
 */
#ifndef PARSER_H
# define PARSER_H

#pragma once

/* #include <stdio.h> */

#include "config.h"

/**
 * \brief parse a configuration file and return configuration structure.
 *
 * \param istream  the stream to the file to be parsed
 *
 * \return structure filled with the parsed values
 */
struct config *parse_config(FILE *istream);

#endif /* ! PARSER_H  */
