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

### Disabled compile flags because of code incompatibility (yet)

# Used for skip strutures alignement requirement
check_c_compiler_flag(-Wno-padded HAS_PADDED_CFLAG)
if (HAS_PADDED_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-padded")
endif()

# Skip check for variable length in arrays
check_c_compiler_flag(-Wno-vla HAS_NOVLA_CFLAG)
if (HAS_NOVLA_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-vla")
endif()

# Using sigaction create this warning
check_c_compiler_flag(-Wno-disabled-macro-expansion HAS_NOMACEXP_CFLAG)
if (HAS_NOMACEXP_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-disabled-macro-expansion")
endif()

# Skip the check for functions calls to with non litteral format string
check_c_compiler_flag(-Wno-format-nonliteral HAS_NOFORMAT_NONLIT_CFLAG)
if (HAS_NOFORMAT_NONLIT_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-nonliteral")
endif()

### Define mandatory CFLAGS

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE=800 -fPIC")

if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
#FreeBSD specific code
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FREEBSD")
endif()

###########################
### Check for DEBUG mode CFLAGS compiler support
###########################

### Define mandatory DEBUG CFLAGS

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -Werror")

### Check sanitizer support

check_c_compiler_flag(-fsanitize=undefined HAS_SAN_UNDEFINED_CFLAG)
if (HAS_SAN_UNDEFINED_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
endif()
check_c_compiler_flag(-fsanitize=address HAS_SAN_ADDR_CFLAG)
if (has_san_addr_cflag)
    set(cmake_c_flags "${cmake_c_flags} -fsanitize=address")
endif()
check_c_compiler_flag(-fsanitize=leak HAS_SAN_LEAK_CFLAG)
if (has_san_leak_cflag)
    set(cmake_c_flags "${cmake_c_flags} -fsanitize=leak")
endif()
check_c_compiler_flag(-fno-omit-frame-pointer HAS_OMIT_FP_CFLAG)
if (HAS_OMIT_FP_CFLAG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer")
endif()

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
