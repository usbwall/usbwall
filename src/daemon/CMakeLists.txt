
set(DAEMON_SRC
  core.c
  devusb.c
  devuser.c
  ipc_pam.c
  config.c
  uw_pid.c
  uw_signal.c
  main.c
  parser.c
  usb_access.c
  complete_id.c
  format_validity.c
  server.c
  )

include_directories(${USB1_INCLUDE_DIRS})
include_directories(${LDAP_INCLUDE_DIRS})

add_library(usbwalld_obj OBJECT ${DAEMON_SRC})
