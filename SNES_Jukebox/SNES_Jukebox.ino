#include <Firmata.h>
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
 ** Digital Pin 8 - SNES APU address 0
 ** Digital Pin 9 - SNES APU address 1
 ** Digital Pin 10 - SNES APU /RD
 ** Digital Pin 11 - SNES APU /WR; SD Card DI; Controller Latch
 ** Digital Pin 12-13 - SNES APU D0-1; SD Card DO, Clk; -, Controller Clock
 ** Analog Pin 0 (AKA Digital Pin 14) - SNES APU /RESET
 ** Analog Pin 1 - SD Card CS
 ** Analog Pin 2 - SNES APU Address 7
 ** Analog Pin 3 - LCD CS
 ** Analog Pin 4 - LCD C/D Select
 ** Analog Pin 5 - Controller Data
 ** Vcc - SNES APU address 6, SNES APU Vcc, SNES APU audio Vcc
 ** Gnd - SNES APU Gnd, SNES APU audio Gnd.
 */

unsigned char port0state;

#define FILES_PER_PAGE 15
FileListMenu fileMenu(FILES_PER_PAGE);
bool fileMenuInitialized = false;
byte selectedFileIndex = 0;
//PortWriteMenu portWriteMenu;
byte currentMenu = 0;

Adafruit_ST7735 lcd = Adafruit_ST7735(A3, A4, 8);
SNESController controller(11, 13, A5);

void readControllerState() {
  digitalWrite(A2, HIGH);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A5, INPUT_PULLUP);
  digitalWrite(11, 0);
  digitalWrite(13, 0);
  controller.update();
  setupAPUPins();
}

void setup()
{
  setupAPUPins();
  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  
  lcd.initR(INITR_REDTAB);
  
  beginLcdWrite();
  clearLcd(lcd);
  endLcdWrite();
}

void startapuwrite(uint16_t address, prog_uchar *data, int len)
{
  resetApu();

  writedata(3,address>>8);
  writedata(2,address&0xFF);
  writedata(1,1);
  writedata(0,0xCC);
  while(readdata(0)!=0xCC);
  for(int i=0;i<len;i++)
  {
    writedata(1, pgm_read_byte(data + i));
    writedata(0,i&0xFF);
    while(readdata(0)!=(i&0xFF));
  }
}


// BEGIN PORT OF APU LOADER DLL

void seekIfNecessary(File &file, uint32_t desiredPosition) {
  if (file.position() == desiredPosition) return;
  file.seek(desiredPosition);
}

byte readSpcRamData(File &spcFile, SPCInfo &info, word address) {
  // Extra RAM
  if (info.getShouldLoadExtraRAM() && address >= 0xFFC0) {
    seekIfNecessary(spcFile, EXTRA_RAM_START + (uint32_t)(address - 0xFFC0));
    return (byte)spcFile.read();
  }
  
  // Bootloader code
  if (address >= info.getBootLocation() && address < (info.getBootLocation() + info.getBootCount())) {
    word bootAddress = address - info.getBootLocation();
    switch (bootAddress) {
      case 0x19: return info.getExtraData(0);
      case 0x1F: return info.getExtraData(1);
      case 0x25: return info.getExtraData(2);
      case 0x2B: return info.getExtraData(3);
      case 0x01: return info.getExtraData(4);
      case 0x04: return info.getExtraData(5);
      case 0x07: return info.getExtraData(6);
      case 0x0A: return info.getExtraData(7);
      case 0x0D: return info.getExtraData(8);
      case 0x10: return info.getExtraData(9);
      case 0x38: return info.getExtraData(10);
      case 0x3E: return info.getExtraData(11);
      case 0x41: return info.getExtraData(12);
      case 0x44: return info.getSPLow();
      case 0x47: return info.getPSW();
      case 0x4b: return info.getA();
      case 0x4d: return info.getY();
      case 0x4f: return info.getX();
      case 0x51: return info.getPCLow();
      case 0x52: return info.getPCHigh();
      default: return pgm_read_byte(bootloaderCode + bootAddress);
    }
  }
  
  // Echo region
  if (info.getEchoEnabled() && address >= info.getEchoRegion() && address < (info.getEchoRegion() + info.getEchoSize())) {
    return 0;
  }
  
  // If no special RAM re-mapping is to be done, just load the byte
  seekIfNecessary(spcFile, RAM_START + (uint32_t)address);
  return (byte)spcFile.read();
}

/**
 * Reads all 128 bytes of DSP data from an SPC file
 */
byte readDspRamData(File &spcFile, byte *target) {
  seekIfNecessary(spcFile, DSP_RAM_START);
  for (byte i = 0; i < 128; i++) {
    target[i] = (byte)spcFile.read();
  }
  target[0x6C] = 0x60;
  target[0x4C] = 0x00;
}

void startSpc700(byte *data) {
  startapuwrite(0x0002, DSPdata, DSP_DATA_LENGTH);
  writedata(2, 0x02);
  writedata(3, 0x00);
  writedata(1, 0x00);
  writedata(0, 0x11);
  while(readdata(0) != 0x11);
  port0state = 0;
  for (int i = 0; i < 128; i++)
  {
    writedata(1, data[i]);
    writedata(0, port0state);
    if(i < 127)
      while(readdata(0) != port0state);
    port0state++;
  }
  while(readdata(0) != 0xAA);
  port0state = 0;
  writedata(2, 0x02);
  writedata(3, 0x00);
  writedata(1, 0x01);
  writedata(0, 0xCC);
  while(readdata(0) != 0xCC);
  for(int i = 2; i < 0xF0; i++) {
    writedata(1, data[i + 128]);
    writedata(0, port0state);
    while(readdata(0) != port0state);
    port0state++;
  }
}

void beginBlockWrite(word address) {
  writedata(1, 1);
  writedata(2, address & 0xFF);
  writedata(3, address >> 8);

  byte i = readdata(0);
  i += 2;
  writedata(0, i);
  while(readdata(0) != i);
  port0state = 0;
}

void writeBytes(byte *data, word count) {
  for(int i = 0; i < count; i++) {
    writedata(1, data[i]);
    writedata(0, port0state);
    while (readdata(0) != port0state);
    port0state++;
  }
}

void uploadSpc(File &file) {
  beginLcdWrite();
  clearLcd(lcd);
  drawText(lcd, "Loading SPC:", 0, 0);
  drawText(lcd, file.name(), 0, 10);
  endLcdWrite();
  ProgressBar progressBar(lcd, 0, 20, 128, 0x10000);

  SPCInfo info;
  beginSdRead();
  info.parseSPC(file);
  endSdRead();

  // A nice, big buffer for SD reads
  byte buffer[SPC_UPLOAD_BUFFER_SIZE];
  
  // Reset the APU
  setupAPUPins();
  resetApu();

  // Read some data from the SD card
  beginSdRead();
  readDspRamData(file, &buffer[0]);
  for (word i = 0; i < SPC_UPLOAD_BUFFER_SIZE - 128; i++) {
    buffer[128 + i] = readSpcRamData(file, info, i);
  }
  endSdRead();

  // Initialize
  startSpc700(buffer);
  
  progressBar.addProgress(256);

  // Write SPC RAM data
  beginBlockWrite(0x100);
  writeBytes(&buffer[384], SPC_UPLOAD_BUFFER_SIZE - 384);
  for (uint32_t i = 0x180; i < 0x10000; i += SPC_UPLOAD_BUFFER_SIZE) {
    // Read more data from the card
    beginSdRead();
    for (uint32_t j = 0; j < SPC_UPLOAD_BUFFER_SIZE && i + j < 0x10000; j++) {
      buffer[j] = readSpcRamData(file, info, (word)(j + i));
    }
    endSdRead();
    
    // Write data to the SPC 700
    writeBytes(buffer, min(SPC_UPLOAD_BUFFER_SIZE, 0x10000 - i));
    progressBar.addProgress(SPC_UPLOAD_BUFFER_SIZE);
  }

  // Reboot at boot pointer
  writedata(3, info.getBootLocation() >> 8);
  writedata(2, info.getBootLocation() & 0xFF);
  byte p0 = readdata(0) + 2;
  if (p0 == 0) p0++;
  writedata(1, 0);
  writedata(0, p0);
  while(readdata(0) != 0x53);
  
  // Write input ports of SPC
  writedata(0, info.getExtraData(0));
  writedata(1, info.getExtraData(1));
  writedata(2, info.getExtraData(2));
  writedata(3, info.getExtraData(3));
  
  // Update ports in the menu
  //portWriteMenu.setLastWrittenPorts(info.getExtraData(0), info.getExtraData(1), info.getExtraData(2), info.getExtraData(3));
}

void uploadSpc(char *filename) {
  File f = beginSdRead(filename);
  endSdRead();
  uploadSpc(f);
  
  beginSdRead();
  f.close();
  endSdRead();
}

// END PORT OF APU LOADER DLL

bool isSpcFile(const char *filename) {
  // Locate dot index
  int dotIndex = -1;
  for (int i = 0; i <= 8 && dotIndex < 0; i++) {
    if (filename[i] == '.') {
      dotIndex = i;
    }
  }
  if (dotIndex < 0) return false;
  
  // Test file extension
  if (filename[dotIndex + 1] != 's' && filename[dotIndex + 1] != 'S') return false;
  if (filename[dotIndex + 2] != 'p' && filename[dotIndex + 2] != 'P') return false;
  if (filename[dotIndex + 3] != 'c' && filename[dotIndex + 3] != 'C') return false;
  
  // SPC file!
  return true;
}

bool isSpcFile(File &f) {
  const char *filename = f.name();
  return isSpcFile(filename);
}

bool isBrrFile(const char *filename) {
  // Locate dot index
  int dotIndex = -1;
  for (int i = 0; i <= 8 && dotIndex < 0; i++) {
    if (filename[i] == '.') {
      dotIndex = i;
    }
  }
  if (dotIndex < 0) return false;
  
  // Test file extension
  if (filename[dotIndex + 1] != 'b' && filename[dotIndex + 1] != 'B') return false;
  if (filename[dotIndex + 2] != 'r' && filename[dotIndex + 2] != 'R') return false;
  if (filename[dotIndex + 3] != 'r' && filename[dotIndex + 3] != 'R') return false;
  
  // SPC file!
  return true;
}

bool isBrrFile(File &f) {
  const char *filename = f.name();
  return isBrrFile(filename);
}

void drawMenuItem(int i, const char *filename, bool isDirectory) {
  prog_uint16_t *icon = NULL;
  if (isDirectory) {
    icon = folderIcon;
  } else if (isSpcFile(filename)) {
    icon = spcFileIcon;
  } else {
    icon = fileIcon;
  }
  drawIcon(lcd, icon, 11, (i + 1) * 10);
  drawText(lcd, filename, 20, (i + 1) * 10);
}

void drawFileSelection() {
  beginLcdWrite();
  lcd.fillRect(0, 10, 10, 150, ST7735_BLACK);
  drawIcon(lcd, selectorIcon, 1, (selectedFileIndex + 1) * 10);
  endLcdWrite();
}

void drawMenu() {
  beginLcdWrite();
  clearLcd(lcd);
  drawText(lcd, " -- Select an SPC --", 0, 0);
  endLcdWrite();
  
  beginSdRead();
  fileMenu.resetPage();
  endSdRead();
  
  char filename[13];
  for (byte i = 0; i < FILES_PER_PAGE; i++) {
    beginSdRead();
    bool isDirectory;
    bool readNext = fileMenu.readNextFile(filename, isDirectory);
    endSdRead();
    if (!readNext) {
      break;
    }
    
    beginLcdWrite();
    drawMenuItem(i, filename, isDirectory);
    endLcdWrite();
  }
  
  drawFileSelection();
}

void loop()
{
  readControllerState();
  
  if (currentMenu == 0) {
    // Initialize the file menuu
    if (!fileMenuInitialized) {
      File f = beginSdRead("/");
      fileMenu.initialize(f);
      endSdRead();
      drawMenu();
      fileMenuInitialized = true;
    }
  
    if (controller.isPressed(SNESController::RIGHT) && !controller.wasPressed(SNESController::RIGHT)) {
      beginSdRead();
      bool pageChanged = fileMenu.nextPage();
      endSdRead();
  
      if (pageChanged) {
        byte pageSize = fileMenu.getCurrentPageSize();
        if (pageSize > 0) {
          if (selectedFileIndex >= pageSize) {
            selectedFileIndex = pageSize - 1;
          }
        }
        
        drawMenu();
      }
    } else if (controller.isPressed(SNESController::LEFT) && !controller.wasPressed(SNESController::LEFT)) {
      beginSdRead();
      bool pageChanged = fileMenu.previousPage();
      endSdRead();
      if (pageChanged) {
        drawMenu();
      }
    } else if (controller.isPressed(SNESController::UP) && !controller.wasPressed(SNESController::UP)) {
      byte pageSize = fileMenu.getCurrentPageSize();
      if (selectedFileIndex > 0) {
        selectedFileIndex--;
      } else {
        selectedFileIndex = pageSize - 1;
      }
      drawFileSelection();
    } else if (controller.isPressed(SNESController::DOWN) && !controller.wasPressed(SNESController::DOWN)) {
      byte pageSize = fileMenu.getCurrentPageSize();
      selectedFileIndex = (selectedFileIndex + 1) % pageSize; 
      drawFileSelection();
    }
    
    if (controller.isPressed(SNESController::A) && !controller.wasPressed(SNESController::A)) {
      beginSdRead();
      File f = fileMenu.getSelectedFile(selectedFileIndex);
      if (f) {
        if (f.isDirectory() ) {
          selectedFileIndex = 0;
          fileMenu.initialize(f);
          endSdRead();
          drawMenu();
        } else {
          if (isSpcFile(f)) {
            endSdRead();
            uploadSpc(f);
            f.close();
          } else if (isBrrFile(f)) {
            endSdRead();
            uploadBrrSongLoader(lcd);
            streamBrrFile(f, lcd);
            f.close();
          }
          if (currentMenu == 0) {
            drawMenu();
          } else {
            //portWriteMenu.drawMenu(lcd);
          }
        }
      }
    }
    
    if (controller.isPressed(SNESController::B) && !controller.wasPressed(SNESController::B)) {
      File newRoot = beginSdRead("/");
      fileMenu.initialize(newRoot);
      endSdRead();
      drawMenu();
    }
  } else if (currentMenu == 1) {
    //portWriteMenu.update(lcd, controller);
  }
  
  // BEGIN DEBUG MENU SWAPPING CODE
  
  bool initMenu = false;
  const byte MENU_COUNT = 2;
  if (controller.isPressed(SNESController::R) && !controller.wasPressed(SNESController::R)) {
    currentMenu = (currentMenu + 1) % MENU_COUNT;
    initMenu = true;
  } else if (controller.isPressed(SNESController::L) && !controller.wasPressed(SNESController::L)) {
    --currentMenu;
    if (currentMenu > 1) currentMenu = 1;
    initMenu = true;
  }
  
  if (initMenu) {
    switch (currentMenu) {
      case 0: {
        //File newRoot = beginSdRead("/");
        //fileMenu.initialize(newRoot);
        //endSdRead();
        drawMenu();
      } break;
      case 1: {
        //portWriteMenu.initialize(lcd);
      } break;
      default: break;
    }
  }
}
