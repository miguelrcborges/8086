#include <stdio.h>
#include <stdlib.h>

#include "8086.h"

typedef unsigned char u8;

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Please specify a file to disassemble.");
		return 1;
	}

	FILE *f = fopen(argv[1], "rb");
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);
	u8 *buf = malloc(len);
	fread(buf, 1, len, f);
	fclose(f);

	printf("bits 16\n");
	for (size_t i = 0; i < len; i++) {
		switch (buf[i] >> 2) {
			case MOV_REG_MEMREG: {
				u8 dw = buf[i] & 0b11;
				i++;
				u8 mod = buf[i] >> 6;
				if (mod != 0b11) {
					fprintf(stderr, "Not yet implemented MOV_REG_MEMREG with MOD different from 0b11\n");
					return 1;
				}

				u8 orig, dest;
				if (dw & 0b10) {
					orig = (buf[i] & 0b111);
					dest = ((buf[i] >> 3) & 0b111);
				} else {
					orig = ((buf[i] >> 3) & 0b111);
					dest = (buf[i] & 0b111);
				}

				printf("mov %s, %s\n", regNames[dw&1][dest], regNames[dw&1][orig]);
				break;
			}
			default: {
				fprintf(stderr, "Unrecognized OP at byte %zu: %hhx.\n", i, buf[i]);
				return 1;
			}
		} 
	}
}
