@echo off

ctime -begin ctm/win32_pulsar.ctm

IF NOT EXIST build mkdir build
pushd build

ECHO]
    ECHO ------------------------------------------
if "%1" equ "release" (
    ECHO *** BUILDING RELEASE BUILD FROM SOURCE ***
) else (
    ECHO ***  BUILDING DEBUG BUILD FROM SOURCE  ***
)
    ECHO ------------------------------------------

REM /MT: Statically link C runtime library
REM /Gm-: Disable incremental builds
REM /Zi: Debug info
REM /Oi: Intrinsics
REM /GR-: Disable run-time type information
REM /EHa-: Disable exceptions
REM /WX: Treat warnings as errors
REM /W4: Warning level 4
REM /wd[xxx]: Disable warning
REM /subsystem:windows,5.1: Coax the compiler into making something that can run on windows XP
REM /opt:ref: Cull unused functions

REM NOTE: /MTd for getting the debug version of the CRT. Should be /MT for release.
set SHARED_FLAGS=/nologo /Gm- /Zo /Oi /GR- /EHa- /fp:fast /fp:except- ^
    /WX /W4 /wd4201 /wd4100 /wd4189 /wd4577 /wd4505 /wd4702 /wd4311 /wd4302 /wd4127 /wd4312 ^
    /D_CRT_SECURE_NO_WARNINGS=1

set DEBUG_FLAGS=/Od /MTd /Zi /DPULSAR_DEBUG=1
set RELEASE_FLAGS=/O2 /MT /DPULSAR_DEBUG=0

set LINKER_FLAGS=/opt:ref /incremental:no
set LINKER_LIBRARIES=user32.lib gdi32.lib opengl32.lib

pushd ..
build\pulsar_code_generator.exe
popd

REM 64 bit build
if "%1" equ "release" (
    cl ..\win32_pulsar.cpp %SHARED_FLAGS% %RELEASE_FLAGS% /link %LINKER_FLAGS% %LINKER_LIBRARIES%
) else (
    cl ..\win32_pulsar.cpp %SHARED_FLAGS% %DEBUG_FLAGS% /link %LINKER_FLAGS% %LINKER_LIBRARIES%
)
set LAST_ERROR=%ERRORLEVEL%

popd

ctime -end ctm/win32_pulsar.ctm %LAST_ERROR%
