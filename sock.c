#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct
{
	uint8_t data[0x800];
	uint8_t regs[4];
	uint16_t last;
	uint16_t pc;
	uint8_t lock;
}vm;

void cycle(vm* machine)
{
	uint8_t inst = machine->data[machine->pc++];
	uint8_t reg = (inst >> 4) & 3;
	uint8_t type = inst >> 6;
	switch(inst & 0xF){
		case 0:{
			switch(type){
				case 0:
					machine->regs[reg] = machine->data[machine->pc++];
					break;
				case 1:
					machine->regs[reg] = machine->regs[machine->data[machine->pc++]];
					break;
				case 0b10:
					machine->data[(machine->pc++ << 8)|machine->pc++] = machine->regs[reg];
					break;
			}
		}break;
		case 1:{
			machine->regs[reg] += type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 2:{
			machine->regs[reg] -= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 3:{
			machine->regs[reg] &= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 4:{
			machine->regs[reg] |= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 5:{
			machine->regs[reg] ^= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 6:{
			if(type == 0b10)
				machine->pc = (machine->data[machine->pc++] << 8)| machine->data[machine->pc++];
		}break;
		case 7:{
			if(type == 0b11){
				uint16_t f =  (machine->data[machine->pc++] << 8)| machine->data[machine->pc++];
				machine->pc = machine->regs[reg] == machine->data[machine->pc++] ? f : machine->pc;
			}
		}break;
		case 8:{
			if(type == 0b11){
				uint16_t f =  (machine->data[machine->pc++] << 8)| machine->data[machine->pc++];
				machine->pc = machine->regs[reg] != machine->data[machine->pc++] ? f : machine->pc;
			}
		}break;
		case 9:{
			if(type == 0b10){
				machine->last = machine->pc;
				machine->pc = (machine->data[machine->pc++] << 8)| machine->data[machine->pc++];
			}
		}break;
		case 10:{
			if(reg == 0b11){
				machine->lock = 1;
				break;
			}
			machine->pc = machine->last;
			machine->last = 0;
		}break;
		case 11:{
			machine->regs[reg] *= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 12:{
			machine->regs[reg] /= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 13:{
			machine->regs[reg] %= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 14:{
			machine->regs[reg] <<= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		case 15:{
			machine->regs[reg] >>= type ? machine->regs[machine->data[machine->pc++]] 
										: machine->data[machine->pc++];
		}break;
		default:
			printf("Unknown Instruction %02X\n", inst);
			break;
	}
	printf("%02X %02X %02X %02X\n", machine->regs[0], machine->regs[1], machine->regs[2], machine->regs[3]);
	printf("Program Counter = %04X\n", machine->pc);
}

int main(int argc, char** argv)
{
	if(argc < 2){
		fprintf(stderr, "[%s] usage: specify path to file containing bytecode.\n", argv[0]);
		return -1;
	}
	
	FILE *fp = fopen(argv[1], "rb");
	if(!fp){
		fprintf(stderr, "[%s] error: nonexistant file specified.\n", argv[0]);
		return -2;
	}
	
	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	if(sz == -1){
		fprintf(stderr, "[%s] error: unusable file specified.\n", argv[0]);
		return -3;
	}
	rewind(fp);
	
	vm* machine = malloc(sizeof(vm));
	memset(machine->data, 0, sizeof(machine->data));
	fread(machine->data, sizeof(char), sz, fp);
	fclose(fp);
	
	memset(machine->regs, 0, sizeof(machine->regs));
	machine->pc = 0;
	machine->last = 0;
	machine->lock = 0;
	
	while(!machine->lock)
		cycle(machine);
	
	free(machine);
	return 0;
}