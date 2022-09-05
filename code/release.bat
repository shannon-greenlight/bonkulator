@echo off

set file_path=.\Bonkulator\version_num.h
set file_path2=.\Bonkulator\is_release.h

echo This will set the Bonkulator software release number in %file_path%

if not defined BONK_RELEASE_NUM goto ask

echo The current release number is: %BONK_RELEASE_NUM%

:ask
set /p release_num=What's the NEW software release number?

set /p ans=That's %release_num%, eh? (y/n)

if %ans% == n goto end
setx BONK_RELEASE_NUM %release_num%

echo // *** Warning! *** Do not edit this file. Use release.bat instead. > %file_path%
echo #ifndef VERSION_NUM_h >> %file_path%
echo #define VERSION_NUM_h >> %file_path%
echo. >> %file_path%
echo #define VERSION_NUM "v%release_num%" >> %file_path%
echo. >> %file_path%
echo #endif >> %file_path%

echo // *** Warning! *** Do not edit this file. release.bat sets value. > %file_path2%
echo #ifndef IS_RELEASE_h >> %file_path2%
echo #define IS_RELEASE_h >> %file_path2%
echo. >> %file_path2%
echo #define IS_RELEASE true >> %file_path2%
echo. >> %file_path2%
echo #endif >> %file_path2%

call make.bat

echo // *** Warning! *** Do not edit this file. release.bat sets value. > %file_path2%
echo #ifndef IS_RELEASE_h >> %file_path2%
echo #define IS_RELEASE_h >> %file_path2%
echo. >> %file_path2%
echo #define IS_RELEASE false >> %file_path2%
echo. >> %file_path2%
echo #endif >> %file_path2%

copy .\out\Bonkulator.ino.bin updater
copy .\out\Bonkulator.ino.elf updater
copy Blink.ino.elf.uf2 updater
echo %release_num% > updater\release.txt

"C:\Program Files\7-Zip\7z.exe" a -r .\releases\bonkulator_updater_%release_num%.zip updater 

:end
echo Goodbye!

pause

