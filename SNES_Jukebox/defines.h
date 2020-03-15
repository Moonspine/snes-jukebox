#ifndef DEFINES_H
#define DEFINES_H

#define REGISTERS_START (uint32_t)0x25
#define RAM_START       (uint32_t)0x100
#define DSP_RAM_START   (uint32_t)0x10100
#define EXTRA_RAM_START (uint32_t)0x101C0

#define SPC_UPLOAD_BUFFER_SIZE 512

static PROGMEM prog_uchar bootloaderCode[] = {
                                                0x8F, 0x00, 0x00, // mov ($00), #[info.getExtraData(4)]
                                                0x8F, 0x00, 0x01, // mov ($01), #[info.getExtraData(5)]
                                                0x8F, 0xFF, 0xFC, // mov ($fc), #[info.getExtraData(6)]
                                                0x8F, 0xFF, 0xFB, // mov ($fb), #[info.getExtraData(7)]
                                                0x8F, 0x4F, 0xFA, // mov ($fa), #[info.getExtraData(8)]
                                                0x8F, 0x31, 0xF1, // mov ($f1), #[info.getExtraData(9)]
                                                0xCD, 0x53,       // mov X, #$53
                                                0xD8, 0xF4,       // mov ($f4), X
                                                0xE4, 0xF4,       // mov A, ($f4)
                                                0x68, 0x00,       // cmp A, #[info.getExtraData(0)]
                                                0xD0, 0xFA,       // bne -6
                                                0xE4, 0xF5,       // mov A, ($f5)
                                                0x68, 0x00,       // cmp A, #[info.getExtraData(1)]
                                                0xD0, 0xFA,       // bne -6
                                                0xE4, 0xF6,       // mov A, ($f6)
                                                0x68, 0x00,       // cmp A, #[info.getExtraData(2)]
                                                0xD0, 0xFA,       // bne -6
                                                0xE4, 0xF7,       // mov A, ($f7)
                                                0x68, 0x00,       // cmp A, #[info.getExtraData(3)]
                                                0xD0, 0xFA,       // bne -6
                                                0xE4, 0xFD,       // mov A, ($fd)
                                                0xE4, 0xFE,       // mov A, ($fe)
                                                0xE4, 0xFF,       // mov A, ($ff)
                                                0x8F, 0x6C, 0xF2, // mov ($f2), #$6c
                                                0x8F, 0x00, 0xF3, // mov ($f3), #[info.getExtraData(10)]
                                                0x8F, 0x4C, 0xF2, // mov ($f2), #$4c
                                                0x8F, 0x00, 0xF3, // mov ($f3), #[info.getExtraData(11)]
                                                0x8F, 0x7F, 0xF2, // mov ($f2), #[info.getExtraData(12)]
                                                0xCD, 0xF5,       // mov X, #[info.getSPLow()]
                                                0xBD,             // mov SP, X
                                                0xE8, 0x00,       // mov A, #[info.getPSW()]
                                                0x2D,             // PUSH A
                                                0x8E,             // POP PSW
                                                0xE8, 0xFF,       // mov A, #[info.getA()]
                                                0x8D, 0x00,       // mov Y, #[info.getY()]
                                                0xCD, 0x00,       // mov X, #[info.getX()]
                                                0x5F, 0x00, 0x00  // JMP [(info.getPCHigh() << 8) + info.getPCLow()]
                                             };
                                             
#define BOOTLOADER_LENGTH 83

static PROGMEM prog_uchar DSPdata[] = {
  0xC4, 0xF2,       // mov ($f2), A
  0x64, 0xF4,       // cmp a, ($f4)
  0xD0, 0xFC,       // bne -4
  0xFA, 0xF5, 0xF3, // mov ($f3), ($f5)
  0xC4, 0xF4,       // mov ($f4), A
  0xBC,             // inc A
  0x10, 0xF2,       // bpl -14
  0x2F, 0xB7        // bra -73
};

#define DSP_DATA_LENGTH 16



#define BRR_TRANSFER_BLOCK_COUNT 50
#define BRR_TRANSFER_BLOCK_SIZE BRR_TRANSFER_BLOCK_COUNT * 9

static PROGMEM prog_uint8_t songLoaderData[] = {
  0x8F, 0x0C, 0xF2, 0x8F, 0x7F, 0xF3, 0x8F, 0x1C, 0xF2, 0x8F, 0x7F, 0xF3, 0x8F, 0x2C, 0xF2, 0x8F,
  0x00, 0xF3, 0x8F, 0x3C, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x2D, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x3D,
  0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x5D, 0xF2, 0x8F, 0x05, 0xF3, 0x8F, 0x00, 0xF2, 0x8F, 0x7F, 0xF3,
  0x8F, 0x01, 0xF2, 0x8F, 0x7F, 0xF3, 0x8F, 0x02, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x03, 0xF2, 0x8F,
  0x08, 0xF3, 0x8F, 0x05, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x07, 0xF2, 0x8F, 0x7F, 0xF3, 0x8F, 0x04,
  0xF2, 0x8F, 0x00, 0xF3, 0xCD, 0x03, 0xC9, 0x00, 0x10, 0xCD, 0x00, 0xC9, 0x01, 0x10, 0xC9, 0x02,
  0x10, 0xC9, 0x03, 0x10, 0xC9, 0x04, 0x10, 0xC9, 0x05, 0x10, 0xC9, 0x06, 0x10, 0xC9, 0x07, 0x10,
  0xC9, 0x08, 0x10, 0x8F, 0x10, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x11, 0xF2, 0x8F, 0x00, 0xF3, 0x8F,
  0x20, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x21, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x30, 0xF2, 0x8F, 0x00,
  0xF3, 0x8F, 0x31, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x40, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x41, 0xF2,
  0x8F, 0x00, 0xF3, 0x8F, 0x50, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x51, 0xF2, 0x8F, 0x00, 0xF3, 0x8F,
  0x60, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x61, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x70, 0xF2, 0x8F, 0x00,
  0xF3, 0x8F, 0x71, 0xF2, 0x8F, 0x00, 0xF3, 0x8F, 0x5C, 0xF2, 0x8F, 0xFE, 0xF3, 0x8F, 0x6C, 0xF2,
  0x8F, 0x20, 0xF3, 0x8F, 0x4C, 0xF2, 0x8F, 0x01, 0xF3, 0x8F, 0x00, 0xF4, 0x8F, 0x00, 0xF6, 0x8F,
  0x00, 0xF7, 0x8F, 0x01, 0x00, 0x8F, 0x00, 0x01, 0x8F, 0x00, 0x02, 0x8F, 0xEF, 0xF5, 0xE4, 0xF5,
  0xC4, 0x03, 0x68, 0x01, 0xF0, 0x03, 0x5F, 0xEE, 0x02, 0x8D, 0x00, 0xE4, 0x00, 0x68, 0x00, 0xD0,
  0x05, 0x8F, 0x10, 0x02, 0x2F, 0x03, 0x8F, 0x20, 0x02, 0xE4, 0xF4, 0xD7, 0x01, 0xFC, 0xAD, 0x00,
  0xD0, 0x02, 0xAB, 0x02, 0xE4, 0xF6, 0xD7, 0x01, 0xFC, 0xAD, 0x00, 0xD0, 0x02, 0xAB, 0x02, 0xE4,
  0xF7, 0xD7, 0x01, 0xFC, 0xAD, 0x00, 0xD0, 0x02, 0xAB, 0x02, 0xE4, 0x03, 0xC4, 0xF5, 0xE4, 0xF5,
  0x64, 0x03, 0xF0, 0xFA, 0xE4, 0xF5, 0xC4, 0x03, 0x68, 0x00, 0xF0, 0x03, 0x5F, 0x09, 0x03, 0x8F,
  0x04, 0xF2, 0xE4, 0x00, 0xC4, 0xF3, 0x8F, 0x7C, 0xF2, 0xE4, 0xF3, 0x28, 0x01, 0x68, 0x01, 0xD0,
  0xF8, 0x8F, 0x00, 0xF3, 0xE4, 0x00, 0xBC, 0x28, 0x01, 0xC4, 0x00, 0x5F, 0xEB, 0x02, 0x00, 0xD0,
  0x05, 0x8F, 0x10, 0x02, 0x2F, 0x03, 0x8F, 0x20, 0x02, 0xE4, 0xF4, 0xD7, 0x01, 0xFC, 0xAD, 0x00,
  0xD0, 0x02, 0xAB, 0x02, 0xE4, 0xF6, 0xD7, 0x01, 0xFC, 0xAD, 0x00, 0xD0, 0x02, 0xAB, 0x02, 0xE4,
  0xF7, 0xD7, 0x01, 0xFC, 0xAD, 0x00, 0xD0, 0x02, 0xAB, 0x02, 0xE4, 0x03, 0xC4, 0xF5, 0xE4, 0xF5,
  0x64, 0x03, 0xF0, 0xFA, 0xE4, 0xF5, 0xC4, 0x03, 0x68, 0x00, 0xF0, 0x03, 0x5F, 0x69, 0x03, 0x8F,
  0x04, 0xF2, 0xE4, 0x00, 0xC4, 0xF3, 0x8F, 0x7C, 0xF2, 0xE4, 0xF3, 0x28, 0x01, 0x68, 0x01, 0xD0,
  0xF8, 0x8F, 0x00, 0xF3, 0xE4, 0x00, 0xBC, 0x28, 0x01, 0xC4, 0x00, 0x5F, 0x4B, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x00, 0x10, 0x00, 0x20, 0x00, 0x20
};

#define SONG_LOADER_DATA_LENGTH 776

#endif

