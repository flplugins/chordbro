@echo off
setlocal

set "ROOT=%~dp0.."
set "SOURCE_DIR=%ROOT%\ripchord-main"
set "BUILD_DIR=%SOURCE_DIR%\build"
set "JUCE_DIR=%USERPROFILE%\Downloads\JUCE"
set "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "VSDEVCMD=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" (
  echo VsDevCmd.bat not found at "%VSDEVCMD%".
  exit /b 1
)

if not exist "%JUCE_DIR%\CMakeLists.txt" (
  echo JUCE source tree not found at "%JUCE_DIR%".
  exit /b 1
)

if not exist "%CMAKE_EXE%" (
  echo CMake executable not found at "%CMAKE_EXE%".
  exit /b 1
)

call "%VSDEVCMD%" -no_logo -arch=x64 -host_arch=x64
if errorlevel 1 exit /b %errorlevel%

where MSBuild.exe >nul 2>nul
if errorlevel 1 (
  echo MSBuild was not added to PATH by VsDevCmd.
  exit /b 1
)

if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"

"%CMAKE_EXE%" --fresh -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 18 2026" -A x64 -T host=x64 -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY -DJUCE_DIR="%JUCE_DIR%"
if errorlevel 1 exit /b %errorlevel%

"%CMAKE_EXE%" --build "%BUILD_DIR%" --config Release --target ChordBro_VST3
exit /b %errorlevel%
