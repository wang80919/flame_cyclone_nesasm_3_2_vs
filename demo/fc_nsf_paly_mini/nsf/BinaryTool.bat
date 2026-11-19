@echo off

set PARAM_1=%1

set EXE_PATH="BinaryTool.exe"

cd /d %~dp0

call %EXE_PATH% -?
call %EXE_PATH% %1 -W=0 -L="" -P="" -I=" "
rem call %EXE_PATH% BinaryTool.exe -W=16 -L=" " -P="" -I=" "
set result=%errorlevel%

if 0 NEQ %result% (
    echo %result%
    pause
) else (
    rem copy "%FILE_NAME%.nes" "..\roms\[FC][Music][MMC5].nes" /y
    rem del "[FC][Music][MMC5].nes"
    rem rename "%FILE_NAME%.nes" "[FC][Music][MMC5].nes"
)

goto:eof