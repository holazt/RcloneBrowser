@echo off
setlocal enabledelayedexpansion

set QT=C:\Qt\5.13.1\msvc2017_64\
set PATH=%QT%\bin;%PATH%

set ROOT="%~dp0.."
set BUILD="%~dp0..\build\build\Release"

set /p VERSION=<"%ROOT%\VERSION"

where /q git.exe
if "%ERRORLEVEL%" equ "0" (
  for /f "tokens=*" %%t in ('git.exe rev-parse --short HEAD') do (
    set COMMIT=%%t
  )
  set VERSION=%VERSION%-!COMMIT!
)

set TARGET="%~dp0\..\release\rclone-browser-%VERSION%-win64"

pushd "%ROOT%"

if not exist release mkdir release

if exist "%TARGET%" rd /s /q "%TARGET%"
if exist "%TARGET%.zip" del "%TARGET%.zip"

if exist build rd /s /q build
mkdir build
cd build

cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_CONFIGURATION_TYPES="Release" -DCMAKE_PREFIX_PATH=%QT% ..
cmake --build . --config Release
popd

mkdir "%TARGET%" 2>nul

copy "%ROOT%\README.md" "%TARGET%\Readme.txt"
copy "%ROOT%\CHANGELOG.md" "%TARGET%\Changelog.txt"
copy "%ROOT%\LICENSE" "%TARGET%\License.txt"
copy "%BUILD%\RcloneBrowser.exe" "%TARGET%"

windeployqt.exe --no-translations --no-angle --no-compiler-runtime --no-svg "%TARGET%\RcloneBrowser.exe"
rd /s /q "%TARGET%\imageformats"

copy "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.23.27820\x64\Microsoft.VC142.CRT\vcruntime140_1.dll" "%TARGET%\vcruntime140_1.dll"

(
echo [Paths]
echo Prefix = .
echo LibraryExecutables = .
echo Plugins = .
)>"%TARGET%\qt.conf"

"c:\Program Files\7-Zip\7z.exe" a -mx=9 -r -tzip "%TARGET%.zip" "%TARGET%"
