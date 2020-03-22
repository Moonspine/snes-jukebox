This project contains source code for the SNES jukebox Arduino project by Moonspine.

The project consists of the the following parts:

1) An Atmega 328p (the microcontroller on the Arduino Uno board) clocked at 16 MHz.
2) A Sony SPC-700 sound module liberated from a functional SNES unit.
3) A color LCD screen with SD card attached (Adafruit product #358: https://www.adafruit.com/products/358)
4) A SNES controller for menu navigation, because nostalgia (Adafruit product #131: http://www.adafruit.com/product/131)

Note: Since this does make use of libraries for the LCD screen, you'll need to get them from Adafruit's site. For convenience, these libraries are:
https://github.com/adafruit/Adafruit-ST7735-Library
https://github.com/adafruit/Adafruit-GFX-Library

You can also download them in your Arduino IDE via "Sketch -> Include Library -> Manage Libraries..."

The latest tested and fully functional versions of each software component are as follows:
- Arduino IDE 1.8.12
- Adafruit GFX library 1.7.5
- Adafruit ST7735 library 1.5.15

If you want to use an earlier version of the IDE, I originally developed this project using the following versions:
- Arduino IDE 1.0.5-r2
- Adafruit GFX library 1.0.2 (commit hash 4edc8942c46e7aeff; See https://github.com/adafruit/Adafruit-GFX-Library/commit/4edc8942c46e7aeff383d00576d71d6b9068e4a8 )
- Adafruit ST7735 library 1.0.4 (Commit hash 74238f6ea8db16f2b; See https://github.com/adafruit/Adafruit-ST7735-Library/commit/74238f6ea8db16f2bfe76e7aee852d536e158e50 )
(You can safely compile the code with either version, and I have tested that it works just fine.)

This program has the following interesting characteristics:

- It can upload SPC files from the SD card into the SPC-700 unit and initiate playback.
- It can stream raw BRR data from the SD card into the SPC-700 unit, effectively turning it into a wave file player.
- It can read and write data using a submenu while the SPC-700 unit is running a program. This can be used to play sound effects and perform other controls that a SNES game may perform during normal operation.


Converting music for streaming on the SPC-700:

 - Download Audacity (http://audacityteam.org/) or use your favorite sound editing program.
 - Open your song (mp3, wav, whatever)
 - Convert it to a mono track (in Audacity, this is under Tracks -> Mix -> Mix Stereo Down to Mono)
 - Speed it up by the appropriate amount. This is a value calculated from the sample rate of your song. The SNES uses a 32 KHz sample rate, and the playback program I've written halves that (mainly due to data throughput issues.) So the formula to use when calculating the final speed is essentially the sample rate of your song (usually 44100) divided by 16000. For example, for 44100, 44100 / 16000 = 2.75625, so the final wave data must be 2.75625 as fast as the original. Now plug this number into the "Speed Multiplier" field in Audacity's "Effect -> Change Speed..." dialog.
 - Save the sped up song as a .wav file.
 - Run TPS/snesbrr.exe like this:
   snesbrr -e YourSong.wav YourSong.brr
 - This .brr file is the file you'll need to drop on the SD card to play on the jukebox.
