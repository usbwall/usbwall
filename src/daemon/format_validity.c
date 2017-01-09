#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "format_validity.h"
#include "../misc/error_handler.h"

/* Serial format
 * - min: 12 characters
 * - max: 126 characters
 * - type: 0-9, A-F
 */ 

int32_t check_serial_format(char *serial)
{
  uint32_t i = 0;
  uint32_t len = 0; 

  /* Check lenght validity */
  len = strlen(serial);
  if ((len < 12) || (len > 126))
  {
    return DEVIDD_ERR_OTHER;
  }

  for (i = 0; i < len; i++)
  {
    /* A character is valid if and only if its format is hexadecimal
       and if it is not a lowercase */
    if (!isxdigit(serial[i]) || islower(serial[i]))
    {
      return DEVIDD_ERR_OTHER;
    } 
  }
  return DEVIDD_SUCCESS;
}

/* Vendor/product format:
 * - fixed size: 4
 * - type: 0-9, A-F
 */

int32_t check_vendor_product_format(void)
{
  /* FIXME */
  return DEVIDD_SUCCESS;
}

/* Bcd_device format:
 * - fixed size: 16
 * - type: 0-1
int32_t check_bcd_format(uint16_t bcd)
{
}

*/
