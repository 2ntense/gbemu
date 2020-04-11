#include "system.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    printf("Start\n");
    gb_t *gb = init_gb();
    load_rom(gb);

    // printf("%02x\n", gb->mem[0x7fff]);

}