# Bonkulator-Code

This repository contains the elements needed to build code for the Bonkulator.
The code is for the Arduino RP2040 Connect and is found in the code/Bonkulator folder. Make sure to get the required libraries from the Arduino-Libs repo.
The code package is designed to be stand-alone and work with any IDE. You don't need the Arduino IDE. However, it will work with the Arduino IDE if you want to use it.

Build files are provided for Windows and Mac.

Here is a list of the batch and shell files:

- make.bat, make.sh ... compiles the code
- make_n_load.bat, make_n_load.sh ... calls make.bat then uploads the code to the Bonkulator
- load_recent.bat, load_recent.sh ... loads most recent compile
- list_ports.bat, list_ports.sh ... lists available COM ports
- release.bat, release.sh ... used to update release number in the code and create zip archives in the releases folder

Folders:

- out ... this folder must be created. It holds compile output
- releases ... this folder contains all of the archived releases
- updater ... this folder contains files needed by user to update their modules. This is what is zipped into the archive files.
- bin ... this folder contains executables for the Mac
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
