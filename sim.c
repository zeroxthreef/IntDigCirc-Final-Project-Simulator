/* simulator for the small cpu design project */

#include <stdio.h>

/* the cpu structure used to store it's state and pass this to everything. Wanted to avoid using
globals, and this makes things flexible */
typedef struct
{
	/* cpu native word size is 4 bit so even C's char type has extra bits */
	/* also, while "char" is normally for characters, im using it as a way to store an 8 bit number */
	unsigned char bus_status, reg_opcode, reg_operand_a, reg_operand_b, reg_result, counter_status;
	/* NOTE: reg_result is 4 bit plus carry as the 5th bit */
} cpu_t;

/* this is the main test program. This simulates changing the switches for setting the bus
to certain values. Changing the data in here will change how it executes */
unsigned char test_program[] = {
	/* OR 0x1 (0001) with 0x8 (1000) (should equal 1001 / 0x9) */
	0x4,
	0x1,
	0x8,
	/* XOR 0x3 (0011) with 0x5 (0101) (should equal 0100 / 0x4) */
	0x7,
	0x3,
	0x5,
	/* ADD 0x7 (0111) with 0x9 (1001) (should equal 0000 (carry 1) / 0x0) */
	0xa,
	0x7,
	0x9,
	/* AND 0x1 (0001) with 0xb (1011) (should equal 0001 / 0x1) */
	0x4,
	0x1,
	0xb
};

void cpu_print_status(cpu_t *cpu);
void cpu_execute(cpu_t *cpu);
int cpu_read_attribute(char attribute, int bit);
int cpu_read_bus(cpu_t *cpu);

int main(int argc, char **argv)
{
	/* initialize the cpu attributes to 0. Essentially the same as pressing the reset button */
	cpu_t cpu = {0, 0, 0, 0, 0, 0};
	int nothing; /* needed for stepping simulation */

	cpu.bus_status = test_program[0]; /* load initial bus value */



	while(1) /* loop forever/until user closes the console window */
	{
		printf("press enter to step clock ======================");
		scanf("%c", &nothing);

		cpu_execute(&cpu);

		cpu_print_status(&cpu);


		/* update the clock */

		cpu.counter_status++;
		cpu.counter_status %= 8;
	}


	return 0;
}

void cpu_print_status(cpu_t *cpu)
{
	printf("Design project CPU status:\n\ncounter: %d%d%d%d, in decimal: %d\n",
	cpu_read_attribute(cpu->counter_status, 3), cpu_read_attribute(cpu->counter_status, 2),
	cpu_read_attribute(cpu->counter_status, 1), cpu_read_attribute(cpu->counter_status, 0), cpu->counter_status);

	printf("bus state %d%d%d%d\n",
	cpu_read_attribute(cpu->bus_status, 3), cpu_read_attribute(cpu->bus_status, 2),
	cpu_read_attribute(cpu->bus_status, 1), cpu_read_attribute(cpu->bus_status, 0));

	printf("opcode register %d%d%d%d\n",
	cpu_read_attribute(cpu->reg_opcode, 3), cpu_read_attribute(cpu->reg_opcode, 2),
	cpu_read_attribute(cpu->reg_opcode, 1), cpu_read_attribute(cpu->reg_opcode, 0));

	printf("operand a register %d%d%d%d\n",
	cpu_read_attribute(cpu->reg_operand_a, 3), cpu_read_attribute(cpu->reg_operand_a, 2),
	cpu_read_attribute(cpu->reg_operand_a, 1), cpu_read_attribute(cpu->reg_operand_a, 0));

	printf("operand b register %d%d%d%d\n",
	cpu_read_attribute(cpu->reg_operand_b, 3), cpu_read_attribute(cpu->reg_operand_b, 2),
	cpu_read_attribute(cpu->reg_operand_b, 1), cpu_read_attribute(cpu->reg_operand_b, 0));

	printf("result register (carry: %d)%d%d%d%d\n", cpu_read_attribute(cpu->reg_result, 4),
	cpu_read_attribute(cpu->reg_result, 3), cpu_read_attribute(cpu->reg_result, 2),
	cpu_read_attribute(cpu->reg_result, 1), cpu_read_attribute(cpu->reg_result, 0));
}

void cpu_execute(cpu_t *cpu)
{
	//if(cpu_read_bus(cpu))
			
	

	/* based on the state, load bus data into registers */
	switch(cpu->counter_status)
	{
		case 0:
			printf("result register is correct\n");
		break;
		case 1:
			cpu->reg_opcode = cpu->bus_status;
			cpu_read_bus(cpu); /* load next instruction onto bus. Same for all of these _except_ when using the
								result register*/
		break;
		case 3:
			cpu->reg_operand_a = cpu->bus_status;
			cpu_read_bus(cpu);
		break;
		case 4:
			cpu->reg_operand_b = cpu->bus_status;
			cpu_read_bus(cpu);
		break;
		case 7:
			/* control what happens based on the data in the opcode register */

			/* this will appear to act the exact same way as the real cpu hardware, but its actually only running one
			operation _then_ multiplexing unlike the real hardware where it does all operations then multiplexes the
			correct one. */
			switch(cpu->reg_opcode)
			{
				case 4: /* bitwise OR operand A with operand B */
					cpu->reg_result = cpu->reg_operand_a | cpu->reg_operand_b;
				break;
				case 6: /* bitwise AND operand A with operand B */
					cpu->reg_result = cpu->reg_operand_a & cpu->reg_operand_b;
				break;
				case 7: /* bitwise XOR operand A with operand B */
					cpu->reg_result = cpu->reg_operand_a ^ cpu->reg_operand_b;
				break;
				case 10: /* Add operand A to operand B */
					/* adding here will take care of the carry bit for us because with carry, its technically a 5 bit number*/
					cpu->reg_result = cpu->reg_operand_a + cpu->reg_operand_b;
				break;
			}
			//cpu_read_bus(cpu);
		break;
	}
}

int cpu_read_attribute(char attribute, int bit)
{
	/* shift then use a bitmask */
	attribute >>= bit;
	attribute &= 1;
	return attribute;
}

int cpu_read_bus(cpu_t *cpu) /* simulates entering new data in switches but only after reading */
{
	static int status = 0;

	/* reset or inc depending on whether or not end of program */
	if(status < (sizeof(test_program) / sizeof(test_program[0])) - 1)
		status++;
	else /* restart program and load first instruction onto bus */
	{
		status = 0;
		printf("program restarting\n");
		cpu->bus_status = test_program[0];
		return 1;
	}
	
	/* normally, load next instruction onto bus */
	cpu->bus_status = test_program[status];
	return 0;
}