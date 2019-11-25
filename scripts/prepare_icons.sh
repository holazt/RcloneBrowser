#!/bin/sh

# requires following programs:
# brew install optipng
# brew install imagemagick
# brew install makeicns

set -eu

# input file: square canvas svg
if [ ! -f "../assets/rclone-browser.svg" ]; then
  echo "Input file ../assets/rclone-browser.svg is missing"
  exit
fi

echo "Converting svg to png"
convert -density 2400 -resize 512x512 -background none ../assets/rclone-browser.svg rclone-browser.png

echo
echo "Creating 512 256 128 64 32 16 image sizes"
SIZES=(512 256 128 64 32 16)
for s in "${SIZES[@]}"
do
  convert rclone-browser.png -resize "$s" rclone-browser-"$s"x"$s".png
  optipng -o7 -strip all rclone-browser-"$s"x"$s".png
done

echo
echo "Creating ico"
./images2ico.py -o ../src/icon.ico rclone-browser-256x256.png rclone-browser-128x128.png rclone-browser-64x64.png rclone-browser-32x32.png rclone-browser-16x16.png

echo
echo "Creating icns"
makeicns -out ../src/icon.icns -512 rclone-browser-512x512.png -256 rclone-browser-256x256.png -128 rclone-browser-128x128.png -64 rclone-browser-64x64.png -32 rclone-browser-32x32.png -16 rclone-browser-16x16.png

# clean temporary files
rm rclone-browser-16x16.png
rm rclone-browser.png

# used for window icon
cp rclone-browser-128x128.png ../src/icon.png

# used for linux instalation
mv rclone-browser-32x32.png ../assets/
mv rclone-browser-64x64.png ../assets/
mv rclone-browser-128x128.png ../assets/
mv rclone-browser-256x256.png ../assets/
mv rclone-browser-512x512.png ../assets/
