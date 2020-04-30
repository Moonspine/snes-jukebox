	org 0
	base $0200
	
start:
	mov $f2, #$6c      // Load flags (echo disabled, mute during setup)
	mov $f3, #$60

	mov $f2, #$5c      // Key all unused voices off
	mov $f3, #$fc

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

	mov $f2, #$05      // Load voice[0].adsr[1] (enable gain)
	mov $f3, #$00

	mov $f2, #$07      // Load voice[0].gain
	mov $f3, #$7F

	mov $f2, #$04      // Load voice[0].src
	mov $f3, #$00
	
	
	mov $f2, #$10      // Load voice[1].leftVolume
	mov $f3, #$7f

	mov $f2, #$11      // Load voice[1].rightVolume
	mov $f3, #$7f

	mov $f2, #$15      // Load voice[1].adsr[1] (enable gain)
	mov $f3, #$00

	mov $f2, #$17      // Load voice[1].gain
	mov $f3, #$7F

	mov $f2, #$14      // Load voice[1].src
	mov $f3, #$02
	
	
	// Set up the source directory (samples at $1000, $2000, $3000, $4000)
	mov x, #$00
	mov $500, x
	mov $502, x
	mov $504, x
	mov $506, x
	mov $508, x
	mov $50A, x
	mov $50C, x
	mov $50E, x
	mov x, #$10
	mov $501, x
	mov $503, x
	mov x, #$20
	mov $505, x
	mov $507, x
	mov x, #$30
	mov $509, x
	mov $50B, x
	mov x, #$40
	mov $50D, x
	mov $50F, x
	
	
	// Set up the initial dummy samples (Looping silence)
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
	
	
	mov x, #$03
	mov $3000, x
	mov x, #$00
	mov $3001, x
	mov $3002, x
	mov $3003, x
	mov $3004, x
	mov $3005, x
	mov $3006, x
	mov $3007, x
	mov $3008, x

	
	mov $f2, #$6c      // Load flags (unmute)
	mov $f3, #$20
	
	
	// Wait for initial setup instructions
startSetup:
	// Write output ports to zero
	mov $f4, #$00
	mov $f6, #$00
	mov $f7, #$00

	// Let the arduino know we're ready for initial setup
	mov $f5, #$EE
	
waitSetupLoop:
	// Wait for Arduino to send setup data
	mov a, $f5
	mov $03, a
	cmp a, #$EE
	beq receiveSetup
	
	jmp waitSetupLoop
	
receiveSetup:
	mov $04, $f4 // RAM[4] = Stereo mode (0x02 = stereo, all else = mono)
	
	mov $05, #$00 // RAM[5] = Stereo transfer swap counter
	mov $06, #$00 // RAM[6] = L (0x00) or R (0x01)
	
	cmp $04, #$02
	bne setupSampleRates
	
	// Set volumes up for stereo playback (voice 0 is left and voice 1 is right)
	mov $f2, #$01
	mov $f3, #$00
	mov $f2, #$10
	mov $f3, #$00

setupSampleRates:
	// Set sample rates
	
	// Low
	mov a, $f7
	mov $f2, #$02
	mov $f3, a
	mov $f2, #$12
	mov $f3, a

	// High
	mov a, $f6
	mov $f2, #$03
	mov $f3, a
	mov $f2, #$13
	mov $f3, a
	
	
	// Begin the whole thing
startLoop:
	mov $f2, #$4c      // Key voice 0, 1 on
	mov $f3, #$03
	
	mov $00, #$01     // RAM[0] = Index of next sample to load
	mov $01, #$00     // RAM[1] = Low byte of next address to write on current channel
	mov $02, #$00     // RAM[2] = High byte of next address to write on current channel
	
	
	// Waits for the Arduino to start its transmission ($01 on port 1)
waitForData:
	// Let the Arduino know we're ready
	mov $f5, #$EF
	
waitLoop:
	// Wait for Arduino to begin
	mov $03, $f5 // RAM[3] = Last command ID
	cmp $03, #$01
	beq receiveData
	
	jmp waitLoop
	

	
	
	// Loops until the arduino claims its data has all been sent
receiveData:
	mov y, #$00
	cmp $00, #$00
	bne startReceive1
	
startReceive0:
	mov $02, #$10     // RAM[2] = High byte of next address to write
	bra receiveByte1

startReceive1:
	mov $02, #$20     // RAM[2] = High byte of next address to write
	
	// Receive 3 bytes from ports 0, 2, and 3
receiveByte1: // TODO: Revisit this
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
	// Do stereo swap
	cmp $04, #$02
	bne skipStereoSwapLogic
	
	inc $05
	cmp $05, #$03
	bne skipStereoSwapLogic
	
swapStereo:
	mov $05, #$00
	
	cmp $06, #$01
	beq swapRToL
	
	// Swap L to R
	mov a, y
	setc
	sbc a, #$09
	bcs swapStereoNoDecrement
	dec $02
swapStereoNoDecrement:
	mov y, a
	clrc
	adc $02, #$20
	mov $06, #$01
	
	jmp skipStereoSwapLogic
	
swapRToL:
	// Swap R to L
	setc
	sbc $02, #$20
	mov $06, #$00
	
skipStereoSwapLogic:
	// Notify the Arduino that we are ready for the next 3 bytes
	mov $f5, $03
	
	// Wait for the response
waitForNextBytes:
	mov a, $f5
	cmp a, $03
	beq waitForNextBytes
	
getNextReceiveCommand:
	mov $03, $f5
	cmp $03, #$00
	beq playData

	jmp receiveByte1

	
	
	// Waits for the DSP to finish playing the previous sample and starts the next sample playing
playData:
	// Set the next source index to loop to
	mov $f2, #$04
	mov a, $00
	mov $f3, a
	
	// Set the next source index for the right channel
	cmp $04, #$02
	bne waitForSampleToFinish
	inc a
	inc a
	mov $f2, #$14
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
	inc $00
	and $00, #$01

	jmp waitForData
