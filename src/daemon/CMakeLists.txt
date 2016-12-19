
set(DAEMON_SRC
  core.c
  devusb.c
  devuser.c
  ipc_pam.c
  ldap_config.c
  main.c
  parser.c
  usb_access.c
  )

add_library(usbwalld_obj OBJECT ${DAEMON_SRC})

