#include "instructions.h"

uint8_t read_opcode(gb_t *gb)
{
    return gb->mem[gb->reg.pc];
}

void parse_opcode(uint8_t opcode)
{
    uint8_t x = opcode >> 6; 
    uint8_t y = (opcode >> 3) & 0b111;
    uint8_t z = opcode & 0b111;
    uint8_t p = (opcode >> 4) & 0b11;
    uint8_t q = (opcode >> 3) & 0b1;

    switch (x)
    {
    case 0b00:
        switch (z)
        {
        case 0:
            switch (y)
            {
            case 0:
                /* code */
                break;
            
            default:
                break;
            }
            break;
        
        default:
            break;
        }
        break;
    case 0b01:
    case 0b10:
    case 0b11:
    
    default:
        break;
    }
}