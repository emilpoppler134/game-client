@echo off

cl^
 /Zi^
 /TC^
 /std:c17^
 /I external\include^
 /Fe"bin\ManOfWar.exe"^
 /Fo"bin\out"^
 /Fd"bin\out"^
 src\main.c^
 /link /LIBPATH:external\lib^
 raylib.lib^
 raylibdll.lib^
 opengl32.lib^
 gdi32.lib^
 shell32.lib^
 user32.lib^
 kernel32.lib^
 msvcrt.lib^
 winmm.lib^
 /NODEFAULTLIB:libcmt.lib

if %errorlevel% neq 0 (
    echo Compilation failed
    exit /b 1
)

bin\ManOfWar.exe
