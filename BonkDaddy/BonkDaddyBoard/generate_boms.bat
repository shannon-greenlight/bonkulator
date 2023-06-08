@echo off

echo This creates BOM files for building the BonkDaddy
echo It requires BonkDaddyBoard.xml as an input. Make sure that it is up to date. Use the KiCAD schematic BOM generator with the bom2grouped_csv.xsl plugin that's supplied with KiCAD.

set bin_dir=C:\Program Files\KiCad\bin
set plugin_dir=C:\Users\shann\Dropbox\KiCAD\BOM
set base_file=BonkDaddyBoard

rem todo fill in LCSC part numbers in the schematic. Then assy service can be used.
rem "%bin_dir%\python" "%plugin_dir%/bom_lcsc.py" "%base_file%.xml" "bom\LCSC\%base_file%_bom.csv"

rem "%bin_dir%\python" "%plugin_dir%/bom_digikey_split.py" "%base_file%.xml" "bom\Digi-Key\%base_file%_split.csv"

"%bin_dir%\python" "%plugin_dir%/bom_digikey_full.py" "%base_file%.xml" "bom\Digi-Key\%base_file%_full.csv"

if not errorlevel 1 echo Success!

pause
