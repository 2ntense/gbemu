#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

#include "system.h"

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10
#define FLAG_Z_INDEX 7
#define FLAG_N_INDEX 6
#define FLAG_H_INDEX 5
#define FLAG_C_INDEX 4

extern void emulate_cycle(gb_t *);

#endif