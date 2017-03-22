@echo off
set PATH=%PATH%;C:\cygwin64\bin
@echo on
getregions.exe ../Objects/*.axf -c -s -o../Source/region.c