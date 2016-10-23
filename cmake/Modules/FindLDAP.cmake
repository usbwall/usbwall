# - Try to find the LDAP libraries
# Once done this will define
#  LDAP_FOUND - system has ldap
#  LDAP_INCLUDE_DIR - the ldap include directory
#  LDAP_LIBRARIES - ldap libraries

find_path(LDAP_INCLUDE_DIR
  NAMES ldap.h
  PATH_SUFFIXES include)

find_library(LDAP_LIBRARIES
  NAMES ldap
  PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LDAP DEFAULT_MSG
                                  LDAP_LIBRARIES LDAP_INCLUDE_DIR)
