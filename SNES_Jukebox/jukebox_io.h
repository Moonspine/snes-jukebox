#ifndef JUKEBOX_IO_H
#define JUKEBOX_IO_H

#include "pin_defs.h"

#if defined(ARDUINO_AVR_UNO)
  bool isSdStarted = false;

  void setupAPUPins() {
    // Prepare I/O pins
    pinMode(PIN_LCD_CLK, INPUT_PULLUP);
    pinMode(PIN_LCD_C_D, INPUT_PULLUP);
    pinMode(PIN_LCD_CS, INPUT_PULLUP);
    
    pinMode(PIN_SD_MISO, INPUT_PULLUP);
    pinMode(PIN_SD_CS, INPUT_PULLUP);
    
    pinMode(PIN_CONTROLLER_DATA, INPUT_PULLUP);

    PORT_DIR_APU_D0_1_CTRL |= 0x0F;
    PORT_OUT_APU_D0_1_CTRL |= 0x0F;
    pinMode(PIN_APU_RST, OUTPUT);
    digitalWrite(PIN_APU_RST, HIGH);
    
    // Deselect SD card
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);
    
    // Select APU
    pinMode(PIN_APU_A7, OUTPUT);
    digitalWrite(PIN_APU_A7, LOW);
    
    // LCD init
    pinMode(PIN_LCD_CS, OUTPUT);
    digitalWrite(PIN_LCD_CS, HIGH);
    pinMode(PIN_LCD_C_D, OUTPUT);
    digitalWrite(PIN_LCD_C_D, LOW);
  }
  
  void beginSdRead() {
    digitalWrite(PIN_APU_A7, HIGH);
    pinMode(PIN_SD_MOSI, OUTPUT);
    pinMode(PIN_SD_CLK, OUTPUT);

    if (isSdStarted) {
      pinMode(PIN_SD_CS, OUTPUT);
      SPI.begin();
    } else {
      SD.begin(PIN_SD_CS);
      isSdStarted = true;
    }
  }
  
  File beginSdRead(const char *filename) {
    beginSdRead();
    return SD.open(filename, FILE_READ);
  }
  
  void endSdRead() {
    SPI.end();
    setupAPUPins();
  }
  
  void endSdRead(File f) {
    f.close();
    endSdRead();
  }
  
  
  void beginLcdWrite() {
    digitalWrite(PIN_APU_A7, HIGH);
    pinMode(PIN_LCD_CS, OUTPUT);
    pinMode(PIN_LCD_C_D, OUTPUT);
    pinMode(PIN_LCD_RST, OUTPUT);
    pinMode(PIN_LCD_MOSI, OUTPUT);
    pinMode(PIN_LCD_CLK, OUTPUT);
    SPI.begin();
  }
  
  void endLcdWrite() {
    SPI.end();
    setupAPUPins();
  }
#elif defined(ARDUINO_AVR_MEGA2560)
  bool isSdStarted = false;
  
  void setupAPUPins() {
    // Prepare I/O pins
    PORT_DIR_APU_CTRL = 0x0F;
    PORT_OUT_APU_CTRL = 0x0F;
    pinMode(PIN_APU_RST, OUTPUT);
    digitalWrite(PIN_APU_RST, HIGH);
    
    // Select APU
    pinMode(PIN_APU_A7, OUTPUT);
    digitalWrite(PIN_APU_A7, LOW);
  }
  
  void beginSdRead() {
    if (!isSdStarted) {
      SD.begin(PIN_SD_CS);
      isSdStarted = true;
    }
  }
  
  File beginSdRead(const char *filename) {
    beginSdRead();
    return SD.open(filename, FILE_READ);
  }
  
  void endSdRead() {
    setupAPUPins();
  }
  
  void endSdRead(File f) {
    f.close();
    endSdRead();
  }
  
  
  void beginLcdWrite() {
    digitalWrite(PIN_APU_A7, HIGH);
    pinMode(PIN_LCD_CS, OUTPUT);
    pinMode(PIN_LCD_C_D, OUTPUT);
    pinMode(PIN_LCD_RST, OUTPUT);
    pinMode(PIN_LCD_MOSI, OUTPUT);
    pinMode(PIN_LCD_CLK, OUTPUT);
  }
  
  void endLcdWrite() {
    setupAPUPins();
  }
#endif

#endif
