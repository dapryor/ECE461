@echo off
set PATH=%PATH%;C:\cygwin64\bin
@echo on
Scripts\getregions.exe Objects/*.axf -c -oSource/region.c
