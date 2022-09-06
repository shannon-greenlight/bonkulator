# Bonkulator-Code
This repository contains the elements needed to build code for the Bonkulator. 
The code is for the Arduino RP2040 Connect and is found in the Bonkulator folder. Make sure to get the required libraries from the Arduino-Libs repo.

The build files are for Windows. They are simple .bat files but depend on having a working Arduino development system.
Certain variables must be edited to adapt these files to a particular installation. 

Here is a list of the batch files and folders:
 
- make.bat ... compiles the code
- make_n_load.bat ... calls make.bat then uploads the code to the Bonkulator
- load_recent.bat ... loads most recent compile
- list_ports.bat ... lists available COM ports
- release.bat ... used to update release number in the code and create release binary zip
- out ... this folder must be created. It holds compile output