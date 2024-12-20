@echo off
del texteitorer.exe >nul 2>&1
windres src.rc -O coff -o src.res
gcc src.res texteitorer.c -o texteitorer.exe -lgdi32 -lcomdlg32 -lhtmlhelp -mwindows
del src.res >nul 2>&1