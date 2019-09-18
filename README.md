RcloneBrowser
=============

[![Downloads][img3]][3] [![Release][img4]][4] [![License][img5]][5]

Simple cross platfrom GUI for rclone command line tool.
Supports Windows, macOS and GNU/Linux.


Being rclone-browser user for some time and got annoyed by small not working bits and pieces. Original repo (https://github.com/mmozeiko/RcloneBrowser) has not been touched for years and in the meantime rclone changed few things breaking some rclone-browser functionality.

I've looked around but could not find anything fully working. Some github users made progress in fixing and adding stuff so I've built upon it.

I used DinCahill's changes (https://github.com/DinCahill/RcloneBrowser) as a base of my version.

I have fixed whatever I found still not working and added few minor tweaks. I've recompiled and repackaged everything using latest Qt (5.13.1). This on its own fixed some issues and added new features like support for dark mode in macOS.

Below sample screenshots:


### macOS
![alt text](https://github.com/kapitainsky/RcloneBrowser/wiki/images/Screenshot5small.png)


### Linux
![alt text](https://github.com/kapitainsky/RcloneBrowser/wiki/images/Screenshot2.png)


### Windows
![alt text](https://github.com/kapitainsky/RcloneBrowser/wiki/images/Screenshot3.png)



All releases (https://github.com/kapitainsky/RcloneBrowser/releases) are signed with my PGP key

Fingerprint: 5173 1FD9 E29C BEC2 8506  8F02 31DB 6AEE 3AC2 8075

https://keys.openpgp.org/vks/v1/by-fingerprint/51731FD9E29CBEC285068F0231DB6AEE3AC28075

Enjoy and report if anything is broken. Please note that I am not a programmer and I doubt I could work on any major changes. Also rclone-browser as it is now is enough for my usage. But small fixes and tweaks are always possible - feel free to report any issues.

Download
--------

Get Windows, macOS and Ubuntu/Debian package on [releases][3] page.

ArchLinux users can install latest release from AUR repository: [rclone-browser][7]. It has been updated to this repo.

Other GNU/Linux users will need to build from source. Actually in my opinion for Linux it is the best option and is real easy. Below are detailed instructions.


Build instructions for Debian/Ubuntu Linux
------------------------------------------
1. Install dependencies `sudo apt install git rclone g++ cmake qtbase5-dev`
2. Clone source code from this repo `git clone https://github.com/kapitainsky/RcloneBrowser.git`
3. Go to source folder `cd RcloneBrowser`
4. Create new `build` folder next to `src` folder - `mkdir build && cd build`
5. Run `cmake ..` from `build` folder to create makefile
6. Run `cmake --build .` from `build` folder to create binary


Build instructions for openSUSE linux
-------------------------------------
1. Install dependencies `sudo zypper install git cmake gcc-c++ rclone libQt5Core-devel libQt5Widgets-devel`
2. Clone source code from this repo `git clone https://github.com/kapitainsky/RcloneBrowser.git`
3. Go to source folder `cd RcloneBrowser`
4. Create new `build` folder next to `src` folder - `mkdir build && cd build`
5. Run `cmake ..` from `build` folder to create makefile
6. Run `cmake --build .` from `build` folder to create binary

Build instructions for macOS
----------------------------
1. If you don't have Homebrew yet install it `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
2. Install dependencies `brew install git cmake rclone qt5`
3. Set Qt environment variables `export PATH="/usr/local/opt/qt/bin:$PATH" &&  export LDFLAGS="-L/usr/local/opt/qt/lib" &&  export CPPFLAGS="-I/usr/local/opt/qt/include" && export PKG_CONFIG_PATH="/usr/local/opt/qt/lib/pkgconfig"`. You can add this to your .bashrc (assuming you use bash) if you want to keep it persistent
4. Clone source code from this repo `git clone https://github.com/kapitainsky/RcloneBrowser.git`
5. Go to source folder `cd RcloneBrowser`
6. Create new `build` folder next to `src` folder - `mkdir build && cd build`
7. Run `cmake ..` from `build` folder to create makefile
8. Run `cmake --build .` from `build` folder to create binary
9. Go to yet another newly created `build` folder `cd build`. Your binary should be here
10. Package your binary with Qt libraries to create self-contained application `macdeployqt rclone-browser.app -executable="rclone-browser.app/Contents/MacOS/rclone-browser" -qmldir=../src/`. Without this step binary won't work without Qt installed








Original readme from https://github.com/mmozeiko/RcloneBrowser 
--------

Features
--------

* Allows to browse and modify any rclone remote, including encrypted ones
* Uses same configuration file as rclone, no extra configuration required
* Supports custom location and encryption for `.rclone.conf` configuration file
* Simultaneously navigate multiple repositories in separate tabs
* Lists files hierarchically with file name, size and modify date
* All rclone commands are executed asynchronously, no freezing GUI
* File hierarchy is lazily cached in memory, for faster traversal of folders
* Allows to upload, download, create new folders, rename or delete files and folders
* Allows to calculate size of folder, export list of files and copy rclone copmmand to clipboard
* Can process multiple upload or download jobs in background
* Drag & drop support for dragging files from local file explorer for uploading
* Streaming media files for playback in player like [mpv][6] or similar
* Mount and unmount folders on macOS and GNU/Linux
* Optionally minimizes to tray, with notifications when upload/download finishes
* Supports portable mode (create .ini file next to executable with same name), rclone and .rclone.conf path now can be relative to executable

Download
--------

Get Windows, macOS and Ubuntu package on [releases][3] page.

For Ubuntu you can also install it from Launchpad: [Rclone Browser][launchpad].

ArchLinux users can install latest release from AUR repository: [rclone-browser][7].

Other GNU/Linux users will need to build from source.

Screenshots
-----------

### Windows

![screenshot1.png][screenshot1]
![screenshot2.png][screenshot2]
![screenshot3.png][screenshot3]
![screenshot4.png][screenshot4]

### Ubuntu

![screenshot5.png][screenshot5]

### macOS

![screenshot6.png][screenshot6]

Build instructions for Windows
------------------------------

1. Get [Visual Studio 2013][8]
2. Install [CMake][9]
3. Install or build from source Qt v5 (64-bit) from [Qt website][10]
4. Set `QTDIR` environment variable to Qt installation, or adjust path to Qt in `bootstrap.cmd` file
5. Run `bootstrap.cmd`, it will generate Visual Studio 2013 solution in `build` folder

Build instructions for GNU/Linux and macOS
------------------------------------------

1. Make sure you have working compiler and [cmake][9] installed
2. Install Qt v5 with package manager or from [Qt website][10]
3. Create new `build` folder next to `src` folder
4. Run `cmake ..` from `build` folder to create makefile
   - if cmake doesn't find Qt, add `-DCMAKE_PREFIX_PATH=path/to/Qt` to previous command
5. Run `cmake --build .` from `build` folder to create binary

License
-------

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.

[1]: https://travis-ci.org/DinCahill/RcloneBrowser/
[2]: https://ci.appveyor.com/project/DinCahill/RcloneBrowser
[3]: https://github.com/kapitainsky/RcloneBrowser/releases
[4]: https://github.com/kapitainsky/RcloneBrowser/releases/latest
[5]: https://github.com/kapitainsky/RcloneBrowser/blob/master/LICENSE
[6]: https://mpv.io/
[7]: https://aur.archlinux.org/packages/rclone-browser
[8]: https://www.visualstudio.com/en-us/news/releasenotes/vs2013-community-vs
[9]: http://www.cmake.org/
[10]: https://www.qt.io/download-open-source/
[img1]: https://api.travis-ci.org/DinCahill/RcloneBrowser.svg?branch=master
[img2]: https://ci.appveyor.com/api/projects/status/github/DinCahill/RcloneBrowser?branch=master&svg=true
[img3]: https://img.shields.io/github/downloads/kapitainsky/RcloneBrowser/total.svg?maxAge=3600
[img4]: https://img.shields.io/github/release/kapitainsky/RcloneBrowser.svg?maxAge=3600
[img5]: https://img.shields.io/github/license/kapitainsky/RcloneBrowser.svg?maxAge=2592000
[screenshot1]: https://raw.githubusercontent.com/wiki/mmozeiko/RcloneBrowser/screenshot1.png
[screenshot2]: https://raw.githubusercontent.com/wiki/mmozeiko/RcloneBrowser/screenshot2.png
[screenshot3]: https://raw.githubusercontent.com/wiki/mmozeiko/RcloneBrowser/screenshot3.png
[screenshot4]: https://raw.githubusercontent.com/wiki/mmozeiko/RcloneBrowser/screenshot4.png
[screenshot5]: https://raw.githubusercontent.com/wiki/mmozeiko/RcloneBrowser/screenshot5.png
[screenshot6]: https://raw.githubusercontent.com/wiki/mmozeiko/RcloneBrowser/screenshot6.png
[launchpad]: https://launchpad.net/~mmozeiko/+archive/ubuntu/rclone-browser
