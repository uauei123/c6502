#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cpu.h"

int load_file(char *filename, uint16_t start_address) 
{
    FILE *file = fopen(filename, "rb");

    if (!file) 
	{
        perror("Error opening file");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size > 0x10000) 
	{
        fprintf(stderr, "File size is too big.\n");
        fclose(file);
        return 0;
    }

    fseek(file, 0, SEEK_SET);
    fread(&MEMORY[start_address], 1, file_size, file);

    fclose(file);

    return 1;
}



int main(int argc, char **argv)
{

	if(argc < 2 || argc > 4)
	{
		printf("Usage: ./c6502 <filename> [start_address] [start_pc]\n");
		return -1;
	}

	uint16_t start = 0;

	if(argc == 3)
		start = (uint16_t) strtoul(argv[2], NULL, 0);

	if(load_file(argv[1], start) == 0)
	{
		return -1;
	}

	CPU cpu;

	cpu_init(&cpu);

	if(argc == 4)
		cpu.pc = (uint16_t) strtoul(argv[3], NULL, 0);

	for(;;)
	{
		system("clear");

		for(int row = 0; row < 32; row++)
		{
			printf("0x%04X: ", row * 16);

			for(int col = 0; col < 16; col++)
			{
				printf("%02X ", MEMORY[row * 16 + col]);
			}
		}

		printf("\n\n");

		printf("PC: 0x%04X\tSP: 0x%02X\n", cpu.pc, cpu.sp);
		printf("A: 0x%02X\tX: 0x%02XtY: 0x%02X\n", cpu.A_reg, cpu.X_reg, cpu.Y_reg);
		printf("Status: ");

		for(int i = 0; i < 7; i++)
		{
			if(i == 5)
				printf("1");
			else
				printf("%d", cpu.status[i]);
		}

		printf("\n");

		cycle(&cpu);
	}

	return 0;
}
