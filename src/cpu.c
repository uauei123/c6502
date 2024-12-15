#include <stdio.h>

#include "cpu.h"



uint8_t MEMORY[0xFFFF];

uint64_t cycles;



int cpu_init(CPU *cpu) {
	cycles = 0;
    cpu->pc = read_word(cpu, RESET_ADDRESS);
    cpu->sp = 0xFF;
    cpu->A_reg = 0;
    cpu->X_reg = 0;
    cpu->Y_reg = 0;
    cpu->status[CARRY_FLAG] = 0;
    cpu->status[ZERO_FLAG] = 0;
    cpu->status[INTERRUPT_DISABLE] = 1;
    cpu->status[DECIMAL_MODE] = 0;
    cpu->status[BREAK_FLAG] = 0;
    cpu->status[OVERFLOW_FLAG] = 0;
    cpu->status[NEGATIVE_FLAG] = 0;
}

uint8_t fetch(CPU *cpu)
{
	printf("read: 0x%04X, A: 0x%02X, cycles %d\n", cpu->pc, cpu->A_reg, cycles);
    return read(cpu, cpu->pc++);
}

uint8_t read(CPU *cpu, uint16_t address)
{
    return MEMORY[address];
    cpu->pc++;
}

uint16_t read_word(CPU *cpu, uint16_t address)
{
	printf("addr read word: 0x%04X, read = 0x%02X, 0x%02X\n", address, MEMORY[address], MEMORY[address + 1]);
	cpu->pc += 2;
	return MEMORY[address] | (MEMORY[address + 1] << 8);
}

uint16_t get_operand_address(CPU *cpu, addr_mode addr)
{
	switch(addr)
	{
		case ADDRESSING_IMMEDIATE:
		{
			return (uint16_t) cpu->pc++;
			break;
		}
		case ADDRESSING_ZERO_PAGE:
		{
			return (uint16_t) read(cpu, cpu->pc++) & 0xFF;
			break;
		}
		case ADDRESSING_ABSOLUTE:
		{
			return read_word(cpu, cpu->pc);
			break;
		}
		case ADDRESSING_ZERO_PAGE_X:
		{
			return (uint16_t) ((read(cpu, cpu->pc++) + cpu->X_reg) & 0xFF);
			break;
		}
		case ADDRESSING_ZERO_PAGE_Y:
		{
			return (uint16_t) ((read(cpu, cpu->pc++) + cpu->Y_reg) & 0xFF);
			break;
		}
		case ADDRESSING_ABSOLUTE_X:
		{
			return (read_word(cpu, cpu->pc) + cpu->X_reg);
			break;
		}
		case ADDRESSING_ABSOLUTE_Y:
		{
			return (read_word(cpu, cpu->pc) + cpu->Y_reg);
			break;
		}
		case ADDRESSING_INDIRECT:
		{
			return read_word(cpu, read_word(cpu, cpu->pc));
			break;
		}
		case ADDRESSING_INDIRECT_X:
		{
            cpu->pc--;
			return read_word(cpu, (read(cpu, cpu->pc + 1) + cpu->X_reg) & 0xFF);
			break;
		}
		case ADDRESSING_INDIRECT_Y:
		{
		    cpu->pc--;
			return read_word(cpu, read(cpu, cpu->pc + 1) & 0xFF) + cpu->Y_reg;
			break;
		}
		case ADDRESSING_RELATIVE:
		{
			return cpu->pc + (int8_t) read(cpu, cpu->pc++) + 1;
			break;
		}
	}
}

uint8_t get_hi(uint16_t value)
{
	return value >> 8;
}

int execute(CPU *cpu, uint8_t opcode)
{
	printf("opcode: 0x%02X, pc: 0x%04X\n", opcode, cpu->pc - 1);
	scanf(" %c");
	opcodes[opcode].ptr(cpu, opcodes[opcode].adm);
	cycles += opcodes[opcode].cycles;
	return 0;
}

int cycle(CPU *cpu)
{
	execute(cpu, fetch(cpu));
	return 0;
}



void set_sign(CPU *cpu, uint8_t data) // inline (?)
{
	cpu->status[NEGATIVE_FLAG] = (data & 0x80) ? 1 : 0;
}

void set_zero(CPU *cpu, uint8_t data) // inline (?)
{
	cpu->status[ZERO_FLAG] = (data == 0) ? 1 : 0;
}

void set_conditional_flag(CPU* cpu, int condition, int flag) // inline (?)
{
	cpu->status[flag] = (condition) ? 1 : 0;
}



void push(CPU *cpu, uint8_t value)
{
	if(cpu->sp == 0)
		return; // handle error?

	cpu->sp--;

	MEMORY[STACK_START + cpu->sp + 1] = value;
}

uint8_t pull(CPU *cpu)
{
	return MEMORY[STACK_START + ++cpu->sp];
}



// ---- OPCODES ----

// ---- IMPLIED ----

void opcode_CLC(CPU *cpu, addr_mode adm)
{
	cpu->status[CARRY_FLAG] = 0;
}

void opcode_CLD(CPU *cpu, addr_mode adm)
{
	cpu->status[DECIMAL_MODE] = 0;
}

void opcode_CLI(CPU *cpu, addr_mode adm)
{
	cpu->status[INTERRUPT_DISABLE] = 0;
}

void opcode_CLV(CPU *cpu, addr_mode adm)
{
	cpu->status[OVERFLOW_FLAG] = 0;
}

void opcode_DEX(CPU *cpu, addr_mode adm)
{
	cpu->X_reg--;

	set_sign(cpu, cpu->X_reg);
	set_zero(cpu, cpu->X_reg);
}

void opcode_DEY(CPU *cpu, addr_mode adm)
{
	cpu->Y_reg--;

	set_sign(cpu, cpu->Y_reg);
	set_zero(cpu, cpu->Y_reg);
}

void opcode_INX(CPU *cpu, addr_mode adm)
{
	cpu->X_reg++;

	set_sign(cpu, cpu->X_reg);
	set_zero(cpu, cpu->X_reg);
}

void opcode_INY(CPU *cpu, addr_mode adm)
{
	cpu->Y_reg++;

	set_sign(cpu, cpu->Y_reg);
	set_zero(cpu, cpu->Y_reg);
}

void opcode_NOP(CPU *cpu, addr_mode adm)
{
	return;
}

void opcode_SEC(CPU *cpu, addr_mode adm)
{
	cpu->status[CARRY_FLAG] = 1;
}

void opcode_SED(CPU *cpu, addr_mode adm)
{
	cpu->status[DECIMAL_MODE] = 1;
}

void opcode_SEI(CPU *cpu, addr_mode adm)
{
	cpu->status[INTERRUPT_DISABLE] = 1;
}

void opcode_TAX(CPU *cpu, addr_mode adm)
{
	cpu->X_reg = cpu->A_reg;

	set_sign(cpu, cpu->X_reg);
	set_zero(cpu, cpu->X_reg);
}

void opcode_TAY(CPU *cpu, addr_mode adm)
{
	cpu->Y_reg = cpu->A_reg;

	set_sign(cpu, cpu->Y_reg);
	set_zero(cpu, cpu->Y_reg);
}

void opcode_TSX(CPU *cpu, addr_mode adm)
{
	cpu->X_reg = cpu->sp;

	set_sign(cpu, cpu->X_reg);
	set_zero(cpu, cpu->X_reg);
}

void opcode_TXA(CPU *cpu, addr_mode adm)
{
	cpu->A_reg = cpu->X_reg;

	set_sign(cpu, cpu->A_reg);
	set_zero(cpu, cpu->A_reg);
}

void opcode_TXS(CPU *cpu, addr_mode adm)
{
	cpu->sp = cpu->X_reg;

	set_sign(cpu, cpu->sp);
	set_zero(cpu, cpu->sp);
}

void opcode_TYA(CPU *cpu, addr_mode adm)
{
	cpu->A_reg = cpu->Y_reg;

	set_sign(cpu, cpu->A_reg);
	set_zero(cpu, cpu->A_reg);
}

void opcode_PHA(CPU *cpu, addr_mode adm)
{
	push(cpu, cpu->A_reg);
}

void opcode_PLA(CPU *cpu, addr_mode adm)
{
	cpu->A_reg = pull(cpu);
}

void opcode_PHP(CPU *cpu, addr_mode adm)
{
	uint8_t psr = 0;

	for(int i = 0; i < 7; i++)
	{
		if(i == 5)
			psr |= 1 << i;
		else
			psr |= cpu->status[i] << i;
	}

	push(cpu, psr);
}

void opcode_PLP(CPU *cpu, addr_mode adm)
{
	uint8_t psr = pull(cpu);

	for(int i = 0; i < 7; i++)
	{
		if(i == 5)
			continue;

		if(i > 5)
			cpu->status[i - 1] = (psr >> i) & 1;
		else
			cpu->status[i] = (psr >> i) & 1;
	}
}

void opcode_RTS(CPU *cpu, addr_mode adm)
{
	uint8_t lo = pull(cpu);
	uint8_t hi = pull(cpu);

	cpu->pc = ((hi << 8) | lo) + 1;
}

void opcode_RTI(CPU *cpu, addr_mode adm)
{
	uint8_t psr = pull(cpu);

	for(int i = 0; i < 7; i++)
	{
		if(i == 5)
			continue;

		if(i > 5)
			cpu->status[i - 1] = (psr >> i) & 1;
		else
			cpu->status[i] = (psr >> i) & 1;
	}

	uint8_t lo = pull(cpu);
	uint8_t hi = pull(cpu);

	cpu->pc = (hi << 8) | lo;
}

void opcode_BRK(CPU *cpu, addr_mode adm)
{
	cpu->pc++;

	push(cpu, (cpu->pc >> 8) & 0xFF);
	push(cpu, cpu->pc & 0xFF);

	uint8_t psr = 0;

	for(int i = 0; i < 7; i++)
	{
		if(i == 5)
			psr |= 1 << i;
		else
			psr |= cpu->status[i] << i;
	}

	push(cpu, psr);

	cpu->status[INTERRUPT_DISABLE] = 1;

	cpu->pc = read_word(cpu, IRQ_ADDRESS);
}

// - OTHER ADDRESSING -

void opcode_ADC(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	uint16_t result = cpu->A_reg + operand + cpu->status[CARRY_FLAG];

	set_conditional_flag(cpu, result > 0xFF, CARRY_FLAG);

	set_conditional_flag(cpu, ((cpu->A_reg ^ operand) & 0x80) == 0 && ((cpu->A_reg ^ result) & 0x80) != 0, OVERFLOW_FLAG); // bad

	set_zero(cpu, result & 0xFF);

	set_sign(cpu, result);

	cpu->A_reg = result & 0xFF;
}

void opcode_AND(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	cpu->A_reg = cpu->A_reg & operand;

	set_sign(cpu, cpu->A_reg);

	set_zero(cpu, cpu->A_reg);
}

void opcode_CMP(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	uint8_t result = cpu->A_reg - operand;

	set_sign(cpu, result);

	set_zero(cpu, result);

	set_conditional_flag(cpu, cpu->A_reg >= operand, CARRY_FLAG);
}

void opcode_CPX(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	uint8_t result = cpu->X_reg - operand;

	set_sign(cpu, result);

	set_zero(cpu, result);

	set_conditional_flag(cpu, cpu->X_reg >= operand, CARRY_FLAG);
}

void opcode_CPY(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	uint8_t result = cpu->Y_reg - operand;

	set_sign(cpu, result);

	set_zero(cpu, result);

	set_conditional_flag(cpu, cpu->Y_reg >= operand, CARRY_FLAG);
}

void opcode_DEC(CPU *cpu, addr_mode adm)
{
	uint8_t address = get_operand_address(cpu, adm);

	uint8_t result = read(cpu, address) - 1;

	MEMORY[address] = result;

	set_sign(cpu, result);

	set_zero(cpu, result);
}

void opcode_EOR(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	cpu->A_reg = cpu->A_reg ^ operand;

	set_sign(cpu, cpu->A_reg);

	set_zero(cpu, cpu->A_reg);
}

void opcode_INC(CPU *cpu, addr_mode adm)
{
	uint8_t address = get_operand_address(cpu, adm);

	uint8_t result = read(cpu, address) + 1;

	MEMORY[address] = result;

	set_sign(cpu, result);

	set_zero(cpu, result);
}

void opcode_JMP(CPU *cpu, addr_mode adm)
{
	uint16_t target_address = get_operand_address(cpu, adm);

	cpu->pc = target_address;
}

void opcode_JSR(CPU *cpu, addr_mode adm)
{
	uint16_t target_address = get_operand_address(cpu, adm);

	push(cpu, (cpu->pc >> 8) & 0xFF);

	push(cpu, cpu->pc & 0xFF);

	cpu->pc = target_address;
}

void opcode_LDA(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	cpu->A_reg = operand;

	set_sign(cpu, cpu->A_reg);

	set_zero(cpu, cpu->A_reg);
}

void opcode_LDX(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	cpu->X_reg = operand;

	set_sign(cpu, cpu->X_reg);

	set_zero(cpu, cpu->X_reg);
}

void opcode_LDY(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	cpu->Y_reg = operand;

	set_sign(cpu, cpu->Y_reg);

	set_zero(cpu, cpu->Y_reg);
}

void opcode_ORA(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	cpu->A_reg = cpu->A_reg | operand;

	set_sign(cpu, cpu->A_reg);

	set_zero(cpu, cpu->A_reg);
}

void opcode_STA(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, adm);

	MEMORY[address] = cpu->A_reg;
}

void opcode_STX(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, adm);

	MEMORY[address] = cpu->X_reg;
}

void opcode_STY(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, adm);

	MEMORY[address] = cpu->Y_reg;
}

void opcode_SBC(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	uint16_t result = cpu->A_reg - operand - (1 - cpu->status[CARRY_FLAG]);

	cpu->A_reg = result & 0xFF;

	set_sign(cpu, cpu->A_reg);

	set_zero(cpu, cpu->A_reg);

	set_conditional_flag(cpu, result < 256, CARRY_FLAG);

	set_conditional_flag(cpu, ((cpu->A_reg ^ result) & 0x80) && ((cpu->A_reg ^ operand) & 0x80), OVERFLOW_FLAG);
}

void opcode_ASL(CPU *cpu, addr_mode adm)
{
	uint8_t operand;
	uint16_t address;

	if(adm == ADDRESSING_ACCUMULATOR)
	{
		operand = cpu->A_reg;
	}
	else
	{
		address = get_operand_address(cpu, adm);
		operand = read(cpu, address);
	}

	set_conditional_flag(cpu, operand & 0x80, CARRY_FLAG);

	uint8_t result = operand << 1;

	if(adm == ADDRESSING_ACCUMULATOR)
	{
		cpu->A_reg = result;
	}
	else
	{
		MEMORY[address] = result;
	}

	set_sign(cpu, result);

	set_zero(cpu, result);
}

void opcode_BIT(CPU *cpu, addr_mode adm)
{
	uint8_t operand = read(cpu, get_operand_address(cpu, adm));

	set_zero(cpu, cpu->A_reg & operand);

	set_sign(cpu, operand);

	set_conditional_flag(cpu, operand & 0x40, OVERFLOW_FLAG);
}

void opcode_LSR(CPU *cpu, addr_mode adm)
{
	uint8_t operand;
	uint16_t address;

	if(adm == ADDRESSING_ACCUMULATOR)
	{
		operand = cpu->A_reg;
	}
	else
	{
		address = get_operand_address(cpu, adm);
		operand = read(cpu, address);
	}

	set_conditional_flag(cpu, operand & 0x01, CARRY_FLAG);

	uint8_t result = operand >> 1;

	if(adm == ADDRESSING_ACCUMULATOR)
	{
		cpu->A_reg = result;
	}
	else
	{
		MEMORY[address] = result;
	}

	set_sign(cpu, result);

	set_zero(cpu, 0);
}

void opcode_ROL(CPU *cpu, addr_mode adm)
{
	uint8_t operand;
	uint16_t address;

	if(adm == ADDRESSING_ACCUMULATOR)
	{
		operand = cpu->A_reg;
	}
	else
	{
		address = get_operand_address(cpu, adm);
		operand = read(cpu, address);
	}

	uint8_t carry = cpu->status[CARRY_FLAG];

	set_conditional_flag(cpu, operand & 0x80, CARRY_FLAG);

	uint8_t result = (operand << 1) | carry;


	if(adm == ADDRESSING_ACCUMULATOR)
	{
		cpu->A_reg = result;
	}
	else
	{
		MEMORY[address] = result;
	}

	set_sign(cpu, result);

	set_zero(cpu, result);
}

void opcode_ROR(CPU *cpu, addr_mode adm)
{
	uint8_t operand;
	uint16_t address;

	if(adm == ADDRESSING_ACCUMULATOR)
	{
		operand = cpu->A_reg;
	}
	else
	{
		address = get_operand_address(cpu, adm);
		operand = read(cpu, address);
	}

	uint8_t carry = cpu->status[CARRY_FLAG];

	set_conditional_flag(cpu, operand & 0x01, CARRY_FLAG);

	uint8_t result = (operand >> 1) | (carry << 7);


	if(adm == ADDRESSING_ACCUMULATOR)
	{
		cpu->A_reg = result;
	}
	else
	{
		MEMORY[address] = result;
	}

	set_sign(cpu, result);

	set_zero(cpu, result);
}

// --- RELATIVE ADDRESSING ---

void opcode_BCC(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[CARRY_FLAG] == 0)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

void opcode_BCS(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[CARRY_FLAG] == 1)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

void opcode_BEQ(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[ZERO_FLAG] == 1)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

void opcode_BNE(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[ZERO_FLAG] == 0)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

void opcode_BMI(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[NEGATIVE_FLAG] == 1)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

void opcode_BPL(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[NEGATIVE_FLAG] == 0)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

void opcode_BVC(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[OVERFLOW_FLAG] == 0)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

void opcode_BVS(CPU *cpu, addr_mode adm)
{
	uint16_t address = get_operand_address(cpu, ADDRESSING_RELATIVE);

	if(cpu->status[OVERFLOW_FLAG] == 1)
	{
		cycles++;

		if(get_hi(cpu->pc) != get_hi(address))
			cycles++;

		cpu->pc = address;
	}
}

// --- OTHER ---

void opcode_UNKNOWN(CPU *cpu, addr_mode adm)
{
	printf("Unknown opcode! 0x%02X, PC: 0x%04X\n", MEMORY[cpu->pc], cpu->pc);
}


Opcode opcodes[256] = {
    // 0x00 - 0x0F
    { opcode_BRK, ADDRESSING_IMPLIED, 7 },   { opcode_ORA, ADDRESSING_INDIRECT_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_ORA, ADDRESSING_ZERO_PAGE, 3 },  { opcode_ASL, ADDRESSING_ZERO_PAGE, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_PHP, ADDRESSING_IMPLIED, 3 },   { opcode_ORA, ADDRESSING_IMMEDIATE, 2 },  { opcode_ASL, ADDRESSING_ACCUMULATOR, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_ORA, ADDRESSING_ABSOLUTE, 4 },   { opcode_ASL, ADDRESSING_ABSOLUTE, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },

    // 0x10 - 0x1F
    { opcode_BPL, ADDRESSING_RELATIVE, 2 },  { opcode_ORA, ADDRESSING_INDIRECT_Y, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_ORA, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_ASL, ADDRESSING_ZERO_PAGE_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_CLC, ADDRESSING_IMPLIED, 2 },   { opcode_ORA, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_ORA, ADDRESSING_ABSOLUTE_X, 4 }, { opcode_ASL, ADDRESSING_ABSOLUTE_X, 7 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },

    // 0x20 - 0x2F
    { opcode_JSR, ADDRESSING_ABSOLUTE, 6 },  { opcode_AND, ADDRESSING_INDIRECT_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_BIT, ADDRESSING_ZERO_PAGE, 3 }, { opcode_AND, ADDRESSING_ZERO_PAGE, 3 },  { opcode_ROL, ADDRESSING_ZERO_PAGE, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_PLP, ADDRESSING_IMPLIED, 4 },   { opcode_AND, ADDRESSING_IMMEDIATE, 2 },  { opcode_ROL, ADDRESSING_ACCUMULATOR, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_BIT, ADDRESSING_ABSOLUTE, 4 },  { opcode_AND, ADDRESSING_ABSOLUTE, 4 },   { opcode_ROL, ADDRESSING_ABSOLUTE, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },

    // 0x30 - 0x3F
    { opcode_BMI, ADDRESSING_RELATIVE, 2 },  { opcode_AND, ADDRESSING_INDIRECT_Y, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_AND, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_ROL, ADDRESSING_ZERO_PAGE_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_SEC, ADDRESSING_IMPLIED, 2 },   { opcode_AND, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_AND, ADDRESSING_ABSOLUTE_X, 4 }, { opcode_ROL, ADDRESSING_ABSOLUTE_X, 7 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },

    // 0x40 - 0x4F
    { opcode_RTI, ADDRESSING_IMPLIED, 6 },   { opcode_EOR, ADDRESSING_INDIRECT_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_EOR, ADDRESSING_ZERO_PAGE, 3 },  { opcode_LSR, ADDRESSING_ZERO_PAGE, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_PHA, ADDRESSING_IMPLIED, 3 },   { opcode_EOR, ADDRESSING_IMMEDIATE, 2 },  { opcode_LSR, ADDRESSING_ACCUMULATOR, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_JMP, ADDRESSING_ABSOLUTE, 3 },  { opcode_EOR, ADDRESSING_ABSOLUTE, 4 },   { opcode_LSR, ADDRESSING_ABSOLUTE, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },

    // 0x50 - 0x5F
    { opcode_BVC, ADDRESSING_RELATIVE, 2 },  { opcode_EOR, ADDRESSING_INDIRECT_Y, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_EOR, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_LSR, ADDRESSING_ZERO_PAGE_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_CLI, ADDRESSING_IMPLIED, 2 },   { opcode_EOR, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_EOR, ADDRESSING_ABSOLUTE_X, 4 }, { opcode_LSR, ADDRESSING_ABSOLUTE_X, 7 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },

    // 0x60 - 0x6F
    { opcode_RTS, ADDRESSING_IMPLIED, 6 },   { opcode_ADC, ADDRESSING_INDIRECT_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_ADC, ADDRESSING_ZERO_PAGE, 3 },  { opcode_ROR, ADDRESSING_ZERO_PAGE, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_PLA, ADDRESSING_IMPLIED, 4 },   { opcode_ADC, ADDRESSING_IMMEDIATE, 2 },  { opcode_ROR, ADDRESSING_ACCUMULATOR, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_JMP, ADDRESSING_INDIRECT, 5 },  { opcode_ADC, ADDRESSING_ABSOLUTE, 4 },   { opcode_ROR, ADDRESSING_ABSOLUTE, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },

    // 0x70 - 0x7F
    { opcode_BVS, ADDRESSING_RELATIVE, 2 },  { opcode_ADC, ADDRESSING_INDIRECT_Y, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_ADC, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_ROR, ADDRESSING_ZERO_PAGE_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_SEI, ADDRESSING_IMPLIED, 2 },   { opcode_ADC, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_ADC, ADDRESSING_ABSOLUTE_X, 4 }, { opcode_ROR, ADDRESSING_ABSOLUTE_X, 7 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },

    // 0x80 - 0x8F
    { opcode_UNKNOWN, ADDRESSING_NONE, 2 },  { opcode_STA, ADDRESSING_INDIRECT_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_STY, ADDRESSING_ZERO_PAGE, 3 }, { opcode_STA, ADDRESSING_ZERO_PAGE, 3 },  { opcode_STX, ADDRESSING_ZERO_PAGE, 3 }, { opcode_UNKNOWN, ADDRESSING_NONE, 3 },
    { opcode_DEY, ADDRESSING_IMPLIED, 2 },   { opcode_UNKNOWN, ADDRESSING_NONE, 2 },   { opcode_TXA, ADDRESSING_IMPLIED, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_STY, ADDRESSING_ABSOLUTE, 4 },  { opcode_STA, ADDRESSING_ABSOLUTE, 4 },   { opcode_STX, ADDRESSING_ABSOLUTE, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 4 },

    // 0x90 - 0x9F
    { opcode_BCC, ADDRESSING_RELATIVE, 2 },  { opcode_STA, ADDRESSING_INDIRECT_Y, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_STY, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_STA, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_STX, ADDRESSING_ZERO_PAGE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 4 },
    { opcode_TYA, ADDRESSING_IMPLIED, 2 },   { opcode_STA, ADDRESSING_ABSOLUTE_Y, 5 }, { opcode_TXS, ADDRESSING_IMPLIED, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_STA, ADDRESSING_ABSOLUTE_X, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },

    // 0xA0 - 0xAF
    { opcode_LDY, ADDRESSING_IMMEDIATE, 2 }, { opcode_LDA, ADDRESSING_INDIRECT_X, 6 }, { opcode_LDX, ADDRESSING_IMMEDIATE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_LDY, ADDRESSING_ZERO_PAGE, 3 }, { opcode_LDA, ADDRESSING_ZERO_PAGE, 3 },  { opcode_LDX, ADDRESSING_ZERO_PAGE, 3 }, { opcode_UNKNOWN, ADDRESSING_NONE, 3 },
    { opcode_TAY, ADDRESSING_IMPLIED, 2 },   { opcode_LDA, ADDRESSING_IMMEDIATE, 2 },  { opcode_TAX, ADDRESSING_IMPLIED, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_LDY, ADDRESSING_ABSOLUTE, 4 },  { opcode_LDA, ADDRESSING_ABSOLUTE, 4 },   { opcode_LDX, ADDRESSING_ABSOLUTE, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 4 },

    // 0xB0 - 0xBF
    { opcode_BCS, ADDRESSING_RELATIVE, 2 },  { opcode_LDA, ADDRESSING_INDIRECT_Y, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_LDY, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_LDA, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_LDX, ADDRESSING_ZERO_PAGE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 4 },
    { opcode_CLV, ADDRESSING_IMPLIED, 2 },   { opcode_LDA, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_TSX, ADDRESSING_IMPLIED, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 4 },
    { opcode_LDY, ADDRESSING_ABSOLUTE_X, 4 },{ opcode_LDA, ADDRESSING_ABSOLUTE_X, 4 }, { opcode_LDX, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 4 },

    // 0xC0 - 0xCF
    { opcode_CPY, ADDRESSING_IMMEDIATE, 2 }, { opcode_CMP, ADDRESSING_INDIRECT_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_CPY, ADDRESSING_ZERO_PAGE, 3 }, { opcode_CMP, ADDRESSING_ZERO_PAGE, 3 },  { opcode_DEC, ADDRESSING_ZERO_PAGE, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_INY, ADDRESSING_IMPLIED, 2 },   { opcode_CMP, ADDRESSING_IMMEDIATE, 2 },  { opcode_DEX, ADDRESSING_IMPLIED, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_CPY, ADDRESSING_ABSOLUTE, 4 },  { opcode_CMP, ADDRESSING_ABSOLUTE, 4 },   { opcode_DEC, ADDRESSING_ABSOLUTE, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },

    // 0xD0 - 0xDF
    { opcode_BNE, ADDRESSING_RELATIVE, 2 },  { opcode_CMP, ADDRESSING_INDIRECT_Y, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_CMP, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_DEC, ADDRESSING_ZERO_PAGE_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_CLD, ADDRESSING_IMPLIED, 2 },   { opcode_CMP, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_CMP, ADDRESSING_ABSOLUTE_X, 4 }, { opcode_DEC, ADDRESSING_ABSOLUTE_X, 7 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },

    // 0xE0 - 0xEF
    { opcode_CPX, ADDRESSING_IMMEDIATE, 2 }, { opcode_SBC, ADDRESSING_INDIRECT_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_CPX, ADDRESSING_ZERO_PAGE, 3 }, { opcode_SBC, ADDRESSING_ZERO_PAGE, 3 },  { opcode_INC, ADDRESSING_ZERO_PAGE, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 5 },
    { opcode_INX, ADDRESSING_IMPLIED, 2 },   { opcode_SBC, ADDRESSING_IMMEDIATE, 2 },  { opcode_NOP, ADDRESSING_IMPLIED, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 },
    { opcode_CPX, ADDRESSING_ABSOLUTE, 4 },  { opcode_SBC, ADDRESSING_ABSOLUTE, 4 },   { opcode_INC, ADDRESSING_ABSOLUTE, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },

    // 0xF0 - 0xFF
    { opcode_BEQ, ADDRESSING_RELATIVE, 2 },  { opcode_SBC, ADDRESSING_INDIRECT_Y, 5 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 8 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 3 },  { opcode_SBC, ADDRESSING_ZERO_PAGE_X, 4 },{ opcode_INC, ADDRESSING_ZERO_PAGE_X, 6 }, { opcode_UNKNOWN, ADDRESSING_NONE, 6 },
    { opcode_SED, ADDRESSING_IMPLIED, 2 },   { opcode_SBC, ADDRESSING_ABSOLUTE_Y, 4 }, { opcode_UNKNOWN, ADDRESSING_NONE, 2 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 },
    { opcode_UNKNOWN, ADDRESSING_NONE, 4 },  { opcode_SBC, ADDRESSING_ABSOLUTE_X, 4 }, { opcode_INC, ADDRESSING_ABSOLUTE_X, 7 }, { opcode_UNKNOWN, ADDRESSING_NONE, 7 }
};