#include "system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

gb_t *init_gb()
{
    gb_t *gb = malloc(sizeof(gb_t));
    memset(&gb->reg, 0, sizeof(gb->reg));
}

void load_rom(gb_t *gb)
{
    int ret, file_size;

    FILE *f = fopen("./roms/tetris.gb", "rb");
    ret = fseek(f, 0, SEEK_END);
    file_size = ftell(f);
    rewind(f);
    ret = fread(gb->mem, sizeof(char), file_size, f);
    fclose(f);
}

void power_seq(gb_t *gb)
{
    /*...*/
}
