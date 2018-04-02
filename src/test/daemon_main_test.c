#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"

#include "../daemon/daemon_main.c"

/**
 * \Daemon Main unit tests 
 */

CTEST(main, create_pidfile) {
	char pid_path[] = "/tmp/usbwall_test_pid";
	remove(pid_path);
	int ret = create_pidfile(pid_path);
	ASSERT_NOT_EQUAL(-1, ret);

	char buffer[100];
	FILE *fp = fopen(pid_path, "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		ASSERT_EQUAL(getpid(), atoi(buffer));
	}
	fclose(fp);
	remove(pid_path);
}

CTEST(main, remove_pidfile) {
	char pid_path[] = "/tmp/usbwall_test_pid_remove";
	FILE *fp = fopen(pid_path, "ab+");
	if( access( pid_path, F_OK ) != -1 ) {
		remove_pidfile(pid_path, fileno(fp));
		if( access( pid_path, F_OK ) != -1 ) {
			CTEST_ERR("Couldn't remove pidfile: %d", fileno(fp));
			ASSERT_FAIL();
		}
	} else {
		CTEST_ERR("Couldn't create dummy pidfile for deletion test.");
		ASSERT_FAIL();
	}
}

/**
Un-testable as is.
CTEST_SKIP(main, daemonize) {
}
*/

/**
Un-testable as is.
CTEST_SKIP(main, parse_args) {
	char *str1[1];
	str1[0] = "-h";
}
*/