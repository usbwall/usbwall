set(PAM_SRC
  pam_usbwall.c
  )

add_library(pam_obj OBJECT ${PAM_SRC})
