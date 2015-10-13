#!/bin/sh


echo "INCOMPLETE AND PROBABLY NOT WORKING YET..."


VERSION=3.6.2
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


#mkdir build_libcxx && cd build_libcxx
#CC=$HOME/clang-$VERSION CXX=$HOME/clang-$VERSION/clang++ cmake -G "Unix Makefiles" -DLIBCXX_CXX_ABI=libsupc++ -DLIBCXX_LIBSUPCXX_INCLUDE_PATHS="/usr/include/c++/4.6/;/usr/include/c++/4.6/x86_64-linux-gnu/" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr $HOME/Clang/libcxx
#make -j 8
#make install
