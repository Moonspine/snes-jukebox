#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "defines.h"
#include "snes_apu.h"
#include "spc_info.h"
#include "snes_controller.h"
#include "file_list_menu.h"
#include "port_write_menu.h"
#include "icons.h"
#include "jukebox_io.h"
#include "brr_streamer.h"
#include "progress_bar.h"
#include "lcd_draw.h"

/* IO Pin Mapping.
 ** Digital Pin 0 - RX
 ** Digital Pin 1 - TX
 ** Digital Pin 2-7 - SNES APU D2-7
 ** Digital Pin 8 - SNES APU address 0; LCD Reset
 ** Digital Pin 9 - SNES APU address 1
 ** Digital Pin 10 - SNES APU /RD
 ** Digital Pin 11 - SNES APU /WR; SD Card / LCD MOSI; Controller Latch
 ** Digital Pin 12 - SNES APU D0; SD Card MISO
 ** Digital Pin 13 - SNES APU D1; SD Card / LCD Clk; Controller Clock
 ** Analog Pin 0 (AKA Digital Pin 14) - SNES APU /RESET
 ** Analog Pin 1 - SD Card CS
 ** Analog Pin 2 - SNES APU Address 7
 ** Analog Pin 3 - LCD CS
 ** Analog Pin 4 - LCD C/D Select
 ** Analog Pin 5 - Controller Data
 ** Vcc - SNES APU address 6, SNES APU Vcc, SNES APU audio Vcc
 ** Gnd - SNES APU Gnd, SNES APU audio Gnd.
 */

FileListMenu fileMenu(15);
PortWriteMenu portWriteMenu;
byte currentMenu = 0;

Adafruit_ST7735 lcd = Adafruit_ST7735(A3, A4, 8);
SNESController controller(11, 13, A5);

void readControllerState() {
  SPI.end();
  digitalWrite(A2, HIGH);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A5, INPUT_PULLUP);
  digitalWrite(11, 0);
  digitalWrite(13, 0);
  controller.update(currentMenu == 0 ? CONTROLLER_DEBOUNCE_DELAY : 0);
  setupAPUPins();
}

void setup() {
  setupAPUPins();
  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  
  lcd.initR(INITR_BLACKTAB);
  
  beginLcdWrite();
  clearLcd(lcd);
  endLcdWrite();
}

void loop() {
  readControllerState();
  
  if (currentMenu == 0) {
    fileMenu.update(lcd, controller);
    if (fileMenu.newPortsWritten()) {
      portWriteMenu.setLastWrittenPorts(fileMenu.getLastWrittenPort(0), fileMenu.getLastWrittenPort(1), fileMenu.getLastWrittenPort(2), fileMenu.getLastWrittenPort(3));
    }
  } else if (currentMenu == 1) {
    portWriteMenu.update(lcd, controller);
  }
  
  bool initMenu = false;
  const byte MENU_COUNT = 2;
  if (controller.justPressed(SNESController::R)) {
    currentMenu = (currentMenu + 1) % MENU_COUNT;
    initMenu = true;
  } else if (controller.justPressed(SNESController::L)) {
    --currentMenu;
    if (currentMenu > 1) {
      currentMenu = 1;
    }
    initMenu = true;
  }
  
  if (initMenu) {
    switch (currentMenu) {
      case 0: {
        fileMenu.drawMenu(lcd);
      } break;
      case 1: {
        portWriteMenu.initialize(lcd);
      } break;
      default: break;
    }
  }
}
