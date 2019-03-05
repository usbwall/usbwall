
set(TEST_SRC
  ctest.h
  usbwall_main_test.c
  daemon_parser_test.c
  daemon_format_validity_test.c
  daemon_uw_pid_test.c
  daemon_usb_access_test.c
  ../daemon/format_validity.c
  ../daemon/uw_pid.c
  )

add_library(usbwall_test_obj OBJECT EXCLUDE_FROM_ALL ${TEST_SRC})

###########################
### Construction of usbwall_test
###########################
add_executable(usbwall_test EXCLUDE_FROM_ALL
  $<TARGET_OBJECTS:usbwall_test_obj>)
