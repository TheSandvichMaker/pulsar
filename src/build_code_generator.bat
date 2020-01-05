@echo off

ctime -begin ctm/pulsar_code_generator.ctm

IF NOT EXIST build mkdir build
pushd build

ECHO]
ECHO -------------------------------
ECHO *** BUILDING CODE GENERATOR ***
ECHO -------------------------------

REM /MT: Statically link C runtime library (goodbye 10 billion MSVCRedist versions!)
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
set FLAGS=/nologo /O2 /MT /Gm- /Zi /Zo /Oi /GR- /EHa- /fp:fast /fp:except- ^
    /WX /W4 /wd4201 /wd4100 /wd4189 /wd4577 /wd4505 /wd4702 /wd4456 /wd4311 /wd4302 /wd4127 /wd4312

set LINKER_FLAGS=/opt:ref /incremental:no

cl ..\pulsar_code_generator.cpp /D_CRT_SECURE_NO_WARNINGS %FLAGS% /link %LINKER_FLAGS%
set LAST_ERROR=%ERRORLEVEL%

popd

ctime -end ctm/pulsar_code_generator.ctm %LAST_ERROR%
