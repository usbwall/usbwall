/**
 * \file core.h
 * \brief Core functions; supervisor for manipulation of other modules.
 */
#ifndef CORE_H
# define CORE_H
#pragma once

/* Globals */
/**
 * \brief core internal global used to trigger the terminaison of the main
 * process.
 */
extern int g_terminaison;
/**
 * \brief core internal global used to notify the main process that a cfg update
 * is needed.
 */
extern int g_cfgupdate;
/* ****** */

/**
 * \brief init usbwall modules and wait for users to login
 * \return return non 0 value in case of error
 *
 * The function will initialize everything needed for usbwall
 * (ipc with pam, devusb and load the configuration). Then, it
 * will wait for users to connect, and only then, supervise the
 * modules to handle devices accessibility,
 */
int usbwall_run(void);

#endif /* ! CORE_H  */
