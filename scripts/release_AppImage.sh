#!/bin/bash

# already set in env
# export PATH="/opt/Qt/5.13.2/bin/:$PATH"
# export CPPFLAGS="-I/opt/Qt/5.13.2/bin/include/"
# export LDFLAGS="-L/opt/Qt/5.13.2/bin/lib/"
# export LD_LIBRARY_PATH="/opt/Qt/5.13.2/bin/lib/:$LD_LIBRARY_PATH"

QTDIR=/opt/Qt/5.13.2

# for x86_64 Linux
# Qt 5.13.2 uses openssl 1.1 and on Ubuntu 16.04 LTS 1.0 is used so
# we build openssl 1.1.1d source using following setup:
# ./config --prefix=/opt/openssl-1.1.1/ && make --jobs=`nproc --all` && sudo make install
# and add to env
# export LD_LIBRARY_PATH=/opt/openssl-1.1.1/lib/:$LD_LIBRARY_PATH

# building AppImage in temporary directory to keep system clean
# use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

# we run it from our project scripts folder
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"/..
VERSION=`cat $ROOT/VERSION`-`git rev-parse --short HEAD`
# linuxdeployqt-6-x86_64.AppImage uses $VERSION env variable for AppImage name
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
:
fi
mkdir "$BUILD"

# create release folder if does not exist
mkdir -p "$ROOT"/release

# clean current version previous build
if ls $ROOT/release/rclone-browser-$VERSION-*.AppImage > /dev/null 2>&1; then
  rm $ROOT/release/rclone-browser-$VERSION-*.AppImage;
fi

# build and install to temporary AppDir folder
cd "$BUILD"
cmake .. -DCMAKE_INSTALL_PREFIX=/usr

if [ `arch` = "armv7l" ]
then
# more processes need swap
make -j 2
else
make --jobs=`nproc --all`
fi

make install DESTDIR=$TEMP_BASE/$TARGET/AppDir

# prepare AppImage
cd "$TEMP_BASE/$TARGET"

# metainfo file
mkdir $TEMP_BASE/$TARGET/AppDir/usr/share/metainfo
cp $ROOT/scripts/rclone-browser.appdata.xml $TEMP_BASE/$TARGET/AppDir/usr/share/metainfo/


if [ `arch` = "armv7l" ]
then
  # armv7l architecture
  # For RPi we use https://github.com/probonopd/linuxdeployqt as linuxdeploy-plugin-qt had issues
  linuxdeployqt AppDir/usr/share/applications/rclone-browser.desktop -qmake=$QTDIR/bin/qmake -no-translations -appimage
else
  # x86_64 architecture
  # https://github.com/linuxdeploy/linuxdeploy
  # https://github.com/linuxdeploy/linuxdeploy-plugin-qt
  linuxdeploy --appdir AppDir
  linuxdeploy-plugin-qt  --appdir AppDir

  # we add openssl 1.1.1 libs to enable ssl for some old Linux distros
  cp /opt/openssl-1.1.1/lib/libssl.so.1.1 ./AppDir/usr/lib/
  cp /opt/openssl-1.1.1/lib/libcrypto.so.1.1 ./AppDir/usr/lib/

  # https://github.com/linuxdeploy/linuxdeploy-plugin-appimage
  # now can create AppImage from AppDir
  linuxdeploy-plugin-appimage --appdir=AppDir
fi

rename 's/Rclone_Browser/rclone-browser/' Rclone_Browser*

cp rclone-browser* $ROOT/release/

# clean AppImage temporary folder
cd ..
rm -rf $TARGET
