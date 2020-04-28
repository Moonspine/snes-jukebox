#ifndef SNES_APU_H
#define SNES_APU_H

#include "pin_defs.h"

#if defined(ARDUINO_AVR_UNO)
  unsigned char readdata(unsigned char address) {
    unsigned char data;
  
    // Set data pins as inputs
    PORT_DIR_APU_D0_1_CTRL = 0x0F;
    PORT_DIR_APU_D2_7 &= ~0xFC;
  
    // Set bits 0, 1 to address
    // Set /RD low (active)
    PORT_OUT_APU_D0_1_CTRL = ((PORT_OUT_APU_D0_1_CTRL & ~0x03) | (address & 0x03)) & ~0x04;
  
    // Read delay
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
  
    // Read data
    data = ((PORT_IN_APU_D0_1_CTRL & 0x30) >> 4) | (PORT_IN_APU_D2_7 & 0xFC);
  
    // Set /RD high (inactive)
    PORT_OUT_APU_D0_1_CTRL |= 0x04;
    
    return data;
  }
  
  void writedata(unsigned char address, unsigned char data) {
    // Set data pins as outputs
    PORT_DIR_APU_D0_1_CTRL = 0x3F;
    PORT_DIR_APU_D2_7 |= 0xFC;
  
    // Set bits 2..7 to data bits 2..7
    PORT_OUT_APU_D2_7 = (PORT_OUT_APU_D2_7 & ~0xFC) | (data & 0xFC);
  
    // Clear address an data bits and set /WR low (active)
    // Set bits 0, 1 to address
    // Set bits 4, 5 to data bits 0, 1
    PORT_OUT_APU_D0_1_CTRL = (PORT_OUT_APU_D0_1_CTRL & ~0x3B) | (address & 0x03) | ((data & 0x03) << 4);
  
    // Write delay
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
  
    // Set /WR high (inactive)
    PORT_OUT_APU_D0_1_CTRL |= 0x08;
  }
#elif defined(ARDUINO_AVR_MEGA2560)
  unsigned char readdata(unsigned char address) {
    unsigned char data;
  
    // Set data pins as inputs
    PORT_DIR_APU_D0_7 = 0x00;
  
    // Set bits 0, 1 to address
    // Set /RD low (active) and /WR high (inactive)
    PORT_OUT_APU_CTRL = 0x04 | (address & 0x03);
  
    // Read delay
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
  
    // Read data
    data = PORT_IN_APU_D0_7;
  
    // Set /RD high (inactive)
    PORT_OUT_APU_CTRL |= 0x08;
    
    return data;
  }
  
  void writedata(unsigned char address, unsigned char data) {
    // Set data pins as outputs
    PORT_DIR_APU_D0_7 = 0xFF;
  
    // Set bits
    PORT_OUT_APU_D0_7 = data;
  
    // Clear address bits and set /WR low (active) and /RD high (inactive)
    // Set bits 0, 1 to address
    PORT_OUT_APU_CTRL = 0x08 | (address & 0x03);
  
    // Write delay
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
  
    // Set /WR high (inactive)
    PORT_OUT_APU_CTRL |= 0x04;
  }
#endif

void resetApu() {
  digitalWrite(PIN_APU_RST, LOW);
  delay(1);
  digitalWrite(PIN_APU_RST, HIGH);
  
  while(readdata(0) != 0xAA);
  while(readdata(1) != 0xBB);
}

#endif
