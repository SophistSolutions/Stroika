#!/bin/bash

GCC_VERSION=4.9.3
INSTALL_DIR=$HOME/gcc-$GCC_VERSION

#unset this to take longer, but be safer about build
DISABLE_BOOTSTRAP_FLAG=--disable-bootstrap

PARALLEL_MAKE_FLAG=-j2

MULTILIB_CONFIGURE_FLAGS=--disable-multilib
LANGUAGES_CONFIGURE_FLAGS=--enable-languages=c,c++
EXTRA_CONFIGURE_FLAGS=

BUILDDIR=`realpath ./BUILD-GCC-$GCC_VERSION`

BLDOUT=$BUILDDIR/BLDOUT.txt



echo "Building GCC $GCC_VERSION to $BUILDDIR with installation to $INSTALL_DIR, and BLDOUT=$BLDOUT..."


rm -rf $BUILDDIR
mkdir -p $BUILDDIR
cd $BUILDDIR


(wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz 2>&1) >> $BLDOUT
echo "tar xf gcc-$GCC_VERSION.tar.gz" >> $BLDOUT
(tar xf gcc-$GCC_VERSION.tar.gz 2>&1) >> $BLDOUT

cd gcc-$GCC_VERSION
echo "./contrib/download_prerequisites" >> $BLDOUT
(./contrib/download_prerequisites 2>&1) >> $BLDOUT

cd ..
mkdir objdir
cd objdir
echo "$PWD/../gcc-$GCC_VERSION/configure --prefix=$INSTALL_DIR $LANGUAGES_CONFIGURE_FLAGS $MULTILIB_CONFIGURE_FLAGS $DISABLE_BOOTSTRAP_FLAG $EXTRA_CONFIGURE_FLAGS" >> $BLDOUT
($PWD/../gcc-$GCC_VERSION/configure --prefix=$INSTALL_DIR $LANGUAGES_CONFIGURE_FLAGS $MULTILIB_CONFIGURE_FLAGS $DISABLE_BOOTSTRAP_FLAG $EXTRA_CONFIGURE_FLAGS 2>&1) >> $BLDOUT

echo "make $PARALLEL_MAKE_FLAG" >> $BLDOUT
(make $PARALLEL_MAKE_FLAG 2>&1) >> $BLDOUT

echo "make install" >> $BLDOUT
(make install 2>&1) >> $BLDOUT

