include(CheckCCompilerFlag)

###########################
### Check for global CFLAGS compiler support
###########################

set(MY_COMPILE_WARNINGS
  "W"
  "Waggregate-return"
  "Wall"
  "Warray-bounds"
  "Wbad-function-cast"
  "Wcast-align"
  "Wcast-qual"
  "Wchar-subscripts"
  "Wcomment"
  "Wconversion"
  # "Wdeclaration-after-statement" # Macro "list_for_each" allocate a struct in code place...
  "Wdisabled-optimization"
  "Wdouble-promotion"
  "Wextra"
  "Wfloat-equal"
  "Wformat"
  "Wformat-nonliteral"
  "Wformat-security"
  "Wformat-y2k"
  "Wignored-qualifiers"
  "Wimplicit"
  "Wimplicit-function-declaration"
  "Wimplicit-int"
  "Winit-self"
  "Winline"
  "Winvalid-pch"
  "Wjump-misses-init"
  "Wlogical-op"
  "Wlong-long"
  "Wmain"
  "Wmissing-braces"
  "Wmissing-declarations"
  "Wmissing-field-initializers"
  "Wmissing-format-attribute"
  "Wmissing-include-dirs"
  "Wmissing-noreturn"
  "Wmissing-parameter-type"
  "Wmissing-prototypes"
  "Wmultichar"
  "Wnested-externs"
  "Wno-endif-labels"
  "Wno-multichar"
  "Wnonnull"
  "Wold-style-declaration"
  "Wold-style-definition"
  "Woverlength-strings"
  "Wpacked"
  "Wpacked-bitfield-compat"
  "Wpadded"
  "Wparentheses"
  "Wpointer-arith"
  "Wpointer-sign"
  "Wredundant-decls"
  "Wreturn-type"
  "Wsequence-point"
  "Wshadow"
  "Wsign-compare"
  "Wsign-conversion"
  # "Wstack-protector"
  "Wstrict-aliasing=3"
  "Wstrict-overflow=5"
  "Wstrict-prototypes"
  "Wsuggest-attribute=const"
  "Wsuggest-attribute=noreturn"
  "Wsuggest-attribute=pure"
  "Wswitch"
  #"Wswitch-default"
  "Wswitch-enum"
  "Wsync-nand"
  "Wtraditional-conversions"
  "Wtrampolines"
  "Wtrigraphs"
  "Wundef"
  "Wuninitialized"
  "Wunknown-pragmas"
  "Wunreachable-code"
  "Wunsafe-loop-optimizations"
  "Wunsuffixed-float-constants"
  "Wunused"
  "Wunused-function"
  "Wunused-label"
  "Wunused-local-typedefs"
  "Wunused-parameter"
  "Wunused-value"
  "Wunused-variable"
  "Wvariadic-macros"
  "Wvector-operation-performance"
  "Wvla"
  "Wvolatile-register-var"
  "Wwrite-strings"
  )

set(MY_COMPILE_OPTIONS
  "ffreestanding"
  "fno-builtin"
  "fno-common"
  "fstack-check"
  "fstack-protector-all"
  "fstack-protector"
  "fstrict-aliasing"
  "fPIC"
  "fPIE"
  "mno-gpopt"
  "pedantic"
  "std=c99"
  "Werror"
   )

# Set -Weverything if possible, else just try to use as many warnings as possibly known
check_c_compiler_flag(-Weverything HAS_WEVERYTHING)
if (HAS_WEVERYTHING)
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything")
else()
foreach(TESTED_OPTION ${MY_COMPILE_WARNINGS})
  # Replace - by _ because CMake variable can't have - in their name
  string(REPLACE "-" "_" MY_FLAG ${TESTED_OPTION})
  check_c_compiler_flag(-${TESTED_OPTION} HAS_${MY_FLAG})
  if (HAS_${MY_FLAG})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -${TESTED_OPTION}")
  endif()
endforeach()
endif()

foreach(TESTED_OPTION ${MY_COMPILE_OPTIONS})
  # Replace - by _ because CMake variable can't have - in their name
  string(REPLACE "-" "_" MY_FLAG ${TESTED_OPTION})
  check_c_compiler_flag(-${TESTED_OPTION} HAS_${MY_FLAG})
  if (HAS_${MY_FLAG})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -${TESTED_OPTION}")
  endif()
endforeach()

###
### Disabled compile flags because of code incompatibility (yet)
###

set(MY_DISABLED_COMPILE_WARNINGS
#  "Wno-discarded-qualifiers"		# Permit building with calls to basename() using __FILE__ which is a const value
  "Wno-padded"				# Used for skip strutures alignement requirement
  "Wno-vla"				# Skip check for variable length in arrays because C90 does not accept them
  "Wno-disabled-macro-expansion"	# Using sigaction create this warning
#  "Wno-format-nonliteral"		# Skip the check for functions calls to with non litteral format string
  "Wno-reserved-id-macro"		# Reserved macro exists in ldap.h on FreeBSD implementation
  "Wno-unused-function"			# PAM under NetBSD implementation has static function included from .h files...
  "Wno-char-subscripts"			# NetBSD FIXME
  "Wno-format-nonliteral"		# NetBSD FIXME
   )

foreach(TESTED_OPTION ${MY_DISABLED_COMPILE_WARNINGS})
  # Replace - by _ because CMake variable can't have - in their name
  string(REPLACE "-" "_" MY_FLAG ${TESTED_OPTION})
  check_c_compiler_flag(-${TESTED_OPTION} HAS_${MY_FLAG})
  if (HAS_${MY_FLAG})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -${TESTED_OPTION}")
  endif()
endforeach()


###
### Define mandatory CFLAGS
###

# _DEFAULT_SOURCE is used for function inet_aton()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEFAULT_SOURCE")

###
### Define OS Specific CFLAGS
###

if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD"
    OR ${CMAKE_SYSTEM_NAME} MATCHES "DragonFly")
# FreeBSD specific options
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLDAP_DEPRECATED")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WITH_DPRINTF")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WITH_GETLINE")
endif()

###########################
### Check for DEBUG mode CFLAGS compiler support
###########################

### Define mandatory DEBUG CFLAGS

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")

if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")

### Check sanitizer support
check_c_compiler_flag(-fsanitize=undefined HAS_SAN_UNDEFINED_CFLAG)
if (HAS_SAN_UNDEFINED_CFLAG)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=undefined")
endif()
check_c_compiler_flag(-fsanitize=address HAS_SAN_ADDR_CFLAG)
if (has_san_addr_cflag)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
endif()
check_c_compiler_flag(-fsanitize=leak HAS_SAN_LEAK_CFLAG)
if (has_san_leak_cflag)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=leak")
endif()
check_c_compiler_flag(-fno-omit-frame-pointer HAS_OMIT_FP_CFLAG)
if (HAS_OMIT_FP_CFLAG)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer")
endif()

endif()

###########################
### Check for RELEASE CFLAGS compiler support
###########################

### Define mandatory RELEASE CFLAGS

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

###########################
### Add src to the include path
###########################

include_directories(.)

###########################
### Find external packages (include dir and lib dir)
###########################

find_package(USB1    REQUIRED)
find_package(LDAP    REQUIRED)
find_package(Threads REQUIRED)
find_package(PAM REQUIRED)

message("-- USB1 include directory : ${USB1_INCLUDE_DIRS}")
message("-- LDAP include directory : ${LDAP_INCLUDE_DIRS}")
message("-- PAM include directory : ${PAM_INCLUDE_DIRS}")

message("-- USB1 library directory : ${USB1_LIBRARIES}")
message("-- LDAP library directory : ${LDAP_LIBRARIES}")
message("-- PAM library directory : ${PAM_LIBRARIES}")

###########################
### continue to subdirectories
###########################

add_subdirectory(daemon)
add_subdirectory(devidd_ctl)
add_subdirectory(misc)
add_subdirectory(pam)

###########################
### Construction of usbwalld
###########################

if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD"
    OR ${CMAKE_SYSTEM_NAME} MATCHES "DragonFly")
# FreeBSD local lib directory
   link_directories(/usr/local/lib)
endif()

link_directories()

###########################
### Construction of usbwalld
###########################

add_executable(usbwalld
  $<TARGET_OBJECTS:misc_obj> $<TARGET_OBJECTS:usbwalld_obj>)
add_executable(deviddctl
  $<TARGET_OBJECTS:devidd_ctl_obj>)
target_link_libraries(usbwalld ${USB1_LIBRARIES} ${LDAP_LIBRARIES})
# if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD"
#     OR ${CMAKE_SYSTEM_NAME} MATCHES "DragonFly")
# # FreeBSD libusb name
#    target_link_libraries(usbwalld usb)
# else()
#    target_link_libraries(usbwalld usb-1.0)
# endif()
target_link_libraries(usbwalld ${CMAKE_THREAD_LIBS_INIT})

###########################
### Construction of pam_usbwall
###########################

add_library(pam_usbwall SHARED $<TARGET_OBJECTS:pam_obj>)
target_link_libraries(pam_usbwall pam)
