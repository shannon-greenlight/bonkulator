@echo off
echo.
echo.
echo Available COM ports
echo -------------------
powershell.exe [System.IO.Ports.SerialPort]::getportnames()
echo.
echo.
SetLocal EnableDelayedExpansion

set port=
for /F "delims=" %%i in (spank_port.txt) do set port=%port%%%i
call :TRIM %port%
set port=%TRIMRESULT%

echo Using: COM%port%
set /p port=What's the COM port? (please enter a number or press ENTER to use COM%port%) 'x' to exit 

if %port% == x exit

echo %port% > spank_port.txt

arduino-cli upload -p COM%port% -b arduino:mbed_nano:nanorp2040connect -i ./out/Bonkulator.ino.bin
pause
EndLocal

:TRIM
  rem echo "%1"
  rem echo "%2"
  rem echo "%3"
  rem echo "%4"
  SET TRIMRESULT=%*
GOTO :EOF
pause
