#include "system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

gb_t *init_gb()
{
#ifdef DEBUG
    printf("Initializing GB...\n");
#endif

    gb_t *gb = malloc(sizeof(gb_t));
    memset(gb, 0, sizeof(*gb));
    return gb;
}

void load_rom(gb_t *gb)
{
#ifdef DEBUG
    printf("Loading ROM...\n");
#endif

    int ret, file_size;
    FILE *f = fopen("./roms/tetris.gb", "rb");
    fseek(f, 0, SEEK_END);
    file_size = ftell(f);
    rewind(f);
    ret = fread(gb->mem, 1, file_size, f);
    fclose(f);
    if (ret != file_size)
    {
        exit(1);
    }
}