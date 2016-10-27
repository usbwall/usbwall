set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -pedantic -D_XOPEN_SOURCE=800")
set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address -fsanitize=undefined -g3 -Werror -Werror=format=0")
set(CMAKE_C_FLAGS_RELEASE "-O2")

add_subdirectory(pam)
add_subdirectory(daemon)
