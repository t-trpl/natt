# About
-  A simple x11 screen temperature scheduler

# Notice
- I have no formal education with gtk, so please excuse any glaring design flaw.

# Packages needed (probably not the exact name for your package manager) 
    cmake
    gcc
    libayatana-appindicator (3.0/dev)
    libgtk-3-dev 
    libx11 
    libxrandr 
    pkg-config

# Usage
- add times and temps to the main box denoted by {hour:minute} where hour is represented in 24hr time. temperature is represented in k, you do not need to explicitly type the k however. 
- at X time it will be set to Y temp gradually over time.
- every so often the daemon will run and check if the temperature should be changed. 
- enabling will enable the daemon and write a config file, and it will also sort all of the times so it looks prettier
- quitting will minimize to tray. you can exit with file->exit or by the tray icon -> exit
- under tools you can set a temperature if you want to preview temps
- by default it will write the config to HOME/.natt/natt.cfg. You could specifiy a different location by modifying the source code. If the path does not exist it will be created.
- you can hide the program to tray by running with flag "-h" or "--hidden" example "{abspath}/natt -h" "{abspath}/natt --hidden"
- the way i intended for this program to be used is to configure once and just run on start up with the hidden option. Note you do need to enable natt once for the daemon to run

# Building
- just run the script ./abuild.sh and the binary will be located at ./build/natt

# AUR
Avaiable on aur https://aur.archlinux.org/packages/natt

# Image
![image](https://github.com/user-attachments/assets/d9b38d00-8307-4840-871c-bd2b73ca39cb)

