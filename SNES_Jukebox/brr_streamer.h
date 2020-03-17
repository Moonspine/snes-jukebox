#ifndef BRR_STREAMER_H
#define BRR_STREAMER_H

#include <SD.h>
#include "text.h"
#include "lcd_draw.h"
#include "snes_apu.h"
#include "jukebox_io.h"


byte port0statebrr = 0;

void beginApuBlockWrite(word address) {
  writedata(3, address >> 8);
  writedata(2, address & 0xFF);
  writedata(1, 1);
  writedata(0, 0xCC);
  while(readdata(0)!=0xCC);
  port0statebrr = 0;
}

void writeApuByte(byte data) {
  writedata(1, data);
  writedata(0, port0statebrr);
  while (readdata(0) != port0statebrr);
  port0statebrr++;
}

void endApuBlockWrite(word executionAddress) {
  writedata(3, executionAddress >> 8);
  writedata(2, executionAddress & 0xFF);
  byte p0 = readdata(0) + 2;
  if (p0 == 0) p0++;
  writedata(1, 0);
  writedata(0, p0);
  while(readdata(1) != 0xEF);
}


void uploadBrrSongLoader(Adafruit_ST7735 &lcd) {
  beginLcdWrite();
  clearLcd(lcd);
  drawPgmText(lcd, TEXT_UPLOADING_LOADER, 0, 0);
  endLcdWrite();
  
  prog_uint8_t *loaderData = songLoaderData;
  resetApu();
  beginApuBlockWrite(0x200);
  for (int i = 0; i < SONG_LOADER_DATA_LENGTH; i++) {
    writeApuByte(pgm_read_byte(loaderData++));
  }
  endApuBlockWrite(0x200);

  beginLcdWrite();
  clearLcd(lcd);
  drawPgmText(lcd, TEXT_LOADER_UPLOADED, 0, 0);
  endLcdWrite();
}

void sendThreeBytes(byte groupNumber, byte data0, byte data1, byte data2) {
  writedata(0, data0);
  writedata(2, data1);
  writedata(3, data2);
  writedata(1, groupNumber);
  while (readdata(1) != groupNumber) {}
}

void uploadBrrBlock(byte *buffer, int count) {
  // Wait for ready signal
  while(readdata(1) != 0xEF) {};

  // Write out the BRR data
  for (uint32_t j = 0; j < count; j++) {
    bool blockEnd = j == count - 1;
    
    byte *brrData = &buffer[j * 9];
    
    brrData[0] |= 0x02; // < Enforce looping
    if (blockEnd) {
      // The last block needs to be marked as an end block
      brrData[0] |= 0x01;
    }

    // Upload the block
    sendThreeBytes(0x01, brrData[0], brrData[1], brrData[2]);
    sendThreeBytes(0x02, brrData[3], brrData[4], brrData[5]);
    sendThreeBytes(0x03, brrData[6], brrData[7], brrData[8]);
  }

  // Notify that the data stream has ended
  writedata(1, 0x00);
}

void streamBrrFile(File &file, Adafruit_ST7735 &lcd) {
  byte buffer[BRR_TRANSFER_BLOCK_SIZE];
  
  beginLcdWrite();
  clearLcd(lcd);
  drawPgmText(lcd, TEXT_STREAMING, 0, 0);
  drawText(lcd, file.name(), 0, 10);
  endLcdWrite();
  
  beginSdRead();
  uint32_t brrBlockCount = file.size() / 9;
  endSdRead();
  uint32_t brrBatchCount = brrBlockCount / BRR_TRANSFER_BLOCK_COUNT;

  uint32_t nextBrrBlock = 0;
  for (uint32_t i = 0; i < brrBatchCount; i++) {
    word nextBlockTransferCount = min(BRR_TRANSFER_BLOCK_COUNT, (word)min(brrBlockCount - nextBrrBlock, 65535));
    
    // Fetch the next BRR blocks
    beginSdRead();
    file.read(buffer, nextBlockTransferCount * 9);
    endSdRead();

    uploadBrrBlock(buffer, nextBlockTransferCount);
    brrBlockCount += nextBlockTransferCount;
  }
  
  // Upload the terminator
  buffer[0] = 0x03;
  for (int i = 1; i <= 8; i++) {
    buffer[i] = 0x02;
  }
  uploadBrrBlock(buffer, 1);
}

#endif

