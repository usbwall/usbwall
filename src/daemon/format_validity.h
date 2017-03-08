#ifndef FORMAT_VALIDITY_H
# define FORMAT_VALIDITY_H

#include <stdint.h>

# define LEN_FIELD 64
# define NB_FIELD_COMPLETE_ID 8
# define DEVID_MAX_LEN 1024
# define MIN_SERIAL 12
# define MAX_SERIAL 126
# define LEN_VENDOR_PRODUCT 4
# define LEN_BCD 16
# define MAX_BUS_PORT 3

# define FIELD_MACHINE 0
# define FIELD_BUS 1
# define FIELD_PORT 2
# define FIELD_SERIAL 3
# define FIELD_VENDOR 4
# define FIELD_PRODUCT 5
# define FIELD_BCD 6
# define FIELD_HORARY 7


/**
  * \brief Check if serial format is valid, i.e: serial length
  * is between 12 and 126 characters, and all characters are
  * encoded in hexadecimal format ([0-9A-F]{12-126}).
  *
  * \param serial the serial identifier to be checked
  *
  * \return DEVIDD_SUCCESS on success (format validity),
  * DEVIDD_ERR on failure
  *
  */
__attribute__((pure))
int32_t check_serial_format(char *serial);

/**
  * \brief Check if vendor id or product id format is valid,
  *  i.e: that identifier is of fixed size 4, and that all
  * characters are encoded in hexadecimal format ([0-9A-F]{4}).
  *
  * \param str the vendor or product identifier to be checked,
  * in string format
  *
  * \return DEVIDD_SUCCESS on success (format validity),
  * DEVIDD_ERR on failure
  *
  */
__attribute__((pure))
int32_t check_vendor_product_format(char *str);

/**
  * \brief Check if bcd_device identifier format is valid, i.e:
  * that bcd_device is of fixed size 16, and that all
  * characters are encoded in hexadecimal format.
  *
  * \param bcd the bcd_device identifier to be checked,
  * in string format
  *
  * \return DEVIDD_SUCCESS on success (format validity),
  * DEVIDD_ERR on failure
  *
  */
__attribute__((pure))
int32_t check_bcd_format(char *bcd);

/**
  * \brief Check if machine format is valid.
  *
  * \param machine the machine identifier to be checked
  *
  * \return DEVIDD_SUCCESS on success (format validity,
  * DEVIDD_ERR on failure
  *
  */
__attribute__((pure))
int32_t check_machine_format(char *machine);

/**
  * \brief Check if bus or port string format is valid, i.e:
  * identifier length is between 0 and 3 characters,
  * and all characters are digits encoded in decimal format
  * ([0-9]{3}).
  *
  * \param str the bus or port identifier to be checked,
  * in string format
  *
  * \return DEVIDD_SUCCESS on success (format validity,
  * DEVIDD_ERR on failure
  *
  */
__attribute__((pure))
int32_t check_bus_port_format(char *str);

/**
  * \brief Check if horary string format is valid, i.e: horary
  * is splited into two fields separated by a dash, and all characters
  * except the dash are digits encoded in hexadecimal format.
  *
  * \param field the horary identifier to be checked
  *
  * \return DEVIDD_SUCCESS on success
  * (format validity) or DEVIDD_ERR on failure
  *
  */
__attribute__((pure))
int32_t check_horaries_format(char *field);

/**
  * \brief Check if a given field of a rule or devid
  * has valid format (for instance: serial, machine...)
  *
  * \param field the field to be checked
  * \param i index of the given field
  *
  * \return validity, variable setted to DEVIDD_SUCCESS on success
  * (format validity), and DEVIDD_ERR on failure
  */
__attribute__((pure))
int32_t check_field_format(char *field, int32_t i);

/**
  * \brief Check if rule format format is correct, by
  * lexing the rule into separated fields, and
  * parsing each field. Number of fields is also checked.
  *
  * \param rule the rule to be checked
  *
  * \return valid, variable setted to DEVIDD_SUCCESS on success
  * (format validity), and DEVIDD_ERR on failure
  *
  */
int32_t check_rule_format(char *rule);



/* Notice that integer doesn't need to be checked: for instance,
 * the only rule for bus and port integers is that
 * they must have less than 3 digits. But uint8_t already
 * forces them to be so. */


#endif /* !FORMAT_VALIDITY_H */



