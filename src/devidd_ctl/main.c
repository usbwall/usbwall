#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>

#include "client.h"
#include "../misc/error_handler.h"
#include "main.h"

int usage(void)
{
  printf("Usage: deviddctl [OPTION]... [HOST]\n");
  printf("  If HOST is not defined, localhost will be used.\n");

  return DEVIDD_SUCCESS;
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{

  usage();

  return DEVIDD_SUCCESS;
}
