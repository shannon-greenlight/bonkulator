@echo off

echo This will fix boot loader. Make sure drive is available.

set /p drive=What's the drive letter?

copy Blink.ino.elf.uf2 %drive%
