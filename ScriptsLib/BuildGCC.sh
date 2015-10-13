#!/bin/bash

GCC_VERSION=4.9.3
INSTALL_DIR=$HOME/gcc-$GCC_VERSION

#unset this to take longer, but be safer about build
DISABLE_BOOTSTRAP_FLAG=--disable-bootstrap

PARALLEL_MAKE_FLAG=-j2

MULTILIB_CONFIGURE_FLAGS=--disable-multilib
LANGUAGES_CONFIGURE_FLAGS=--enable-languages=c,c++
EXTRA_CONFIGURE_FLAGS=


wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz
tar xvf gcc-$GCC_VERSION.tar.gz
cd gcc-$GCC_VERSION
./contrib/download_prerequisites
cd ..
mkdir objdir
cd objdir
$PWD/../gcc-$GCC_VERSION/configure --prefix=$INSTALL_DIR $LANGUAGES_CONFIGURE_FLAGS -$MULTILIB_CONFIGURE_FLAGS $DISABLE_BOOTSTRAP_FLAG $EXTRA_CONFIGURE_FLAGS
make $PARALLEL_MAKE_FLAG
make install

