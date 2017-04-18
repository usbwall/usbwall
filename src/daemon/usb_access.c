#include "usb_access.h"

#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

/**
 * \brief usb_access internal function that will write 0/1 in the given file.
 *
 * \param value  The boolean value to be written to the file
 * \param file_path  The path to the file we must write to
 *
 * \return non zero value if an error occured
 */
static int write_bool(int value, const char *file_path)
{
  assert(file_path);

  int fd = open(file_path, O_WRONLY | O_TRUNC);
  if (fd < 0)
    return 1;

  const char *boolean_value = value ? "1" : "0";
  int rcode = write(fd, boolean_value, 1) < 0;
  close(fd);

  return !!rcode;
}

/**
 * \brief usb_access internal function that convert a ports (uint8_t)
 * array into a string.
 *
 * \param ports  the array of ports
 * \param ports_nb  the size of the ports array
 *
 * \return A malloced string representing the ports, separated by dot.
 *  Return NULL on error
 */
static char *ports_to_string(uint8_t *ports, uint8_t ports_nb)
{
  assert(ports && ports_nb != 0);

  char result[ports_nb * 4]; // max length of a uint8_t is 3(+1 for the dot)
  result[0] = '\0';

  for (uint8_t idx = 0; idx < ports_nb; ++idx)
  {
    char *temp_result = strdup(result);
    sprintf(result, "%s%d.", temp_result, ports[idx]);
    free(temp_result);
  }

  char *last_dot = strrchr(result, '.');
  *last_dot = '\0';

  ptrdiff_t distance = last_dot - result;
  assert(distance > 0);
  size_t length = (size_t)distance;

  char *ports_str = malloc(sizeof (char) * length);
  if (!ports_str)
    return NULL;

  return strcpy(ports_str, result);
}

__attribute__((pure))
int device_is_valid(struct devusb *device)
{
  assert(device);

  /* Is a device without serial invalid?? */
  return device->ports_nb && device->bus && device->serial;
}

void set_usb_default_access(int value)
{
  syslog(LOG_INFO, "Setting all devices default access to %d", value);

  char file_path[1024] = { '\0' };
  int idx = 1;
  do
  {
    sprintf(file_path, "/sys/bus/usb/devices/usb%d/authorized_default", idx);
    idx++;
  } while (!write_bool(value, file_path));
}

int update_device_access(struct devusb *device, int value)
{
  assert(device && device_is_valid(device));

  char *ports_str = ports_to_string(device->ports, device->ports_nb);
  if (!ports_str)
    return 1;

  char file_path[1024] = { '\0' };
  sprintf(file_path,
          "/sys/bus/usb/devices/%d-%s/authorized",
          device->bus,
          ports_str);

  const char *access_str = value ? "Authorizing" : "Forbid";
  syslog(LOG_INFO,
         "%s device %s on %d-%s",
         access_str,
         device->serial,
         device->bus,
         ports_str);
  free(ports_str);

  return write_bool(value, file_path);
}

void update_devices_access(struct linked_list *authorized,
                           struct linked_list *forbidden)
{
  assert(authorized && forbidden);

  list_for_each(auth_node_ptr, authorized)
  {
    struct devusb *device = auth_node_ptr->data;
    assert(device_is_valid(device) && "Only valid devices are supported");

    if (update_device_access(device, 1))
      syslog(LOG_WARNING, "Update allowed device error : %s", device->serial);
  }

  list_for_each(forb_node_ptr, forbidden)
  {
    struct devusb *device = forb_node_ptr->data;
    assert(device_is_valid(device) && "Only valid devices are supported");

    if (update_device_access(device, 0))
      syslog(LOG_WARNING, "Update forbidden device error : %s", device->serial);
  }
}
