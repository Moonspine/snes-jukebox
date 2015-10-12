#ifndef PORT_WRITE_MENU_H
#define PORT_WRITE_MENU_H

#include "jukebox_io.h"
#include "snes_apu.h"
#include "lcd_draw.h"
#include "snes_controller.h"

static PROGMEM prog_uchar READS_Y[] = { 50 };
static PROGMEM prog_uchar WRITES_Y[] = { 110 };
static PROGMEM prog_uint16_t INITIAL_REPEAT_DELAY[] = { 500 };
static PROGMEM prog_uint16_t REPEAT_DELAY[] = { 150 };

class PortWriteMenu {
public:

  PortWriteMenu() {
    currentSelection = 0;
  }
  
  void setLastWrittenPorts(byte port0, byte port1, byte port2, byte port3) {
    originalPorts[0] = lastWrittenPorts[0] = currentlyWritingPorts[0] = port0;
    originalPorts[1] = lastWrittenPorts[1] = currentlyWritingPorts[1] = port1;
    originalPorts[2] = lastWrittenPorts[2] = currentlyWritingPorts[2] = port2;
    originalPorts[3] = lastWrittenPorts[3] = currentlyWritingPorts[3] = port3;
  }

  void initialize(Adafruit_ST7735 &lcd) {
    drawMenu(lcd);
  }
  
  void drawMenu(Adafruit_ST7735 &lcd) {
    beginLcdWrite();
    clearLcd(lcd);
    drawText(lcd, "- Port write menu -", 0, 0);
    drawText(lcd, "Port#   0  1  2  3", 0, 20);
    drawText(lcd, "Currently reading", 0, 40);
    drawText(lcd, "Currently written", 0, 70);
    drawText(lcd, "Orig:", 0, 80);
    drawText(lcd, "Last:", 0, 90);
    drawText(lcd, "Write:", 0, pgm_read_byte(WRITES_Y));
    endLcdWrite();
    
    drawPortReads(lcd, pgm_read_byte(READS_Y));
    drawOriginalPorts(lcd, 80);
    drawLastWritten(lcd, 90);
    drawWritingPorts(lcd, pgm_read_byte(WRITES_Y));
    
    // - Port write menu -
    //
    // Port#   0  1  2  3
    //
    // Currently reading
    //        ff ff ff ff
    //
    // Currently written
    // Orig:  ff ff ff ff
    // Last:  ff ff ff ff
    //
    // Write: ff ff ff ff
    //
  }
  
  void update(Adafruit_ST7735 &lcd, SNESController &controller) {
    drawPortReads(lcd, pgm_read_byte(READS_Y));
    
    // Update selection stuff
    if (controller.justPressed(SNESController::UP)) {
      currentlyWritingPorts[currentSelection]++;
      drawWritingPorts(lcd, pgm_read_byte(WRITES_Y));
      lastRepeatTimestamp = millis() & 0xffff;
      timeUntilRepeat = pgm_read_word(INITIAL_REPEAT_DELAY);
    } else if (controller.justPressed(SNESController::DOWN)) {
      currentlyWritingPorts[currentSelection]--;
      drawWritingPorts(lcd, pgm_read_byte(WRITES_Y));
      lastRepeatTimestamp = millis() & 0xffff;
      timeUntilRepeat = pgm_read_word(INITIAL_REPEAT_DELAY);
    } else if (controller.justPressed(SNESController::LEFT)) {
      --currentSelection;
      if (currentSelection > 3) currentSelection = 3;
    } else if (controller.justPressed(SNESController::RIGHT)) {
      currentSelection = (currentSelection + 1) % 4;
    }
    
    if (controller.isPressed(SNESController::UP) || controller.isPressed(SNESController::DOWN)) {
      word currentRepeatTimestamp = millis() & 0xffff;
      
      timeUntilRepeat -= currentRepeatTimestamp - lastRepeatTimestamp;
      if (timeUntilRepeat < 0) {
        if (controller.isPressed(SNESController::UP)) {
          currentlyWritingPorts[currentSelection]++;
          drawWritingPorts(lcd, pgm_read_byte(WRITES_Y));
        } else {
          currentlyWritingPorts[currentSelection]--;
          drawWritingPorts(lcd, pgm_read_byte(WRITES_Y));
        }
        timeUntilRepeat = pgm_read_word(REPEAT_DELAY);
      }
      
      lastRepeatTimestamp = currentRepeatTimestamp;
    }
    
    drawSelectionReticule(lcd, pgm_read_byte(WRITES_Y));
    
    if (controller.justPressed(SNESController::A)) {
      writedata(currentSelection, currentlyWritingPorts[currentSelection]);
      lastWrittenPorts[currentSelection] = currentlyWritingPorts[currentSelection];
      drawLastWritten(lcd, 90);
    } else if (controller.justPressed(SNESController::Y)) {
      writedata(currentSelection, currentlyWritingPorts[currentSelection]);
      drawTemporaryWritingPorts(lcd, 90);
    } else if (controller.justReleased(SNESController::Y)) {
      writedata(currentSelection, lastWrittenPorts[currentSelection]);
      drawLastWritten(lcd, 90);
    }
    
  }

private:

  byte currentSelection;
  
  byte originalPorts[4];
  byte lastWrittenPorts[4];
  byte currentlyWritingPorts[4];
  
  word lastRepeatTimestamp;
  int timeUntilRepeat;
  
  void drawPorts(Adafruit_ST7735 &lcd, byte y, const byte *ports) {
    beginLcdWrite();
    lcd.fillRect(40, y, 88, 10, ST7735_BLACK);
    drawHexByte(lcd, ports[0], 40, y);
    drawHexByte(lcd, ports[1], 58, y);
    drawHexByte(lcd, ports[2], 76, y);
    drawHexByte(lcd, ports[3], 94, y);
    endLcdWrite();
  }
  
  void drawPortReads(Adafruit_ST7735 &lcd, byte y) {
    byte reads[4];
    
    for (byte i = 0; i < 4; i++) {
      reads[i] = readdata(i);
    }
    drawPorts(lcd, y, reads);
  }
  
  void drawLastWritten(Adafruit_ST7735 &lcd, byte y) {
    drawPorts(lcd, y, lastWrittenPorts);
  }
  
  void drawOriginalPorts(Adafruit_ST7735 &lcd, byte y) {
    drawPorts(lcd, y, originalPorts);
  }
  
  void drawWritingPorts(Adafruit_ST7735 &lcd, byte y) {
    drawPorts(lcd, y, currentlyWritingPorts);
  }
  
  void drawTemporaryWritingPorts(Adafruit_ST7735 &lcd, byte y) {
    byte values[4];
    for (byte i = 0; i < 4; i++) {
      if (i == currentSelection) {
        values[i] = currentlyWritingPorts[i];
      } else {
        values[i] = lastWrittenPorts[i];
      }
    }
    drawPorts(lcd, y, values);
  }
  
  void drawSelectionReticule(Adafruit_ST7735 &lcd, byte y) {
    beginLcdWrite();
    for (byte i = 0; i < 4; i++) {
      word color = (i == currentSelection) ? ST7735_BLUE : ST7735_BLACK;
      lcd.drawRect(40 + i * 18 - 2, y - 2, 16, 14, color);
    }
    endLcdWrite();
  }
};

#endif

