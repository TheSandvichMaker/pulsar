@echo off

set FILETYPES=*.h *.cpp *inl *.c

echo[
echo STATICS FOUND:
findstr -s -n -i -l "static" %FILETYPES%

echo[
echo --------------------------------------
echo[

echo LOCAL PERSISTS FOUND:
findstr -s -n -i -l "local_persist" %FILETYPES%
echo[
echo GLOBALS FOUND:
findstr -s -n -i -l "global" %FILETYPES%
