#ifndef PORT_WRITE_MENU_H
#define PORT_WRITE_MENU_H

#include "jukebox_io.h"
#include "snes_apu.h"
#include "text.h"
#include "lcd_draw.h"
#include "utils.h"
#include "snes_controller.h"

#define READS_Y 50
#define WRITES_Y 110
#define INITIAL_REPEAT_DELAY 500
#define REPEAT_DELAY 150
#define MAX_FRAME_DELAY 33 // 30 FPS

class PortWriteMenu {
public:

  PortWriteMenu() {
    currentSelection = 0;
    debounceDelay = 0;
  }
  
  void setLastWrittenPorts(byte port0, byte port1, byte port2, byte port3) {
    originalPorts[0] = lastWrittenPorts[0] = currentlyWritingPorts[0] = port0;
    originalPorts[1] = lastWrittenPorts[1] = currentlyWritingPorts[1] = port1;
    originalPorts[2] = lastWrittenPorts[2] = currentlyWritingPorts[2] = port2;
    originalPorts[3] = lastWrittenPorts[3] = currentlyWritingPorts[3] = port3;
  }

  void initialize(Adafruit_ST7735 &lcd) {
    drawMenu(lcd);
    lastUpdateTimestamp = (word)(millis() & 0xFFFF);
    frameDelay = 0;
  }
  
  void drawMenu(Adafruit_ST7735 &lcd) {
    beginLcdWrite();
    clearLcd(lcd);
    drawPgmText(lcd, TEXT_PORT_WRITE_MENU, 0, 0);
    drawPgmText(lcd, TEXT_PORT_NUMBERS, 0, 20);
    drawPgmText(lcd, TEXT_CURRENTLY_READING, 0, 40);
    drawPgmText(lcd, TEXT_CURRENTLY_WRITTEN, 0, 70);
    drawPgmText(lcd, TEXT_CURRENTLY_ORIGINAL, 0, 80);
    drawPgmText(lcd, TEXT_CURRENTLY_LAST, 0, 90);
    drawPgmText(lcd, TEXT_CURRENTLY_WRITE, 0, WRITES_Y);
    endLcdWrite();
    
    drawPortReads(lcd, READS_Y, true);
    drawOriginalPorts(lcd, 80);
    drawLastWritten(lcd, 90);
    drawWritingPorts(lcd, WRITES_Y);

    drawSelectionReticule(lcd, WRITES_Y);
    
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
    word elapsedTime = computeElapsedTime(lastUpdateTimestamp);
    
    if (frameDelay > 0) {
      if (frameDelay < elapsedTime) {
        frameDelay = 0;
      } else {
        frameDelay -= (byte) elapsedTime;
      }
    }
    if (frameDelay == 0) {
      drawPortReads(lcd, READS_Y, false);
      frameDelay = MAX_FRAME_DELAY;
    }
    
    if (debounceDelay > 0) {
      if (debounceDelay < elapsedTime) {
        debounceDelay = 0;
      } else {
        debounceDelay -= elapsedTime;
      }
    }

    const byte increment = controller.isPressed(SNESController::X) ? 16 : 1;
    if (debounceDelay == 0) {
      // Update selection stuff
      byte lastSelection = currentSelection;
      if (controller.justPressed(SNESController::UP)) {
        currentlyWritingPorts[currentSelection] += increment;
        drawWritingPorts(lcd, WRITES_Y);
        timeUntilRepeat = INITIAL_REPEAT_DELAY;
        debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
      } else if (controller.justPressed(SNESController::DOWN)) {
        currentlyWritingPorts[currentSelection] -= increment;
        drawWritingPorts(lcd, WRITES_Y);
        timeUntilRepeat = INITIAL_REPEAT_DELAY;
        debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
      } else if (controller.justPressed(SNESController::LEFT)) {
        --currentSelection;
        if (currentSelection > 3) {
          currentSelection = 3;
        }
        debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
      } else if (controller.justPressed(SNESController::RIGHT)) {
        currentSelection = (currentSelection + 1) % 4;
        debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
      }

      if (lastSelection != currentSelection) {
        drawSelectionReticule(lcd, WRITES_Y);
      }
      
      if (controller.justPressed(SNESController::A)) {
        writedata(currentSelection, currentlyWritingPorts[currentSelection]);
        lastWrittenPorts[currentSelection] = currentlyWritingPorts[currentSelection];
        drawLastWritten(lcd, 90);
        debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
      } else if (controller.justPressed(SNESController::Y)) {
        writedata(currentSelection, currentlyWritingPorts[currentSelection]);
        drawTemporaryWritingPorts(lcd, 90);
        debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
      } else if (controller.justPressed(SNESController::B)) {
        currentlyWritingPorts[currentSelection] = lastWrittenPorts[currentSelection];
        drawWritingPorts(lcd, WRITES_Y);
        debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
      }
    }
    
    if (controller.isPressed(SNESController::UP) || controller.isPressed(SNESController::DOWN)) {
      timeUntilRepeat -= elapsedTime;
      if (timeUntilRepeat < 0) {
        if (controller.isPressed(SNESController::UP)) {
          currentlyWritingPorts[currentSelection] += increment;
          drawWritingPorts(lcd, WRITES_Y);
        } else {
          currentlyWritingPorts[currentSelection] -= increment;
          drawWritingPorts(lcd, WRITES_Y);
        }
        timeUntilRepeat = REPEAT_DELAY;
      }
    }
    
    if (controller.justReleased(SNESController::Y)) {
      writedata(currentSelection, lastWrittenPorts[currentSelection]);
      drawLastWritten(lcd, 90);
      debounceDelay = CONTROLLER_DEBOUNCE_DELAY;
    }
  }

private:

  byte currentSelection;
  
  byte lastPortReads[4];
  byte originalPorts[4];
  byte lastWrittenPorts[4];
  byte currentlyWritingPorts[4];
  
  int timeUntilRepeat;
  word debounceDelay;
  word lastUpdateTimestamp;
  byte frameDelay;
  
  void drawPorts(Adafruit_ST7735 &lcd, byte y, const byte *ports) {
    beginLcdWrite();
    lcd.fillRect(40, y, 88, 10, ST7735_BLACK);
    drawHexByte(lcd, ports[0], 40, y);
    drawHexByte(lcd, ports[1], 58, y);
    drawHexByte(lcd, ports[2], 76, y);
    drawHexByte(lcd, ports[3], 94, y);
    endLcdWrite();
  }
  
  void drawPortReads(Adafruit_ST7735 &lcd, byte y, bool forceRedraw) {
    byte reads[4];
    
    bool shouldRedraw = forceRedraw;
    for (byte i = 0; i < 4; ++i) {
      byte newData = readdata(i);
      if (newData != lastPortReads[i]) {
        shouldRedraw = true;
        lastPortReads[i] = newData;
      }
    }
    
    if (shouldRedraw) {
      drawPorts(lcd, y, lastPortReads);
    }
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
    for (byte i = 0; i < 4; ++i) {
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
    for (byte i = 0; i < 4; ++i) {
      word color = (i == currentSelection) ? ST7735_RED : ST7735_BLACK;
      const byte x0 = 40 + i * 18 - 2;
      const byte y0 = y - 2;
      //lcd.drawLine(x0, y0, x0 + 16, y0, color);
      lcd.drawLine(x0, y0 + 14, x0 + 16, y0 + 14, color);
    }
    endLcdWrite();
  }
};

#endif
