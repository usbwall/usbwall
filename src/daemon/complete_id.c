/**
 * \file complete_id.c
 * \Brief FIXME
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
/* #include <unistd.h> */
#include <string.h>
/* #include <time.h> */
#include <assert.h>

#include "misc/error_handler.h"
#include "complete_id.h"

/********************************************************************************/

/**
 *  \brief Transform devusb fields into strings, and store it in array_id
 *
 *  \param array_id  the destination array
 *  \param devusb  the source struct
 *
 *  \return error code
 *
 */

int32_t struct_to_string_array(char **array_id, struct devusb *devusb)
{
  uint8_t i = 0;
  char *str_port = NULL;
  char *buf_ports = NULL;

  assert(devusb != NULL && "devusb is null");
  assert(array_id != NULL && "array_id is null");


  if (devusb->machine == NULL || devusb->serial == NULL
      || devusb->ports == NULL)
    return devidd_log("devidd", DEVIDD_ERR_IO, LOG_ERR);

  /* Prevent from buffer overflow */
  if (strlen(devusb->machine) > LEN_STR
      || strlen(devusb->serial) > LEN_STR)
    return devidd_log("devidd", DEVIDD_ERR_IO, LOG_ERR);

  /* Copy devusb machine and serial fields into array */
  strcpy(array_id[0], devusb->machine);
  strcpy(array_id[3], devusb->serial);

  /*if (sprintf(array_id[2], "%u", devusb->port) <= 0)
    return devidd_log("devidd", DEVIDD_ERR_IO, LOG_ERR);*/

  str_port = malloc(LEN_STR);
  buf_ports = malloc(LEN_STR);


  for (i = 0; i < devusb->ports_nb; i++)
  {
      if (sprintf(str_port, "%u", devusb->ports[i]) <= 0)
      {
        free(buf_ports);
        free(str_port);
        return devidd_log("devidd", DEVIDD_ERR_IO, LOG_ERR);
      }
      strcat(buf_ports, str_port);
      if (i != devusb->ports_nb - 1)
        strcat(buf_ports, ".");
  }
  strcpy(array_id[2], buf_ports);
  free(str_port);
  free(buf_ports);

  /* Transform vendor, product, bcd_device, bus, last_co into string,
      and store it in array_id */
  if ((sprintf(array_id[4], "%x", devusb->vendor) <= 0)
      || (sprintf(array_id[5], "%x", devusb->product) <= 0)
      || (sprintf(array_id[6], "%x", devusb->bcd_device) <= 0)
      || (sprintf(array_id[1], "%u", devusb->bus) <= 0)
      || (sprintf(array_id[7], "%" PRId64, devusb->last_co) <= 0))
    return devidd_log("devidd", DEVIDD_ERR_IO, LOG_ERR);

  return DEVIDD_SUCCESS;
}

/********************************************************************************/

/**
 * \brief Make a string from all the array fields, and store it in the
    devusb->complete_id field
 *
 * \param array_id is the array of strings which serves to create complete_id
 * \param devusb stores the string complete_id
 *
 * \return error code
 *
 */
char *fill_complete_id(char **array_id, struct devusb *devusb)
{
  int i = 0;
  int32_t ret = 0;
  char *str_complete_id = NULL;

  assert(array_id != NULL && "array_id is null");

  str_complete_id = malloc(LEN_STR * (NB_FIELD_COMPLETE_ID + 1));

  /* Transform devusb fields into string array */
  ret = struct_to_string_array(array_id, devusb);

  /* Concatenate each element of the array_id into complete_id string,
     stored in devusb struct */
  if (ret != DEVIDD_SUCCESS)
  {
    free(str_complete_id);
    return NULL;
  }

  for (i = 0; i < NB_FIELD_COMPLETE_ID; i++)
  {
    strcat(str_complete_id, array_id[i]);
    strcat(str_complete_id, ":");
  }

  return str_complete_id;
}

int32_t allocate_array_id(char ***array_id)
{
  int i = 0;
  char **ptr = NULL;

  assert(ptr == NULL && "ptr is not null");

  ptr = malloc(NB_FIELD_COMPLETE_ID * sizeof (char*));

  if (!ptr)
    return devidd_log("devidd", DEVIDD_ERR_MEM, LOG_ERR);
  else
  {
    for (i = 0; i < NB_FIELD_COMPLETE_ID; i++)
    {
      ptr[i] = malloc(LEN_STR * sizeof (char));
      if (!ptr[i])
        return devidd_log("devidd", DEVIDD_ERR_MEM, LOG_ERR);
      ptr[i][LEN_STR - 1] = '\0';
    }
  }

  *array_id = ptr;

  assert(ptr != NULL && "ptr is null");

  return DEVIDD_SUCCESS;
}

void free_array_id(char ***array_id)
{
  int i = 0;
  char **ptr = *array_id;

  for (i = 0; i < NB_FIELD_COMPLETE_ID; i++)
    free(ptr[i]);

  free(ptr);
}
