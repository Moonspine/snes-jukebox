	org 0
	base $0200
	
start:

	mov $f2, #$0c      // Load master volume left
	mov $f3, #$7f
	
	mov $f2, #$1c      // Load master volume right
	mov $f3, #$7f
	
	mov $f2, #$2c      // Load echo volume left
	mov $f3, #$00
	
	mov $f2, #$3c      // Load echo volume right
	mov $f3, #$00

	mov $f2, #$2d      // Disable pitch modulation
	mov $f3, #$00

	mov $f2, #$3d      // Disable noise
	mov $f3, #$00

	mov $f2, #$5d      // Load src directory
	mov $f3, #$05

	
	mov $f2, #$00      // Load voice[0].leftVolume
	mov $f3, #$7f

	mov $f2, #$01      // Load voice[0].rightVolume
	mov $f3, #$7f

	mov $f2, #$02      // Load voice[0].pitch[L]
	mov $f3, #$00

	mov $f2, #$03      // Load voice[0].pitch[H] (1/2 of standard pitch for data compression)
	mov $f3, #$08

	mov $f2, #$05      // Load voice[0].adsr[1] (enable gain)
	mov $f3, #$00

	mov $f2, #$07      // Load voice[0].gain
	mov $f3, #$7F

	mov $f2, #$04      // Load voice[0].src
	mov $f3, #$00
	
	
	// Set up the initial dummy sample (Looping silence)
	mov x, #$03
	mov $1000, x
	mov x, #$00
	mov $1001, x
	mov $1002, x
	mov $1003, x
	mov $1004, x
	mov $1005, x
	mov $1006, x
	mov $1007, x
	mov $1008, x
	
	
	// Silence unused voices
	mov $f2, #$10      // Voice[1]
	mov $f3, #$00
	mov $f2, #$11
	mov $f3, #$00
	
	mov $f2, #$20      // Voice[2]
	mov $f3, #$00
	mov $f2, #$21
	mov $f3, #$00
	
	mov $f2, #$30      // Voice[3]
	mov $f3, #$00
	mov $f2, #$31
	mov $f3, #$00
	
	mov $f2, #$40      // Voice[4]
	mov $f3, #$00
	mov $f2, #$41
	mov $f3, #$00
	
	mov $f2, #$50      // Voice[5]
	mov $f3, #$00
	mov $f2, #$51
	mov $f3, #$00
	
	mov $f2, #$60      // Voice[6]
	mov $f3, #$00
	mov $f2, #$61
	mov $f3, #$00
	
	mov $f2, #$70      // Voice[7]
	mov $f3, #$00
	mov $f2, #$71
	mov $f3, #$00
	
	mov $f2, #$5c      // Key unused voices off
	mov $f3, #$fe

	
	mov $f2, #$6c      // Load flags (echo disabled)
	mov $f3, #$20
	
	
	// Begin the whole thing
startLoop:

	mov $f2, #$4c      // Key voice 0 on
	mov $f3, #$01
	
	mov $f4, #$00
	mov $f6, #$00
	mov $f7, #$00
	
	mov $00, #$01     // RAM[0] = Index of next sample to load
	mov $01, #$00     // RAM[1] = Low byte of next address to write
	mov $02, #$00     // RAM[2] = High byte of next address to write
	
	
	// Waits for the Arduino to start its transmission ($01 on port 1)
waitForData:

	// Let the Arduino know we're ready
	mov $f5, #$EF
	
waitLoop:

	// Wait for Arduino to begin
	mov a, $f5
	mov $03, a
	cmp a, #$01
	beq receiveData
	
	jmp waitLoop
	

	
	
	// Loops until the arduino claims its data has all been sent
receiveData:

	mov y, #$00
	mov a, $00
	cmp a, #$00
	bne startReceive1
	
startReceive0:
	mov $02, #$10     // RAM[2] = High byte of next address to write
	bra receiveByte1

startReceive1:
	mov $02, #$20     // RAM[2] = High byte of next address to write
	
	// Receive 3 bytes from ports 0, 2, and 3
receiveByte1:
	mov a, $f4
	mov ($01)+y, a
	inc y
	cmp y, #$00
	bne receiveByte2
	inc $02
receiveByte2:
	mov a, $f6
	mov ($01)+y, a
	inc y
	cmp y, #$00
	bne receiveByte3
	inc $02
receiveByte3:
	mov a, $f7
	mov ($01)+y, a
	inc y
	cmp y, #$00
	bne doneWithThreeBytes
	inc $02
	
doneWithThreeBytes:

	// Notify the Arduino that we are ready for the next 3 bytes
	mov a, $03
	mov $f5, a
	
	// Wait for the response
waitForNextBytes:
	mov a, $f5
	cmp a, $03
	beq waitForNextBytes
	
getNextReceiveCommand:
	
	mov a, $f5
	mov $03, a
	
	cmp a, #$00
	beq playData

	jmp receiveByte1

	
	
	
	// Waits for the DSP to finish playing the previous sample and starts the next sample playing
playData:

	// Set the next source index to loop to
	mov $f2, #$04
	mov a, $00
	mov $f3, a

waitForSampleToFinish:

	mov $f2, #$7c
	
continueWaitingForSample:
	mov a, $f3
	and a, #$01
	cmp a, #$01
	bne continueWaitingForSample

finishPlay:

	// Clear endX
	mov $f3, #$00
	
	// Switch which sample we're on
	mov a, $00
	inc a
	and a, #$01
	mov $00, a

	jmp waitForData
	
	
	
	
	org $0300 // $500 in RAM (base is at $200)
srcdir:
	dw $1000  // First sample is at $1000 (up to 4KB, but likely only 630 bytes)
	dw $1000
	dw $2000  // Second sample is at $2000 (up to 4KB, but likely only 630 bytes)
	dw $2000
