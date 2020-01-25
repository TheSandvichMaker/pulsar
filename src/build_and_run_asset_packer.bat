@echo off

call "build_asset_packer.bat"
if %LAST_ERROR% equ 0 (
    build\pulsar_asset_packer.exe
) else (
    echo Compiler exited with errors. Asset packer didn't run.
)
