#ifndef _8086_H
#define _8086_H

typedef unsigned char OPCODE;
enum _OPCODE_7bit_header {
	IMD_MEMREG = 0b110011,
	MEM_ACM = 0b1010000,
	ACM_MEM = 0b1010001,
};

enum _OPCODE_6bit_header {
	MOV_REG_MEMREG = 0b100010,	
};

enum _OPCODE_4bit_header {
	IMD_REG = 0b1011,	
};

char *regNames[2][8] = {
	{
		"al",
		"cl",
		"dl",
		"bl",
		"ah",
		"ch",
		"dh",
		"bh",
	},
	{
		"ax",
		"cx",
		"dx",
		"bx",
		"sp",
		"bp",
		"si",
		"di"
	}
};

char *displacementBases[8] = {
	"bx + si",
	"bx + di",
	"bp + si",
	"bp + di",
	"si",
	"di",
	"bp",
	"bx"
};


#endif /* _8086_H */
