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

unsigned char port0state;

#define FILES_PER_PAGE 15

FileListMenu fileMenu(FILES_PER_PAGE);
bool fileMenuInitialized = false;
byte selectedFileIndex = 0;
PortWriteMenu portWriteMenu;
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

void startapuwrite(uint16_t address, prog_uchar *data, int len) {
  resetApu();

  writedata(3, address >> 8);
  writedata(2, address & 0xFF);
  writedata(1, 1);
  writedata(0, 0xCC);
  while(readdata(0) != 0xCC);
  for(int i = 0;i < len; i++) {
    writedata(1, pgm_read_byte(data + i));
    writedata(0,i & 0xFF);
    while(readdata(0) != (i & 0xFF));
  }
}


// BEGIN PORT OF APU LOADER DLL

void seekIfNecessary(File &file, uint32_t desiredPosition) {
  if (file.position() == desiredPosition) return;
  file.seek(desiredPosition);
}

word readSpcRamData(File &spcFile, SPCInfo &info, word startAddress, byte *out, word readCount) {
  seekIfNecessary(spcFile, RAM_START + (uint32_t)startAddress);
  
  const word realReadCount = (word)min((uint32_t)readCount, (word)min(spcFile.size() - RAM_START - (uint32_t)startAddress, (uint32_t)65535));
  spcFile.read(out, realReadCount);
  
  const uint32_t endAddress = (uint32_t)startAddress + (uint32_t)realReadCount;

  // Load extra RAM
  if (info.getShouldLoadExtraRAM() && endAddress > 0xFFC0) {
    const word extraRamStart = max(startAddress, 0xFFC0);
    seekIfNecessary(spcFile, EXTRA_RAM_START + (uint32_t)(extraRamStart - 0xFFC0));
    spcFile.read(&out[extraRamStart - startAddress], 0xFFFF - extraRamStart + 1);
  }
  
  // Load bootloader code
  const uint32_t bootStart = max(startAddress, info.getBootLocation());
  const uint32_t bootEnd = min(endAddress, (uint32_t)(info.getBootLocation() + info.getBootCount()));
  for (uint32_t i = bootStart; i < bootEnd; ++i) {
    const word bootOffset = i - info.getBootLocation();
    const word outOffset = i - startAddress;
    switch (bootOffset) {
      case 0x19: out[outOffset] = info.getExtraData(0); break;
      case 0x1F: out[outOffset] = info.getExtraData(1); break;
      case 0x25: out[outOffset] = info.getExtraData(2); break;
      case 0x2B: out[outOffset] = info.getExtraData(3); break;
      case 0x01: out[outOffset] = info.getExtraData(4); break;
      case 0x04: out[outOffset] = info.getExtraData(5); break;
      case 0x07: out[outOffset] = info.getExtraData(6); break;
      case 0x0A: out[outOffset] = info.getExtraData(7); break;
      case 0x0D: out[outOffset] = info.getExtraData(8); break;
      case 0x10: out[outOffset] = info.getExtraData(9); break;
      case 0x38: out[outOffset] = info.getExtraData(10); break;
      case 0x3E: out[outOffset] = info.getExtraData(11); break;
      case 0x41: out[outOffset] = info.getExtraData(12); break;
      case 0x44: out[outOffset] = info.getSPLow(); break;
      case 0x47: out[outOffset] = info.getPSW(); break;
      case 0x4b: out[outOffset] = info.getA(); break;
      case 0x4d: out[outOffset] = info.getY(); break;
      case 0x4f: out[outOffset] = info.getX(); break;
      case 0x51: out[outOffset] = info.getPCLow(); break;
      case 0x52: out[outOffset] = info.getPCHigh(); break;
      default: out[outOffset] = pgm_read_byte(bootloaderCode + bootOffset); break;
    }
  }
  
  // Silence echo region
  if (info.getEchoEnabled()) {
    const uint32_t echoStart = max(startAddress, info.getEchoRegion());
    const uint32_t echoEnd = min(endAddress, (uint32_t)(info.getEchoRegion() + info.getEchoSize()));
    for (uint32_t i = echoStart; i < echoEnd; ++i) {
      out[i - startAddress] = 0;
    }
  }
  
  return realReadCount;
}

/**
 * Reads all 128 bytes of DSP data from an SPC file
 */
byte readDspRamData(File &spcFile, byte *target) {
  seekIfNecessary(spcFile, DSP_RAM_START);
  spcFile.read(target, 128);
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
  readSpcRamData(file, info, 0, &buffer[128], SPC_UPLOAD_BUFFER_SIZE - 128);
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
    readSpcRamData(file, info, (word)i, buffer, min(SPC_UPLOAD_BUFFER_SIZE, (word)(0x10000 - i)));
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
  portWriteMenu.setLastWrittenPorts(info.getExtraData(0), info.getExtraData(1), info.getExtraData(2), info.getExtraData(3));
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

void loop() {
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
  
    if (controller.justPressed(SNESController::RIGHT)) {
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
    } else if (controller.justPressed(SNESController::LEFT)) {
      beginSdRead();
      bool pageChanged = fileMenu.previousPage();
      endSdRead();
      if (pageChanged) {
        drawMenu();
      }
    } else if (controller.justPressed(SNESController::UP)) {
      byte pageSize = fileMenu.getCurrentPageSize();
      if (selectedFileIndex > 0) {
        selectedFileIndex--;
      } else {
        selectedFileIndex = pageSize - 1;
      }
      drawFileSelection();
    } else if (controller.justPressed(SNESController::DOWN)) {
      byte pageSize = fileMenu.getCurrentPageSize();
      selectedFileIndex = (selectedFileIndex + 1) % pageSize; 
      drawFileSelection();
    }
    
    if (controller.justPressed(SNESController::A)) {
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
            portWriteMenu.drawMenu(lcd);
          }
        }
      }
    }
    
    if (controller.justPressed(SNESController::B)) {
      File newRoot = beginSdRead("/");
      fileMenu.initialize(newRoot);
      endSdRead();
      drawMenu();
    }
  } else if (currentMenu == 1) {
    portWriteMenu.update(lcd, controller);
  }
  
  // BEGIN DEBUG MENU SWAPPING CODE
  
  bool initMenu = false;
  const byte MENU_COUNT = 2;
  if (controller.justPressed(SNESController::R)) {
    currentMenu = (currentMenu + 1) % MENU_COUNT;
    initMenu = true;
  } else if (controller.justPressed(SNESController::L)) {
    --currentMenu;
    if (currentMenu > 1) currentMenu = 1;
    initMenu = true;
  }
  
  if (initMenu) {
    switch (currentMenu) {
      case 0: {
        drawMenu();
      } break;
      case 1: {
        portWriteMenu.initialize(lcd);
      } break;
      default: break;
    }
  }
}
