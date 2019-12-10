@echo off

set FILETYPES=*.h *.cpp *inl *.c

echo[
echo TODOS FOUND:
findstr -s -n -i -l "TODO" %FILETYPES%
