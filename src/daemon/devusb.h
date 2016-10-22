#pragma once

struct devusb
{
  char *serial;
  int bus;
  int port;
};

int init_devusb(void);
void close_devusb(void);
struct devusb **devices_get(void);
void free_devices(struct devusb **devices);
int update_devices(struct devusb **devices);
