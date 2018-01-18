/**
 * \file uw_pid.h
 * \brief Pid handling functions.
 */
#ifndef UW_PID_H
# define UW_PID_H

#pragma once

/**
 * \brief create the pidfile associated with the usbwall daemon.
 *
 * \param pidfile  path of the pidfile to create
 *
 * \return the file descriptor corresponding to the pidfile, -1
 * if an error occured.
 */
int uw_create_pidfile(const char *pidfile);

/**
 * \brief remove the pidfile associated with the usbwall daemon.
 *
 * \param pidfile  path of the pidfile to delete.
 * \param pidfile_fd  filedescriptor of the pidfile
 *
 * The function remove the pidfile from the filesystem. But, it
 * will also unlock it and close the associated filedescriptor
 */
void uw_remove_pidfile(const char *pidfile, int pidfile_fd);

#endif /* !UW_PID_H */
