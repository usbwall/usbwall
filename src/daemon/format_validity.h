#ifndef FORMAT_VALIDITY_H
# define FORMAT_VALIDITY_H

#include <stdint.h> 
 
int32_t check_serial_format(char *serial);

int32_t check_vendor_product_format(void); /* uint16_t id */

int32_t check_bcd_format(uint16_t bcd);

/* Notice that bus and port doesn't need to be checked: the only rule is that they must have less than 3 digits. But uint8_t already forces them to be so. */


#endif /* !FORMAT_VALIDITY_H */



