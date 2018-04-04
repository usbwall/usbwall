
set(TEST_SRC
  ctest.h
  usbwall_main_test.c
  daemon_parser_test.c
  daemon_format_validity_test.c
  daemon_main_test.c
  daemon_usb_access_test.c
  )

include_directories(../daemon/)
add_library(usbwall_test_obj OBJECT ${TEST_SRC})


