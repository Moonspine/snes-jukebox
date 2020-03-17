#ifndef SPC_H
#define SPC_H

unsigned char port0state;

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

#endif

