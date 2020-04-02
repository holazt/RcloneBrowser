@echo off

call :setESC

setlocal enabledelayedexpansion

if "%1" == "" (
  echo Please specify x86 ^(32-bit^) or x64 ^(64-bit^) architecture in cmdline
  goto :eof
)

set BOTH=0
if not "%1" == "x86" if not "%1" == "x64" set BOTH=1

if %BOTH% == 1  (
  echo Only x86 ^(32-bit^) or x64 ^(64-bit^) architectures are supported!
  goto :eof
)

set ARCH=%1
call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %ARCH% || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)



if "%ARCH%" == "x86" (
set QT=C:\Qt\5.14.2\msvc2017\
) else (
set QT=C:\Qt\5.14.2\msvc2017_64\
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
  set VERSION_COMMIT=%VERSION%-!COMMIT!
)

if "%ARCH%" == "x86" (
  set TARGET="%~dp0\..\release\rclone-browser-%VERSION_COMMIT%-windows-32-bit"
  set TARGET_EXE="%~dp0\..\release\rclone-browser-%VERSION_COMMIT%-windows-32-bit"
) else (
  set TARGET="%~dp0\..\release\rclone-browser-%VERSION_COMMIT%-windows-64-bit"
  set TARGET_EXE="%~dp0\..\release\rclone-browser-%VERSION_COMMIT%-windows-64-bit"
)

pushd "%ROOT%"

if not exist release mkdir release || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

if exist "%TARGET%" rd /s /q "%TARGET%" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
if exist "%TARGET%.zip" del "%TARGET%.zip" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
if exist "%TARGET_EXE%.exe" del "%TARGET_EXE%.exe" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

if exist build rd /s /q build || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
mkdir build || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
cd build || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

if "%ARCH%" == "x86" (
cmake -G %CMAKEGEN% -A Win32 -DCMAKE_CONFIGURATION_TYPES="Release" -DCMAKE_PREFIX_PATH=%QT% .. || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
) else (
cmake -G %CMAKEGEN% -A x64 -DCMAKE_CONFIGURATION_TYPES="Release" -DCMAKE_PREFIX_PATH=%QT% .. || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
)

cmake --build . --config Release || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
popd

mkdir "%TARGET%" 2>nul || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

copy "%ROOT%\README.md" "%TARGET%\Readme.md" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
copy "%ROOT%\CHANGELOG.md" "%TARGET%\Changelog.md" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
copy "%ROOT%\LICENSE" "%TARGET%\License.txt" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
copy "%BUILD%\RcloneBrowser.exe" "%TARGET%" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

windeployqt.exe --no-translations --no-angle --no-compiler-runtime --no-svg "%TARGET%\RcloneBrowser.exe" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
rd /s /q "%TARGET%\imageformats" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

rem include all MSVCruntime dlls
copy "%VCToolsRedistDir%\%ARCH%\Microsoft.VC142.CRT\msvcp140.dll" "%TARGET%\" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
copy "%VCToolsRedistDir%\%ARCH%\Microsoft.VC142.CRT\vcruntime140*.dll" "%TARGET%\" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

rem include relevant openssl libraries
if "%ARCH%" == "x86" (
copy "c:\Program Files (x86)\openssl-1.1.1f-win32\libssl-1_1.dll" "%TARGET%\" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
copy "c:\Program Files (x86)\openssl-1.1.1f-win32\libcrypto-1_1.dll" "%TARGET%\" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
) else (
copy "c:\Program Files\openssl-1.1.1f-win64\libssl-1_1-x64.dll" "%TARGET%\" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
copy "c:\Program Files\openssl-1.1.1f-win64\libcrypto-1_1-x64.dll" "%TARGET%\" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
)

(
echo [Paths]
echo Prefix = .
echo LibraryExecutables = .
echo Plugins = .
)>"%TARGET%\qt.conf" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

rem https://www.7-zip.org/
rem create zip archive of all files
"c:\Program Files\7-Zip\7z.exe" a -mx=9 -r -tzip "%TARGET%.zip" "%TARGET%" || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)

rem create proper installer
rem Inno Setup installer by https://github.com/jrsoftware/issrc
rem in case user wants to install both 32bits and 64bits versions we need two AppId
rem 64bits ;AppId={{0AF9BF43-8D44-4AFF-AE60-6CECF1BF0D31}
rem 32bits ;AppId={{5644ED3A-6028-47C0-9796-29548EF7CEA3}
if "%ARCH%" == "x86" (
"c:\Program Files (x86)\Inno Setup 6"\iscc "/dMyAppVersion=%VERSION%" "/dMyAppId={{5644ED3A-6028-47C0-9796-29548EF7CEA3}" "/dMyAppDir=rclone-browser-%VERSION_COMMIT%-windows-32-bit" "/dMyAppArch=x86" /O"../release" /F"rclone-browser-%VERSION_COMMIT%-windows-32-bit" rclone-browser-win-installer.iss || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
) else (
"c:\Program Files (x86)\Inno Setup 6"\iscc "/dMyAppVersion=%VERSION%" "/dMyAppId={{0AF9BF43-8D44-4AFF-AE60-6CECF1BF0D31}" "/dMyAppDir=rclone-browser-%VERSION_COMMIT%-windows-64-bit" "/dMyAppArch=x64" /O"../release" /F"rclone-browser-%VERSION_COMMIT%-windows-64-bit" rclone-browser-win-installer.iss || ( call :setESC & echo. & echo. & echo %ESC%[91mBuild FAILED.%ESC%[0m  & EXIT /B 1)
)

rem Build OK

if "%ARCH%" == "x86" (
call :setESC & echo. & echo. & echo %ESC%[92mWindows 32-bit build OK.%ESC%[0m & exit /B 0
) else (
call :setESC & echo. & echo. & echo %ESC%[92mWindows 64-bit build OK.%ESC%[0m & exit /B 0
)

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)
