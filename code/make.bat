@echo off

set product_name=Bonkulator
echo "Make" %product_name%

.\arduino-cli compile -b arduino:mbed_nano:nanorp2040connect --output-dir .\out --libraries .\Bonkulator\libraries Bonkulator

if errorlevel 1 (
   if [%1]==[] echo Compiler Error: %errorlevel%
   set /p exitkey= "Press any key to continue..."
   exit /b %errorlevel%
)

exit /b 0
