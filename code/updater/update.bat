@echo off
echo.
echo.
echo Available COM ports
echo -------------------
powershell.exe [System.IO.Ports.SerialPort]::getportnames()
echo.
echo.
set /p port=What's the COM port? (please enter a number)

@mode com%port% baud=1200
pause

bossac.exe -d -U true -i -e -w -v -b Spankulator.ino.bin -R
pause
