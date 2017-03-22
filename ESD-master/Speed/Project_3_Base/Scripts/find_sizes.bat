echo off
set PATH=%PATH%;C:\cygwin64\bin
Scripts\getregions.exe Objects/*.axf -z -oObjects/function_sizes.txt
sort /R Objects\function_sizes.txt /O Objects\sorted_function_sizes.txt
