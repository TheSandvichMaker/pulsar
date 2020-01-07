@echo off

call "build_asset_packer.bat"
echo %LAST_ERROR%
REM TODO: Make this only run if LAST_ERROR is 0
build\pulsar_asset_packer.exe
