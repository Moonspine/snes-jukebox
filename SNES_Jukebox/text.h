#ifndef TEXT_H
#define TEXT_H

#define MAX_PGM_TEXT_LENGTH 21

// BRR Streaming
static PROGMEM const char TEXT_UPLOADING_LOADER[] = "Upoading loader...";
static PROGMEM const char TEXT_LOADER_UPLOADED[] = "Loader uploaded!";
static PROGMEM const char TEXT_STREAMING[] = "Streaming:";
static PROGMEM const char TEXT_PRESS_B_TO_STOP[] = "Press 'B' to stop";
static PROGMEM const char TEXT_PRESS_A_TO_PAUSE[] = "Press 'A' to pause";
static PROGMEM const char TEXT_PRESS_A_TO_UNPAUSE[] = "Press 'A' to unpause";
static PROGMEM const char TEXT_PRESS_X_TO_TOGGLE_LOOP[] =   "'X' to toggle loop";
static PROGMEM const char TEXT_PRESS_LEFT_RIGHT_TO_SEEK[] = "Press < or > to seek";
static PROGMEM const char TEXT_MONO[] = "Monaural";
static PROGMEM const char TEXT_STEREO[] = "Stereo";

// SPC loading
static PROGMEM const char TEXT_SELECT_FILE[] = "-- Select SPC, BRR --";
static PROGMEM const char TEXT_LOADING_SPC[] = "Loading SPC:";
static PROGMEM const char TEXT_SONG[] = "Song:";
static PROGMEM const char TEXT_GAME[] = "Game:";
static PROGMEM const char TEXT_COMPOSER[] = "Composer:";
static PROGMEM const char TEXT_UNKNOWN[] = "Unknown:";

// Port write menu
static PROGMEM const char TEXT_PORT_WRITE_MENU[] = "- Port write menu -";
static PROGMEM const char TEXT_PORT_NUMBERS[] = "Port#   0  1  2  3";
static PROGMEM const char TEXT_CURRENTLY_READING[] = "Currently reading";
static PROGMEM const char TEXT_CURRENTLY_WRITTEN[] = "Currently written";
static PROGMEM const char TEXT_CURRENTLY_ORIGINAL[] = "Orig:";
static PROGMEM const char TEXT_CURRENTLY_LAST[] = "Last:";
static PROGMEM const char TEXT_CURRENTLY_WRITE[] = "Write:";

void copyPgmString(const char *source, byte *dest, word charLimit) {
  word index = 0;
  while ((dest[index++] = pgm_read_byte(source++)) != 0 && index < charLimit) {}
  dest[charLimit - 1] = 0;
}

#endif
