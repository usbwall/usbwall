set(CMAKE_C_COMPILER "gcc")

cmake_minimum_required(VERSION 2.6)
project(usbwall)

set(USBWALL_VERSION 0.1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out)

add_subdirectory(src)
add_subdirectory(doc)
