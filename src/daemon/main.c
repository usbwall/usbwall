#include "daemon_main.c"

int main(int argc, char *argv[])
{
  if (parse_args(argc, argv))
    return 0;

  if (signal_config())
  {
    syslog(LOG_ERR, "Signal handling init failed.");

    return 1; /* a valid signal handling is mandatory */
  }

  const char *pidfile = "/var/run/usbwall.pid";
  int pidfile_fd = -1;
  if ((pidfile_fd = create_pidfile(pidfile)) == -1)
    return DEVIDD_ERR_OTHER; /* pidfile is mandatory to ensure unique instance */

  syslog(LOG_INFO, "Usbwall started");
  int rcode = usbwall_run();
  syslog(LOG_INFO, "Usbwall terminated");

  remove_pidfile(pidfile, pidfile_fd);

  closelog();

  return !!rcode;
}
