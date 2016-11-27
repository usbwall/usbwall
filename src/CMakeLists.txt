include(CheckCCompilerFlag)

###########################
### Check for global CFLAGS compiler support
###########################

check_c_compiler_flag(-Weverything HAS_WEVERYTHING_CFLAG)
if (HAS_WEVERYTHING_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything")
endif()

check_c_compiler_flag(-std=c99 HAS_STDC99_CFLAG)
if (HAS_STDC99_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

check_c_compiler_flag(-Wall HAS_WALL_CFLAG)
if (HAS_WALL_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

check_c_compiler_flag(-Wextra HAS_WEXTRA_CFLAG)
if (HAS_WEXTRA_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
endif()

check_c_compiler_flag(-pedantic HAS_PEDANTIC_CFLAG)
if (HAS_PEDANTIC_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
endif()

check_c_compiler_flag(-Wunused-macros HAS_UNUSED_MACROS_CFLAG)
if (HAS_UNUSED_MACROS_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-macros")
endif()

### Disabled compile flags because of code incompatibility (yet)

# Used for skip strutures alignement requirement
check_c_compiler_flag(-Wno-padded HAS_PADDED_CFLAG)
if (HAS_PADDED_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-padded")
endif()

### Define mandatory CFLAGS

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE=800")

if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    # FreeBSD specific code
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FREEBSD")
endif()

###########################
### Check for DEBUG mode CFLAGS compiler support
###########################

check_c_compiler_flag(-fsanitize=address HAS_SAN_ADDR_CFLAG)
if (HAS_SAN_ADDR_CFLAG)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
endif()

check_c_compiler_flag(-fsanitize=undefined HAS_SAN_UNDEF_CFLAG)
if (HAS_SAN_UNDEF_CFLAG)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=undefined")
endif()

### Define mandatory DEBUG CFLAGS

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3")

###########################
### Check for RELEASE CFLAGS compiler support
###########################

### Define mandatory RELEASE CFLAGS

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

###########################
### Add src to the include path
###########################

include_directories(.)

###########################
### continue to subdirectories
###########################

add_subdirectory(daemon)
add_subdirectory(misc)
add_subdirectory(pam)

###########################
### Construction of usbwalld
###########################
find_package(USB1    REQUIRED)
find_package(LDAP    REQUIRED)
find_package(Threads REQUIRED)
add_executable(usbwalld
  $<TARGET_OBJECTS:misc_obj> $<TARGET_OBJECTS:usbwalld_obj>)
target_link_libraries(usbwalld ldap)
target_link_libraries(usbwalld usb-1.0)
target_link_libraries(usbwalld ${CMAKE_THREAD_LIBS_INIT})

###########################
### Construction of pam_usbwall
###########################

find_package(PAM REQUIRED)
add_library(pam_usbwall SHARED $<TARGET_OBJECTS:pam_obj>)
target_link_libraries(pam_usbwall pam)
