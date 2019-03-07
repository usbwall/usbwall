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

### Linux/OpenSuse

#### Compilation dependencies

~~~sh
sudo zypper install cmake libusb-1_0-devel openldap2-devel pam-devel doxygen graphviz
~~~

#### Runtime dependencies

~~~sh
sudo apt install libusb-1_0-0
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

#### Compilation dependencies

~~~sh
sudo pkg_add cmake
~~~

#### Runtime dependencies

~~~sh
sudo pkg_add openldap-client openpam-20141014
~~~

~~~sh
Libusb on OpenBSD 6.1 : Sudo pkg_add libusb1-1.0.20p0
~~~

~~~sh
Libusb on OpenBSD 6.2 : Sudo pkg_add libusb1-1.0.21
~~~

## Dependencies from source

usbwall can be configured either with gcc or clang as compiler. Any other compiler might be used, but it has not been tested. Feedback is welcomed if you try using another compiler.
