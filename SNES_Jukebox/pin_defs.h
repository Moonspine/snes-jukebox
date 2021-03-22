#ifndef PIN_DEFS
#define PIN_DEFS


#if defined(ARDUINO_AVR_UNO)
  /* 
   * Arduino Uno pin mapping
   * 
   * Digital Pin 0 - RX
   * Digital Pin 1 - TX
   * Digital Pin 2-7 - SNES APU D2-7
   * Digital Pin 8 - SNES APU address 0; LCD Reset
   * Digital Pin 9 - SNES APU address 1
   * Digital Pin 10 - SNES APU /RD
   * Digital Pin 11 - SNES APU /WR; SD Card / LCD MOSI; Controller Latch
   * Digital Pin 12 - SNES APU D0; SD Card MISO
   * Digital Pin 13 - SNES APU D1; SD Card / LCD Clk; Controller Clock
   * Analog Pin 0 (AKA Digital Pin 14) - SNES APU /RESET
   * Analog Pin 1 - SD Card CS
   * Analog Pin 2 - SNES APU Address 7
   * Analog Pin 3 - LCD CS
   * Analog Pin 4 - LCD C/D Select
   * Analog Pin 5 - Controller Data
   * Vcc - SNES APU address 6, SNES APU Vcc, SNES APU audio Vcc
   * Gnd - SNES APU Gnd, SNES APU audio Gnd.
   */
  
  #define PIN_SD_CS               A1
  
  #define PIN_LCD_CS              A3
  #define PIN_LCD_C_D             A4
  #define PIN_LCD_RST              8
  #define PIN_LCD_MOSI            11
  #define PIN_LCD_CLK             13
  
  #define PIN_CONTROLLER_DATA     A5
  #define PIN_CONTROLLER_LATCH    11
  #define PIN_CONTROLLER_CLK      13
  
  #define PIN_APU_RST             A0
  #define PIN_APU_A7              A2
  
  #define PIN_SD_MOSI             11
  #define PIN_SD_MISO             12
  #define PIN_SD_CLK              13
  
  #define PORT_DIR_APU_D2_7       DDRD
  #define PORT_OUT_APU_D2_7       PORTD
  #define PORT_IN_APU_D2_7        PIND
  
  #define PORT_DIR_APU_D0_1_CTRL  DDRB
  #define PORT_OUT_APU_D0_1_CTRL  PORTB
  #define PORT_IN_APU_D0_1_CTRL   PINB
#elif defined(ARDUINO_AVR_MEGA2560)
 /* Arduino Mega pin mapping
  *  
  * D50 (PB3) - SD MISO
  * D51 (PB2) - SD/LCD MOSI
  * D52 (PB1) - SD/LCD CLK
  * D53 (PB0) - SD CS
  * D49 (PL0) - LCD CS
  * D48 (PL1) - LCD C/D
  * D47 (PL2) - LCD RST
  * D41 (PG0) - Controller Clock
  * D40 (PG1) - Controller Latch
  * D39 (PG2) - Controller Data
  * D54 (PF0) - APU A0
  * D55 (PF1) - APU A1
  * D56 (PF2) - APU /WR
  * D57 (PF3) - APU /RD
  * D22 (PA0) - APU /RESET
  * D23 (PA1) - APU A7
  * D62..D69 (PK) - APU Data
  * Vcc - APU A6, APU Vcc, APU Audio Vcc
  * Gnd - APU Gnd, APU Audio Gnd
  */

  #define PIN_SD_CS               53
  
  #define PIN_LCD_CS              49
  #define PIN_LCD_C_D             48
  #define PIN_LCD_RST             47
  #define PIN_LCD_MOSI            51
  #define PIN_LCD_CLK             52
  
  #define PIN_CONTROLLER_DATA     39
  #define PIN_CONTROLLER_LATCH    40
  #define PIN_CONTROLLER_CLK      41
  
  #define PIN_APU_RST             22
  #define PIN_APU_A7              23
  
  #define PIN_SD_MOSI             51
  #define PIN_SD_MISO             50
  #define PIN_SD_CLK              52
  
  #define PORT_DIR_APU_D0_7       DDRK
  #define PORT_OUT_APU_D0_7       PORTK
  #define PORT_IN_APU_D0_7        PINK
  
  #define PORT_DIR_APU_CTRL       DDRF
  #define PORT_OUT_APU_CTRL       PORTF
  #define PORT_IN_APU_CTRL        PINF
#endif

#endif
