.org $8000
start:
	LDX #$10
	LDA #$00
	TAY
clearmem:
	STA ($80),Y
	INY
	DEX
	BNE clearmem
loop:
	JMP loop

.org $FFFC
.word start
