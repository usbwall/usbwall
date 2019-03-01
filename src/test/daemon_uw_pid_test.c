#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ctest.h"

#include "uw_pid.c"

/**
 * \Daemon uw_pid unit tests
 */

CTEST(uw_pid, uw_create_pidfile) {
	char pid_path[] = "/tmp/usbwall_test_pid";
	remove(pid_path);
	int ret = uw_create_pidfile(pid_path);
	ASSERT_NOT_EQUAL(-1, ret);

	char buffer[100];
	FILE *fp = fopen(pid_path, "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		ASSERT_EQUAL(getpid(), atoi(buffer));
	}
	fclose(fp);
	remove(pid_path);
}

CTEST(uw_pid, uw_remove_pidfile) {
	char pid_path[] = "/tmp/usbwall_test_pid_remove";
	FILE *fp = fopen(pid_path, "ab+");
	if( access( pid_path, F_OK ) != -1 ) {
		uw_remove_pidfile(pid_path, fileno(fp));
		if( access( pid_path, F_OK ) != -1 ) {
			CTEST_ERR("Couldn't remove pidfile: %d", fileno(fp));
			ASSERT_FAIL();
		}
	} else {
		CTEST_ERR("Couldn't create dummy pidfile for deletion test.");
		ASSERT_FAIL();
	}
}
