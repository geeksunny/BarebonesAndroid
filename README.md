BarebonesAndroid
=============

BarebonesAndroid is a command line program that automates removing unwanted system apps from your device.

About this project
------------

This is a program to automate backing up your /system/app directory and then steps through a list of packages offering to remove them from your device. It runs on your PC and uses ADB to interface with your device.

I wrote this to automate the process of trimming a system down after flashing a new ROM.

This was based on CyanogenMod 7.  The list of apps is written into the source code and is therefore static.  Because it is based on CM7, it may not be compatible with other ROMs.

This project comes in two flavors: A Bash script and a C++ based command line application.

### Requirements

To use this you must...
* Have ADB set up correctly and talking with your device.
* Have read/write access to your device's /system folder. (Be rooted.)

Future plans
------------

This source code is currently up for archival purposes.  I do not currently have any plans to update this any further.  It will probably be rewritten and improved upon in the future.