@echo off
echo.
echo.
echo Update The Bonkulator's software to:
type release.txt
echo.
echo Available COM ports
echo -------------------
powershell.exe [System.IO.Ports.SerialPort]::getportnames()
echo.
echo.
set /p port=What's the COM port? (please enter a number or press ENTER to use COM%port%) 'x' to exit 

if %port% == x exit

arduino-cli upload -p COM%port% -b arduino:mbed_nano:nanorp2040connect -i ./Bonkulator.ino.bin
pause
