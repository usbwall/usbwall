#include "usb_access.h"

#include <fcntl.h>
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
  int fd = open(file_path, O_WRONLY | O_TRUNC);
  if (fd < 0)
    return 1;

  const char *boolean_value = value ? "1" : "0";
  if (write(fd, boolean_value, 1) < 0)
    return 1;

  close(fd);

  return 0;
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
  char result[ports_nb * 4]; // max length of a uint8_t is 3(+1 for the dot)
  result[0] = '\0';

  for (uint8_t idx = 0; idx < ports_nb; ++idx)
    sprintf(result, "%s%d.", result, ports[idx]);

  char *last_dot = strrchr(result, '.');
  *last_dot = '\0';

  size_t length = (size_t)(last_dot - result); // allways positive
  char *ports_str = malloc(sizeof (char) * length);
  if (!ports_str)
    return NULL;

  return strcpy(ports_str, result);
}

/**
 * \brief usb_access internal function that check the possibility to
 * manipulate the given device in the sysfs.
 *
 * \param device  the device to check
 *
 * \return 0 if not valid, 1 otherwhise
 */
static int device_is_valid(struct devusb *device)
{
  return device->ports_nb && device->bus && device->serial;
}

void set_usb_default_access(int value)
{
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
  char *ports_str = ports_to_string(device->ports, device->ports_nb);
  if (!ports_str)
    return 1;

  char file_path[1024] = { '\0' };

  sprintf(file_path,
          "/sys/bus/usb/devices/%d-%s/authorized",
          device->bus,
          ports_str);
  syslog(LOG_INFO,
         "Authorizing device %s on %d-%s",
         device->serial,
         device->bus,
         ports_str);
  free(ports_str);

  return write_bool(value, file_path);
}

void update_devices_access(struct devusb **authorized,
                           struct devusb **forbidden)
{
  if (authorized)
  {
    for (int i = 0; authorized[i]; ++i)
      if (!device_is_valid(authorized[i]))
        syslog(LOG_INFO, "skipping update for unavailable device ...");
      else if (update_device_access(authorized[i], 1))
        syslog(LOG_WARNING,
               "Update authorized device error : %s",
               authorized[i]->serial);
  }

  if (forbidden)
  {
    for (int i = 0; forbidden[i]; ++i)
      if (update_device_access(authorized[i], 0))
        syslog(LOG_WARNING,
               "Update forbidden device error : %s",
               authorized[i]->serial);
  }
}
