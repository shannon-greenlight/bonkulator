@echo off
set file_path=.\bonkdaddy2hp_gerbers.zip
echo Making zip file: %file_path%
del %file_path%
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.gbr
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.gbrjob
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.drl
pause
