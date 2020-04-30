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
  while(readdata(0) != 0xCC) {}
  port0statebrr = 0;
}

void writeApuByte(byte data) {
  writedata(1, data);
  writedata(0, port0statebrr);
  while (readdata(0) != port0statebrr) {}
  ++port0statebrr;
}

void endApuBlockWrite(word executionAddress) {
  writedata(3, executionAddress >> 8);
  writedata(2, executionAddress & 0xFF);
  byte p0 = readdata(0) + 2;
  if (p0 == 0) {
    ++p0;
  }
  writedata(1, 0);
  writedata(0, p0);
  while(readdata(1) != 0xEE) {}
}


void uploadBrrSongLoader(Adafruit_ST7735 &lcd) {
  beginLcdWrite();
  clearLcd(lcd);
  drawPgmText(lcd, TEXT_UPLOADING_LOADER, 0, 0);
  endLcdWrite();
  
  const uint8_t *loaderData = songLoaderData;
  resetApu();
  beginApuBlockWrite(0x200);
  for (int i = 0; i < SONG_LOADER_DATA_LENGTH; ++i) {
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

void uploadBrrBlock(byte *buffer, int count, bool stereo) {
  // Wait for ready signal
  while(readdata(1) != 0xEF) {};

  // The last block needs to be marked as an end block
  buffer[(count - 1) * 9] |= 0x01;
  if (stereo && count >= 2) {
    buffer[(count - 2) * 9] |= 0x01;
  }

  // Write out the BRR data
  for (uint32_t j = 0; j < count; ++j) {
    byte *brrData = &buffer[j * 9];
    
    brrData[0] |= 0x02; // < Enforce looping

    // Upload the block
    sendThreeBytes(0x01, brrData[0], brrData[1], brrData[2]);
    sendThreeBytes(0x02, brrData[3], brrData[4], brrData[5]);
    sendThreeBytes(0x03, brrData[6], brrData[7], brrData[8]);
  }

  // Notify that the data stream has ended
  writedata(1, 0x00);
}

/**
 * Sets the pitch and stereo mode up (call after uploadBrrSongLoader() but before streaming any BRR data)
 * 
 * @param sampleRate The BRR file's sample rate
 * @param stereo If true, the BRR file contains stereo data
 */
void setupLoaderParameters(word sampleRate, bool stereo) {
  word pitch = (word)(((uint32_t)sampleRate * (uint32_t)0x1000) / 32000);
  
  while(readdata(1) != 0xEE) {};
  writedata(0, stereo ? 0x02 : 0x01);
  writedata(2, (pitch & 0xFF00) >> 8);
  writedata(3, pitch & 0xFF);
  writedata(1, 0xEE);
}

#if defined(ARDUINO_AVR_UNO)
  void writeStreamInfo(Adafruit_ST7735 &lcd, const char *filename, word sampleRate, bool stereo) {
    beginLcdWrite();
    clearLcd(lcd);
    drawPgmText(lcd, TEXT_STREAMING, 0, 0);
    drawText(lcd, filename, 0, 10);
    drawPgmText(lcd, TEXT_PRESS_B_TO_STOP, 0, 30);
    endLcdWrite();
  }
#elif defined(ARDUINO_AVR_MEGA2560)
  void writeStreamInfo(Adafruit_ST7735 &lcd, const char *filename, word sampleRate, bool stereo) {
    char buffer[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
    word temp = sampleRate;
    word multiplier = 10000;
    byte offset = 0;
    while (multiplier > 0 && sampleRate > 0) {
      if (multiplier == 100) {
        buffer[offset++] = '.';
      }
      if (sampleRate >= multiplier) {
        byte digit = temp / multiplier;
        temp -= digit * multiplier;
        buffer[offset++] = '0' + digit;
      } else {
        buffer[offset++] = '0';
      }
      multiplier /= 10;
    }
    buffer[offset++] = ' ';
    if (sampleRate >= 1000) {
      buffer[offset++] = 'K';
    }
    buffer[offset++] = 'H';
    buffer[offset++] = 'z';
    
    beginLcdWrite();
    clearLcd(lcd);
    drawPgmText(lcd, TEXT_STREAMING, 0, 0);
    drawText(lcd, filename, 0, 10);
    drawText(lcd, buffer, 0, 30);
    drawPgmText(lcd, stereo ? TEXT_STEREO : TEXT_MONO, 0, 40);
    drawPgmText(lcd, TEXT_PRESS_B_TO_STOP, 0, 60);
    endLcdWrite();
  }
#endif

void streamBrrFileData(File &file, bool stereo) {
  byte buffer[BRR_TRANSFER_BLOCK_SIZE];

  beginSdRead();
  uint32_t brrBlockCount = (file.size() - file.position()) / 9;
  endSdRead();
  uint32_t brrBatchCount = brrBlockCount / BRR_TRANSFER_BLOCK_COUNT;

  uint32_t nextBrrBlock = 0;
  for (uint32_t i = 0; i < brrBatchCount; ++i) {
    word nextBlockTransferCount = min(BRR_TRANSFER_BLOCK_COUNT, (word)min(brrBlockCount - nextBrrBlock, 65535));

    // Fetch the next BRR blocks
    beginSdRead();
    file.read(buffer, nextBlockTransferCount * 9);
    endSdRead();

    uploadBrrBlock(buffer, nextBlockTransferCount, stereo);
    nextBrrBlock += nextBlockTransferCount;

    // Cancel playback (the Uno requires a little more I/O here due to limited pin assignments)
#if defined(ARDUINO_AVR_UNO)
    digitalWrite(PIN_APU_A7, HIGH);
    digitalWrite(PIN_CONTROLLER_LATCH, 0);
#endif
    digitalWrite(PIN_CONTROLLER_LATCH, 1);
    digitalWrite(PIN_CONTROLLER_LATCH, 0);
    byte bState = digitalRead(PIN_CONTROLLER_DATA);
#if defined(ARDUINO_AVR_UNO)
    digitalWrite(PIN_APU_A7, LOW);
#endif
    if (bState == 0) {
      break;
    }
  }
  
  // Upload the terminator
  buffer[0] = 0x03;
  for (int i = 1; i < 18; ++i) {
    buffer[i] = 0x00;
  }
  buffer[9] = 0x03;
  uploadBrrBlock(buffer, stereo ? 2 : 1, stereo);
}

void streamBrrFile(File &file, Adafruit_ST7735 &lcd) {
  // Initialize brr streaming sample rate
  setupLoaderParameters(BRR_FILE_SAMPLE_RATE, false);

  writeStreamInfo(lcd, file.name(), BRR_FILE_SAMPLE_RATE, false);
  streamBrrFileData(file, false);
}

void streamBr2File(File &file, Adafruit_ST7735 &lcd) {
  byte header[4];
  
  beginSdRead();
  file.read(header, 4);
  endSdRead();

  if (header[0] != 1) {
    // Version mismatch
    endSdRead();
    // TODO: Error message?
    return;
  }

  word sampleRate = ((word)header[1] << 8) + header[2];
  setupLoaderParameters(sampleRate, header[3] == 0x02);

  writeStreamInfo(lcd, file.name(), sampleRate, header[3] == 0x02);
  streamBrrFileData(file, header[3] == 0x02);
}

#endif
