@echo off
set file_path=.\bonkdaddy_panel2hp_gerbers.zip
echo Making zip file: %file_path%
del %file_path%
pause 1
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.gbr
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.gbrjob
"C:\Program Files\7-Zip\7z.exe" a %file_path% .\gerber\*.drl
pause

