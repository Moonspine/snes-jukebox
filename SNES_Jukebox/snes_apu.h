#ifndef SNES_APU_H
#define SNES_APU_H

unsigned char readdata(unsigned char address)
{
  unsigned char data;

  DDRB = 0x0F;
  DDRD &= ~0xFC;
  PORTB &= ~0x03;
  PORTB |= (address & 0x03);

  PORTB &= ~0x04;
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  data=((PINB&0x30)>>4) | (PIND&0xFC);
  PORTB |= 0x04;
  return data;
}

void writedata(unsigned char address, unsigned char data)
{
  DDRB = 0x3F;
  DDRD |= 0xFC;

  PORTD &= ~0xFC;
  PORTD |= (data & 0xFC);

  PORTB &= ~0x33;
  PORTB |= (address & 0x03);
  PORTB |= ((data & 0x03)<<4);
  PORTB &= ~0x08;
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  PORTB |= 0x08;
}

void resetApu() {
  digitalWrite(A0, LOW);
  delay(1);
  digitalWrite(A0, HIGH);
  
  while(readdata(0)!=0xAA);
  while(readdata(1)!=0xBB);
}

#endif

