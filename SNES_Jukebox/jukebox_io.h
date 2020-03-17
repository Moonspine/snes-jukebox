#ifndef JUKEBOX_IO_H
#define JUKEBOX_IO_H

void setupAPUPins() {
  PORTB = 0x0F;
  DDRB = 0x0F;
  PORTC = 0x01;
  DDRC = 0x01;
  
  // Deselect SD card
  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  
  // Select APU
  pinMode(A2, OUTPUT);
  digitalWrite(A2, LOW);
  
  // LCD init
  pinMode(A3, OUTPUT);
  digitalWrite(A3, HIGH);
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW);
}

void beginSdRead() {
  digitalWrite(A2, HIGH);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  SD.begin(A1);
}

File beginSdRead(char *filename) {
  beginSdRead();
  return SD.open(filename, FILE_READ);
}

void endSdRead() {
  SPI.end();
  setupAPUPins();
}

void endSdRead(File f) {
  f.close();
  endSdRead();
}



void beginLcdWrite() {
  digitalWrite(A2, HIGH);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  SPI.begin();
}

void endLcdWrite() {
  SPI.end();
  setupAPUPins();
}

#endif

