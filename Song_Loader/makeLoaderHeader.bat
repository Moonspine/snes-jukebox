del song_loader.bin
del song_loader.h
..\Bin\TPS\bass -arch=snes-smp-canonical -o song_loader.bin song_loader.asm
..\Bin\Moonspine\Bin2Header song_loader.bin > song_loader.h
