#ifndef SNES_APU_H
#define SNES_APU_H

#include "pin_defs.h"

unsigned char readdata(unsigned char address) {
  unsigned char data;

  PORT_DIR_APU_D0_1_CTRL = 0x0F;
  PORT_DIR_APU_D2_7 &= ~0xFC;
  PORT_OUT_APU_D0_1_CTRL &= ~0x03;
  PORT_OUT_APU_D0_1_CTRL |= (address & 0x03);

  PORT_OUT_APU_D0_1_CTRL &= ~0x04;
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  data = ((PORT_IN_APU_D0_1_CTRL & 0x30) >> 4) | (PORT_IN_APU_D2_7 & 0xFC);
  PORT_OUT_APU_D0_1_CTRL |= 0x04;
  return data;
}

void writedata(unsigned char address, unsigned char data) {
  PORT_DIR_APU_D0_1_CTRL = 0x3F;
  PORT_DIR_APU_D2_7 |= 0xFC;

  PORT_OUT_APU_D2_7 &= ~0xFC;
  PORT_OUT_APU_D2_7 |= (data & 0xFC);

  PORT_OUT_APU_D0_1_CTRL &= ~0x33;
  PORT_OUT_APU_D0_1_CTRL |= (address & 0x03);
  PORT_OUT_APU_D0_1_CTRL |= ((data & 0x03) << 4);
  PORT_OUT_APU_D0_1_CTRL &= ~0x08;
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  PORT_OUT_APU_D0_1_CTRL |= 0x08;
}

void resetApu() {
  digitalWrite(PIN_APU_RST, LOW);
  delay(1);
  digitalWrite(PIN_APU_RST, HIGH);
  
  while(readdata(0) != 0xAA);
  while(readdata(1) != 0xBB);
}

#endif

