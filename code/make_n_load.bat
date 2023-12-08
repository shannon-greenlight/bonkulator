@echo off
setlocal

:: Start the stopwatch
for /f %%i in ('powershell -Command "(Get-Date).Ticks"') do set start=%%i

:: Call the command
call ./make

:: Stop the stopwatch
for /f %%i in ('powershell -Command "(Get-Date).Ticks"') do set end=%%i

:: Calculate the elapsed time and convert it to HH:MM:SS format
for /f "tokens=1-3 delims=:." %%a in ('powershell -Command "$elapsed = [timespan]::FromTicks(%end% - %start%); $elapsed.ToString('hh\:mm\:ss')"') do (
   set hh=%%a
   set mm=%%b
   set ss=%%c
)

:: Print the elapsed time
echo.
echo Make took this much time: %hh%:%mm%:%ss%

if errorlevel 1 (
   echo Compiler Error: %errorlevel%
   exit /b %errorlevel%
)

call ./load_recent