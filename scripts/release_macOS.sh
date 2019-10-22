#!/bin/sh

set -e

export LDFLAGS="-L/usr/local/opt/qt/lib"
export CPPFLAGS="-I/usr/local/opt/qt/include"

QTDIR=/usr/local/opt/qt

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"/..
VERSION=`cat $ROOT/VERSION`-`git rev-parse --short HEAD`
BUILD="$ROOT"/build
TARGET=rclone-browser-$VERSION-macOS
APP="$TARGET"/"Rclone Browser.app"

rm -rf "$BUILD"
mkdir -p "$BUILD"
cd "$BUILD"
# brew install cmake qt5
cmake .. -DCMAKE_PREFIX_PATH="$QTDIR" -DCMAKE_BUILD_TYPE=Release
# brew install coreutils
make --jobs=`nproc --all`
cd build
"$QTDIR"/bin/macdeployqt rclone-browser.app -executable="rclone-browser.app/Contents/MacOS/rclone-browser" -qmldir=../src/
cd ../..


mkdir -p release
cd release
rm -rf "$TARGET"*
mkdir "$TARGET"
cp "$ROOT"/README.md "$TARGET"/Readme.txt
cp "$ROOT"/CHANGELOG.md "$TARGET"/Changelog.txt
cp "$ROOT"/LICENSE "$TARGET"/License.txt
cp -R "$BUILD"/build/rclone-browser.app "$APP"
mv "$APP"/Contents/MacOS/rclone-browser "$APP"/Contents/MacOS/"Rclone Browser"

sed -i .bak 's/rclone-browser/Rclone Browser/g' "$APP"/Contents/Info.plist
rm "$APP"/Contents/*.bak

cat >"$APP"/Contents/MacOS/qt.conf <<EOF
[Paths]
Plugins = Plugins
EOF

# brew install p7zip
7za a -mx=9 -r -tzip "$TARGET".zip "$TARGET"

## gpg --detach-sign "$TARGET".zip.sig "$TARGET".zip
