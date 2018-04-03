\brief This file explain how to install USBWALL

# INSTALL

## Dependencies using distribution packages

Packages names may differ from those given below depending on the version of your distribution.

### Linux/Debian

#### Compilation dependencies

~~~sh
sudo apt install cmake libusb-1.0-0-dev libldap2-dev libpam0g-dev
~~~

#### Runtime dependencies

~~~sh
sudo apt install libusb-1.0-0
~~~

### FreeBSD and DragonFly BSD

#### Compilation dependencies

~~~sh
sudo pkg install cmake openldap-client
~~~

#### Runtime dependencies

~~~sh
sudo pkg install openldap-client
~~~

### NetBSD

#### Compilation dependencies

~~~sh
sudo pkgin install cmake libusb1
~~~

#### Runtime dependencies

### OpenBSD

usbwall is not yet available on OpenBSD because it does not make use of PAM and usbwall cannot work without PAM yet.

## Dependencies from source

usbwall can be configured either with gcc or clang as compiler. Any other compiler might be used, but it has not been tested. Feedback is welcomed if you try using another compiler.
