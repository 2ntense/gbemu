#include "system.h"
#include "instructions.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    gb_t *gb = init_gb();
    load_rom(gb);

#ifdef DEBUG
    printf("Start loop\n");
#endif
    while (1)
    {
        emulate_cycle(gb);
    }
}
