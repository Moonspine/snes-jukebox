#ifndef BRR_STREAMER_H
#define BRR_STREAMER_H

#include <SD.h>
#include "text.h"
#include "lcd_draw.h"
#include "progress_bar.h"
#include "snes_apu.h"
#include "jukebox_io.h"


#define PAUSE_TEXT_Y 100

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
  void writeStreamInfo(Adafruit_ST7735 &lcd, File &file, word sampleRate, bool stereo) {
    beginLcdWrite();
    clearLcd(lcd);
    drawPgmText(lcd, TEXT_STREAMING, 0, 0);
    drawText(lcd, file.name(), 0, 10);
    drawPgmText(lcd, TEXT_PRESS_B_TO_STOP, 0, 30);
    endLcdWrite();
  }
#elif defined(ARDUINO_AVR_MEGA2560)
  void drawSampleRate(Adafruit_ST7735 &lcd, uint8_t x, uint8_t y, word sampleRate) {
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

    drawText(lcd, buffer, x, y);
  }

  void drawSongLength(Adafruit_ST7735 &lcd, uint8_t x, uint8_t y, word sampleRate, bool stereo, uint32_t fileSize) {
    char buffer[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    uint32_t brrBlocks = fileSize / 9;
    if (stereo) {
      brrBlocks /= 2;
    }
    uint32_t samples = brrBlocks * 16;
    uint32_t totalSeconds = samples / sampleRate;
    uint8_t seconds = totalSeconds % 60;
    uint8_t minutes = (totalSeconds / 60) % 60;
    uint8_t hours = min(totalSeconds / 3600, 255);

    uint8_t offset = 0;
    if (hours > 0) {
      if (hours >= 100) {
        buffer[offset++] = '0' + (hours / 100);
      }
      if (hours >= 10) {
        buffer[offset++] = '0' + ((hours / 10) % 10);
      }
      if (hours >= 1) {
        buffer[offset++] = '0' + (hours % 10);
      }
      buffer[offset++] = ':';
    }
    if (hours > 0 || minutes > 0) {
      if (hours > 0 || minutes >= 10) {
        buffer[offset++] = '0' + ((minutes / 10) % 10);
      }
      if (hours > 0 || minutes >= 1) {
        buffer[offset++] = '0' + (minutes % 10);
      }
      buffer[offset++] = ':';
    }
    if (hours > 0 || minutes > 0 || seconds >= 10) {
      buffer[offset++] = '0' + ((seconds / 10) % 10);
    }
    buffer[offset++] = '0' + (seconds % 10);

    drawText(lcd, buffer, x, y);
  }

  void writeStreamInfo(Adafruit_ST7735 &lcd, File &file, word sampleRate, bool stereo) {
    beginSdRead();
    uint32_t fileSize = file.size();
    endSdRead();
    
    beginLcdWrite();
    clearLcd(lcd);
    drawPgmText(lcd, TEXT_STREAMING, 0, 0);
    drawText(lcd, file.name(), 0, 10);
    drawSampleRate(lcd, 0, 30, sampleRate);
    drawPgmText(lcd, stereo ? TEXT_STEREO : TEXT_MONO, 0, 40);
    drawSongLength(lcd, 0, 50, sampleRate, stereo, fileSize);
    drawPgmText(lcd, TEXT_PRESS_B_TO_STOP, 0, 90);
    drawPgmText(lcd, TEXT_PRESS_A_TO_PAUSE, 0, PAUSE_TEXT_Y);
    drawPgmText(lcd, TEXT_PRESS_LEFT_RIGHT_TO_SEEK, 0, 110);
    endLcdWrite();
  }
#endif

void streamBrrFileData(File &file, bool stereo, Adafruit_ST7735 &lcd, SNESController &controller) {
  byte buffer[BRR_TRANSFER_BLOCK_SIZE];

  beginSdRead();
  uint32_t brrBlockCount = (file.size() - file.position()) / 9;
  endSdRead();
  uint32_t brrBatchCount = brrBlockCount / BRR_TRANSFER_BLOCK_COUNT;
  if (brrBatchCount * BRR_TRANSFER_BLOCK_COUNT < brrBlockCount) {
    ++brrBatchCount;
  }

#if defined(ARDUINO_AVR_MEGA2560)
  ProgressBar progressBar(lcd, 0, 70, 128, brrBatchCount);
#endif

  uint32_t nextBrrBlock = 0;
  for (uint32_t i = 0; i < brrBatchCount; ++i) {
    word nextBlockTransferCount = min(BRR_TRANSFER_BLOCK_COUNT, (word)min(brrBlockCount - nextBrrBlock, 65535));

    // Fetch the next BRR blocks
    beginSdRead();
    file.read(buffer, nextBlockTransferCount * 9);
    endSdRead();

    // Upload the BRR blocks and advance
    uploadBrrBlock(buffer, nextBlockTransferCount, stereo);
    nextBrrBlock += nextBlockTransferCount;

#if defined(ARDUINO_AVR_UNO)
    // Cancel playback (the Uno requires a little more I/O here due to limited pin assignments)
    digitalWrite(PIN_APU_A7, HIGH);
    digitalWrite(PIN_CONTROLLER_LATCH, 0);
    digitalWrite(PIN_CONTROLLER_LATCH, 1);
    digitalWrite(PIN_CONTROLLER_LATCH, 0);
    bool bState = digitalRead(PIN_CONTROLLER_DATA) == 0;
    digitalWrite(PIN_APU_A7, LOW);
#elif defined(ARDUINO_AVR_MEGA2560)
    bool bState = false;
    controller.update(CONTROLLER_DEBOUNCE_DELAY);
    if (controller.justPressed(SNESController::B)) {
      // Stop
      bState = true;
    } else if (controller.justPressed(SNESController::A)) {
      // Pause/unpause
      controller.clearJustPressed();

      // Upload looping silence
      buffer[0] = 0x03;
      for (int i = 1; i < 18; ++i) {
        buffer[i] = 0x00;
      }
      buffer[9] = 0x03;
      uploadBrrBlock(buffer, stereo ? 2 : 1, stereo);

      lcd.fillRect(0, PAUSE_TEXT_Y, 128, 20, ST7735_BLACK);
      drawPgmText(lcd, TEXT_PRESS_A_TO_UNPAUSE, 0, PAUSE_TEXT_Y);
      bool paused = true;
      while (paused) {
        controller.update(CONTROLLER_DEBOUNCE_DELAY);
        if (controller.justPressed(SNESController::A)) {
          paused = false;
          lcd.fillRect(0, PAUSE_TEXT_Y, 128, 10, ST7735_BLACK);
          drawPgmText(lcd, TEXT_PRESS_A_TO_PAUSE, 0, PAUSE_TEXT_Y);
          drawPgmText(lcd, TEXT_PRESS_LEFT_RIGHT_TO_SEEK, 0, PAUSE_TEXT_Y + 10);
        } else if (controller.justPressed(SNESController::B)) {
          controller.clearJustPressed();
          paused = false;
          bState = true;
        }
      }
    } else if (controller.justPressed(SNESController::LEFT)) {
      // Seek backward
      uint32_t seekAmount = min(i, BRR_SEEK_AMOUNT * (stereo ? 2 : 1));
      i -= seekAmount;
      nextBrrBlock -= seekAmount * BRR_TRANSFER_BLOCK_COUNT;
      file.seek(file.position() - seekAmount * BRR_TRANSFER_BLOCK_COUNT * 9);
      progressBar.setProgress(i);
    } else if (controller.justPressed(SNESController::RIGHT)) {
      // Seek forward
      uint32_t seekAmount = BRR_SEEK_AMOUNT * (stereo ? 2 : 1);
      i += seekAmount;
      nextBrrBlock += seekAmount * BRR_TRANSFER_BLOCK_COUNT;
      file.seek(file.position() + (uint32_t)seekAmount * BRR_TRANSFER_BLOCK_COUNT * 9);
      progressBar.setProgress(i);
    }
    controller.clearJustPressed();

    progressBar.addProgress(1);
#endif

    if (bState) {
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

void streamBrrFile(File &file, Adafruit_ST7735 &lcd, SNESController &controller) {
  // Initialize brr streaming sample rate
  setupLoaderParameters(BRR_FILE_SAMPLE_RATE, false);

  writeStreamInfo(lcd, file, BRR_FILE_SAMPLE_RATE, false);
  streamBrrFileData(file, false, lcd, controller);
}

void streamBr2File(File &file, Adafruit_ST7735 &lcd, SNESController &controller) {
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

  writeStreamInfo(lcd, file, sampleRate, header[3] == 0x02);
  streamBrrFileData(file, header[3] == 0x02, lcd, controller);
}

#endif
