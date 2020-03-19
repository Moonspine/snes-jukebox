#ifndef PIN_DEFS
#define PIN_DEFS

/* IO Pin Mapping.
 ** Digital Pin 0 - RX
 ** Digital Pin 1 - TX
 ** Digital Pin 2-7 - SNES APU D2-7
 ** Digital Pin 8 - SNES APU address 0; LCD Reset
 ** Digital Pin 9 - SNES APU address 1
 ** Digital Pin 10 - SNES APU /RD
 ** Digital Pin 11 - SNES APU /WR; SD Card / LCD MOSI; Controller Latch
 ** Digital Pin 12 - SNES APU D0; SD Card MISO
 ** Digital Pin 13 - SNES APU D1; SD Card / LCD Clk; Controller Clock
 ** Analog Pin 0 (AKA Digital Pin 14) - SNES APU /RESET
 ** Analog Pin 1 - SD Card CS
 ** Analog Pin 2 - SNES APU Address 7
 ** Analog Pin 3 - LCD CS
 ** Analog Pin 4 - LCD C/D Select
 ** Analog Pin 5 - Controller Data
 ** Vcc - SNES APU address 6, SNES APU Vcc, SNES APU audio Vcc
 ** Gnd - SNES APU Gnd, SNES APU audio Gnd.
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

#endif

