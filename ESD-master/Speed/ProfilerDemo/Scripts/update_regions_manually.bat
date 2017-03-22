@echo off
set PATH=%PATH%;C:\cygwin64\bin
@echo on
getregions.exe ../Objects/*.axf -c -o../Source/region.c