[![License](https://img.shields.io/github/license/turanic/usbwall.svg)](https://github.com/Turanic/usbwall/blob/master/LICENSE)
[![Build Status](https://travis-ci.org/Turanic/usbwall.svg?branch=master)](https://travis-ci.org/Turanic/usbwall)

# USBWall

## Preamble

The goal of the usbwall project is to support dynamic USB device filtering
depending on a per-user centralized authorized device list. When a user log
in, a custom PAM module will alert a daemon, that will get back the list of
the authorized devices from LDAP.

## Dependencies

- libusb
- libldap
- libpam

## Build the project

A bootstrap script has been made to help you build the project. You can simply
start the build with *./bootstrap && make -j*. To build the project in debugging
mode, just add the *DEBUG* parameter to the bootstrap script.

The output binaries are located in the *out* directory.

## Configuration

### Daemon configuration
The project use a configuration file, using a syntax similar to usual ldap.conf
files. The configuration file must be named *usbwall.cfg* and located in the
*/etc* folder.

A sample configuration file is described in this package. Report to this file in
order to configure properly usbwall.

### PAM configuration
The *libpam_usbwall* library also need to be configured to be used by PAM. To do
this simply move the generated library to the pam modules folder. Example :
~~~sh
mv [path/to/usbwall]/out/libpam_usbwall.so /usr/lib/security/pam_usbwall.so
~~~

Finally, you need to configure PAM to load the module. To do that, simply
modified the corresponding pam configuration file in */etc/pam.d/* and add
this line :
~~~sh
session optional pam_usbwall.so debug
~~~

- *optional* means that if the module fails, the user is not disconnected from
the host.
- replace *pam_usbwall.so* by the name of the module you moved in the modules
folder.
- *debug* if an optional argument that specify if the debug mode is activated or
not.

## Contributors

Arthur d'Avray   (arthur.davray@epita.fr)  
Mathilde Beylier (mathilde.beylier@epita.fr)  
Sylvain Leroy    (sylvain@unmondelibre.fr)  
Damien Pradier   (damien.pradier@epita.fr)

This project was also inspired from the previous libpam-devid project made by
Philippe Thierry (phil@reseau-libre.net).
