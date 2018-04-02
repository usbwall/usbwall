#include "daemon_main.c"

/********************************************************************************/
int main(int argc, char *argv[])
{
  int rcode = 0;

  /* Command line parsing */
  rcode = parse_args(argc, argv);
  if (rcode != DEVIDD_SUCCESS)
    return rcode;

  if (uw_signal_config())
  {
    syslog(LOG_ERR, "Signal handling init failed.");

    return DEVIDD_ERR_OTHER; /* a valid signal handling is mandatory */
  }

  /* We start the daemon */
  syslog(LOG_INFO, "Usbwall started");
  rcode = usbwall_run();
  syslog(LOG_INFO, "Usbwall terminated");

  closelog();

  return !!rcode;
}
