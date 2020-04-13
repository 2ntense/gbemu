#include "instructions.h"
#include <stdio.h>
#include <stdbool.h>

uint8_t *get_r(uint8_t y, gb_t *gb)
{
    switch (y)
    {
    case 0:
        return &gb->reg.bc.reg.hi;
    case 1:
        return &gb->reg.bc.reg.lo;
    case 2:
        return &gb->reg.de.reg.hi;
    case 3:
        return &gb->reg.de.reg.lo;
    case 4:
        return &gb->reg.hl.reg.hi;
    case 5:
        return &gb->reg.hl.reg.lo;
    case 6:
        return &gb->mem[gb->reg.hl.pair];
    case 7:
        return &gb->reg.af.reg.hi;
    default:
        printf("No register at y==%d\n", y);
        return NULL;
    }
}

uint16_t *get_rp(uint8_t p, gb_t *gb)
{
    switch (p)
    {
    case 0:
        return &gb->reg.bc.pair;
    case 1:
        return &gb->reg.de.pair;
    case 2:
        return &gb->reg.hl.pair;
    case 3:
        return &gb->reg.af.pair;
    default:
        printf("No register pair at y==%d\n", p);
        return NULL;
    }
}

uint16_t fetch_nn(gb_t *gb)
{
    return (gb->mem[gb->reg.pc + 1] << 8) | gb->mem[gb->reg.pc + 2];
}

uint8_t fetch_n(gb_t *gb)
{
    return gb->mem[gb->reg.pc + 1];
}

void set_flag(uint8_t flag, gb_t *gb)
{
    gb->reg.af.reg.lo |= flag;
}

void reset_flag(uint8_t flag, gb_t *gb)
{
    gb->reg.af.reg.lo &= ~flag;
}

uint8_t get_flag(uint8_t flag, gb_t *gb)
{
    uint8_t *f = &gb->reg.af.reg.lo;
    switch (flag)
    {
    case FLAG_Z:
        return (*f & FLAG_Z) >> FLAG_Z_INDEX;
    case FLAG_N:
        return (*f & FLAG_N) >> FLAG_N_INDEX;
    case FLAG_H:
        return (*f & FLAG_H) >> FLAG_H_INDEX;
    case FLAG_C:
        return (*f & FLAG_C) >> FLAG_C_INDEX;
    default:
        printf("get_flag, flag not found: 0x%02x", flag);
        break;
    }
}

void ld_nn_sp(uint16_t nn, gb_t *gb)
{
    gb->mem[nn] = gb->reg.sp;
}

void ld_rp_nn(uint16_t *rp, uint16_t nn)
{
    *rp = nn;
}

void nop(gb_t *gb)
{
    gb->reg.pc += 1;
}

void add_hl_rp(uint16_t *rp, gb_t *gb)
{
    // gb->reg.hl.pair = *rp;
}

void ld_bc_a(gb_t *gb)
{
    gb->mem[gb->reg.bc.pair] = gb->reg.af.reg.hi;
}

void ld_de_a(gb_t *gb)
{
    gb->mem[gb->reg.de.pair] = gb->reg.af.reg.hi;
}

void ldi_hl_a(gb_t *gb)
{
    gb->mem[gb->reg.hl.pair++] = gb->reg.af.reg.hi;
}

void ldd_hl_a(gb_t *gb)
{
    gb->mem[gb->reg.hl.pair--] = gb->reg.af.reg.hi;
}

void ld_a_bc(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.bc.pair];
}

void ld_a_de(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.de.pair];
}

void ldi_a_hl(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.hl.pair++];
}

void ldd_a_hl(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.hl.pair--];
}

void inc_rp(uint16_t *rp)
{
    *rp++;
}

void dec_rp(uint16_t *rp)
{
    *rp--;
}

void inc_r(uint8_t *r)
{
    *r++;
}

void dec_r(uint8_t *r)
{
    *r--;
}

void ld_r_n(uint8_t *r, uint8_t n)
{
    *r = n;
}

void rlca(gb_t *gb)
{
    uint8_t *a = &gb->reg.af.reg.hi;
    if (*a >> 7 == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_Z, gb);
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    *a = (*a << 1) | (*a >> (sizeof(*a) * 8 - 1));
}

void rrca(gb_t *gb)
{
    uint8_t *a = &gb->reg.af.reg.hi;
    if (*a & 1 == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_Z, gb);
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    *a = (*a >> 1) | (*a << (sizeof(*a) * 8 - 1));
}

void rla(gb_t *gb)
{
    uint8_t *a = &gb->reg.af.reg.hi;
    if (*a >> 7 == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_Z, gb);
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    *a = (*a << 1) | (*a >> (sizeof(*a) * 8 - 1));
    if (get_flag(FLAG_C, gb) == 1)
    {
        *a |= 1;
    }
    else
    {
        *a &= 0;
    }
}

void rra(gb_t *gb)
{
    uint8_t *a = &gb->reg.af.reg.hi;
    if (*a & 1 == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_Z, gb);
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    *a = (*a >> 1) | (*a << (sizeof(*a) * 8 - 1));
    if (get_flag(FLAG_C, gb) == 1)
    {
        *a |= (1 << 7);
    }
    else
    {
        *a &= ~(1 << 7);
    }
}

void daa(gb_t *gb)
{
    // TODO daa
}

void cpl(gb_t *gb)
{
    gb->reg.af.reg.hi = ~gb->reg.af.reg.hi;
}

void scf(gb_t *gb)
{
    set_flag(FLAG_C, gb);
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void ccf(gb_t *gb)
{
    if (get_flag(FLAG_C, gb) == 1)
    {
        reset_flag(FLAG_C, gb);
    }
    else
    {
        set_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void halt(gb_t *gb)
{
    // TODO HALT
}

//  TODO ld_r_r testing
void ld_r_r(uint8_t y, uint8_t z, gb_t *gb)
{
    uint8_t *dest = get_r(y, gb);
    *dest = *get_r(z, gb);
}

// TODO add_a_r check opcode 0xC6
void add_a_r(uint8_t z, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint8_t *src = get_r(z, gb);
    uint16_t res = *reg_a + *src;
    *reg_a = (uint8_t)(res & 0xFF);
    // H flag
    if (((*reg_a & 0xF) + (*src & 0xF)) > 0xF)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if (res & 0xFF00 > 0)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    // Z flag
    if (*reg_a == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    reset_flag(FLAG_N, gb);
}

// TODO add_a_r check opcode 0xC6
void adc_a_r(uint8_t z, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint8_t *src = get_r(z, gb);
    uint16_t res = *reg_a + *src + get_flag(FLAG_C, gb);
    *reg_a = (uint8_t)(res & 0xFF);
    // H flag
    if (((*reg_a & 0xF) + (*src & 0xF)) > 0xF)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if (res & 0xFF00 > 0)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    // Z flag
    if (*reg_a == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    reset_flag(FLAG_N, gb);
}

void sub_a_r(uint8_t z, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint8_t *src = get_r(z, gb);
    // TODO check expression
    if (((int16_t)(*reg_a & 0xF) - ((int16_t)(*src & 0xF))) < 0)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        set_flag(FLAG_H, gb);
    }
    // C flag
    if (*src > *reg_a)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    *reg_a -= *src;
    // Z flag
    if (*reg_a == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    set_flag(FLAG_N, gb);
}

void sbc_a_r(uint8_t z, gb_t *gb)
{
    
}

uint8_t fetch_opcode(gb_t *gb)
{
    return gb->mem[gb->reg.pc];
}

void parse_opcode(uint8_t opcode, gb_t *gb)
{
    uint8_t x = opcode >> 6;
    uint8_t y = (opcode >> 3) & 0b111;
    uint8_t z = opcode & 0b111;
    uint8_t p = (opcode >> 4) & 0b11;
    uint8_t q = (opcode >> 3) & 0b1;

    switch (x)
    {
    case 0:
        switch (z)
        {
        case 0:
            switch (y)
            {
            case 0:
                nop(gb);
                break;
            case 1:
                ld_nn_sp(fetch_nn(gb), gb);
                break;
            case 2:
                // stop()
            case 3:
            default:
                break;
            }

        case 1:
            switch (q)
            {
            case 0:
                ld_rp_nn(get_rp(p, gb), fetch_nn(gb));
                break;
            case 1:
                add_hl_rp(get_rp(p, gb), gb);
                break;
            default:
                break;
            }
        case 2:
            switch (q)
            {
            case 0:
                switch (p)
                {
                case 0:
                    ld_bc_a(gb);
                    break;
                case 1:
                    ld_de_a(gb);
                    break;
                case 2:
                    ldi_hl_a(gb);
                    break;
                case 3:
                    ldd_hl_a(gb);
                    break;
                default:
                    break;
                }
                break;
            case 1:
                switch (p)
                {
                case 0:
                    ld_a_bc(gb);
                    break;
                case 1:
                    ld_a_de(gb);
                    break;
                case 2:
                    ldi_a_hl(gb);
                    break;
                case 3:
                    ldd_a_hl(gb);
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        case 3:
            switch (q)
            {
            case 0:
                inc_rp(get_rp(p, gb));
                break;
            case 1:
                dec_rp(get_rp(p, gb));
                break;
            default:
                break;
            }
            break;
        case 4:
            inc_r(get_r(y, gb));
            break;
        case 5:
            dec_r(get_r(y, gb));
            break;
        case 6:
            ld_r_n(get_r(y, gb), fetch_n(gb));
            break;
        case 7:
            switch (y)
            {
            case 0:
                rlca(gb);
                break;
            case 1:
                rrca(gb);
                break;
            case 2:
                rla(gb);
                break;
            case 3:
                rra(gb);
                break;
            case 4:
                daa(gb);
                break;
            case 5:
                cpl(gb);
                break;
            case 6:
                scf(gb);
                break;
            case 7:
                ccf(gb);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    case 1:
        if (z == 6 && y == 6)
        {
            halt(gb);
        }
        else
        {
            ld_r_r(y, z, gb);
        }
        break;
    case 2:
        switch (y)
        {
        case 0:
            add_a_r(z, gb);
            break;
        case 1:
            adc_a_r(z, gb);
            break;
        case 2:
            sub_a_r(z, gb);
            break;
        case 3:
            sbc_a_r(z, gb);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        default:
            break;
        }
        break;
    case 3:

    default:
        break;
    }
}

// void execute_opcode(uint8_t opcode, gb_t *gb)
// {
//     opcode = gb->mem[gb->reg.pc];
//     gb->reg.pc++;
//     switch (opcode)
//     {
//     case 0x06: // lD nn,n
//         ld_nn_n(gb->reg.b, gb->mem[gb->reg.pc]);
//         gb->reg.pc++;
//         break;
//     case 0x0E:
//         ld_nn_n(gb->reg.c, gb->mem[gb->reg.pc]);
//         gb->reg.pc++;
//         break;
//     case 0x16:
//         ld_nn_n(gb->reg.d, gb->mem[gb->reg.pc]);
//         gb->reg.pc++;
//         break;
//     case 0x1E:
//         ld_nn_n(gb->reg.e, gb->mem[gb->reg.pc]);
//         gb->reg.pc++;
//         break;
//     case 0x26:
//         ld_nn_n(gb->reg.h, gb->mem[gb->reg.pc]);
//         gb->reg.pc++;
//         break;
//     case 0x2E:
//         ld_nn_n(gb->reg.l, gb->mem[gb->reg.pc]);
//         gb->reg.pc++;
//         break;

//     default:
//         break;
//     }
// }

void post_power_seq(gb_t *gb)
{
    gb->reg.pc = 0x100;
    gb->reg.af.pair = 0x01B0;
    gb->reg.bc.pair = 0x0013;
    gb->reg.de.pair = 0x00D8;
    gb->reg.hl.pair = 0x014D;
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

void emulate_cycle(gb_t *gb)
{
    uint8_t opcode = fetch_opcode(gb);
    parse_opcode(opcode, gb);
}