@echo off

set backup_folder=\back\Bonkulator\code\

echo This will backup the Bonkulator folder and related libraries

if defined BONK_BACKUP_DRIVE goto ask_num

:ask
set /p drive=What's the drive letter?
setx BONK_BACKUP_DRIVE %drive%
setx BONK_BACKUP_NUMBER ""
goto ask_num2

:ask_num
set drive=%BONK_BACKUP_DRIVE%

:ask_num2
if not defined BONK_BACKUP_NUMBER ( 
	set /p backup_number=What's the backup number for %backup_folder%?
) else (
	set backup_number=%BONK_BACKUP_NUMBER%
)

set backup_path=%drive%:%backup_folder%Bonkulator-%backup_number%

set /p ans=That's %backup_path%, eh? (y/n)

if %ans% == n goto ask

echo. >> checkin_comment.txt
echo Check-in comment for Bonkulator-%backup_number% >> checkin_comment.txt
call "C:\Program Files\Notepad++\notepad++" -n999999 checkin_comment.txt

robocopy Bonkulator %backup_path%\Bonkulator\ /e 
rem xcopy /i /exclude:excluded_from_backup.txt Bonkulator %backup_path%\Bonkulator /e
xcopy /i /exclude:excluded_from_backup.txt Updater %backup_path%\updater /e
robocopy "C:\Users\shann\Dropbox\My Web Sites\GreenfaceLabs\htdocs\wp-content\themes\gfl_theme\bonk_cp" %backup_path%\bonk_cp /e

copy *.bat %backup_path%
copy *.exe %backup_path%
copy *.uf2 %backup_path%
copy checkin_comment.txt %backup_path%

set /A backup_number=backup_number+1

setx BONK_BACKUP_NUMBER %backup_number%

pause