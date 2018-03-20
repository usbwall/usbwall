#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"

#include "../daemon/parser.c"

/**
 * \Parser module unit tests 
 */

CTEST_SKIP(parser, skip_comments) {
	char str1[] = "This Is a valid Line.";
	char str2[] = "#This Is a comment.";
	char str3[] = "c#ke";
	char str4[] = "";

	skip_comments(str1);
	skip_comments(str2);
	skip_comments(str3);
	skip_comments(str4);

    ASSERT_STR("This Is a valid Line.", str1);
    ASSERT_STR("\0", str2);
    ASSERT_STR("c#ke", str3);
    ASSERT_STR("", str4);
}

CTEST_SKIP(parser, line_is_empty) {
	char str1[] = "     "; // spaces
	char str2[] = "      not	empty"; // spaces, tab and words
	char str3[] = "		"; // tabs

	ASSERT_EQUAL(1, line_is_empty(str1));
	ASSERT_EQUAL(0, line_is_empty(str2));
	ASSERT_EQUAL(1, line_is_empty(str3));
}

CTEST(parser, parse_line) {
	char str1_line[] = "uri ldap://usbwall.org:389/";
	char str1_field_OK[] = "uri";
	char str1_field_KO[] = "url";

	char str2_line[] = "uri ldap usbwall.org 389";
	char str2_field_OK[] = "ldap usbwall.org 389";

	ASSERT_STR("ldap://usbwall.org:389/", parse_line(str1_line, str1_field_OK));
	ASSERT_STR(NULL, parse_line(str1_line, str1_field_KO));
	ASSERT_STR(NULL, parse_line(str2_line, str2_field_OK));
	ASSERT_STR(NULL, parse_line("", ""));
}


