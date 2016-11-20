find_package(USB1    REQUIRED)
find_package(LDAP    REQUIRED)
find_package(Threads REQUIRED)
set(DAEMON_SRC
  core.c
  devusb.c
  devuser.c
  ipc_pam.c
  ldap_config.c
  main.c
  usb_access.c
  )

add_executable(usbwalld ${DAEMON_SRC})
target_link_libraries(usbwalld ldap)
target_link_libraries(usbwalld usb-1.0)
target_link_libraries(usbwalld ${CMAKE_THREAD_LIBS_INIT})
