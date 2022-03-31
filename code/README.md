# Spankulator-Code
This repository contains the elements needed to build code for the Spankulator. 
The code is for the Arduino Nano 33 IoT and is found in the Spankulator folder.

The build files are for Windows. They are simple .bat files but depend on having a working Arduino development system.
Certain variables must be edited to adapt these files to a particular installation. 

Here is a list of the batch files and folders:
 
- make.bat ... compiles the code
- make_n_load.bat ... calls make.bat then uploads the code to the Spankulator
- load_recent.bat ... loads most recent compile
- list_ports.bat ... lists available COM ports
- release.bat ... used to update release number in the code and create release binary zip
- out ... this folder must be created. It holds compile output