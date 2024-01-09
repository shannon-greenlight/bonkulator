# Bonkulator

The Bonkulator is a Eurorack module that fits into a 3U x 14HP slot. It features 8 programmable outputs and 6 programmable inputs. The front-panel user controls consist of a 128×64 pixel 2-color OLED display, a rotary encoder for adjusting parameters and a button for each trigger and output.

# main_board, panel

These repos contain the materials needed to construct the Bonkulator main board and the front panel.
The files are for KiCAD 6.0. Go to the Greenlight Kicad repo to get the needed libraries.
The gerber files work with JLCPC and will most likely work with other board manufacturers.

Assembly instructions are in main_board\fab\bonkulator_assembly.docx.

When using the JLCPCB assembly service, the BOM is split into 3 parts.

- bom/lcsc/bonkulator_bom.xlsx : The parts to be assembled by JLCPCB
- bom/digi-key/bonkulator_bom.xlsx : The remaining parts to be assembled by user
- bom/other/other_bom.xlsx : The parts that must be bought from sources other than JLCPCB and Digi-Key

Also, when an assembly service is used, a component position file is required. This file's path is: fab/Bonkulator_cpl.xlsx

notes: The bom and cpl files are auto-generated from the csv BOM files produced by KiCAD. You will need to refresh the data when changes to the schematic that affect the BOM or changes to the board layout occur.
If they exist, the extra parts and additional data fields that appear on the schematic help to produce the different BOM files. They must be maintained for these BOMs to come out right.

# code

This repository contains the elements needed to build code for the Bonkulator.

# BonkDaddy

The Bonk Daddy is an addition to the Bonkulator that provides these benefits:

- Front Panel USB access
- Power sensing shuts off USB to Arduino when synth power is off
- Reset switch
- USB Power indicator
- Trig In indicators
- Boot Loader indicator
- Extremely useful to those who use Terminal Remote mode

The folder structure of the Bonk Daddy is similar to the Bonkulator Hardware folder, main_board. Currently there is no support for assembly service.
