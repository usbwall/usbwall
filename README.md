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

The project use a configuration file, using a syntax similar to usual ldap.conf
files. The configuration file must be named *usbwall.cfg* and located in the
*/etc* folder.

A sample configuration file is described in this package. Report to this file in
order to configure properly usbwall.

## Contributors

Arthur d'Avray (arthur.davray@epita.fr)  
Sylvain Leroy  (sylvain@unmondelibre.fr)  
Damien Pradier (damien.pradier@epita.fr)

This project was also inspired from the previous libpam-devid project made by
Philippe Thierry (phil@reseau-libre.net).
