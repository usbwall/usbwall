
set(TEST_SRC
  ctest.h
  daemon_main_test.c
  daemon_parser_test.c
  )

include_directories(../daemon/)
add_library(usbwall_test_obj OBJECT ${TEST_SRC})


