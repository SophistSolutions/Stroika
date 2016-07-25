#!/bin/sh

VERSION=${VERSION-3.8.1}
BUILDDIR=BUILDDIR-LLVM-$VERSION

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
../llvm/configure --prefix=$HOME/clang-$VERSION --enable-optimized --enable-targets=host --disable-compiler-version-checks
#cmake -G "Unix Makefiles" ../llvm
make $PARALELLMAKEARG
make install
cd ..

