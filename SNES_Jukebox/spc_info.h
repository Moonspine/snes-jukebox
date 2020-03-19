#ifndef SPC_INFO_H
#define SPC_INFO_H

#include <SD.h>

#include "defines.h"

struct SPCInfo {
public:
  SPCInfo() : shouldLoadExtraRAM(false) {}

  void parseSPC(File &f) {
    int temp;
    
    // Load registers
    f.seek(REGISTERS_START);
    PC_Low = (byte)f.read();
    PC_High = (byte)f.read();
    A = (byte)f.read();
    X = (byte)f.read();
    Y = (byte)f.read();
    PSW = (byte)f.read();
    SP_Low = (byte)f.read();
    
    // Parse extra RAM data
    f.seek(RAM_START + 0xF1);
    temp = f.read();
    if (temp & 0x80) {
      shouldLoadExtraRAM = true;
    }
    
    // Parse echo data
    f.seek(DSP_RAM_START + 0x6C);
    echoEnabled = (f.read() & 0x20) == 0;
    temp = f.read();
    echoRegion = (word)temp << 8;
    f.seek(DSP_RAM_START + 0x7D);
    temp = f.read();
    echoSize = (word)(temp & 0x0F) * 2048;
    if (echoSize == 0) {
      echoSize = 4;
    }
    
    // Locate the best spot for the bootloader
    int echoEnd = echoRegion + echoSize;
    bootCount = 0;
    byte lastByte;
    bool first = true;
    for (bootLocation = 0xFFBF; bootLocation >= 0x100; --bootLocation) {
      if (!echoEnabled || bootLocation > echoEnd || bootLocation < echoRegion) {
        f.seek(RAM_START + bootLocation);
        temp = f.read();
        if(first || temp == lastByte) {
          ++bootCount;
        } else {
          bootCount = 0;
        }
        first = false;
        lastByte = temp;
        if(bootCount == BOOTLOADER_LENGTH) {
          break;
        }
      } else {
        bootCount = 0;
      }
    }

    if(bootLocation == 0xFF) {
      // TODO: Warning if this occurs?
      //if(echoSize < BOOTLOADER_LENGTH) {
      //  return;	//Not enough SPCram for boot code.
      //} else {
        bootLocation = echoRegion;
        bootCount = BOOTLOADER_LENGTH;
      //}
    }
    
    // Load extra data needed for the bootloader
    f.seek(RAM_START + 0xF4);
    extraData[0] = (byte)f.read();
    extraData[1] = (byte)f.read();
    extraData[2] = (byte)f.read();
    extraData[3] = (byte)f.read();
    f.seek(RAM_START);
    extraData[4] = (byte)f.read();
    extraData[5] = (byte)f.read();
    f.seek(RAM_START + 0xFA);
    extraData[8] = (byte)f.read();
    extraData[7] = (byte)f.read();
    extraData[6] = (byte)f.read();
    f.seek(RAM_START + 0xF1);
    extraData[9] = (byte)f.read();
    f.seek(DSP_RAM_START + 0x6C);
    extraData[10] = (byte)f.read();
    f.seek(DSP_RAM_START + 0x4C);
    extraData[11] = (byte)f.read();
    f.seek(RAM_START + 0xF2);
    extraData[12] = (byte)f.read();
  }
  
  byte getPCHigh() { return PC_High; }
  byte getPCLow() { return PC_Low; }
  byte getA() { return A; }
  byte getX() { return X; }
  byte getY() { return Y; }
  byte getPSW() { return PSW; }
  byte getSPLow() { return SP_Low; }
  
  bool getShouldLoadExtraRAM() { return shouldLoadExtraRAM; }
  bool getEchoEnabled() { return echoEnabled; }
  word getEchoRegion() { return echoRegion; }
  word getEchoSize() { return echoSize; }
  word getBootLocation() { return bootLocation; }
  byte getBootCount() { return bootCount; }
  
  byte getExtraData(byte index) {
    return extraData[index];
  }
  
  bool isInPortZero() {
    return extraData[0] == 0 && extraData[1] == 0 && extraData[2] == 0 && extraData[3] == 0;
  }
  
private:
  byte PC_High;
  byte PC_Low;
  byte A;
  byte X;
  byte Y;
  byte PSW;
  byte SP_Low;
  
  bool shouldLoadExtraRAM;
  bool echoEnabled;
  word echoRegion;
  word echoSize;
  word bootLocation;
  byte bootCount;
  
  byte extraData[13];
};

#endif

