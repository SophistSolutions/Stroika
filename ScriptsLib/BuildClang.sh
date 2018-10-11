#!/bin/sh

#VERSION=3.7.1
#VERSION=3.8.1
#VERSION=3.9.1
#VERSION=4.0.1
#VERSION=5.0.0
VERSION=${VERSION-4.0.1}
BUILDDIR=BUILDDIR-LLVM-$VERSION
INSTALL_DIR="${INSTALL_DIR:=/private-compiler-builds/clang-$VERSION}"

PARALELLMAKEARG=-j4

echo Building LLVM $VERSION in $BUILDDIR
rm -rf $BUILDDIR
mkdir -p $BUILDDIR
cd $BUILDDIR

wget http://llvm.org/releases/$VERSION/cfe-$VERSION.src.tar.xz
wget http://llvm.org/releases/$VERSION/llvm-$VERSION.src.tar.xz
wget http://llvm.org/releases/$VERSION/compiler-rt-$VERSION.src.tar.xz
wget http://llvm.org/releases/$VERSION/libcxx-$VERSION.src.tar.xz
wget http://llvm.org/releases/$VERSION/libcxxabi-$VERSION.src.tar.xz

tar xf cfe-$VERSION.src.tar.xz
tar xf llvm-$VERSION.src.tar.xz
tar xf compiler-rt-$VERSION.src.tar.xz
tar xf libcxx-$VERSION.src.tar.xz
tar xf libcxxabi-$VERSION.src.tar.xz

ln -s llvm-$VERSION.src llvm
ln -s `realpath libcxx-$VERSION.src` llvm/projects/libcxx
ln -s `realpath libcxxabi-$VERSION.src` llvm/projects/libcxxabi
ln -s `realpath cfe-$VERSION.src` llvm/tools/clang

mkdir build-llvm
cd build-llvm

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=INSTALL_DIR -DLLVM_OPTIMIZED_TABLEGEN=true ../llvm
make $PARALELLMAKEARG
make install
