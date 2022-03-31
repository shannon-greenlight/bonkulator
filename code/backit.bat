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

echo Check-in comment for Bonkulator-%backup_number% > checkin_comment.txt
call notepad checkin_comment.txt

xcopy /i /exclude:excluded_from_backup.txt Bonkulator %backup_path%\Bonkulator /e
xcopy /i /exclude:excluded_from_backup.txt Updater %backup_path%\updater /e

copy *.bat %backup_path%
copy checkin_comment.txt %backup_path%

set /A backup_number=backup_number+1

setx BONK_BACKUP_NUMBER %backup_number%

pause