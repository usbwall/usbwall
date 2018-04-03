/**
 * \file complete_id.h
 * \brief handle for ldap configuration parsing and storage
 */
#ifndef COMPLETE_ID_H
# define COMPLETE_ID_H

#pragma once

/* #include <stdint.h> */

#include "devusb.h"

/* Maximal length for each field of complete_id string */
# define LEN_STR 64

/* Number of fields in complete_id */
# define NB_FIELD_COMPLETE_ID 8

int32_t struct_to_string_array(char **array_id, struct devusb *devusb);

char *fill_complete_id(char **array_id, struct devusb *devusb);

int32_t allocate_array_id(char ***array_id);

void free_array_id(char ***array_id);

#endif /* ! COMPLETE_ID_H  */
