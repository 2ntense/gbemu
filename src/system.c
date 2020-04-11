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
    file_size = ftell(f) + 1;
    rewind(f);
    ret = fread(gb->mem, sizeof(char), file_size, f);
    fclose(f);
}

void power_seq(gb_t *gb)
{
    /*...*/
}

void post_power_seq(gb_t *gb)
{
    gb->reg.pc = 0x100;
    write_reg_af(gb, 0x01B0);
    write_reg_bc(gb, 0x0013);
    write_reg_de(gb, 0x00D8);
    write_reg_hl(gb, 0x014D);
    gb->reg.sp = 0xFFFE;
    gb->mem[0xFF05] = 0x00;
    gb->mem[0xFF06] = 0x00;
    gb->mem[0xFF07] = 0x00;
    gb->mem[0xFF10] = 0x80;
    gb->mem[0xFF11] = 0xBF;
    gb->mem[0xFF12] = 0xF3;
    gb->mem[0xFF14] = 0xBF;
    gb->mem[0xFF16] = 0x3F;
    gb->mem[0xFF17] = 0x00;
    gb->mem[0xFF19] = 0xBF;
    gb->mem[0xFF1A] = 0x7F;
    gb->mem[0xFF1B] = 0xFF;
    gb->mem[0xFF1C] = 0x9F;
    gb->mem[0xFF1E] = 0xBF;
    gb->mem[0xFF20] = 0xFF;
    gb->mem[0xFF21] = 0x00;
    gb->mem[0xFF22] = 0x00;
    gb->mem[0xFF23] = 0xBF;
    gb->mem[0xFF24] = 0x77;
    gb->mem[0xFF25] = 0xF3;
    gb->mem[0xFF26] = 0xF1;
    gb->mem[0xFF40] = 0x91;
    gb->mem[0xFF42] = 0x00;
    gb->mem[0xFF43] = 0x00;
    gb->mem[0xFF45] = 0x00;
    gb->mem[0xFF47] = 0xFC;
    gb->mem[0xFF48] = 0xFF;
    gb->mem[0xFF49] = 0xFF;
    gb->mem[0xFF4A] = 0x00;
    gb->mem[0xFF4B] = 0x00;
    gb->mem[0xFFFF] = 0x00;
}

uint16_t read_reg_af(gb_t *gb)
{
    return (gb->reg.a << 8) | gb->reg.f;
}

uint16_t read_reg_bc(gb_t *gb)
{
    return (gb->reg.b << 8) | gb->reg.c;
}

uint16_t read_reg_de(gb_t *gb)
{
    return (gb->reg.d << 8) | gb->reg.e;
}

uint16_t read_reg_hl(gb_t *gb)
{
    return (gb->reg.h << 8) | gb->reg.l;
}

void write_reg_af(gb_t *gb, uint16_t val)
{
    gb->reg.a = val >> 8;
    gb->reg.f = val & 0xFF;
}
void write_reg_bc(gb_t *gb, uint16_t val)
{
    gb->reg.b = val >> 8;
    gb->reg.c = val & 0xFF;
}
void write_reg_de(gb_t *gb, uint16_t val)
{
    gb->reg.d = val >> 8;
    gb->reg.e = val & 0xFF;
}
void write_reg_hl(gb_t *gb, uint16_t val)
{
    gb->reg.h = val >> 8;
    gb->reg.l = val & 0xFF;
}
