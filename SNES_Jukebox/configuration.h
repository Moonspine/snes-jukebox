#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/*
 * Define this if your LCD colors have red and blue flipped.
 * It's supposed to be taken care of by the INITR_* constants for the LCD, but I have an LCD with a green tab that operates better with the black tab constant so... go figure.
 */
//#define FLIP_COLOR_R_B

/*
 * You may need to change this line depending on the specific LCD you're using (See the Adafruit documentation for more information).
 * Valid values are:
 *  INITR_BLACKTAB
 *  INITR_GREENTAB
 */
#define LCD_TYPE INITR_BLACKTAB

/*
 * The playback sample rate for raw *.brr files.
 * This does not affect *.br2 playback.
 */
#define BRR_FILE_SAMPLE_RATE 16000

#endif
