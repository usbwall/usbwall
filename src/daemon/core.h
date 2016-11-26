/**
 * \file core.h
 * \brief Core functions; supervisor for manipulation of other modules.
 */
#pragma once

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

/**
 * \brief signal handling main function. It will register which signals
 * must be listened and enable the signal handler.
 * \return return non 0 value in case of error
 */
int signal_config(void);
