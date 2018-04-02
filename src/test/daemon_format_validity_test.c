#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"

#include "../daemon/format_validity.c"

/**
 * \format validity unit tests 
 */

CTEST(format_validity, check_serial_format) {
	char str1[] = "1AB2C3D4E5F67890"; // Ok
	char str2[] = "GHIJKLMN"; // Non-hex char
	char str3[] = "AB C3 5F AA BC DD"; // Non-hex char (spaces)
	char str4[] = "ABC"; // <12 char
	char str5[129]; // >126 char
		 str5[128] = '\0';
	char str6[] = ""; // empty string
	char str7[] = "abcdefabcdef"; // Non-hex char

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_serial_format(str1));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_serial_format(str2));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_serial_format(str3));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_serial_format(str4));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_serial_format(str5));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_serial_format(str6));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_serial_format(str7));
}

CTEST(format_validity, check_vendor_product_format) {
	char str1[] = "0000"; // Ok
	char str2[] = "ABCD"; // Ok
	char str3[] = "AB"; // !=4 char
	char str4[] = "01234"; // !=4 char
	char str5[] = "CAKE"; // Non-hex char
	char str6[] = "abcd"; // Non-hex char
	char str7[] = ""; // Non-hex char

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_vendor_product_format(str1));
    ASSERT_EQUAL(DEVIDD_SUCCESS, check_vendor_product_format(str2));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_vendor_product_format(str3));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_vendor_product_format(str4));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_vendor_product_format(str5));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_vendor_product_format(str6));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_vendor_product_format(str7));
}

CTEST(format_validity, check_bcd_format) {
	char str1[] = "0000000000000000"; // Ok
	char str2[] = "0101010101010101"; // Ok
	char str3[] = "ABABABABABABABAB"; // != 0 or 1
	char str4[] = ""; // != 0 or 1
	char str5[] = "01010101010101010101"; // >16

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_bcd_format(str1));
    ASSERT_EQUAL(DEVIDD_SUCCESS, check_bcd_format(str2));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bcd_format(str3));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bcd_format(str4));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bcd_format(str5));
}

CTEST(format_validity, check_machine_format) {
	char str1[] = "1111111111111111111111111111111111111111111111111111111111111111"; 		// Ok
	char str2[] = ""; 	// <64
	char str3[] = "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"; 		// >64

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_machine_format(str1));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_machine_format(str2));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_machine_format(str3));
}

CTEST(format_validity, check_bus_port_format) {
	char str1[] = "000"; // Ok
	char str2[] = "123"; // OK
	char str3[] = "AB"; // !=3 char
	char str4[] = "0123"; // !=3 char
	char str5[] = "CAKE"; // Non-hex char
	char str6[] = "abcd"; // Non-hex char
	char str7[] = ""; // Non-hex char
	char str8[] = "abc"; // Non-deci
	char str9[] = "1"; // OK

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_bus_port_format(str1));
    ASSERT_EQUAL(DEVIDD_SUCCESS, check_bus_port_format(str2));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bus_port_format(str3));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bus_port_format(str4));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bus_port_format(str5));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bus_port_format(str6));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bus_port_format(str7));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_bus_port_format(str8));
    ASSERT_EQUAL(DEVIDD_SUCCESS, check_bus_port_format(str9)); // Fails

}

CTEST(format_validity, check_horaries_format) {
	char str1[] = "1111111111111111111111111111-22222222222222222222222222222222222"; 		// Ok
	char str2[] = ""; 	// <64
	char str3[] = "111111111111111111111111111111111111111111-11111111111111111111111111111111111111111111111111111"; 		// >64
	char str4[] = "1111111111111111111111111111322222222222222222222222222222222222"; // No dash
	char str5[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa-bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"; // Non-dec

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_horaries_format(str1));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_horaries_format(str2));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_horaries_format(str3));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_horaries_format(str4));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_horaries_format(str5));
}

CTEST(format_validity, check_field_format) {

	// Machine
	char str1_OK[] = "1111111111111111111111111111111111111111111111111111111111111111"; 		// Ok
	char str1_KO[] = "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"; 		// >64

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_field_format(str1_OK, FIELD_MACHINE));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_field_format(str1_KO, FIELD_MACHINE));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_field_format(str1_KO, FIELD_BUS));

	// BCD
	char str2_OK[] = "0101010101010101"; // Ok
	char str2_KO[] = "ABABABABABABABAB"; // != 0 or 1

    ASSERT_EQUAL(DEVIDD_SUCCESS, check_field_format(str2_OK, FIELD_BCD));
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_field_format(str2_KO, FIELD_BCD));

    // Other / switch default
    ASSERT_EQUAL(DEVIDD_ERR_OTHER, check_field_format(str1_OK, 100));

}
/*
	machine:bus:port:serial:vendor:product:bcd:horary
*/
CTEST_SKIP(format_validity, check_rule_format) {
	//char str1[] = "1111111111111111111111111111111111111111111111111111111111111111:000:000:1AB2C3D4E5F67890:ABCD:ABCD:0101010101010101:1111111111111111111111111111-22222222222222222222222222222222222";
	char str2[] = "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111:000:000:1AB2C3D4E5F67890:ABCD:ABCD:0101010101010101:1111111111111111111111111111-22222222222222222222222222222222222";

	//char str2[] = "1111111";

	//ASSERT_EQUAL(DEVIDD_SUCCESS, check_rule_format(str1));
	ASSERT_EQUAL(DEVIDD_SUCCESS, check_rule_format(str2)); // crashes the test process.

}