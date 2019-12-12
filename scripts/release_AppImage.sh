#!/bin/bash

if [ "$1" = "SIGN" ]; then
  export SIGN="1"
fi

# x86_64 build on CentOS 7.7
# i686 build on Ubuntu 16.04 LTS
# armv7l build on raspbian stretch

# Qt path and flags set in env
# export PATH="/opt/Qt/5.14.0/bin/:$PATH"
# export CPPFLAGS="-I/opt/Qt/5.14.0/bin/include/"
# export LDFLAGS="-L/opt/Qt/5.14.0/bin/lib/"
# export LD_LIBRARY_PATH="/opt/Qt/5.14.0/bin/lib/:$LD_LIBRARY_PATH"


# for x86 platform
# Qt 5.14.0 uses openssl 1.1 and some older distros still use 1.0
# we build openssl 1.1.1d from source using following setup:
# ./config shared --prefix=/opt/openssl-1.1.1/ && make --jobs=`nproc --all` && sudo make install
# and add to build env
# export LD_LIBRARY_PATH="/opt/openssl-1.1.1/lib/:$LD_LIBRARY_PATH"

# building AppImage in temporary directory to keep system clean
# use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
  TEMP_BASE=/dev/shm
else
  TEMP_BASE=/tmp
fi

# we run it from our project scripts folder
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"/..
VERSION=$(cat "$ROOT"/VERSION)-$(git rev-parse --short HEAD)
# linuxdeploy uses $VERSION env variable for AppImage name
export VERSION=$VERSION
BUILD="$ROOT"/build
TARGET=rclone-browser-$VERSION.AppImage

# clean AppImage temporary folder
if [ -d "$TEMP_BASE/$TARGET" ]; then
  rm -rf "$TEMP_BASE/$TARGET"
fi
mkdir "$TEMP_BASE/$TARGET"

# clean build folder
if [ -d "$BUILD" ]; then
  rm -rf "$BUILD"
fi
mkdir "$BUILD"

# create release folder if does not exist
mkdir -p "$ROOT"/release

# clean current version previous build
if [ $(arch) = "armv7l" ] && [ -f "$ROOT"/release/rclone-browser-"$VERSION"-armhf.AppImage ]; then
  rm "$ROOT"/release/rclone-browser-"$VERSION"-armhf.AppImage
fi

if [ $(arch) = "i686" ] && [ -f "$ROOT"/release/rclone-browser-"$VERSION"-i386.AppImage ]; then
  rm "$ROOT"/release/rclone-browser-"$VERSION"-i386.AppImage
fi

if [ $(arch) = "x86_64" ] && [ -f "$ROOT"/release/rclone-browser-"$VERSION"-x86_64.AppImage ]; then
  rm "$ROOT"/release/rclone-browser-"$VERSION"-amd64.AppImage
fi

# build and install to temporary AppDir folder
cd "$BUILD"

if [ $(arch) = "armv7l" ]; then
  # more threads need swap on 1GB RAM RPi
  cmake .. -DCMAKE_INSTALL_PREFIX=/usr
  make -j 2
fi

if [ $(arch) = "x86_64" ]; then
  # starting with Qt 5.14.0 default gcc on Centos 7 is too old
  # install gcc 7 on centos 7
  # sudo yum install -y  centos-release-scl devtoolset-7-gcc*
  # enable gcc 7 before running this script
  # scl enable devtoolset-7 bash
  # latest cmake is required
  /opt/cmake/cmake-3.15.5-Linux-x86_64/bin/cmake .. -DCMAKE_INSTALL_PREFIX=/usr
  make --jobs=$(nproc --all)
fi

if [ $(arch) = "i686" ]; then
  cmake .. -DCMAKE_INSTALL_PREFIX=/usr
  make --jobs=$(nproc --all)
fi

make install DESTDIR="$TEMP_BASE"/"$TARGET"/AppDir

# prepare AppImage
cd "$TEMP_BASE/$TARGET"

# metainfo file
#mkdir $TEMP_BASE/$TARGET/AppDir/usr/share/metainfo
#cp $ROOT/assets/rclone-browser.appdata.xml $TEMP_BASE/$TARGET/AppDir/usr/share/metainfo/

# copy info files to AppImage
cp "$ROOT"/README.md "$TEMP_BASE"/"$TARGET"/AppDir/Readme.md
cp "$ROOT"/CHANGELOG.md "$TEMP_BASE"/"$TARGET"/AppDir/Changelog.md
cp "$ROOT"/LICENSE "$TEMP_BASE"/"$TARGET"/AppDir/License.txt

# https://github.com/linuxdeploy/linuxdeploy
# https://github.com/linuxdeploy/linuxdeploy-plugin-qt
linuxdeploy --appdir AppDir --desktop-file=AppDir/usr/share/applications/rclone-browser.desktop
linuxdeploy-plugin-qt --appdir AppDir

if [ $(arch) != "armv7l" ]
then
  # we add openssl 1.1.1 libs needed for distros still using openssl 1.0
  cp /opt/openssl-1.1.1/lib/libssl.so.1.1 ./AppDir/usr/bin/
  cp /opt/openssl-1.1.1/lib/libcrypto.so.1.1 ./AppDir/usr/bin/
fi

# https://github.com/linuxdeploy/linuxdeploy-plugin-appimage
linuxdeploy-plugin-appimage --appdir=AppDir


# raspberry pi build
if [ $(arch) = "armv7l" ]; then
  rename 's/Rclone_Browser/rclone-browser/' Rclone_Browser*
fi

# x86 build
if [ $(arch) = "i686" ]; then
  rename 's/Rclone_Browser/rclone-browser/' Rclone_Browser*
fi

# x86_64 build
if [ $(arch) = "x86_64" ]; then
  rename Rclone_Browser rclone-browser Rclone_Browser*
fi

cp ./*AppImage "$ROOT"/release/

# clean AppImage temporary folder
cd ..
rm -rf "$TARGET"
