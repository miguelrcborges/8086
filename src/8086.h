#ifndef _8086_H
#define _8086_H

enum _OPCODE {
	MOV_REG_MEMREG = 0b100010,	
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

typedef unsigned char OPCODE;

#endif /* _8086_H */
