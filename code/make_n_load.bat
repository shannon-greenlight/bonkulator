call ./make it
if errorlevel 1 (
   echo Compiler Error: %errorlevel%
   exit /b %errorlevel%
)

call ./load_recent