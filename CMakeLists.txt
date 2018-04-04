cmake_minimum_required(VERSION 2.6)
project(usbwall C)

set(USB1_INCLUDE_DIR "/usr/include/libusb-1.0/")
set(USB1_LIBRARIES "libusb.h")

set(LDAP_INCLUDE_DIR "/usr/include/")
set(LDAP_LIBRARY "ldap.h")

set(PAM_INCLUDE_DIR "/usr/include/security/")
set(PAM_LIBRARY "pam_modules.h")

#add_executable(myexe USB1::USB1)

set(USBWALL_VERSION 0.4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules/)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out)

include_directories(src/include)
add_subdirectory(src)
add_subdirectory(doc)
