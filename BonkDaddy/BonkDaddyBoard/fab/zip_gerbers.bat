@echo off
rem powershell.bonkdaddy (get-item ./).Basename 
set file_path=.\bonkdaddy_gerbers.zip
echo Making zip file: %file_path%
del %file_path%
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.gbr
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.gbrjob
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.drl
pause
