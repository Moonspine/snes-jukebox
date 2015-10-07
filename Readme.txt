This project contains source code for the SNES jukebox Arduino project by Moonspine.

I will take the time to do a full writeup in the coming weeks, but the project consists of the the following parts:

1) An Atmega 328p (the microcontroller on the Arduino Uno board) clocked at 16 MHz.
2) A Sony SPC-700 sound module liberated from a functional SNES unit.
3) A color LCD screen with SD card attached (Adafruit product #358: https://www.adafruit.com/products/358)

Note: Since this does make use of libraries for the LCD screen, you'll need to get them from Adafruit's site. See product 358's description for details.


This program has the following interesting characteristics:

- It can upload SPC files from the SD card into the SPC-700 unit and initiate playback.
- It can stream raw BRR data from the SD card into the SPC-700 unit, effectively turning it into a wave file player.
- It can read and write data using a submenu while the SPC-700 unit is running a program. This can be used to play sound effects and perform other controls that a SNES game may perform during normal operation.


Known issues:

- The port read/write menu is presently disabled due to memory issues. I will re-enable it upon completion of planned code cleanup and refactoring.


The future:

- Eventually, I'd like to complete everything in the TODO.txt list.
- As previously mentioned, I will be posting a writeup on the construction of the device (complete with pictures and schematics). The timeframe on this is presently unknown, but I'll try to make it a priority.
