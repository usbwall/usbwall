#include <stdlib.h>
#include <signal.h>
#include <syslog.h>

#include "misc/error_handler.h"
#include "ipc_pam.h"
#include "core.h" /* Globals variables declaration */
#include "uw_signal.h"

/**
 * \brief core internal function handling signal when received.
 * \param signo  id number of the signal received
 */
static void uw_signal_handler(int signo)
{
  syslog(LOG_DEBUG, "Received signal no %d", signo);

  if (signo == SIGTERM)
  {
    syslog(LOG_INFO, "SIGTERM received");
    close_ipc_pam(); /* it will close all connections with pam */
    g_terminaison = 1;
  }
  else if (signo == SIGHUP)
  {
    syslog(LOG_INFO, "SIGHUP received");
    g_cfgupdate = 1;
  }
}

int uw_signal_config(void)
{
  struct sigaction action;
  action.sa_handler = uw_signal_handler;
  sigfillset(&action.sa_mask);
  action.sa_flags = SA_RESTART;

  if (sigaction(SIGTERM, &action, NULL) == -1)
    return 1;
  if (sigaction(SIGHUP, &action, NULL) == -1)
    return 1;

  return DEVIDD_SUCCESS;
}
