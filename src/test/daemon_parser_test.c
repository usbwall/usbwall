#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"

#include "../daemon/parser.c"
#include "../daemon/config.h"

/**
 * \Parser module unit tests 
 */

CTEST(parser, skip_comments) {
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
    ASSERT_STR("c#ke", str3); // Fails
    ASSERT_STR("", str4);
}

CTEST(parser, line_is_empty) {
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

	//char str3_line[] = "non";

	ASSERT_STR("ldap://usbwall.org:389/", parse_line(str1_line, str1_field_OK));
	ASSERT_STR(NULL, parse_line(str1_line, str1_field_KO));
	ASSERT_STR(NULL, parse_line(str2_line, str2_field_OK));
	ASSERT_STR(NULL, parse_line("", ""));
	//ASSERT_STR(NULL, parse_line(str3_line, "")); // Fails
}

CTEST(parser, store_cfg_value) { // Fails

	struct config *config = malloc(sizeof(struct config));

  	if (!config){
  		CTEST_LOG("Couldn't allocate config struct.");
  		ASSERT_FAIL();
  	}

  	char str1_l[] = "uri ldap://usbwall.org:389/";
	char str1_f[] = "uri";
	store_cfg_value(config, "uri", parse_line(str1_l, str1_f));
	ASSERT_STR("ldap://usbwall.org:389/", config->uri);
}


CTEST_SKIP(parser, parse_config) { // fails
	char conf1_f[] = "test_conf1_ok.cfg";
	char conf2_f[] = "test_conf2_ko.cfg";

	FILE *f1p;
	FILE *f2p;

	f1p = fopen(conf1_f, "r");
	f2p = fopen(conf2_f, "r");

	struct config *cfg1 = parse_config(f1p);
	struct config *cfg2 = parse_config(f2p);

	CTEST_LOG("cfg1: %s", cfg1->uri);
	CTEST_LOG("cfg2: %s", cfg2->uri);

	ASSERT_FAIL();

}


