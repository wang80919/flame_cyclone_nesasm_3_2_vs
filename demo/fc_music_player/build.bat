@echo off

set IN_FILE_NAME=fc_demo_main.asm
set OUT_FILE_NAME=fc_demo_main.nes
set NESASM3=nesasm_Gui_3_2.exe

cd /d %~dp0

call "%NESASM3%" -in %IN_FILE_NAME% -out "%OUT_FILE_NAME%"
set result=%errorlevel%

IF 0 NEQ %result% (
    pause
) else (
    rem TODO
)

goto:eof