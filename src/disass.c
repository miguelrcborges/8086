#include <stdio.h>
#include <stdlib.h>

#include "8086.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef short i16;

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
		switch (buf[i] >> 1) {
			case IMD_MEMREG: {
				u8 is_wide = buf[i] & 1;
				i++;
				u8 mod = buf[i] >> 6;
				u16 v;
				switch (mod) {
					case 0b11: {
						char *width;
						if (is_wide) {
							v = buf[i+1] + ((int)buf[i+2] << 8);
							width = "word";
							i += 2;
						} else {
							v = buf[i+1];
							width = "byte";
							if (v & (1 << 7)) {
								v |= ((1 << 8) - 1) << 8;
							}
							width = "byte";
							i += 1;
						}
						printf("mov %s, %s %hu\n", regNames[is_wide][buf[i]&0b111], width, v);
						break;
					}
					case 0b00: {
						if ((buf[i] & 0b111) == 0b110) {
							u16 address = buf[i+1] + ((int)buf[i+2] << 8);
							char *width;
							i += 2;
							if (is_wide) {
								v = buf[i+1] + ((int)buf[i+2] << 8);
								width = "word";
								i += 2;
							} else {
								v = buf[i+1];
								if (v & (1 << 7)) {
									v |= ((1 << 8) - 1) << 8;
								}
								width = "byte";
								i += 1;
							}
							printf("mov [%hu], %s %hu\n", address, width, v);
							break;
						}
						if (is_wide) {
							v = buf[i+1] + ((int)buf[i+2] << 8);
							i += 2;
						} else {
							v = buf[i+1];
							if (v & (1 << 7)) {
								v |= ((1 << 8) - 1) << 8;
							}
							i += 1;
						}
						printf("mov [%s], %hu\n", displacementBases[buf[i]&0b111], v);
						break;
					}
					default: {
						i16 dis;
						char *width;
						char *disbase = displacementBases[buf[i]&0b111];
						if (mod == 0b10) {
							dis = buf[i+1] + ((int)buf[i+2] << 8);
							width = "word";
							i += 2;
						} else {
							dis = buf[i+1];
							width = "byte";
							i += 1;
							if (dis & (1 << 7)) {
								dis |= ((1 << 8) - 1) << 8;
							}
						}
						if (is_wide) {
							v = buf[i+1] + ((int)buf[i+2] << 8);
							i += 2;
						} else {
							v = buf[i+1];
							if (v & (1 << 7)) {
								v |= ((1 << 8) - 1) << 8;
							}
							i += 1;
						}
						if (dis != 0) {
							printf("mov [%s + %hd], %s %hu\n", disbase, dis, width, v);
						} else {
							printf("mov [%s], %s %hu\n", disbase, width, dis);
						}
					}
				}
				continue;
			}

			case MEM_ACM: {
				i16 v = buf[i+1] + ((int)buf[i+2] << 8);
				if (buf[i] & 1) {
					i += 2;
				} else {
					u8 v = buf[i+1];
					if (v & (1 << 7)) {
						v |= ((1 << 8) - 1) << 8;
					}
					i++;
				}
				printf("mov ax, [%hd]\n", v);
				continue;
			}

			case ACM_MEM: {
				if (buf[i] & 1) {
					i16 v = buf[i+1] + ((int)buf[i+2] << 8);
					printf("mov [%hu], ax\n", v);
					i += 2;
				} else {
					u8 v = buf[i+1];
					printf("mov [%hhu], ax\n", v);
					i++;
				}
				continue;
			}
		}


		switch (buf[i] >> 2) {
			case MOV_REG_MEMREG: {
				u8 dw = buf[i] & 0b11;
				i++;
				u8 mod = buf[i] >> 6;
				u8 reg = (buf[i] >> 3) & 0b111;

				switch (mod) {
					case 0b11: {
						u8 orig, dest;
						if (dw & 0b10) {
							orig = (buf[i] & 0b111);
							dest = reg;
						} else {
							orig = reg;
							dest = (buf[i] & 0b111);
						}
						printf("mov %s, %s\n", regNames[dw&1][dest], regNames[dw&1][orig]);
						break;
					}
					case 0b00: {
						if ((buf[i] & 0b111) == 0b110) {
							u16 address = buf[i+1] + ((int)buf[i+2] << 8);
							i += 2;
							if (dw & 0b10) {
								printf("mov %s, [%hu]\n", regNames[dw&1][reg], address);
							} else {
								printf("mov [%hu], %s\n", address, regNames[dw&1][reg]);
							}
							break;
						}
						if (dw & 0b10) {
							printf("mov %s, [%s]\n", regNames[dw&1][reg], displacementBases[buf[i]&0b111]);
						} else {
							printf("mov [%s], %s\n", displacementBases[buf[i]&0b111], regNames[dw&1][reg]);
						}
						break;
					}
					default: {
						i16 dis;
						char *disbase = displacementBases[buf[i]&0b111];
						if (mod == 0b10) {
							dis = buf[i+1] + ((int)buf[i+2] << 8);
							i += 2;
						} else {
							dis = buf[i+1];
							i += 1;
							if (dis & (1 << 7)) {
								dis |= ((1 << 8) - 1) << 8;
							}
						}
						if (dw & 0b10) {
							if (dis != 0) {
								printf("mov %s, [%s + %hd]\n", regNames[dw&1][reg], disbase, dis);
							} else {
								printf("mov %s, [%s]\n", regNames[dw&1][reg], disbase);
							}
						} else {
							if (dis != 0) {
								printf("mov [%s + %hd], %s\n", disbase, dis, regNames[dw&1][reg]);
							} else {
								printf("mov [%s], %s\n", disbase, regNames[dw&1][reg]);
							}
						}
					}
				}
				continue;
			}
		} 


		switch (buf[i] >> 4) {
			case IMD_REG: {
				i16 v;
				u8 reg = buf[i] & 0b111;
				u8 is_wide = (buf[i]&0b1000) >> 3;
				if (buf[i] & 0b1000) {
					v = buf[i+1] + ((int)buf[i+2] << 8);
					i += 2;
				} else {
					v = buf[i+1]; 
					i += 1;
					if (v & (1 << 7)) {
						v |= ((1 << 8) - 1) << 8;
					}
				}
				printf("mov %s, %hd\n", regNames[is_wide][reg], v);
				continue;
			}
		}

		fprintf(stderr, "Unrecognized OP at byte %zu: %hhx.\n", i, buf[i]);
		return 1;
	}
}
