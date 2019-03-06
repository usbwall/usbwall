#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"

#include "daemon/usb_access.c"
#include "misc/error_handler.h"

/**
 * \daemon usb access module unit tests 
 */

CTEST(usb_access, write_bool) { // fails
	char file1_path[] = "/tmp/usbwall_write_bool_test";
	int val1 = 1;
	int val2 = 0;
	int val3 = 4;

	remove(file1_path);
	
	int fd = open(file1_path, O_WRONLY|O_CREAT|O_TRUNC, 0640);
	close(fd);
	ASSERT_EQUAL(DEVIDD_SUCCESS, write_bool(val1, file1_path));

	char buffer[100];
	FILE *fp = fopen(file1_path, "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		ASSERT_EQUAL(val1, atoi(buffer));
	}
	fclose(fp);
	remove(file1_path);

	fd = open(file1_path, O_WRONLY|O_CREAT|O_TRUNC, 0640);
	close(fd);

	ASSERT_EQUAL(DEVIDD_SUCCESS, write_bool(val2, file1_path));

	fp = fopen(file1_path, "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		ASSERT_EQUAL(val2, atoi(buffer));
	}
	fclose(fp);
	remove(file1_path);
	fd = open(file1_path, O_WRONLY|O_CREAT|O_TRUNC, 0640);
	close(fd);

	ASSERT_EQUAL(DEVIDD_SUCCESS, write_bool(val3, file1_path));

	fp = fopen(file1_path, "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		ASSERT_EQUAL(val1, atoi(buffer));
	}
	fclose(fp);

	remove(file1_path);
}


CTEST_SKIP(usb_access, ports_to_string){
	uint8_t ports1[3];
	ports1[0] = 1;
	ports1[1] = 2;
	ports1[2] = 3;

	char str1[] = "1.2.3";

	ASSERT_STR(str1, ports_to_string(ports1, 3));


	uint8_t ports2[10];
	ports2[0] = 1;
	ports2[1] = 2;
	ports2[2] = 3;
	ports2[3] = 1;
	ports2[4] = 2;
	ports2[5] = 3;
	ports2[6] = 1;
	ports2[7] = 2;
	ports2[8] = 3;
	ports2[9] = 3;

	char str2[] = "1.2.3.1.2.3.1.2.3.3";

	ASSERT_STR(str2, ports_to_string(ports2, 10));


	uint8_t ports3[1];
	char str3[] = "0";
	ASSERT_STR(str3, ports_to_string(ports3, 10));
	// Buffer overflow 
	// Fails and allows reading data from stack.
}
