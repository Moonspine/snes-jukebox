#ifndef FILE_LIST_MENU_H
#define FILE_LIST_MENU_H

#include "jukebox_io.h"
#include "spc.h"
#include "brr_streamer.h"
#include "icons.h"
#include "progress_bar.h"
#include "text.h"
#include "lcd_draw.h"

struct FileListMenu {
public:

  FileListMenu(byte filesPerPage) : initialized(false), selectedFileIndex(0) {
    this->filesPerPage = filesPerPage;
  }
  
  void initialize(File &root) {
    if (this->root) {
      this->root.close();
    }
    this->root = root;
    currentFileIndexOffset = 0;
    totalFileCount = 0xFFFF;
    lastReadFile = 0;
    pageStartOffset = root.position();
  }
  
  void update(Adafruit_ST7735 &lcd, SNESController &controller) {
    justWroteSpc = false;
    
    // Initialize the file menuu
    if (!initialized) {
      File f = beginSdRead("/");
      initialize(f);
      endSdRead();
      drawMenu(lcd);
      initialized = true;
    }
  
    if (controller.justPressed(SNESController::RIGHT)) {
      beginSdRead();
      bool pageChanged = nextPage();
      endSdRead();
  
      if (pageChanged) {
        drawMenu(lcd);
      }
    } else if (controller.justPressed(SNESController::LEFT)) {
      beginSdRead();
      bool pageChanged = previousPage();
      endSdRead();
      if (pageChanged) {
        drawMenu(lcd);
      }
    } else if (controller.justPressed(SNESController::UP)) {
      if (selectedFileIndex > 0) {
        --selectedFileIndex;
      } else {
        selectedFileIndex = getCurrentPageSize() - 1;
      }
      drawFileSelection(lcd);
    } else if (controller.justPressed(SNESController::DOWN)) {
      selectedFileIndex = (selectedFileIndex + 1) % getCurrentPageSize(); 
      drawFileSelection(lcd);
    }
    
    if (controller.justPressed(SNESController::A)) {
      beginSdRead();
      File f = getSelectedFile(selectedFileIndex);
      if (f) {
        if (f.isDirectory() ) {
          selectedFileIndex = 0;
          initialize(f);
          endSdRead();
          drawMenu(lcd);
        } else {
          if (isSpcFile(f)) {
            endSdRead();
            uploadSpc(lcd, f);
            f.close();
          } else if (isBrrFile(f)) {
            endSdRead();
            uploadBrrSongLoader(lcd);
            streamBrrFile(f, lcd);
            f.close();
          } else if (isBr2File(f)) {
            endSdRead();
            uploadBrrSongLoader(lcd);
            streamBr2File(f, lcd);
            f.close();
          }
          drawMenu(lcd);
        }
      }
    }
    
    if (controller.justPressed(SNESController::B)) {
      File newRoot = beginSdRead("/");
      initialize(newRoot);
      endSdRead();
      drawMenu(lcd);
    }
  }
  
  bool readNextFile(char *filename, bool &isDirectory) {
    if (lastReadFile >= totalFileCount) return false;
    
    File f = root.openNextFile();
    if (!f) {
      totalFileCount = lastReadFile;
      return false;
    } else {
      memcpy(filename, f.name(), 12);
      filename[12] = 0;
      isDirectory = f.isDirectory();
      f.close();
    }
    
    ++lastReadFile;
    
    return true;
  }
  
  void resetPage() {
    root.seek(pageStartOffset);
    lastReadFile = currentFileIndexOffset;
  }

  bool previousPage() {
    if (currentFileIndexOffset == 0) {
      return false;
    }
    
    currentFileIndexOffset -= filesPerPage;
    
    lastReadFile = 0;
    root.rewindDirectory();
    for (uint32_t i = 0; i < currentFileIndexOffset; ++i) {
      File f = root.openNextFile();
      f.close();
    }
    lastReadFile = currentFileIndexOffset;
    pageStartOffset = root.position();

    return true;
  }
  
  bool nextPage() {
    if (totalFileCount - currentFileIndexOffset <= filesPerPage) {
      return false;
    }
    
    char temp[13];
    bool temp2;
    while (lastReadFile < currentFileIndexOffset && readNextFile(temp, temp2)) {
      // Call readNextFile() until at end of page
    }
    
    // Attempt to read the next tile
    uint32_t pos = root.position();
    bool atEnd = !readNextFile(temp, temp2);
    
    // Reset file
    root.seek(pos);
    --lastReadFile;
    
    // If we couldn't read the next file, we failed
    if (atEnd) {
      return false;
    }
   
    // Otherwise, we succeeded at switching pages
    pageStartOffset = root.position();
    currentFileIndexOffset += filesPerPage;
    return true;
  }

  File getSelectedFile(byte selectedIndex) {
    resetPage();
    char temp[13];
    bool temp2;
    for (byte b = 0; b < selectedIndex; ++b) {
      readNextFile(temp, temp2);
    }
    File result;
    result = root.openNextFile();
    ++lastReadFile;
    return result;
  }
  
  void drawMenu(Adafruit_ST7735 &lcd) {
    beginLcdWrite();
    clearLcd(lcd);
    drawPgmText(lcd, TEXT_SELECT_FILE, 0, 0);
    endLcdWrite();
    
    beginSdRead();
    resetPage();
    endSdRead();
    
    char filename[13];
    for (byte i = 0; i < filesPerPage; ++i) {
      beginSdRead();
      bool isDirectory;
      bool readNext = readNextFile(filename, isDirectory);
      endSdRead();

      if (!readNext) {
        break;
      }
      
      beginLcdWrite();
      drawMenuItem(lcd, i, filename, isDirectory);
      endLcdWrite();
    }
    
    drawFileSelection(lcd);
  }
  
  bool newPortsWritten() {
    return justWroteSpc;
  }
  
  byte getLastWrittenPort(byte index) {
    return lastWrittenPorts[index % 4];
  }

private:
  bool initialized;
  byte selectedFileIndex;

  byte filesPerPage;

  File root;
  
  word currentFileIndexOffset;
  word totalFileCount;
  word lastReadFile;
  uint32_t pageStartOffset;
  
  bool justWroteSpc;
  byte lastWrittenPorts[4];
  
  
  byte getCurrentPageSize() {
    return (byte)min(totalFileCount - currentFileIndexOffset, (word)filesPerPage);
  }
  
  void drawMenuItem(Adafruit_ST7735 &lcd, int i, const char *filename, bool isDirectory) {
    const uint8_t *icon = NULL;
    if (isDirectory) {
      icon = folderIcon;
    } else if (isSpcFile(filename)) {
      icon = spcFileIcon;
    } else if (isBrrFile(filename)) {
      icon = brrFileIcon;
    } else if (isBr2File(filename)) {
      icon = br2FileIcon;
    } else {
      icon = fileIcon;
    }
    drawIcon(lcd, icon, 11, (i + 1) * 10);
    drawText(lcd, filename, 20, (i + 1) * 10);
  }
  
  void drawFileSelection(Adafruit_ST7735 &lcd) {
    const byte pageSize = getCurrentPageSize();
    if (pageSize > 0) {
      selectedFileIndex = selectedFileIndex % pageSize;
    }
    beginLcdWrite();
    lcd.fillRect(0, 10, 10, 150, ST7735_BLACK);
    drawIcon(lcd, selectorIcon, 1, (selectedFileIndex + 1) * 10);
    endLcdWrite();
  }
  
  bool isSpcFile(const char *filename) {
    // Locate dot index
    int dotIndex = -1;
    for (int i = 0; i <= 8 && dotIndex < 0; ++i) {
      if (filename[i] == '.') {
        dotIndex = i;
      }
    }
    if (dotIndex < 0) {
      return false;
    }
    
    // Test file extension
    if (filename[dotIndex + 1] != 's' && filename[dotIndex + 1] != 'S') {
      return false;
    }
    if (filename[dotIndex + 2] != 'p' && filename[dotIndex + 2] != 'P') {
      return false;
    }
    if (filename[dotIndex + 3] != 'c' && filename[dotIndex + 3] != 'C') {
      return false;
    }
    
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
    for (int i = 0; i <= 8 && dotIndex < 0; ++i) {
      if (filename[i] == '.') {
        dotIndex = i;
      }
    }
    if (dotIndex < 0) return false;
    
    // Test file extension
    if (filename[dotIndex + 1] != 'b' && filename[dotIndex + 1] != 'B') {
      return false;
    }
    if (filename[dotIndex + 2] != 'r' && filename[dotIndex + 2] != 'R') {
      return false;
    }
    if (filename[dotIndex + 3] != 'r' && filename[dotIndex + 3] != 'R') {
      return false;
    }
    
    // SPC file!
    return true;
  }
  
  bool isBrrFile(File &f) {
    const char *filename = f.name();
    return isBrrFile(filename);
  }

  bool isBr2File(const char *filename) {
    // Locate dot index
    int dotIndex = -1;
    for (int i = 0; i <= 8 && dotIndex < 0; ++i) {
      if (filename[i] == '.') {
        dotIndex = i;
      }
    }
    if (dotIndex < 0) return false;
    
    // Test file extension
    if (filename[dotIndex + 1] != 'b' && filename[dotIndex + 1] != 'B') {
      return false;
    }
    if (filename[dotIndex + 2] != 'r' && filename[dotIndex + 2] != 'R') {
      return false;
    }
    if (filename[dotIndex + 3] != '2' && filename[dotIndex + 3] != 'R') {
      return false;
    }
    
    // SPC file!
    return true;
  }
  
  bool isBr2File(File &f) {
    const char *filename = f.name();
    return isBr2File(filename);
  }
  
  void uploadSpc(Adafruit_ST7735 &lcd, File &file) {
    beginLcdWrite();
    clearLcd(lcd);
    drawPgmText(lcd, TEXT_LOADING_SPC, 0, 0);
    drawText(lcd, file.name(), 0, 10);
    endLcdWrite();
    
    // A nice, big buffer for SD reads
    byte buffer[SPC_UPLOAD_BUFFER_SIZE];
    
    ProgressBar progressBar(lcd, 0, 20, 128, 0x10000);
    drawSongInfo(lcd, file, buffer, 30);
    
    SPCInfo info;
    beginSdRead();
    info.parseSPC(file);
    endSdRead();

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
    for (uint32_t i = 0x100 + (SPC_UPLOAD_BUFFER_SIZE - 384); i < 0x10000; i += SPC_UPLOAD_BUFFER_SIZE) {
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
    if (p0 == 0) {
      ++p0;
    }
    writedata(1, 0);
    writedata(0, p0);
    while(readdata(0) != 0x53) {}
    
    // Write input ports of SPC
    writedata(0, info.getExtraData(0));
    writedata(1, info.getExtraData(1));
    writedata(2, info.getExtraData(2));
    writedata(3, info.getExtraData(3));
    
    // Update ports in the menu
    justWroteSpc = true;
    lastWrittenPorts[0] = info.getExtraData(0);
    lastWrittenPorts[1] = info.getExtraData(1);
    lastWrittenPorts[2] = info.getExtraData(2);
    lastWrittenPorts[3] = info.getExtraData(3);
  }
  
  void drawSongInfo(Adafruit_ST7735 &lcd, File &file, byte *tempBuffer, byte yOffset) {
    byte offset = yOffset;
    offset = drawSongInfoField(lcd, file, tempBuffer, 0x2E, offset + 10, TEXT_SONG);
    offset = drawSongInfoField(lcd, file, tempBuffer, 0x4E, offset + 10, TEXT_GAME);
    offset = drawSongInfoField(lcd, file, tempBuffer, 0xB1, offset + 10, TEXT_COMPOSER);
  }
  
  byte drawSongInfoField(Adafruit_ST7735 &lcd, File &file, byte *tempBuffer, uint32_t fileOffset, byte yOffset, const char *pgmFieldLabel) {
    // Output field label
    beginLcdWrite();
    byte offset = yOffset;
    drawPgmText(lcd, pgmFieldLabel, 0, offset);
    offset += 10;
    endLcdWrite();
    
    // Read field value
    beginSdRead();
    seekIfNecessary(file, fileOffset);
    file.read(tempBuffer, 0x20);
    endSdRead();
    tempBuffer[32] = 0;
    
    // Output field value
    beginLcdWrite();
    if (tempBuffer[0] == 0) {
      drawPgmText(lcd, TEXT_UNKNOWN, 0, offset);
      offset += 10;
    } else {
      const char *str = (const char *) tempBuffer;
      drawText(lcd, str, 0, offset);
      offset += (strlen(str) / 21 + 1) * 10;
    }
    endLcdWrite();
    
    return offset;
  }
};

#endif
