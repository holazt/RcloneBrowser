@echo off
setlocal enabledelayedexpansion

if "%1" == "" (
  echo No architecture x86 or x64 specified in cmdline!
  goto :eof
)

set ARCH=%1
call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%

if "%ARCH%" == "x86" (
set QT=C:\Qt\5.13.1\msvc2017\
) else (
set QT=C:\Qt\5.13.1\msvc2017_64\
)
set PATH=%QT%\bin;%PATH%


set ROOT="%~dp0.."
set BUILD="%~dp0..\build\build\release"
set CMAKEGEN="Visual Studio 16 2019"

set /p VERSION=<"%ROOT%\VERSION"

where /q git.exe
if "%ERRORLEVEL%" equ "0" (
  for /f "tokens=*" %%t in ('git.exe rev-parse --short HEAD') do (
    set COMMIT=%%t
  )
  set VERSION=%VERSION%-!COMMIT!
)

if "%ARCH%" == "x86" (
  set TARGET="%~dp0\..\release\rclone-browser-%VERSION%-win32"
) else (
  set TARGET="%~dp0\..\release\rclone-browser-%VERSION%-win64"
)

pushd "%ROOT%"

if not exist release mkdir release

if exist "%TARGET%" rd /s /q "%TARGET%"
if exist "%TARGET%.zip" del "%TARGET%.zip"

if exist build rd /s /q build
mkdir build
cd build

if "%ARCH%" == "x86" (
cmake -G %CMAKEGEN% -A Win32 -DCMAKE_CONFIGURATION_TYPES="Release" -DCMAKE_PREFIX_PATH=%QT% ..
) else (
cmake -G %CMAKEGEN% -A x64 -DCMAKE_CONFIGURATION_TYPES="Release" -DCMAKE_PREFIX_PATH=%QT% ..
)

cmake --build . --config Release
popd

mkdir "%TARGET%" 2>nul

copy "%ROOT%\README.md" "%TARGET%\Readme.txt"
copy "%ROOT%\CHANGELOG.md" "%TARGET%\Changelog.txt"
copy "%ROOT%\LICENSE" "%TARGET%\License.txt"
copy "%BUILD%\RcloneBrowser.exe" "%TARGET%"

windeployqt.exe --no-translations --no-angle --no-compiler-runtime --no-svg "%TARGET%\RcloneBrowser.exe"
rd /s /q "%TARGET%\imageformats"

rem include all MSVCruntime dlls
copy "%VCToolsRedistDir%\%ARCH%\Microsoft.VC142.CRT\msvcp140.dll" "%TARGET%\"
copy "%VCToolsRedistDir%\%ARCH%\Microsoft.VC142.CRT\vcruntime140*.dll" "%TARGET%\"

rem for Windows 32 bits build include relevant openssl libraries
if "%ARCH%" == "x86" (
copy "c:\Program Files (x86)\openssl-1.1.1d-win32\libssl-1_1.dll" "%TARGET%\"
copy "c:\Program Files (x86)\openssl-1.1.1d-win32\libcrypto-1_1.dll" "%TARGET%\"
)

(
echo [Paths]
echo Prefix = .
echo LibraryExecutables = .
echo Plugins = .
)>"%TARGET%\qt.conf"

rem https://www.7-zip.org/
"c:\Program Files\7-Zip\7z.exe" a -mx=9 -r -tzip "%TARGET%.zip" "%TARGET%"
