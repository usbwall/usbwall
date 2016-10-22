set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "-g3")
set(CMAKE_C_FLAGS_RELEASE "-O2 -Werror")

add_subdirectory(pam)
add_subdirectory(daemon)
