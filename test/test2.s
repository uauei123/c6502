.org $8000
start:
	LDA #21
	LDX #0
divide:
	INX
	SBC #2
	BEQ result
	JMP divide
result:
	TXA
	AND $8011 ; A & 0x10
loop:
	JMP loop

.org $FFFC
.word start
