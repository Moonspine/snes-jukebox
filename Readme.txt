1) Introduction


1a) Description

This project contains source code for the SNES jukebox Arduino project by Moonspine.

You can build this project using either an Atmega 328p (the chip on the Arduino Uno board) or an Atmega2560 (the chip on the Arduino Mega board).
The wiring diagram for these two projects differs dramatically due to the 2560's expanded I/O capabilities. I do not have a schematic for the Mega version yet, but I will add one when I can.
If you want to try it yourself, have a look at pin_defs.h for the "Arduino Mega pin mapping."

This program has the following interesting characteristics:

- It can upload SPC files from the SD card into the SPC-700 unit and initiate playback.
- It can stream raw BRR data from the SD card into the SPC-700 unit, effectively turning it into a wave file player.
- It can read and write data using a submenu while the SPC-700 unit is running a program. This can be used to play sound effects and perform other controls that a SNES game may perform during normal operation.


1b) Hardware

The project consists of the the following parts:

1) An Atmega 328p or 2560 (the microcontrollers on the Arduino Uno and Mega boards, respectively) clocked at 16 MHz.
2) A Sony SPC-700 sound module liberated from a functional SNES unit. (Or a broken one; the sound module rarely fails so it's probably pretty safe to scrap broken SNES units from eBay.)
3) A color LCD screen with SD card attached (Adafruit product #358: https://www.adafruit.com/products/358)
4) A SNES controller for menu navigation, because nostalgia (Adafruit product #131: http://www.adafruit.com/product/131)


1c) Software

Since this does make use of libraries for the LCD screen, you'll need to get them from Adafruit's site. For convenience, these libraries are:
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
Note that if you do this, you may have to check out an earlier commit (probably around b1bfc5774a4ab44ca or earlier). I have not tested my most recent changes on the old IDE.


2) Configuration

Have a look at SNES_Jukebox/configuration.h and SNES_Jukebox/pin_defs.h for configuration options and pinout information.


3) Streaming

There are two streaming file formats which are supported:
- BRR - The SNES native BRR format, consisting of raw BRR data.
- BR2 - My own custom BRR container format which includes sample rate info and the capability for stereo sound.


3a) BR2

The better file format to use with the SNES Jukebox is the BR2 file format. As mentioned above, it allows stereo sound and song-driven control over the sample rate. To create one, follow these steps:

- Use Audacity (http://audacityteam.org/) or your favorite sound editing program to convert your music file to a *.wav or a *.mp3 file.
- Run Bin/Moonspine/wav2br2.exe on your *.wav or *.mp3 file like this:
  wav2br2 YourSong.mp3 32000 mono YourSong.br2\
- That's it! Your song is ready to drop onto the SD card and play in your SNES Jukebox!

Note that the sample rate and mono/stereo settings will be stored in the BR2 file and will be understood by the SNES Jukebox.
You can enter any arbitrary sample rate, but I recommend using 16000 or less for stereo tracks and 44100 or less for mono tracks. The SPC-700 can only accept data so fast.
Additionally, the Uno has a much harder time with this than the Mega, so the sample rate may need to be lowered on the Uno.
The reason for this is that the expanded I/O pin count on the Mega allows the code to dispense with many context switches which hobble the poor Uno.
Here is the list of tested rates:

Uno (Atmega328p):
- 32KHz mono - OK
- 44.1KHz mono - significant glitches
- 16KHz stereo - OK
- 20KHz stereo - significant glitches

Mega (Atmega2560):
- 44.1KHz mono - OK
- 16KHz stereo - OK
- 20KHz stereo - slight glitches

As you can see from this data, the Uno is perfectly fine for 16KHz stereo and 32KHz monaural songs, but it just can't quite keep up at any higher rates.
If you're building this project from scratch, I highly recommend building the Mega version. (Either version is just fine for SNES SPC file uploads, though. This only matters for BRR streaming.)

Note that the SPC-700's native sample rate is 32KHz. Oversampling may have some quality benefits due to the way the DSP's filter algorithms work, but the actual output sample rate will still only be 32KHz.


3b) BRR

If you still want to create a legacy BRR file (monaural, 16KHz as defined in configuration.h) for streaming, you can follow these steps:

 - Download Audacity (http://audacityteam.org/) or use your favorite sound editing program.
 - Open your song (mp3, wav, whatever)
 - Convert it to a mono track (in Audacity, this is under Tracks -> Mix -> Mix Stereo Down to Mono)
 - Speed it up by the appropriate amount. This is a value calculated from the sample rate of your song and the BRR_FILE_SAMPLE_RATE constant defined in configuration.h. The formula to use when calculating the final speed is essentially the sample rate of your song (usually 44100) divided by BRR_FILE_SAMPLE_RATE. For the default BRR_FILE_SAMPLE_RATE value of 16000 and an input wave file of 44.1KHz, 44100 / 16000 = 2.75625, so the final wave data must be 2.75625 as fast as the original. Now plug this number into the "Speed Multiplier" field in Audacity's "Effect -> Change Speed..." dialog.
 - Save the sped up song as a .wav file.
 - Run TPS/snesbrr.exe like this:
   snesbrr -e YourSong.wav YourSong.brr
 - This .brr file is the file you'll need to drop on the SD card to play on the jukebox.


3c) BR2 File Format

If you're curious about the BR2 file format, here's a brief description:

byte version = 1;
word sampleRate;
byte stereo; // 1 for mono, 2 for stereo
byte data[sampleRate / 16 * 9 * stereo];

The data field contains raw BRR data. For monaural tracks, this is just the same BRR data you would have rendered into a *.brr file.
For stereo tracks, this field contains both channels' data, interleaved one BRR block (9 bytes) at a time. So it would look something like this:

byte leftBlock0[9];
byte rightBlock0[9];
byte leftBlock1[9];
byte rightBlock1[9];
// ...
byte leftBlockN[9];
byte rightBlockN[9];
