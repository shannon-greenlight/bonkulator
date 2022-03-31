@echo off

echo This will compare a backup to the Spankulator folder

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
	set /p backup_number=What's the backup number?
) else (
	set backup_number=%BONK_BACKUP_NUMBER%
	set /a backup_number -= 1
)

"C:\Program Files\Beyond Compare 4\BComp.com" %drive%:\back\Bonkulator\code\Bonkulator-%backup_number% .

