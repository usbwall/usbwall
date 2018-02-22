/**
 * \file backend_file.h
 * \Brief FIXME
 */
#ifndef BACKEND_FILE_H
# define BACKEND_FILE_H

#pragma once

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
int uw_open_file(void);

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
int uw_read_file(int argc, char *argv[]);

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
int uw_write_file(int argc, char *argv[]);

/**
 * \brief Entry function of the daemon
 *
 * \param argc  number of arguments
 *
 * \return 0 if success, any other value if any error occured
 */
int uw_close_file(int argc, char *argv[]);

#endif /* !BACKEND_FILE_H */
