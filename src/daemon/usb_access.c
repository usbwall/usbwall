#include "usb_access.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

/**
 * \brief devusb internal function that will write 0/1 in the given file.
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

void set_usb_default_access(int value)
{
  char file_path[64] = { '\0' };
  int idx = 1;
  do
  {
    sprintf(file_path, "/sys/bus/usb/devices/usb%d/authorized_default", idx);
    idx++;
  } while (!write_bool(value, file_path));
}

int update_device_access(struct devusb *device, int value)
{
  if (!device->port || !device->port || !device->serial)
  {
    syslog(LOG_INFO, "skipping update for unavailable device ...");

    return 0;
  }

  char file_path[64] = { '\0' };

  sprintf(file_path,
          "/sys/bus/usb/devices/%hd-%hd/authorized",
          device->bus,
          device->port);
  syslog(LOG_INFO,
         "Authorizing device %s on %d-%d",
         device->serial,
         device->bus,
         device->port);

  return write_bool(value, file_path);
}

void update_devices_access(struct devusb **authorized,
                           struct devusb **forbidden)
{
  if (authorized)
  {
    for (int i = 0; authorized[i]; ++i)
      if (update_device_access(authorized[i], 1))
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
