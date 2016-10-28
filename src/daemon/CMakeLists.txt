find_package(USB1 REQUIRED)
find_package(LDAP REQUIRED)
set(DAEMON_SRC
  devusb.c
  devuser.c
  socket.c
  ldap_config.c
  main.c
  )

add_executable(usbwalld ${DAEMON_SRC})
target_link_libraries(usbwalld ldap)
target_link_libraries(usbwalld usb-1.0)
