#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "pin_defs.h"
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

FileListMenu fileMenu(15);
PortWriteMenu portWriteMenu;
byte currentMenu = 0;

Adafruit_ST7735 lcd = Adafruit_ST7735(PIN_LCD_CS, PIN_LCD_C_D, PIN_LCD_RST);
SNESController controller(PIN_CONTROLLER_LATCH, PIN_CONTROLLER_CLK, PIN_CONTROLLER_DATA);

void setup() {
  setupAPUPins();
  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);
  
  lcd.initR(INITR_BLACKTAB);
  
  beginLcdWrite();
  clearLcd(lcd);
  endLcdWrite();
}

void loop() {
  // Read controller
  SPI.end();
  digitalWrite(PIN_APU_A7, HIGH);
  controller.update(currentMenu == 0 ? CONTROLLER_DEBOUNCE_DELAY : 0);
  setupAPUPins();
  
  // Update menu
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
        }
        break;
      case 1: {
          portWriteMenu.initialize(lcd);
        }
        break;
      default:
        break;
    }
  }
}

