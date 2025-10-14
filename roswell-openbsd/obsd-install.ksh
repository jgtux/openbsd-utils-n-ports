#!/bin/ksh

# roswell version to build
# ROSWELL_VERSION="latest"

INCLUDEDIR="/usr/local/include"
LDDIR="/usr/local/lib"
CPPFLAGS="-I${INCLUDEDIR}"
LDFLAGS="-L${LDDIR}"
AUTOMAKE_VERSION="1.17"
AUTOCONF_VERSION="2.72"
export CPPFLAGS LDFLAGS AUTOMAKE_VERSION AUTOCONF_VERSION
# GMAKE_VERSION=""
# LIBCURL_HEADER="$INCLUDEDIR/curl/curl.h"

# UNSAFE FIX FOR MISSING LIBUTIL IN ROSWELL SBCL BUILDING
# ln -s /usr/lib/libutil.so.20.0 /usr/lib/libutil.so.18.0

if  [[ "$1" = "--user" ]] ; then
	  echo "WARNING: local install needs deps already installed"
	  INSTALL_PATH="${HOME}/.local"


    type automake-${AUTOMAKE_VERSION} 1> /dev/null || { echo "automake-${AUTOMAKE_VERSION} is needed."; exit 1; }
    type autoconf-${AUTOCONF_VERSION} 1> /dev/null || { echo "autoconf-${AUTOCONF_VERSION} is needed."; exit 1; }
    type curl 1> /dev/null || { echo "curl is needed."; exit 1; }
    type gmake 1> /dev/null || { echo "gmake is needed."; exit 1; }

    cd ./roswell
    ./bootstrap
    ./configure --prefix ${INSTALL_PATH}

    make clean install

    ${INSTALL_PATH}/bin/ros setup

else
	  (( $(id -u) != 0 )) && echo "user needs to be root." && exit 1
	  INSTALL_PATH="/usr/local"

    type automake-${AUTOMAKE_VERSION} 1> /dev/null || { echo "Installing automake..."; pkg_add automake-${AUTOMAKE_VERSION} 1> /dev/null || exit 1; }
    type autoconf-${AUTOCONF_VERSION} 1> /dev/null || { echo "Installing autoconf..."; pkg_add autoconf-${AUTOCONF_VERSION} 1> /dev/null || exit 1; }
    type curl 1> /dev/null || { echo "Installing curl..."; pkg_add curl 1> /dev/null || exit 1; }
    type gmake 1> /dev/null || { echo "Installing gmake..."; pkg_add gmake 1> /dev/null || exit 1; }


    cd ./roswell
    ./bootstrap
    ./configure --prefix ${INSTALL_PATH}

    make clean install

    ${INSTALL_PATH}/bin/ros setup
fi

