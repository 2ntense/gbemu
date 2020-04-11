#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

#define MAIN_RAM_SIZE 1024 * 8
#define VIDEO_RAM_SIZE 1024 * 8
#define MEMORY_SIZE 0xFFFF
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_BUFFER_WIDTH 256
#define SCREEN_BUFFER_HEIGHT 256

typedef struct reg_t
{
	uint8_t a;
	uint8_t f;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
} reg_t;

typedef struct gb_t
{
	reg_t reg;
	uint8_t mem[MEMORY_SIZE];
} gb_t;

extern gb_t *init_gb();
extern void load_rom(gb_t *);

void write_reg_af(gb_t *, uint16_t);
void write_reg_de(gb_t *, uint16_t);
void write_reg_bc(gb_t *, uint16_t);
void write_reg_hl(gb_t *, uint16_t);

#endif