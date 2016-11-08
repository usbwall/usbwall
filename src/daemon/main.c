#include <syslog.h>

#include "core.h"

int main(void)
{
  openlog("usbwall", LOG_CONS | LOG_PID, LOG_USER);

  int rcode = usbwall_run();

  closelog();

  return !!rcode;
}
