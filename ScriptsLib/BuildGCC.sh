#!/bin/bash

#GCC_VERSION=5.4.0
#GCC_VERSION=6.4.0
#GCC_VERSION=7.2.0
GCC_VERSION="${GCC_VERSION:=7.2.0}"
INSTALL_DIR="${INSTALL_DIR:=/private-compiler-builds/gcc-$GCC_VERSION}"

#unset this to take longer, but be safer about build
DISABLE_BOOTSTRAP_FLAG=--disable-bootstrap

PARALLEL_MAKE_FLAG=-j2

MULTILIB_CONFIGURE_FLAGS=--disable-multilib
LANGUAGES_CONFIGURE_FLAGS=--enable-languages=c,c++
EXTRA_CONFIGURE_FLAGS=

#sometimes realpath (old systems fails if dir doest exist, so create and delete)
mkdir -p ./BUILD-GCC-$GCC_VERSION
BUILDDIR=`realpath ./BUILD-GCC-$GCC_VERSION`
rm -rf ./BUILD-GCC-$GCC_VERSION


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

