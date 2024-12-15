#ifndef CPU_H
#define CPU_H



#include <stdint.h>



// ------- ADDRESSES -----------------
#define STACK_START         0x0100
#define STACK_END           0x01FF
#define NMI_ADDRESS         0xFFFA
#define RESET_ADDRESS       0xFFFC
#define IRQ_ADDRESS	        0xFFFE
// -----------------------------------



// ------- FLAGS ----------
#define CARRY_FLAG        0
#define ZERO_FLAG         1
#define INTERRUPT_DISABLE 2
#define DECIMAL_MODE      3
#define BREAK_FLAG        4
#define OVERFLOW_FLAG     5
#define NEGATIVE_FLAG     6
// ------------------------



extern uint8_t MEMORY[0xFFFF];



extern uint64_t cycles;



typedef struct {
    uint16_t pc;        // PROGRAM COUNTER
    uint8_t  sp;        // STACK POINTER
    uint8_t  A_reg;     // ACCUMULATOR
    uint8_t  X_reg;     // INDEX REGISTER X
    uint8_t  Y_reg;     // INDEX REGISTER Y
    uint8_t  status[7]; // PROCESSOR STATUS
} CPU;

typedef enum {
	ADDRESSING_ACCUMULATOR,
	ADDRESSING_IMPLIED,
	ADDRESSING_IMMEDIATE,
	ADDRESSING_ZERO_PAGE,
	ADDRESSING_ABSOLUTE,
	ADDRESSING_ZERO_PAGE_X,
	ADDRESSING_ZERO_PAGE_Y,
	ADDRESSING_ABSOLUTE_X,
	ADDRESSING_ABSOLUTE_Y,
	ADDRESSING_INDIRECT,
	ADDRESSING_INDIRECT_X,
	ADDRESSING_INDIRECT_Y,
	ADDRESSING_RELATIVE,
	ADDRESSING_NONE
} addr_mode;



typedef void (*OpcodeFunction)(CPU *cpu, addr_mode adm);



typedef struct {
	OpcodeFunction ptr;
	addr_mode adm;
	uint8_t cycles;
} Opcode;



int cpu_init(CPU *cpu);
uint8_t fetch(CPU *cpu);
int execute(CPU *cpu, uint8_t opcode);
int cycle(CPU *cpu);

uint16_t get_operand_address(CPU *cpu, addr_mode addr);

uint8_t read(CPU *cpu, uint16_t address);
uint16_t read_word(CPU *cpu, uint16_t address);

uint8_t get_hi(uint16_t value);



void set_sign(CPU *cpu, uint8_t data);
void set_zero(CPU *cpu, uint8_t data);
void set_conditional_flag(CPU *cpu, int condition, int flag);



void push(CPU *cpu, uint8_t value);
uint8_t pull(CPU *cpu);



// ----- OPCODES -------

// - IMPLIED ADDRESSING -
void opcode_BRK(CPU *cpu, addr_mode adm);
void opcode_CLC(CPU *cpu, addr_mode adm);
void opcode_CLD(CPU *cpu, addr_mode adm);
void opcode_CLI(CPU *cpu, addr_mode adm);
void opcode_CLV(CPU *cpu, addr_mode adm);
void opcode_DEX(CPU *cpu, addr_mode adm);
void opcode_DEY(CPU *cpu, addr_mode adm);
void opcode_INX(CPU *cpu, addr_mode adm);
void opcode_INY(CPU *cpu, addr_mode adm);
void opcode_NOP(CPU *cpu, addr_mode adm);
void opcode_PHA(CPU *cpu, addr_mode adm);
void opcode_PHP(CPU *cpu, addr_mode adm);
void opcode_PLA(CPU *cpu, addr_mode adm);
void opcode_PLP(CPU *cpu, addr_mode adm);
void opcode_RTI(CPU *cpu, addr_mode adm);
void opcode_RTS(CPU *cpu, addr_mode adm);
void opcode_SEC(CPU *cpu, addr_mode adm);
void opcode_SED(CPU *cpu, addr_mode adm);
void opcode_SEI(CPU *cpu, addr_mode adm);
void opcode_TAX(CPU *cpu, addr_mode adm);
void opcode_TAY(CPU *cpu, addr_mode adm);
void opcode_TSX(CPU *cpu, addr_mode adm);
void opcode_TXA(CPU *cpu, addr_mode adm);
void opcode_TXS(CPU *cpu, addr_mode adm);
void opcode_TYA(CPU *cpu, addr_mode adm);

// - OTHER ADDRESSING -

void opcode_ADC(CPU *cpu, addr_mode adm);
void opcode_AND(CPU *cpu, addr_mode adm);
void opcode_CMP(CPU *cpu, addr_mode adm);
void opcode_CPX(CPU *cpu, addr_mode adm);
void opcode_CPY(CPU *cpu, addr_mode adm);
void opcode_DEC(CPU *cpu, addr_mode adm);
void opcode_EOR(CPU *cpu, addr_mode adm);
void opcode_INC(CPU *cpu, addr_mode adm);
void opcode_JMP(CPU *cpu, addr_mode adm);
void opcode_JSR(CPU *cpu, addr_mode adm);
void opcode_LDA(CPU *cpu, addr_mode adm);
void opcode_LDX(CPU *cpu, addr_mode adm);
void opcode_LDY(CPU *cpu, addr_mode adm);
void opcode_ORA(CPU *cpu, addr_mode adm);
void opcode_STA(CPU *cpu, addr_mode adm);
void opcode_STX(CPU *cpu, addr_mode adm);
void opcode_STY(CPU *cpu, addr_mode adm);
void opcode_SBC(CPU *cpu, addr_mode adm);
void opcode_ASL(CPU *cpu, addr_mode adm);
void opcode_BIT(CPU *cpu, addr_mode adm);
void opcode_LSR(CPU *cpu, addr_mode adm);
void opcode_ROL(CPU *cpu, addr_mode adm);
void opcode_ROR(CPU *cpu, addr_mode adm);

// - RELATIVE ADDRESSING -

void opcode_BCC(CPU *cpu, addr_mode adm);
void opcode_BCS(CPU *cpu, addr_mode adm);
void opcode_BEQ(CPU *cpu, addr_mode adm);
void opcode_BNE(CPU *cpu, addr_mode adm);
void opcode_BMI(CPU *cpu, addr_mode adm);
void opcode_BPL(CPU *cpu, addr_mode adm);
void opcode_BVC(CPU *cpu, addr_mode adm);
void opcode_BVS(CPU *cpu, addr_mode adm);

// -------------------

extern Opcode opcodes[256];

#endif
