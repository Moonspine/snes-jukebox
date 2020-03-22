#ifndef LCD_DRAW_H
#define LCD_DRAW_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


void clearLcd(Adafruit_ST7735 &lcd) {
  lcd.fillScreen(ST7735_BLACK);
}

void drawText(Adafruit_ST7735 &lcd, const char *text, int x, int y, bool inverted) {
  lcd.setCursor(x, y);
  lcd.setTextColor(inverted ? ST7735_BLACK : ST7735_WHITE);
  lcd.setTextWrap(true);
  lcd.print(text);
}

void drawText(Adafruit_ST7735 &lcd, const char *text, int x,  int y) {
  drawText(lcd, text, x, y, false);
}

void drawPgmText(Adafruit_ST7735 &lcd, const char *text, int x,  int y, bool inverted) {
  byte buffer[MAX_PGM_TEXT_LENGTH + 1];
  copyPgmString(text, buffer, MAX_PGM_TEXT_LENGTH + 1);
  drawText(lcd, (const char *) buffer, x, y, inverted);
}

void drawPgmText(Adafruit_ST7735 &lcd, const char *text, int x,  int y) {
  drawPgmText(lcd, text, x, y, false);
}

void drawByte(Adafruit_ST7735 &lcd, byte data, int x, int y) {
  char text[4] = { ' ', ' ', ' ', '\0' };
  
  byte ones = data % 10;
  text[2] = '0' + ones;
  
  if (data >= 10) {
    byte tens = (data / 10) % 10;
    text[1] = '0' + tens;
    
    if (data >= 100) {
      byte hundreds = data / 100;
      text[0] = '0' + hundreds;
    }
  }
  
  drawText(lcd, text, x, y);
}

char getHex(byte value) {
  if (value < 10) {
    return '0' + value;
  }
  return 'A' + (value - 10);
}

void drawHexByte(Adafruit_ST7735 &lcd, byte data, int x, int y) {
  char text[4] = { ' ', ' ', '\0' };
  
  byte ones = data % 16;
  text[1] = getHex(ones);
  
  byte tens = (data / 16) % 16;
  text[0] = getHex(tens);
  
  drawText(lcd, text, x, y);
}

void drawWord(Adafruit_ST7735 &lcd, word data, int x, int y) {
  char text[6] = { ' ', ' ', ' ', ' ', ' ', '\0' };
  
  int nextChar = 4;
  while (data > 0) {
    text[nextChar] = (data % 10) + '0';
    data /= 10;
    --nextChar;
  }
  
  if (nextChar == 4) {
    text[4] = '0';
  }
  
  drawText(lcd, text, x, y);
}

void drawBinaryByte(Adafruit_ST7735 &lcd, byte data, int x, int y) {
  char text[9];
  
  text[8] = 0;
  text[7] = (data & 0x01) + '0'; data >>= 1;
  text[6] = (data & 0x01) + '0'; data >>= 1;
  text[5] = (data & 0x01) + '0'; data >>= 1;
  text[4] = (data & 0x01) + '0'; data >>= 1;
  text[3] = (data & 0x01) + '0'; data >>= 1;
  text[2] = (data & 0x01) + '0'; data >>= 1;
  text[1] = (data & 0x01) + '0'; data >>= 1;
  text[0] = (data & 0x01) + '0';
  
  drawText(lcd, text, x, y);
}

void drawIcon(Adafruit_ST7735 &lcd, const uint16_t *data, int xOffset, int yOffset) {
  const uint16_t *currentData = data;
  word width = pgm_read_word(currentData++);
  word height = pgm_read_word(currentData++);
  
  for (word y = 0; y < height; ++y) {
    for (word x = 0; x < width; ++x) {
      word color = pgm_read_word(currentData++);
      lcd.drawPixel(x + xOffset, y + yOffset, color);
    }
  }
}


#endif
