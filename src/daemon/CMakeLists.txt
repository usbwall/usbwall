
set(DAEMON_SRC
  core.c
  devusb.c
  devuser.c
  ipc_pam.c
  config.c
  main.c
  parser.c
  usb_access.c
  complete_id.c
  format_validity.c
  )

add_library(usbwalld_obj OBJECT ${DAEMON_SRC})

