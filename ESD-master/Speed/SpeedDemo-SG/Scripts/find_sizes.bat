echo off
set PATH=%PATH%;C:\cygwin64\bin
getregions.exe ../Objects/*.axf -z -o../Objects/function_sizes.txt
sort /R ..\Objects\function_sizes.txt /O ..\Objects\sorted_function_sizes.txt
