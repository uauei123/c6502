.org $90
.word $2010

.org $80
.byte $5A

.org $2010
.byte $4B

.org $100
.word $2010

.org $70
.word $2000

.org $3012
.word $2010

.org $8000
start:
	LDA #$10 ; loads 10
        LDA #$00
        
        LDA $80  ; loads 5A
        LDA #$00

        LDA $2010 ; loads 4B
        LDA #$00

        LDX #$02
        LDA $7E,X ; loads 5A
        LDA #$00

        LDY #$02
        LDX $7E,Y
        TXA        ; loads 5A
        LDA #$00

        LDX #$10
        LDA $2000,X
        LDA #$00

        LDY #$10
        LDA $2000,Y
        LDA #$00

        JMP ($4000)

re:

        LDX #$01
        LDA ($8F,X)
        LDA #$00

        LDY #$10
        LDA ($70),Y
        LDA #$00

        JMP halt

indirect:
        LDA #$19
        LDA #$00
        JMP re

halt:
        JMP halt


.org $FFFC
.word start

.org $4000
.word indirect
