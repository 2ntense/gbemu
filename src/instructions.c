#include "instructions.h"
#include <stdio.h>
#include <stdbool.h>

#define HIGH_BYTE(b) (b >> 8)
#define LOW_BYTE(b) (b & 0xFF)

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
    // return (gb->mem[gb->reg.pc + 1] << 8) | gb->mem[gb->reg.pc + 2];
    return (gb->mem[gb->reg.pc + 2] << 8) | gb->mem[gb->reg.pc + 1];
}

uint8_t fetch_n(gb_t *gb)
{
    return gb->mem[gb->reg.pc + 1];
}

int8_t fetch_d(gb_t *gb)
{
    return (int8_t)gb->mem[gb->reg.pc + 1];
}

uint8_t read_byte(gb_t *gb)
{
    gb->reg.pc++;
    return gb->mem[gb->reg.pc];
}

uint16_t read_short(gb_t *gb)
{
    uint8_t lo = read_byte(gb);
    uint8_t hi = read_byte(gb);
    return (hi << 8) | lo;
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

uint8_t *pop_stack(gb_t *gb)
{
    return &gb->mem[--gb->reg.sp];
}

void nop(gb_t *gb)
{
    gb->reg.pc++;
}

void ld_nn_sp(gb_t *gb)
{
    uint16_t nn = read_short(gb);
    gb->mem[nn] = LOW_BYTE(gb->reg.sp);
    gb->mem[nn + 1] = HIGH_BYTE(gb->reg.sp);
    gb->reg.pc++;
}

void stop(gb_t *gb)
{
    // TODO stop
    gb->reg.pc += 2;
}

void jr_d(gb_t *gb)
{
    int8_t d = (int8_t)read_byte(gb);
    if (d == 0)
    {
        gb->reg.pc++;
    }
    else
    {
        gb->reg.pc += d;
    }
}

void jr_cc_d(uint8_t y, gb_t *gb)
{
    uint8_t met = 0;
    switch (y)
    {
    case 4:
        if (get_flag(FLAG_Z, gb) == 0)
        {
            met = 1;
        }
        break;
    case 5:
        if (get_flag(FLAG_Z, gb) == 1)
        {
            met = 1;
        }
    case 6:
        if (get_flag(FLAG_C, gb) == 0)
        {
            met = 1;
        }
    case 7:
        if (get_flag(FLAG_C, gb) == 1)
        {
            met = 1;
        }
        break;
    default:
        break;
    }
    int8_t d = (int8_t)read_byte(gb);
    if (met == 1 && d != 0)
    {
        gb->reg.pc += d;
    }
    else
    {
        gb->reg.pc++;
    }
}

void ld_rp_nn(uint8_t p, gb_t *gb)
{
    uint16_t nn = read_short(gb);
    uint16_t *rp = get_rp(p, gb);
    *rp = nn;
    gb->reg.pc++;
}

void add_hl_rp(uint8_t p, gb_t *gb)
{
    uint16_t *reg_hl = &gb->reg.hl.pair;
    uint16_t *rp = get_rp(p, gb);
    uint32_t res = *reg_hl + *rp;
    *reg_hl = (uint16_t)(res & 0xFFFF);
    // H flag
    if (((*reg_hl & 0xFF) + (*rp & 0xFF)) > 0xFF)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if (res & 0xFFFF0000 > 0)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    gb->reg.pc++;
}

void ld_bc_a(gb_t *gb)
{
    gb->mem[gb->reg.bc.pair] = gb->reg.af.reg.hi;
    gb->reg.pc++;
}

void ld_de_a(gb_t *gb)
{
    gb->mem[gb->reg.de.pair] = gb->reg.af.reg.hi;
    gb->reg.pc++;
}

void ldi_hl_a(gb_t *gb)
{
    gb->mem[gb->reg.hl.pair++] = gb->reg.af.reg.hi;
    gb->reg.pc++;
}

void ldd_hl_a(gb_t *gb)
{
    gb->mem[gb->reg.hl.pair--] = gb->reg.af.reg.hi;
    gb->reg.pc++;
}

void ld_a_bc(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.bc.pair];
    gb->reg.pc++;
}

void ld_a_de(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.de.pair];
    gb->reg.pc++;
}

void ldi_a_hl(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.hl.pair++];
    gb->reg.pc++;
}

void ldd_a_hl(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[gb->reg.hl.pair--];
    gb->reg.pc++;
}

void inc_rp(uint8_t p, gb_t *gb)
{
    uint16_t *rp = get_rp(p, gb);
    *rp++;
    gb->reg.pc++;
}

void dec_rp(uint8_t p, gb_t *gb)
{
    uint16_t *rp = get_rp(p, gb);
    *rp--;
    gb->reg.pc++;
}

void inc_r(uint8_t y, gb_t *gb)
{
    uint8_t *r = get_r(y, gb);
    *r++;
    gb->reg.pc++;
}

void dec_r(uint8_t y, gb_t *gb)
{
    uint8_t *r = get_r(y, gb);
    *r--;
    gb->reg.pc++;
}

void ld_r_n(uint8_t y, gb_t *gb)
{
    uint8_t n = read_byte(gb);
    uint8_t *r = get_r(y, gb);
    *r = n;
    gb->reg.pc++;
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
    gb->reg.pc++;
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
    gb->reg.pc++;
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
    gb->reg.pc++;
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
    gb->reg.pc++;
}

void daa(gb_t *gb)
{
    // TODO daa
}

void cpl(gb_t *gb)
{
    gb->reg.af.reg.hi = ~gb->reg.af.reg.hi;
    set_flag(FLAG_N, gb);
    set_flag(FLAG_H, gb);
    gb->reg.pc++;
}

void scf(gb_t *gb)
{
    set_flag(FLAG_C, gb);
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    gb->reg.pc++;
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
    gb->reg.pc++;
}

void halt(gb_t *gb)
{
    // TODO HALT
    gb->reg.pc++;
}

//  TODO ld_r_r testing
void ld_r_r(uint8_t y, uint8_t z, gb_t *gb)
{
    uint8_t *dest = get_r(y, gb);
    *dest = *get_r(z, gb);
    gb->reg.pc++;
}

void alu_add(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint16_t res = *reg_a + src;
    *reg_a = (uint8_t)(res & 0xFF);
    // H flag
    if (((*reg_a & 0xF) + (src & 0xF)) > 0xF)
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

void alu_adc(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint16_t res = *reg_a + src + get_flag(FLAG_C, gb);
    *reg_a = (uint8_t)(res & 0xFF);
    // H flag
    if (((*reg_a & 0xF) + (src & 0xF)) > 0xF)
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

void alu_sub(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    // TODO check expression
    if (((int16_t)(*reg_a & 0xF) - ((int16_t)(src & 0xF))) < 0)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if (src > *reg_a)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    *reg_a -= src;
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

void alu_sbc(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    // TODO check expression
    if (((int16_t)(*reg_a & 0xF) - ((int16_t)(src & 0xF))) < 0)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if ((src + get_flag(FLAG_C, gb)) > *reg_a)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    *reg_a -= (src + get_flag(FLAG_C, gb));
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

void alu_and(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    *reg_a &= src;
    if (*reg_a == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    reset_flag(FLAG_N, gb);
    set_flag(FLAG_H, gb);
    reset_flag(FLAG_C, gb);
}

void alu_xor(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    *reg_a ^= src;
    if (*reg_a == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    reset_flag(FLAG_C, gb);
}

void alu_or(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    *reg_a |= src;
    if (*reg_a == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    reset_flag(FLAG_C, gb);
}

void alu_cp(uint8_t src, gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint8_t res = *reg_a - src;
    // Z flag
    if (res == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    // H flag
    if (((int16_t)(*reg_a & 0xF) - ((int16_t)(src & 0xF))) < 0)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if (src > *reg_a)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    set_flag(FLAG_N, gb);
}

// TODO add_a_r check opcode 0xC6
void add_a_r(uint8_t z, gb_t *gb)
{
    alu_add(*get_r(z, gb), gb);
    gb->reg.pc++;
}

// TODO add_a_r check opcode 0xC6
void adc_a_r(uint8_t z, gb_t *gb)
{
    alu_adc(*get_r(z, gb), gb);
    gb->reg.pc++;
}

void sub_a_r(uint8_t z, gb_t *gb)
{
    alu_sub(*get_r(z, gb), gb);
    gb->reg.pc++;
}

void sbc_a_r(uint8_t z, gb_t *gb)
{
    alu_sbc(*get_r(z, gb), gb);
    gb->reg.pc++;
}

void and_a_r(uint8_t z, gb_t *gb)
{
    alu_and(*get_r(z, gb), gb);
    gb->reg.pc++;
}

void xor_a_r(uint8_t z, gb_t *gb)
{
    alu_xor(*get_r(z, gb), gb);
    gb->reg.pc++;
}

void or_a_r(uint8_t z, gb_t *gb)
{
    alu_or(*get_r(z, gb), gb);
    gb->reg.pc++;
}

void cp_a_r(uint8_t z, gb_t *gb)
{
    alu_cp(*get_r(z, gb), gb);
    gb->reg.pc++;
}

void ret_cc(uint8_t y, gb_t *gb)
{
    uint8_t met = 0;
    switch (y)
    {
    case 0:
        if (get_flag(FLAG_Z, gb) == 0)
        {
            met = 1;
        }
        break;
    case 1:
        if (get_flag(FLAG_Z, gb) == 1)
        {
            met = 1;
        }
        break;
    case 2:
        if (get_flag(FLAG_C, gb) == 0)
        {
            met = 1;
        }
        break;
    case 3:
        if (get_flag(FLAG_C, gb) == 1)
        {
            met = 1;
        }
        break;
    default:
        break;
    }
    if (met == 1)
    {
        // TODO ret_cc check
        gb->reg.pc = *pop_stack(gb);
    }
    else
    {
        gb->reg.pc++;
    }
}

void ldh_n_a(gb_t *gb)
{
    // TODO add check, address must between ff00 - ffff
    uint8_t n = read_byte(gb);
    gb->mem[0xFF00 + n] = gb->reg.af.reg.hi;
    gb->reg.pc++;
}

void add_sp_d(gb_t *gb)
{
    uint16_t *sp = &gb->reg.sp;
    int8_t d = read_byte(gb);
    uint16_t res = *sp + d;
    *sp = res;
    // H flag
    if (((*sp & 0xF) + (d & 0xF)) > 0xF)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if (res & 0xFFFF0000 > 0)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_Z, gb);
    reset_flag(FLAG_N, gb);
    gb->reg.pc++;
}

void ldh_a_n(gb_t *gb)
{
    // TODO add check, address must between ff00 - ffff
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint8_t n = read_byte(gb);
    *reg_a = gb->mem[0xFF00 + n];
    gb->reg.pc++;
}

void ld_hl_sp(gb_t *gb)
{
    uint16_t *reg_hl = &gb->reg.hl.pair;
    int8_t d = read_byte(gb);
    uint16_t res = gb->reg.sp + d;
    *reg_hl = res;
    // H flag
    if (((*reg_hl & 0xF) + (d & 0xF)) > 0xF)
    {
        set_flag(FLAG_H, gb);
    }
    else
    {
        reset_flag(FLAG_H, gb);
    }
    // C flag
    if (res & 0xFFFF0000 > 0)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_Z, gb);
    reset_flag(FLAG_N, gb);
    gb->reg.pc++;
}

void pop_rp(uint16_t p, gb_t *gb)
{
    uint16_t *rp = get_rp(p, gb);
    uint8_t hi = gb->mem[gb->reg.sp - 1];
    uint8_t lo = gb->mem[gb->reg.sp];
    uint16_t res = (hi << 8) | lo;
    *rp = res;
    gb->reg.pc++;
    gb->reg.sp++;
}

void ret(gb_t *gb)
{
    uint16_t *sp = &gb->reg.sp;
    uint8_t hi = gb->mem[*sp - 1];
    uint8_t lo = gb->mem[*sp];
    gb->reg.pc = (hi << 8) | lo;
    *sp -= 2;
}

void reti(gb_t *gb)
{
    // TODO reti
    ret(gb);
}

void jp_hl(gb_t *gb)
{
    gb->reg.pc = gb->reg.hl.pair;
}

void ld_sp_hl(gb_t *gb)
{
    gb->reg.sp = gb->reg.hl.pair;
}

void jp_cc_nn(uint8_t y, gb_t *gb)
{
    uint8_t met = 0;
    switch (y)
    {
    case 0:
        if (get_flag(FLAG_Z, gb) == 0)
        {
            met = 1;
        }
        break;
    case 1:
        if (get_flag(FLAG_Z, gb) == 1)
        {
            met = 1;
        }
        break;
    case 2:
        if (get_flag(FLAG_C, gb) == 0)
        {
            met = 1;
        }
        break;
    case 3:
        if (get_flag(FLAG_C, gb) == 1)
        {
            met = 1;
        }
        break;
    default:
        break;
    }
    uint16_t nn = read_short(gb);
    if (met == 1 && nn > 0)
    {
        gb->reg.pc = nn;
    }
    else
    {
        gb->reg.pc++;
    }
}

void ldh_c_a(gb_t *gb)
{
    gb->mem[0xFF00 + gb->reg.bc.reg.lo] = gb->reg.af.reg.hi;
    gb->reg.pc++;
}

void ld_nn_a(gb_t *gb)
{
    uint16_t nn = read_short(gb);
    gb->mem[nn] = gb->reg.af.reg.hi;
    gb->reg.pc++;
}

void ldh_a_c(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[0xFF00 + gb->reg.bc.reg.lo];
    gb->reg.pc++;
}

void ld_a_nn(gb_t *gb)
{
    uint16_t nn = read_short(gb);
    gb->reg.af.reg.hi = gb->mem[nn];
    gb->reg.pc++;
}

void jp_nn(gb_t *gb)
{
    uint16_t nn = read_short(gb);
    gb->reg.pc = nn;
}

void di(gb_t *gb)
{
    // TODO di
    gb->reg.pc++;
}

void ei(gb_t *gb)
{
    // TODO ei
    gb->reg.pc++;
}

void call_cc_nn(uint8_t y, gb_t *gb)
{
    uint8_t met = 0;
    switch (y)
    {
    case 0:
        if (get_flag(FLAG_Z, gb) == 0)
        {
            met = 1;
        }
        break;
    case 1:
        if (get_flag(FLAG_Z, gb) == 1)
        {
            met = 1;
        }
        break;
    case 2:
        if (get_flag(FLAG_C, gb) == 0)
        {
            met = 1;
        }
        break;
    case 3:
        if (get_flag(FLAG_C, gb) == 1)
        {
            met = 1;
        }
        break;
    default:
        break;
    }
    uint16_t nn = read_byte(gb);
    if (met == 1 && nn > 0)
    {
        gb->reg.pc = nn;
    }
    else
    {
        gb->reg.pc++;
    }
}

void push_rp(uint8_t p, gb_t *gb)
{
    uint16_t *rp = get_rp(p, gb);
    gb->mem[--gb->reg.sp] = HIGH_BYTE(*rp);
    gb->mem[--gb->reg.sp] = LOW_BYTE(*rp);
    gb->reg.pc++;
}

void call_nn(gb_t *gb)
{
    gb->mem[--gb->reg.sp] = HIGH_BYTE(gb->reg.pc);
    gb->mem[--gb->reg.sp] = LOW_BYTE(gb->reg.pc);
    uint16_t nn = read_short(gb);
    if (nn > 0)
    {
        gb->reg.pc = nn;
    }
    else
    {
        gb->reg.pc++;
    }
}

void add_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_add(n, gb);
    gb->reg.pc++;
}

void adc_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_adc(n, gb);
    gb->reg.pc++;
}

void sub_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_sub(n, gb);
    gb->reg.pc++;
}

void sbc_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_sbc(n, gb);
    gb->reg.pc++;
}

void and_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_and(n, gb);
    gb->reg.pc++;
}

void xor_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_xor(n, gb);
    gb->reg.pc++;
}

void or_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_or(n, gb);
    gb->reg.pc++;
}

void cp_a_n(gb_t *gb)
{
    uint8_t n = read_byte(gb);
    alu_cp(n, gb);
    gb->reg.pc++;
}

void rst(uint8_t y, gb_t *gb)
{
    gb->reg.pc = y * 8;
}

void rlc(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t msb = *b >> 7;
    uint8_t res = (*b << 1) | msb;
    *b = res;
    if (msb == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    if (msb == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        set_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void rrc(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t lsb = *b & 1;
    uint8_t res = (*b >> 1) | (lsb << 7);
    *b = res;
    if (lsb == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    if (lsb == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        set_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void rl(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t c = get_flag(FLAG_C, gb);
    uint8_t msb = *b >> 7;
    uint8_t res = (*b << 1) | c;
    *b = res;
    if (msb == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    if (msb == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        set_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void rr(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t c = get_flag(FLAG_C, gb);
    uint8_t lsb = *b & 1;
    uint8_t res = (*b >> 1) | (c << 7);
    *b = res;
    if (lsb == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    if (lsb == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        set_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void sla(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t msb = *b >> 7;
    uint8_t res = *b << 1;
    *b = res;
    if (msb == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    if (msb == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void sra(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t lsb = *b & 1;
    uint8_t msb = *b >> 7;
    uint8_t res = (*b >> 1) | (msb << 7);
    *b = res;
    if (lsb == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    if (lsb == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void swap(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    // uint8_t lo = *b & 0x0F;
    // uint8_t hi = *b & 0xF0 >> 4;
    // uint8_t res = (lo << 4) | hi;
    uint8_t res = (*b << 4) | (*b >> 4);
    *b = res;
    if (res == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
    reset_flag(FLAG_C, gb);
}

void srl(uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t lsb = *b & 1;
    uint8_t res = *b >> 1;
    *b = res;
    if (lsb == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        reset_flag(FLAG_Z, gb);
    }
    if (lsb == 1)
    {
        set_flag(FLAG_C, gb);
    }
    else
    {
        reset_flag(FLAG_C, gb);
    }
    reset_flag(FLAG_N, gb);
    reset_flag(FLAG_H, gb);
}

void bit(uint8_t y, uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t res = (*b >> y) & 1;
    if (res == 0)
    {
        set_flag(FLAG_Z, gb);
    }
    else
    {
        set_flag(FLAG_Z, gb);
    }
    reset_flag(FLAG_N, gb);
    set_flag(FLAG_H, gb);
}

void res(uint8_t y, uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t res = *b & ~(1 << y);
    *b = res;
}

void set(uint8_t y, uint8_t z, gb_t *gb)
{
    uint8_t *b = get_r(z, gb);
    uint8_t res = *b | (1 << y);
    *b = res;
}

void parse_cb_opcode(uint8_t opcode, gb_t *gb)
{
    uint8_t x = opcode >> 6;
    uint8_t y = (opcode >> 3) & 0b111;
    uint8_t z = opcode & 0b111;
    gb->reg.pc++;
    switch (x)
    {
    case 0:
        switch (y)
        {
        case 0:
            rlc(z, gb);
            break;
        case 1:
            rrc(z, gb);
            break;
        case 2:
            rl(z, gb);
            break;
        case 3:
            rr(z, gb);
            break;
        case 4:
            sla(z, gb);
            break;
        case 5:
            sra(z, gb);
            break;
        case 6:
            swap(z, gb);
            break;
        case 7:
            srl(z, gb);
            break;
        default:
            break;
        }
        break;
    case 1:
        bit(y, z, gb);
        break;
    case 2:
        res(y, z, gb);
        break;
    case 3:
        set(y, z, gb);
        break;
    default:
        break;
    }
    gb->reg.pc++;
}

void parse_opcode(uint8_t opcode, gb_t *gb)
{
    uint8_t x = opcode >> 6;
    uint8_t y = (opcode >> 3) & 0b111;
    uint8_t z = opcode & 0b111;
    uint8_t p = (opcode >> 4) & 0b11;
    uint8_t q = (opcode >> 3) & 0b1;
#ifdef DEBUG
    printf("Parsing pc:%04x, opcode:%02x, x:%d, y:%d, z:%d, p:%d, q:%d\n", gb->reg.pc, opcode, x, y, z, p, q);
#endif
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
                return;
            case 1:
                ld_nn_sp(gb);
                return;
            case 2:
                stop(gb);
                return;
            case 3:
                jr_d(gb);
                return;
            case 4:
            case 5:
            case 6:
            case 7:
                jr_cc_d(y, gb);
                return;
            default:
                return;
            }
            return;
        case 1:
            switch (q)
            {
            case 0:
                ld_rp_nn(p, gb);
                return;
            case 1:
                add_hl_rp(p, gb);
                return;
            default:
                return;
            }
            return;
        case 2:
            switch (q)
            {
            case 0:
                switch (p)
                {
                case 0:
                    ld_bc_a(gb);
                    return;
                case 1:
                    ld_de_a(gb);
                    return;
                case 2:
                    ldi_hl_a(gb);
                    return;
                case 3:
                    ldd_hl_a(gb);
                    return;
                default:
                    return;
                }
                return;
            case 1:
                switch (p)
                {
                case 0:
                    ld_a_bc(gb);
                    return;
                case 1:
                    ld_a_de(gb);
                    return;
                case 2:
                    ldi_a_hl(gb);
                    return;
                case 3:
                    ldd_a_hl(gb);
                    return;
                default:
                    return;
                }
                return;
            default:
                return;
            }
            return;
        case 3:
            switch (q)
            {
            case 0:
                inc_rp(p, gb);
                return;
            case 1:
                dec_rp(p, gb);
                return;
            default:
                return;
            }
            return;
        case 4:
            inc_r(y, gb);
            return;
        case 5:
            dec_r(y, gb);
            return;
        case 6:
            ld_r_n(y, gb);
            return;
        case 7:
            switch (y)
            {
            case 0:
                rlca(gb);
                return;
            case 1:
                rrca(gb);
                return;
            case 2:
                rla(gb);
                return;
            case 3:
                rra(gb);
                return;
            case 4:
                daa(gb);
                return;
            case 5:
                cpl(gb);
                return;
            case 6:
                scf(gb);
                return;
            case 7:
                ccf(gb);
                return;
            default:
                return;
            }
            return;
        default:
            return;
        }
        return;
    case 1:
        if (z == 6 && y == 6)
        {
            halt(gb);
        }
        else
        {
            ld_r_r(y, z, gb);
        }
        return;
    case 2:
        switch (y)
        {
        case 0:
            add_a_r(z, gb);
            return;
        case 1:
            adc_a_r(z, gb);
            return;
        case 2:
            sub_a_r(z, gb);
            return;
        case 3:
            sbc_a_r(z, gb);
            return;
        case 4:
            and_a_r(z, gb);
            return;
        case 5:
            xor_a_r(z, gb);
            return;
        case 6:
            or_a_r(z, gb);
            return;
        case 7:
            cp_a_r(z, gb);
            return;
        default:
            return;
        }
        return;
    case 3:
        switch (z)
        {
        case 0:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                ret_cc(y, gb);
                return;
            case 4:
                ldh_n_a(gb);
                return;
            case 5:
                add_sp_d(gb);
                return;
            case 6:
                ldh_a_n(gb);
                return;
            case 7:
                ld_hl_sp(gb);
                return;
            default:
                return;
            }
            return;
        case 1:
            switch (q)
            {
            case 0:
                pop_rp(p, gb);
                return;
            case 1:
                switch (p)
                {
                case 0:
                    ret(gb);
                    return;
                case 1:
                    reti(gb);
                    return;
                case 2:
                    jp_hl(gb);
                    return;
                case 3:
                    ld_sp_hl(gb);
                default:
                    return;
                }
                return;
            default:
                return;
            }
            return;
        case 2:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                jp_cc_nn(y, gb);
                return;
            case 4:
                ldh_c_a(gb);
                return;
            case 5:
                ld_nn_a(gb);
                return;
            case 6:
                ldh_a_c(gb);
                return;
            case 7:
                ld_a_nn(gb);
                return;
            default:
                return;
            }
            return;
        case 3:
            switch (y)
            {
            case 0:
                jp_nn(gb);
                return;
            case 1:
                parse_cb_opcode(opcode, gb);
                return;
            case 6:
                di(gb);
                return;
            case 7:
                ei(gb);
                return;
            default:
                return;
            }
            return;
        case 4:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                call_cc_nn(y, gb);
                return;
            default:
                return;
            }
            return;
        case 5:
            if (q == 0)
            {
                push_rp(p, gb);
            }
            else if (q == 1 && p == 0)
            {
                call_nn(gb);
            }
            return;
        case 6:
            switch (y)
            {
            case 0:
                add_a_n(gb);
                return;
            case 1:
                adc_a_n(gb);
                return;
            case 2:
                sub_a_n(gb);
                return;
            case 3:
                sbc_a_n(gb);
                return;
            case 4:
                and_a_n(gb);
                return;
            case 5:
                xor_a_n(gb);
                return;
            case 6:
                or_a_n(gb);
                return;
            case 7:
                cp_a_n(gb);
                return;
            default:
                return;
            }
            return;
        case 7:
            rst(y, gb);
            return;
        default:
            return;
        }
        return;
    default:
        return;
    }
}

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
    uint8_t opcode = gb->mem[gb->reg.pc];
    parse_opcode(opcode, gb);
}