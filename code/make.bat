@echo off

set new_make=true
set build_path=.\out
set verbose=
rem set verbose=-verbose
set action=-compile
rem set action=-dump-prefs
rem set tool_root=C:\Program Files\WindowsApps
set tool_root=.
set tool_dir=ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt
set data_dir=C:\Users\shann\OneDrive\Documents\ArduinoData\packages
set code_dir=C:\Users\shann\Dropbox\moog\bonkulator\code\Bonkulator
set secret_dir=%code_dir%\secrets
rem set target_device=arduino:samd:nano_33_iot -vid-pid=2341_8057
set target_device=arduino:mbed_nano:nanorp2040connect -vid-pid=2341_005E
rem set ide_ver=10816
set ide_ver=10819
rem set build_path=.\out
rem set build_path=C:\Users\shann\AppData\Local\Temp\arduino_build_344020
rem set cache_dir=C:\Users\shann\AppData\Local\Temp\arduino_cache_buddies
set cache_dir=C:\Users\shann\AppData\Local\Temp\arduino_cache_buddies

rem set runtime_tools_1=runtime.tools.arm-none-eabi-gcc.path=%data_dir%\arduino\tools\arm-none-eabi-gcc\7-2017q4
set runtime_tools_1=runtime.tools.openocd.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2
rem set runtime_tools_2=runtime.tools.arm-none-eabi-gcc-7-2017q4.path=%data_dir%\arduino\tools\arm-none-eabi-gcc\7-2017q4
set runtime_tools_2=runtime.tools.openocd-0.11.0-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2
rem set runtime_tools_3=runtime.tools.CMSIS.path=%data_dir%\arduino\tools\CMSIS\4.5.0
set runtime_tools_3=runtime.tools.bossac.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2
rem set runtime_tools_4=runtime.tools.CMSIS-4.5.0.path=%data_dir%\arduino\tools\CMSIS\4.5.0
set runtime_tools_4=runtime.tools.bossac-1.9.1-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2
rem set runtime_tools_5=runtime.tools.openocd.path=%data_dir%\arduino\tools\openocd\0.10.0-arduino7
set runtime_tools_5=runtime.tools.dfu-util.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1
rem set runtime_tools_6=runtime.tools.openocd-0.10.0-arduino7.path=%data_dir%\arduino\tools\openocd\0.10.0-arduino7
set runtime_tools_6=runtime.tools.dfu-util-0.10.0-arduino1.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1
rem set runtime_tools_7=runtime.tools.bossac.path=%data_dir%\arduino\tools\bossac\1.7.0-arduino3
set runtime_tools_7=runtime.tools.arm-none-eabi-gcc.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4
rem set runtime_tools_8=runtime.tools.bossac-1.7.0-arduino3.path=%data_dir%\arduino\tools\bossac\1.7.0-arduino3
set runtime_tools_8=runtime.tools.arm-none-eabi-gcc-7-2017q4.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4
rem set runtime_tools_9=runtime.tools.arduinoOTA.path=%data_dir%\arduino\tools\arduinoOTA\1.2.1
set runtime_tools_9=runtime.tools.rp2040tools.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\rp2040tools\1.0.2
rem set runtime_tools_10=runtime.tools.arduinoOTA-1.2.1.path=%data_dir%\arduino\tools\arduinoOTA\1.2.1
set runtime_tools_10=runtime.tools.rp2040tools-1.0.2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\rp2040tools\1.0.2

rem set runtime_tools_11=runtime.tools.CMSIS-Atmel.path=%data_dir%\arduino\tools\CMSIS-Atmel\1.2.0
rem set runtime_tools_12=runtime.tools.CMSIS-Atmel-1.2.0.path=%data_dir%\arduino\tools\CMSIS-Atmel\1.2.0

rem set product_name=Spankulator
set product_name=Bonkulator

rem echo Arg: %1

if %new_make%==true (
	echo "New Make" %product_name%
	%tool_root%\%tool_dir%\arduino-builder %action% -logger=machine -hardware %tool_root%\%tool_dir%\hardware -hardware %data_dir% -tools %tool_root%\%tool_dir%\tools-builder -tools %tool_root%\%tool_dir%\hardware\tools\avr -tools %data_dir% -built-in-libraries %tool_root%\%tool_dir%\libraries -libraries %code_dir%\libraries -fqbn=%target_device% -ide-version=%ide_ver% -build-path %build_path% -warnings=default -build-cache %cache_dir% -prefs=build.warn_data_percentage=75 -prefs=%runtime_tools_1% -prefs=%runtime_tools_2% -prefs=%runtime_tools_3% -prefs=%runtime_tools_4% -prefs=%runtime_tools_5% -prefs=%runtime_tools_6% -prefs=%runtime_tools_7% -prefs=%runtime_tools_8% -prefs=%runtime_tools_9% -prefs=%runtime_tools_10% %verbose% %code_dir%\%product_name%.ino
)
if errorlevel 1 (
   if [%1]==[] echo Compiler Error: %errorlevel%
   set /p exitkey= "Press any key to continue..."
   exit /b %errorlevel%
)
rem set /p exitkey= "Press any key to continue..."

exit /b 0
rem "C:\Program Files\WindowsApps\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\arduino-builder" -compile -logger=machine -hardware C:\Program Files\WindowsApps\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\hardware -hardware C:\Users\shann\OneDrive\Documents\ArduinoData\packages -tools C:\Program Files\WindowsApps\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\tools-builder -tools C:\Program Files\WindowsApps\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\hardware\tools\avr -tools C:\Users\shann\OneDrive\Documents\ArduinoData\packages -built-in-libraries C:\Program Files\WindowsApps\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\libraries -libraries C:\Users\shann\Dropbox\moog\bonkulator\code\Bonkulator\libraries -fqbn=arduino:mbed_nano:nanorp2040connect -ide-version=10819 -build-path C:\Users\shann\AppData\Local\Temp\arduino_build_827297 -warnings=default -build-cache C:\Users\shann\AppData\Local\Temp\arduino_cache_619494 -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.openocd.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2 -prefs=runtime.tools.openocd-0.11.0-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2 -prefs=runtime.tools.bossac.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2 -prefs=runtime.tools.bossac-1.9.1-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2 -prefs=runtime.tools.dfu-util.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1 -prefs=runtime.tools.dfu-util-0.10.0-arduino1.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1 -prefs=runtime.tools.arm-none-eabi-gcc.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4 -prefs=runtime.tools.arm-none-eabi-gcc-7-2017q4.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4 -prefs=runtime.tools.rp2040tools.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\rp2040tools\1.0.2 -prefs=runtime.tools.rp2040tools-1.0.2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\rp2040tools\1.0.2 -verbose C:\Users\shann\Dropbox\moog\bonkulator\code\Bonkulator\Bonkulator.ino
rem .\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\arduino-builder -compile -logger=machine -hardware .\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\hardware -hardware C:\Users\shann\OneDrive\Documents\ArduinoData\packages -tools .\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\tools-builder -tools .\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\hardware\tools\avr -tools C:\Users\shann\OneDrive\Documents\ArduinoData\packages -built-in-libraries .\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\libraries -libraries C:\Users\shann\Dropbox\moog\bonkulator\code\Bonkulator\libraries -libraries C:\Users\shann\Dropbox\moog\bonkulator\code\Bonkulator\secrets -fqbn=arduino:mbed_nano:nanorp2040connect -ide-version=10819 -build-path .C:\Users\shann\AppData\Local\Temp\arduino_build_827297 -warnings=default -build-cache C:\Users\shann\AppData\Local\Temp\arduino_cache_619494 -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.openocd.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2 -prefs=runtime.tools.openocd-0.11.0-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\openocd\0.11.0-arduino2 -prefs=runtime.tools.bossac.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2 -prefs=runtime.tools.bossac-1.9.1-arduino2.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\bossac\1.9.1-arduino2 -prefs=runtime.tools.dfu-util.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1 -prefs=runtime.tools.dfu-util-0.10.0-arduino1.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\dfu-util\0.10.0-arduino1 -prefs=runtime.tools.arm-none-eabi-gcc.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4 -prefs=runtime.tools.arm-none-eabi-gcc-7-2017q4.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arm-none-eabi-gcc\7-2017q4 -prefs=runtime.tools.rp2040tools.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\rp2040tools\1.0.2 -prefs=runtime.tools.arduinoOTA-1.2.1.path=C:\Users\shann\OneDrive\Documents\ArduinoData\packages\arduino\tools\arduinoOTA\1.2.1  C:\Users\shann\Dropbox\moog\bonkulator\code\Bonkulator\Bonkulator.ino