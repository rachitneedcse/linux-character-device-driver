@echo off
echo Building test_user program...

REM Compile the test program
gcc -o ..\test_user.exe ..\src\test_user.c

if %ERRORLEVEL% EQU 0 (
    echo Successfully compiled test_user.exe
    echo You can now run: test_user.exe
) else (
    echo Compilation failed!
    echo Make sure you have gcc installed (MinGW, Cygwin, or WSL)
    pause
    exit /b 1
) 