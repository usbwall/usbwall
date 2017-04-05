# - Try to find the PAM libraries
# Once done this will define
#  PAM_FOUND - system has pam
#  PAM_INCLUDE_DIR - the pam include directory
#  PAM_LIBRARIES - pam libraries

find_path(PAM_INCLUDE_DIR
	  NAMES pam_modules.h
	  PATH_SUFFIXES security pam)
find_library(PAM_LIBRARY pam)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PAM DEFAULT_MSG
                                  PAM_LIBRARY PAM_INCLUDE_DIR)

set(PAM_LIBRARIES ${PAM_LIBRARY} )
set(PAM_INCLUDE_DIRS ${PAM_INCLUDE_DIR} )
