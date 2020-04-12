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

/**
 * Pairs AF, BC, DE, HL
 * First reg (A in AF) is hi.
 */
union reg_pair_t {
	struct
	{
		uint8_t lo;
		uint8_t hi;
	} reg;
	uint16_t pair;
};

typedef struct reg_t
{
	union reg_pair_t af;
	union reg_pair_t bc;
	union reg_pair_t de;
	union reg_pair_t hl;
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

#endif