@echo off
windres src.rc -O coff -o src.res
gcc src.res texteitorer.c -o texteitorer.exe -mwindows
del src.res