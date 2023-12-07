# Bonkulator

The Bonkulator is a Eurorack module that fits into a 3U x 14HP slot. It features 8 programmable outputs and 6 programmable inputs. The front-panel user controls consist of a 128×64 pixel 2-color OLED display, a rotary encoder for adjusting parameters and a button for each trigger and output.

# main_board, panel

Main Board in the release is on hold. New release coming soon! The current files haven't been made into a board yet. A release will be created after the boards have been built and tested.

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
The code is for the Arduino RP2040 Connect and is found in the code/Bonkulator folder. Make sure to get the required libraries from the Arduino-Libs repo.

Build files are provided for Windows and the Mac.

Here is a list of the batch and zshell files:

- make.bat, make.sh ... compiles the code
- make_n_load.bat, make_n_load.sh ... calls make.bat then uploads the code to the Bonkulator
- load_recent.bat, load_recent.sh ... loads most recent compile
- list_ports.bat ... lists available COM ports
- release.bat ... used to update release number in the code and create release binary zip

Folders:

- out ... this folder must be created. It holds compile output
- Bonkulator ... folder where the code exists
- secrets_example ... copy this folder to Bonkulator and rename it: secrets. Edit secrets.h to enter your preferred WiFi network and password.

The Arduino libraries from the Arduino-Libs repo must be included. Put them in the 'code' folder in a folder named 'libraries'.
Copy the entire contents of the folder, not the folder itself.

If you are using a Mac, you must call the .sh files from a Terminal window. Use the form ./filename.sh to execute the .sh file.
Use the following instructions to open a Terminal window in the code folder:

1. Open Finder.
2. Navigate to the directory where you want to open the Terminal.
3. Click to select the folder.
4. From the menu bar at the top of the screen, click on Finder.
5. In the dropdown menu, hover over Services.
6. In the Services menu, click on New Terminal at Folder.

This will open a new Terminal window that's already navigated to the directory you selected in Finder. If you don't see the New Terminal at Folder option, you may need to enable it in System Preferences -> Keyboard -> Shortcuts -> Services.

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
