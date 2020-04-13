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
    return (gb->mem[gb->reg.pc + 1] << 8) | gb->mem[gb->reg.pc + 2];
}

uint8_t fetch_n(gb_t *gb)
{
    return gb->mem[gb->reg.pc + 1];
}

int8_t fetch_d(gb_t *gb)
{
    return (int8_t)gb->mem[gb->reg.pc + 1];
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

void set_pc(uint16_t pc, gb_t *gb)
{
    gb->reg.pc = pc;
}

void set_sp(uint16_t sp, gb_t *gb)
{
    gb->reg.sp = sp;
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
}

// TODO add_a_r check opcode 0xC6
void adc_a_r(uint8_t z, gb_t *gb)
{
    alu_adc(*get_r(z, gb), gb);
}

void sub_a_r(uint8_t z, gb_t *gb)
{
    alu_sub(*get_r(z, gb), gb);
}

void sbc_a_r(uint8_t z, gb_t *gb)
{
    alu_sbc(*get_r(z, gb), gb);
}

void and_a_r(uint8_t z, gb_t *gb)
{
    alu_and(*get_r(z, gb), gb);
}

void xor_a_r(uint8_t z, gb_t *gb)
{
    alu_xor(*get_r(z, gb), gb);
}

void or_a_r(uint8_t z, gb_t *gb)
{
    alu_or(*get_r(z, gb), gb);
}

void cp_a_r(uint8_t z, gb_t *gb)
{
    alu_cp(*get_r(z, gb), gb);
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
        set_pc(*pop_stack(gb), gb);
    }
}

void ldh_n_a(gb_t *gb)
{
    uint8_t n = fetch_n(gb);
    gb->mem[0xFF00 + n] = gb->reg.af.reg.hi;
}

void add_sp_d(gb_t *gb)
{
    uint16_t *sp = &gb->reg.sp;
    int8_t d = fetch_d(gb);
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
}

void ldh_a_n(gb_t *gb)
{
    uint8_t *reg_a = &gb->reg.af.reg.hi;
    uint8_t n = fetch_n(gb);
    *reg_a = gb->mem[0xFF00 + n];
}

void ld_hl_sp(gb_t *gb)
{
    uint16_t *reg_hl = &gb->reg.hl.pair;
    int8_t d = fetch_d(gb);
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
}

void pop_rp(uint16_t p, gb_t *gb)
{
    uint16_t *rp = get_rp(p, gb);
    uint8_t hi = gb->mem[gb->reg.sp - 1];
    uint8_t lo = gb->mem[gb->reg.sp];
    uint16_t res = (hi << 8) | lo;
    *rp = res;
    gb->reg.sp += 2;
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
    uint16_t nn = fetch_nn(gb);

    switch (y)
    {
    case 0:
        if (get_flag(FLAG_Z, gb) == 0)
        {
            gb->reg.pc = nn;
        }
        break;
    case 1:
        if (get_flag(FLAG_Z, gb) == 1)
        {
            gb->reg.pc = nn;
        }
        break;
    case 2:
        if (get_flag(FLAG_C, gb) == 0)
        {
            gb->reg.pc == nn;
        }
        break;
    case 3:
        if (get_flag(FLAG_C, gb) == 1)
        {
            gb->reg.pc == nn;
        }
        break;
    default:
        break;
    }
}

void ldh_c_a(gb_t *gb)
{
    gb->mem[0xFF00 + fetch_n(gb)];
}

void ld_nn_a(gb_t *gb)
{
    gb->mem[fetch_nn(gb)] = gb->reg.af.reg.hi;
}

void ldh_a_c(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[0xFF00 + fetch_n(gb)];
}

void ld_a_nn(gb_t *gb)
{
    gb->reg.af.reg.hi = gb->mem[fetch_nn(gb)];
}

void jp_nn(gb_t *gb)
{
    gb->reg.pc = fetch_nn(gb);
}

void di(gb_t *gb)
{
}

void ei(gb_t *gb)
{
}

// TODO implement
void call_cc_nn(uint8_t y, gb_t *gb)
{
    uint16_t nn = fetch_nn(gb);
    switch (y)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;

    default:
        break;
    }
}

void push_rp(uint8_t p, gb_t *gb)
{
    uint16_t *rp = get_rp(p, gb);
    gb->mem[--gb->reg.sp] = HIGH_BYTE(*rp);
    gb->mem[--gb->reg.sp] = LOW_BYTE(*rp);
}

void call_nn(gb_t *gb)
{
    gb->mem[--gb->reg.sp] = HIGH_BYTE(gb->reg.pc);
    gb->mem[--gb->reg.sp] = LOW_BYTE(gb->reg.pc);
    gb->reg.pc = fetch_nn(gb);
}

void add_a_n(gb_t *gb)
{
    alu_add(fetch_n(gb), gb);
}

void adc_a_n(gb_t *gb)
{
    alu_adc(fetch_n(gb), gb);
}

void sub_a_n(gb_t *gb)
{
    alu_sub(fetch_n(gb), gb);
}

void sbc_a_n(gb_t *gb)
{
    alu_sbc(fetch_n(gb), gb);
}

void and_a_n(gb_t *gb)
{
    alu_and(fetch_n(gb), gb);
}

void xor_a_n(gb_t *gb)
{
    alu_xor(fetch_n(gb), gb);
}

void or_a_n(gb_t *gb)
{
    alu_or(fetch_n(gb), gb);
}

void cp_a_n(gb_t *gb)
{
    alu_cp(fetch_n(gb), gb);
}

void rst(uint8_t y, gb_t *gb)
{
    gb->reg.pc = y * 8;
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
            and_a_r(z, gb);
            break;
        case 5:
            xor_a_r(z, gb);
            break;
        case 6:
            or_a_r(z, gb);
            break;
        case 7:
            cp_a_r(z, gb);
            break;
        default:
            break;
        }
        break;
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
                break;
            case 4:
                ldh_n_a(gb);
                break;
            case 5:
                add_sp_d(gb);
                break;
            case 6:
                ldh_a_n(gb);
                break;
            case 7:
                ld_hl_sp(gb);
                break;
            default:
                break;
            }
            break;
        case 1:
            switch (q)
            {
            case 0:
                pop_rp(p, gb);
                break;
            case 1:
                switch (p)
                {
                case 0:
                    ret(gb);
                    break;
                case 1:
                    reti(gb);
                    break;
                case 2:
                    jp_hl(gb);
                    break;
                case 3:
                    ld_sp_hl(gb);
                default:
                    break;
                }
                break;

            default:
                break;
            }
            break;
        case 2:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                jp_cc_nn(y, gb);
                break;
            case 4:
                ldh_c_a(gb);
                break;
            case 5:
                ld_nn_a(gb);
                break;
            case 6:
                ldh_a_c(gb);
                break;
            case 7:
                ld_a_nn(gb);
                break;
            default:
                break;
            }
            break;
        case 3:
            switch (y)
            {
            case 0:
                jp_nn(gb);
                break;
            case 1:
                // CB prefix
                break;
            case 6:
                di(gb);
                break;
            case 7:
                ei(gb);
                break;
            default:
                break;
            }
            break;
        case 4:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                call_cc_nn(y, gb);
                break;
            default:
                break;
            }
            break;
        case 5:
            if (q == 0)
            {
                push_rp(p, gb);
            }
            else if (q == 1 && p == 0)
            {
                call_nn(gb);
            }
            break;
        case 6:
            switch (y)
            {
            case 0:
                add_a_n(gb);
                break;
            case 1:
                adc_a_n(gb);
                break;
            case 2:
                sub_a_n(gb);
                break;
            case 3:
                sbc_a_n(gb);
                break;
            case 4:
                and_a_n(gb);
                break;
            case 5:
                xor_a_n(gb);
                break;
            case 6:
                or_a_n(gb);
                break;
            case 7:
                cp_a_n(gb);
                break;
            default:
                break;
            }
            break;
        case 7:
            rst(y, gb);
            break;
        default:
            break;
        }
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