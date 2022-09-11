@echo off

set build_path=.\out
rem verbose=
set set verbose=-verbose
set action=-compile
rem set action=-dump-prefs
rem set bin_root=C:\Program Files\WindowsApps
set bin_root=.
set bin_dir=arduino_bin
set data_dir=C:\Users\shann\OneDrive\Documents\ArduinoData\packages
set code_dir=.\Bonkulator
set library_dir=C:\Users\shann\Dropbox\moog\Arduino-Libs
set secret_dir=%code_dir%\secrets
set target_device=arduino:mbed_nano:nanorp2040connect -vid-pid=2341_005E
set ide_ver=10819
set cache_dir=.\cache

set runtime_tools_1=runtime.tools.openocd.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2
set runtime_tools_2=runtime.tools.openocd-0.11.0-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2
set runtime_tools_3=runtime.tools.bossac.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2
set runtime_tools_4=runtime.tools.bossac-1.9.1-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2
set runtime_tools_5=runtime.tools.dfu-util.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1
set runtime_tools_6=runtime.tools.dfu-util-0.10.0-arduino1.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1
set runtime_tools_7=runtime.tools.arm-none-eabi-gcc.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4
set runtime_tools_8=runtime.tools.arm-none-eabi-gcc-7-2017q4.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4
set runtime_tools_9=runtime.tools.rp2040tools.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\rp2040tools\1.0.2
set runtime_tools_10=runtime.tools.rp2040tools-1.0.2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\rp2040tools\1.0.2

set product_name=Bonkulator

echo "Make" %product_name%
echo -libraries %library_dir% -fqbn=%target_device%
%bin_root%\%bin_dir%\arduino-builder %action% -logger=machine -hardware %bin_root%\%bin_dir%\hardware -hardware %data_dir% -tools %bin_root%\%bin_dir%\tools-builder -tools %bin_root%\%bin_dir%\hardware\tools\avr -tools %data_dir% -built-in-libraries %bin_root%\%bin_dir%\libraries -libraries %library_dir% -fqbn=%target_device% -ide-version=%ide_ver% -build-path %build_path% -warnings=default -build-cache %cache_dir% -prefs=build.warn_data_percentage=75 -prefs=%runtime_tools_1% -prefs=%runtime_tools_2% -prefs=%runtime_tools_3% -prefs=%runtime_tools_4% -prefs=%runtime_tools_5% -prefs=%runtime_tools_6% -prefs=%runtime_tools_7% -prefs=%runtime_tools_8% -prefs=%runtime_tools_9% -prefs=%runtime_tools_10% %verbose% %code_dir%\%product_name%.ino

if errorlevel 1 (
   if [%1]==[] echo Compiler Error: %errorlevel%
   set /p exitkey= "Press any key to continue..."
   exit /b %errorlevel%
)

exit /b 0
