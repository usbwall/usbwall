#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libusb.h>

#include "config.h"
#include "backend_ldap.h"
#include "backend_file.h"
#include "devuser.h"
#include "usb_access.h"
#include "complete_id.h"
#include "../misc/error_handler.h"
#include "format_validity.h"
#include "server.h"
#include "devusb.h"

/**
 * \brief maximum possible size of a device serial id.
 */
#define SERIAL_MAX_SIZE 64

#if !(defined(__FreeBSD__) || defined(__DragonFly__))
/**
 * \brief interval time in second where usb plugging event are checked
 */
#define HOTPLUG_CHECK_INTV 3
#endif

/**
 * \brief Maximum port numbers for a device. The USB3 standard say 7
 * is the current max depth. It should never be set above 255!
 */
#define MAX_PORTS_NB 7

/* Globals */
#if !(defined(__FreeBSD__) || defined(__DragonFly__))
/**
 * \brief devusb internal global used to register/deregister the hotplug
 * callback.
 */
static libusb_hotplug_callback_handle g_callback_handler;
/**
 * \brief devusb internal global to define if the wait_for_hotplug thread
 * need to continue its checks or exit (1 to continue, 0 for exit).
 */
static int g_wait_for_hotplugs;
/**
 * \brief devusb internal global used to reference the wait_for_hotplug thread.
 * It is started in the module init and exited in the module exit.
 */
static pthread_t g_hotplug_thread;
#endif

/**
 * \brief devusb internal global used to reference the server_core thread,
 * which launch the devidd_ctl server.
 * It is started in the module init and exited in the module exit.
 */
static pthread_t g_devidd_ctl_thread;

/* ****** */

/**
 * \brief devusb internal function that will extract the serial from a device.
 *
 * \param device  the libusb object associated with the device
 * \param usb_infos  the libusb descriptor object associated with the device
 *
 * \return the device serial id. NULL if an error occured.
 *
 * The function will access to the device string descriptor and extract the
 * serial.
 */
static char *device_serial_get(struct libusb_device *device,
                               struct libusb_device_descriptor *usb_infos)
{
  assert(device && usb_infos);

  libusb_device_handle *udev = NULL;
  int rcode = libusb_open(device, &udev);
  if (rcode != LIBUSB_SUCCESS)
  {
    syslog(LOG_WARNING, "Failed to open device : %s", libusb_strerror(rcode));

    return NULL;
  }

  char *serial = NULL;
  if (usb_infos->iSerialNumber) // the device does have an unique identifier
  {
    unsigned char tmp_dev_serial[SERIAL_MAX_SIZE] = { '\0' };
    const int ret = libusb_get_string_descriptor_ascii(udev,
                                                       usb_infos->iSerialNumber,
                                                       tmp_dev_serial,
                                                       SERIAL_MAX_SIZE);
    if (ret > 0) // ret < 0 is a LIBUSB_ERROR
    {
      /**
       * \todo
       * Some serials extracted from descriptors may not be unique, and
       * some are just useless garbage. We need to find a way to identify those
       * bad serial ids.
       */

      if (check_serial_format((char *) tmp_dev_serial) != DEVIDD_SUCCESS)
      {
        return NULL;
      }

      const size_t len = (size_t)ret;
      serial = malloc(len + 1);
      if (serial)
        memcpy(serial, tmp_dev_serial, len + 1);
    }
    else
      syslog(LOG_WARNING,
             "Unable to extract usb string descriptor : %s",
             libusb_strerror(ret));
  }
  libusb_close(udev);

  return serial;
}

/**
 * \brief devusb internal function that will extract ports from a device.
 *
 * \param device  the libusb object associated with the device
 * \param ports  pointer to the array of ports. The pointed pointer
 * must be NULL! The function will be in charge of allocating the array.
 *
 * \return the number of device ports. 0 is returned in case of error.
 */
static uint8_t device_ports_get(struct libusb_device *device, uint8_t **ports)
{
  assert(device && ports);

  uint8_t result[MAX_PORTS_NB];
  int res = libusb_get_port_numbers(device, result, MAX_PORTS_NB);
  if (res < 0 || res > 7)
  {
      syslog(LOG_WARNING, "Cannot get ports of device!");

      return 0;
  }

  uint8_t ports_nb = (uint8_t)res;

  *ports = malloc(sizeof (uint8_t) * ports_nb);
  if (!(*ports))
    return 0;

  memcpy(*ports, result, ports_nb);

  return ports_nb;
}


/**
 * \brief devusb internal function that will return the hostname
 * \return the hostname string. NULL if an error occured
 */
static char *get_machine_name(void)
{
  char *name = NULL;

  name = malloc(BUF_MACH);

  if (name == NULL)
  {
    return NULL;
  }

  if (gethostname(name, BUF_MACH) != 0)
  {
    return NULL;
  }

  return name;
}


/**
 * \brief devusb internal function that fill all the devusb fields except
 * complete_id
 *
 * \param result  the devusb struct to fill
 * \param usb_infos the descriptor from which we extract the serial, vendor,
 *        product, bcd_device of the device
 * \param device the device from which we extract the serial, bus and port
 *
 * \return error
 *
 * The device will extract all the information from the device
 * and store it in a devusb structure.
 */
static int32_t fill_simple_id(struct devusb **result,
                              struct libusb_device_descriptor *usb_infos,
                              struct libusb_device *device)
{
  if (!(*result) || !usb_infos || !device)
    return DEVIDD_ERR_OTHER;
  (*result)->last_co = time(NULL);
  (*result)->machine = get_machine_name();
  if ((*result)->machine == NULL)
    return DEVIDD_ERR_OTHER;
  (*result)->serial = device_serial_get(device, usb_infos);
  if ((*result)->serial == NULL)
    return DEVIDD_ERR_OTHER;
  (*result)->vendor = usb_infos->idVendor;
  (*result)->product = usb_infos->idProduct;
  (*result)->bcd_device = usb_infos->bcdDevice;
  (*result)->bus = libusb_get_bus_number(device);
  (*result)->ports_nb = device_ports_get(device, &(*result)->ports);

  return DEVIDD_SUCCESS;
}

/**
 * \brief devusb internal function that convert a libusb device object to a
 * devusb structure
 *
 * \param device  the structure to convert
 *
 * \return the converted devusb structure. NULL if an error occured
 *
 * The device will extract all the information from the device
 * and store it in a devusb structure.
 */
static struct devusb *device_to_devusb(struct libusb_device *device)
{
  char **array_id = NULL;
  char *str_complete_id = NULL;
  int32_t err = DEVIDD_SUCCESS;
  int32_t err_array_id = 0;
  int32_t err_get_desc = 0;
  struct libusb_device_descriptor usb_infos;

  assert(device);

  struct devusb *result = calloc(1, sizeof(struct devusb));
  if (!result)
    return NULL;

  /* Allocate str_complete_id with the size of each of its id */
  str_complete_id = malloc(LEN_STR * (NB_FIELD_COMPLETE_ID + 1));
  /* Allocate array_id and each of its cells */
  err_array_id = allocate_array_id(&array_id);
  /* Extract usb descriptor from device */
  err_get_desc = libusb_get_device_descriptor(device, &usb_infos);

  /* If one of these operations failed, free ressources and return */
  if (!str_complete_id || (err_array_id != DEVIDD_SUCCESS)
      || (err_get_desc != LIBUSB_SUCCESS))
  {
    free(result);
    free(str_complete_id);
    free_array_id(&array_id);
    return NULL;
  }

  /* Fill all the fields of devusb, exept complete_id */
  if (fill_simple_id(&result, &usb_infos, device) != DEVIDD_SUCCESS)
    err = DEVIDD_ERR_OTHER;

  /* Fill devusb field complete_id, by concatenation of all the others fields */
  result->complete_id = fill_complete_id(array_id, result);
  if (!result->complete_id)
    err = DEVIDD_ERR_OTHER;

  /* Free the elements that are no more needed */
  free_array_id(&array_id);
  free(str_complete_id);

  /* As all ressources were freed, handle errors */
  if (err != DEVIDD_SUCCESS)
  {
    free(result);
    return NULL;
  }

  assert(result != NULL && "result is null");

  return result;
}


#if !(defined(__FreeBSD__) || defined(__DragonFly__))
/**
 * \brief devusb internal function that check if the device is accessible by the
 * current user and
 *
 * \param ctx  unused
 * \param dev  the libusb device object associated with the plugged device
 * \param event  unused
 * \param user_data  unused
 *
 * \return non zero value if an error occured
 *
 * The callback is called by libusb when a usb device is plugged (see libusb
 * documentation for more informations). It will update the device
 * accessibility.
 */
static int hotplug_callback(struct libusb_context *ctx __attribute__((unused)),
                            struct libusb_device *dev,
                            libusb_hotplug_event event __attribute__((unused)),
                            void *user_data __attribute__((unused)))
{
  struct devusb *device = device_to_devusb(dev);
  if (!device)
  {
    syslog(LOG_WARNING, "Corrupted hotplug devusb");

    return 1;
  }
  syslog(LOG_DEBUG, "New device detected : %s\n", device->complete_id);

  if (!device_is_valid(device))
  {
    syslog(LOG_WARNING, "Invalid device plugged.");
    free(device);

    return 1;
  }

  struct linked_list *users = usernames_get();
  int authorized_status = 0;

  const struct config *cfg = configuration_get();
  /**
   * \todo FIXME for having duplicated code
   */
  if (!strcmp(cfg->backend, "file"))
    {
      list_for_each(user_node_ptr, users)
	if (check_devid(device->complete_id, uw_ldap_devids_get(user_node_ptr->data))
	    == DEVIDD_SUCCESS)
	  {
	    authorized_status = 1;
	    break;
	  }
      list_destroy(users, 1);
    }

  if (!strcmp(cfg->backend, "ldap"))
      {
	  list_for_each(user_node_ptr, users)
	      if (check_devid(device->complete_id, uw_ldap_devids_get(user_node_ptr->data))
		  == DEVIDD_SUCCESS)
		  {
		      authorized_status = 1;
		      break;
		  }
	  list_destroy(users, 1);
      }

  int rcode = 0;
  if ((rcode = update_device_access(device, authorized_status)))
    syslog(LOG_WARNING, "Device access update error");

  free(device);

  return !!rcode;
}

/**
 * \brief devusb internal function that will check every HOTPLUG_CHECK_INTV
 * seconds if an usb has been plugged.
 *
 * \param arg  unused
 *
 * \return NULL
 *
 * The function must be launched in a separated thread, where it will check
 * every HOTPLUG_CHECK_INTV seconds for an usb event. If an event come up, it
 * will make libusb call the hotplug callback.
 */
__attribute__((noreturn))
static void *wait_for_hotplug(void *arg __attribute__((unused)))
{
  struct timeval tv = { 0, 0 };
  while (g_wait_for_hotplugs)
  {
    libusb_handle_events_timeout_completed(NULL, &tv, NULL);
    sleep(HOTPLUG_CHECK_INTV);
  }

  pthread_exit(NULL);
}
#endif

int init_devusb(void)
{
  assert(configuration_get());

  syslog(LOG_INFO, "Initializing USB bindings");

  if (libusb_init(NULL) != LIBUSB_SUCCESS)
  {
    syslog(LOG_ERR, "Init error - libusb can not be initiated");

    return 1;
  }

#if !(defined(__FreeBSD__) || defined(__DragonFly__))
  if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
  {
    syslog(LOG_ERR, "Init error - your system does not support hotplug");

    return 1;
  }

  /**
   * \todo
   * FIXME ????
   */
  libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);

  g_wait_for_hotplugs = 1;
  int rcode =
    libusb_hotplug_register_callback(NULL,
                                     LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
                                     /**
                                      * \todo
                                      * FIXME: LIBUSB_HOTPLUG_NO_FLAGS should be
                                      * used instead of 0, but the enum value is
                                      * not recognized on the all OS
                                      **/
                                     (libusb_hotplug_flag)0,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     LIBUSB_HOTPLUG_MATCH_ANY,
                                     hotplug_callback,
                                     NULL,
                                     &g_callback_handler);

  if (rcode != LIBUSB_SUCCESS)
  {
    libusb_exit(NULL);
    syslog(LOG_DEBUG, "Hotplug callback failed to register");

    return 1;
  }

  set_usb_default_access(0);

  /* start the wait_for_hotplug thread */
  pthread_create(&g_hotplug_thread, NULL, wait_for_hotplug, NULL);

  /* FIXME: return code not checked */
#endif

  /* Start the server_core thread that launch the devidd_ctl server */
  pthread_create(&g_devidd_ctl_thread, NULL, &serv_core, NULL);
  syslog(LOG_DEBUG, "Devusb initialized successfully");

  return 0;
}

struct linked_list *devices_get(void)
{
  struct linked_list *devusb_list = list_make();
  if (!devusb_list)
    return NULL;

  libusb_device **device_list = NULL;
  const ssize_t device_list_size = libusb_get_device_list(NULL, &device_list);
  if (device_list_size < 0)
    syslog(LOG_WARNING,
           "Failed to retrieve device list : %s",
           libusb_strerror((enum libusb_error)device_list_size));

  /* iterate over all devices and extract needed infos */
  for (ssize_t i = 0; i < device_list_size; ++i)
  {
    struct devusb *element = device_to_devusb(device_list[i]);
    if (!element)
    {
      syslog(LOG_DEBUG, "Corrupted devusb on iteration %zd", i);
      break;
    }
    list_add_back(devusb_list, element);
  }
  libusb_free_device_list(device_list, 1);

  return devusb_list;
}

void close_devusb(void)
{
#if !(defined(__FreeBSD__) || defined(__DragonFly__))
  g_wait_for_hotplugs = 0;
  pthread_join(g_hotplug_thread, NULL);
  libusb_hotplug_deregister_callback(NULL, g_callback_handler);
#endif

  libusb_exit(NULL);

  syslog(LOG_DEBUG, "Devusb closed");

  return;
}
