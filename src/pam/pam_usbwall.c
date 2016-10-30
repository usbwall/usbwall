#include <security/pam_ext.h>
#include <security/pam_modules.h>

#include <stdio.h>

static int  fetch_debug(int argc, const char **argv);
static void notify_daemon(void);

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh __attribute__((unused)),
                                   int flags          __attribute__((unused)),
                                   int argc,
                                   const char **argv)
{
  fetch_debug(argc, argv);
  notify_daemon();

  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh __attribute__((unused)),
                                    int flags          __attribute__((unused)),
                                    int argc,
                                    const char **argv)
{
  fetch_debug(argc, argv);
  return PAM_SUCCESS;
}

/************************************
 * Static functions implementations *
 ************************************/

static int fetch_debug(int argc, const char **argv)
{
  for (int i = 0; i < argc; ++i)
    puts(argv[i]);

  return 0;
}

static void notify_daemon(void)
{
  puts("ouhlala, je notifie le daemon !");
}
