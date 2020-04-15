#include "system.h"
#include "instructions.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    gb_t *gb = init_gb();
    load_rom(gb);
    post_power_seq(gb);

#ifdef DEBUG
    printf("Start loop\n");
#endif
    while (1)
    {
        emulate_cycle(gb);
#ifdef DEBUG
#define DELAY_MS 1000
        usleep(200 * DELAY_MS);
#endif
    }
}
