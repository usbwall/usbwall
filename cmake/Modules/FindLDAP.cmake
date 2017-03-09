# - Try to find the LDAP libraries
# Once done this will define
#  LDAP_FOUND - system has ldap
#  LDAP_INCLUDE_DIR - the ldap include directory
#  LDAP_LIBRARIES - ldap libraries

find_path(LDAP_INCLUDE_DIR
  NAMES ldap.h
  PATH_SUFFIXES include)

find_library(LDAP_LIBRARY
  NAMES ldap
  PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LDAP DEFAULT_MSG
                                  LDAP_LIBRARY LDAP_INCLUDE_DIR)

set(LDAP_LIBRARIES ${LDAP_LIBRARY} )
set(LDAP_INCLUDE_DIRS ${LDAP_INCLUDE_DIR} )
