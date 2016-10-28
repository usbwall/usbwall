#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/types.h>
#include <linux/netlink.h>
#include <sys/un.h>

#include "socket.h"

const char *socket_path = "\0usbwall";

static int die(const char *s)
{
	write(2,s,strlen(s));
	return 1;
}

int accept_user(int netlink_fd)
{
	char buffer[1];
	int client_fd = 0;
	if ((client_fd = accept(netlink_fd, NULL, NULL)) == -1)
		return -1;

	int len = recv(client_fd, buffer, sizeof(buffer), 0);
	if (len == -1)
		return -1;

	printf("New user just connected.\n");
	return 0;
}

int init_socket(void)
{
	// Unix Domain Socket
	struct sockaddr_un addr;

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1)
		return die("Failed to create socket.\n");

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	if (*socket_path == '\0')
	{
		*addr.sun_path = '\0';
		strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
	}
	else
	{
		strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
		unlink(socket_path);
	}

	// Unlink the socket if it already exist
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		return die("Bind failed\n");
	else
		puts("Bind success!");

	// Listen to netlink socket
	if (listen(fd, 5) == -1) {
		perror("listen error");
		exit(-1);
	}
	return fd;
}
