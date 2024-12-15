.org $8000
start:
 LDA #$00
 LDX #$00
 LDY #$00
 CLD
 CLC
 CLV
 SEI

 ; TEST 1
 LDA #$42
 STA $0200
 LDA $0200
 BEQ lda_fail ; shouldnt branch

 ; TEST 2
 LDX #$3
 STX $0203
 LDA $0200,X
 BEQ ldx_fail ; should not branch

 ; TEST 3
 LDY #$08
 STY $0208
 LDA $0200,Y
 BEQ ldy_fail ; should not branch

 ; TEST 4
 LDA #$10
 ADC #$05
 CMP #$15
 BNE adc_fail ; should not branch

 ; TEST 5
 SBC #$05
 CMP #$10
 BNE sbc_fail ; should not branch

 ; TEST 6
 LDA #$F0
 AND #$0F
 CMP #$00
 BNE and_fail ; should not branch
 
 ; TEST 7
 ORA #$0F
 CMP #$0F
 BNE ora_fail ; should not branch

 ; TEST 8
 EOR #$0F
 CMP #$00
 BNE eor_fail ; should not branch

 ; TEST 9
 LDA #$AA
 PHA
 PLA
 CMP #$AA
 BNE pla_fail ; should not branch

 ; TEST 10
 LDA #$00
 BEQ beq_pass ; should branch
 JMP beq_fail

beq_pass:
 LDA #$01
 BNE bne_pass ; should branch
 JMP bne_fail

bne_pass:
 LDA #$80
 BMI bmi_pass ; should branch
 JMP bmi_fail

bmi_pass:
 LDA #$7F
 BPL bpl_pass ; should branch
 JMP bpl_fail

bpl_pass:
 ; all test passed
 LDA #$00 ; indicates success
 JMP halt



lda_fail:  
 LDA #$01 
 JMP halt

ldx_fail:  
 LDA #$02 
 JMP halt

ldy_fail:  
 LDA #$03 
 JMP halt

adc_fail:  
 LDA #$04 
 JMP halt

sbc_fail:  
 LDA #$05 
 JMP halt

and_fail:  
 LDA #$06 
 JMP halt

ora_fail:  
 LDA #$07 
 JMP halt

eor_fail:  
 LDA #$08 
 JMP halt

pla_fail:  
 LDA #$09 
 JMP halt

beq_fail:  
 LDA #$0A 
 JMP halt

bne_fail:  
 LDA #$0B 
 JMP halt

bmi_fail:  
 LDA #$0C 
 JMP halt

bpl_fail:  
 LDA #$0D 
 JMP halt



halt:
 JMP halt

.org $FFFC
.word start