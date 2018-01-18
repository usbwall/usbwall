/**
 * \file uw_signal.h
 * \brief FIXME
 */
#ifndef UW_SIGNAL_H
# define UW_SIGNAL_H

#pragma once

/**
 * \brief signal handling main function. It will register which signals
 * must be listened and enable the signal handler.
 * \return return non 0 value in case of error
 */
int uw_signal_config(void);

#endif /* !UW_SIGNAL_H */
