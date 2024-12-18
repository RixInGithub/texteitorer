@echo off
windres src.rc -O coff -o src.res
gcc src.res texteitorer.c -o texteitorer.exe -lgdi32 -lcomdlg32 -mwindows
del src.res