@echo off

echo This creates BOM files for building the BonkDaddy2HP
echo It requires BonkDaddy2HP_Board.xml as an input. Make sure that it is up to date. Use the KiCAD schematic BOM generator with the bom2grouped_csv.xsl plugin that's supplied with KiCAD.

set bin_dir=C:\Program Files\KiCad\bin
set plugin_dir=C:\Users\shann\Dropbox\KiCAD\BOM
set base_file=BonkDaddy2HP_Board

"%bin_dir%\python" "%plugin_dir%/bom_lcsc.py" "%base_file%.xml" "bom\LCSC\%base_file%_bom.csv"

"%bin_dir%\python" "%plugin_dir%/bom_digikey_split.py" "%base_file%.xml" "bom\Digi-Key\%base_file%_split.csv"

"%bin_dir%\python" "%plugin_dir%/bom_digikey_full.py" "%base_file%.xml" "bom\Digi-Key\%base_file%_full.csv"

if not errorlevel 1 echo Success!

pause
