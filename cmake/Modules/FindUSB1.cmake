# - Try to find the USB1 libraries
# Once done this will define
#  USB1_FOUND - system has libusb-1.0
#  USB1_INCLUDE_DIR - the libusb-1.0 include directory
#  USB1_LIBRARIES - libusb-1.0 libraries

find_path(USB1_INCLUDE_DIR
  NAMES libusb.h
  PATH_SUFFIXES include libusb-1.0)

find_library(USB1_LIBRARIES
  NAMES usb-1.0 usb
  PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(USB1 DEFAULT_MSG
                                  USB1_LIBRARIES USB1_INCLUDE_DIR)
