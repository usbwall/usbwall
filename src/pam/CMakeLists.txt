set(PAM_SRC pam_usbwall.c)

add_library(pam_usbwall SHARED ${PAM_SRC})
target_link_libraries(pam_usbwall pam)
