This project contains source code for the SNES jukebox Arduino project by Moonspine.

The project consists of the the following parts:

1) An Atmega 328p (the microcontroller on the Arduino Uno board) clocked at 16 MHz.
2) A Sony SPC-700 sound module liberated from a functional SNES unit.
3) A color LCD screen with SD card attached (Adafruit product #358: https://www.adafruit.com/products/358)
4) A SNES controller for menu navigation, because nostalgia (Adafruit product #131: http://www.adafruit.com/product/131)

Note: Since this does make use of libraries for the LCD screen, you'll need to get them from Adafruit's site. For convenience, these libraries are:
https://github.com/adafruit/Adafruit-ST7735-Library
https://github.com/adafruit/Adafruit-GFX-Library

Additionally, I do not have it working with the latest Arduino IDE as of yet. I may fix this in the future, but to run this program correctly, you'll need to download a 1.0.x version of the IDE (I tested this on 1.0.5-r2).


This program has the following interesting characteristics:

- It can upload SPC files from the SD card into the SPC-700 unit and initiate playback.
- It can stream raw BRR data from the SD card into the SPC-700 unit, effectively turning it into a wave file player.
- It can read and write data using a submenu while the SPC-700 unit is running a program. This can be used to play sound effects and perform other controls that a SNES game may perform during normal operation.


Converting music for streaming on the SPC-700:

 - Download Audacity (http://audacityteam.org/) or use your favorite sound editing program.
 - Open your song (mp3, wav, whatever)
 - Convert it to a mono track (in Audacity, this is under Tracks -> Stereo Track to Mono
 - Speed it up by the appropriate amount. This is a value calculated from the sample rate of your song. The SNES uses a 32 KHz sample rate, and the playback program I've written halves that (mainly due to data throughput issues.) So the formula to use when calculating the final speed is essentially the sample rate of your song (usually 44100) divided by 16000. For example, for 44100, 44100 / 16000 = 2.75625, so the final wave data must be 2.75625 as fast as the original. Audacity uses a relative (rather than an absolute) speed value, so you need to subtract 1 from that (to get 1.75625). It also uses a percent, so you need to multiply it by 100 (to get 175.625). Now plug this number into Audacity's "Effect -> Change Speed..." dialog.
 - Save the sped up song as a .wav file.
 - Run TPS/snesbrr.exe like this:
   snesbrr -e YourSong.wav YourSong.brr
 - This .brr file is the file you'll need to drop on the SD card to play on the jukebox.


Known issues:

- The port read/write menu is presently disabled due to memory issues. I will re-enable it upon completion of planned code cleanup and refactoring.


The future:

- Eventually, I'd like to complete everything in the TODO.txt list.
