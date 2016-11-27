#ifndef UDS_H_
# define UDS_H_

/**
 * \brief ipc_pam internal constant global used as the Unique Domain Socket
 * name. It must be the same one in PAM and daemon
 *  Only the first 127 characters are used
 */
static const char *socket_path = "\0usbwall";

#endif /* UDS_H_ */
