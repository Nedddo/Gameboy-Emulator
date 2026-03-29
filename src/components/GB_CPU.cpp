//
// Created by Ned on 29/03/2026.
//

#include "GB_CPU.h"

#include <iomanip>
#include <iostream>

void GB_CPU::init()
{
    // set registers to their post boot rom values
    AF = 0x01B0;
    BC = 0x0013;
    DE = 0x00D8;
    HL = 0x014D;
    // set PC to the point immediately after the boot rom
    PC = 0x100;
    SP = 0xFFFE;
}


void GB_CPU::fetch()
{
    opcode = bus.read8Bit(PC++);
}

unsigned int GB_CPU::step()
{
    fetch();
    return decodeAndExecute();
}


void GB_CPU::printRegisters() const
{
    std::cout << "Registers:\n";
    std::cout << "\nAF:    0x" << std::hex << std::setw(4) << std::setfill('0') << AF;
    std::cout << "\nBC:    0x" << std::setw(4) << std::setfill('0') << BC;
    std::cout << "\nDE:    0x" << std::setw(4) << std::setfill('0') << DE;
    std::cout << "\nHL:    0x" << std::setw(4) << std::setfill('0') << DE;

    std::cout << "\n\nPC:    0x" << std::setw(4) << std::setfill('0') << PC;
    std::cout << "\nSP:    0x" << std::setw(4) << std::setfill('0') << SP;
}

// ---- CPU INSTRUCTIONS ----
void GB_CPU::ADD(const uint8_t n)
{
    // Addition updates all flags, so we 0 the F register
    F = 0x00;
    // check for carry
    if (0xFF - n < A)
    {
        // set carry flag
        F |= 0x10;
    }
    // check for half carry
    if ((A & 0xF) + (n & 0xF) > 0xF)
    {
        // enable half carry flag
        F |= 0x20;
    }
    // technically better to do this AFTER updating A, (then we can just do A == 0) but looks nicer
    if (A + n == 0)
    {
        // enable Z flag
        F |= 0x80;
    }
    // update accumulator register
    A += n;
}

void GB_CPU::SUB(const uint8_t n)
{
    // Addition updates all flags, so we 0 the F register
    F = 0x40;
    // check for carry
    if (n > A)
    {
        // set carry flag
        F |= 0x10;
    }
    // check for half carry
    if ((n & 0xF) > (A & 0xF))
    {
        // enable half carry flag
        F |= 0x20;
    }
    // technically better to do this AFTER updating A, (then we can just do A == 0) but looks nicer
    if (A - n == 0)
    {
        // enable Z flag
        F |= 0x80;
    }
    // update accumulator register
    A -= n;
}

void GB_CPU::CP(const uint8_t n)
{
    // Comparisons are basically subtractions where we throw away the result
    // Addition updates all flags, so we 0 the F register
    F = 0x40;
    // check for carry
    if (n > A)
    {
        // set carry flag
        F |= 0x10;
    }
    // check for half carry
    if ((n & 0xF) > (A & 0xF))
    {
        // enable half carry flag
        F |= 0x20;
    }
    // technically better to do this AFTER updating A, (then we can just do A == 0) but looks nicer
    if (A - n == 0)
    {
        // enable Z flag
        F |= 0x80;
    }
}

void GB_CPU::INC(uint8_t& reg)
{
    // INC effects Z flag, H flag and unsets N flag.
    // Set effected flags to 0, so we can update them:
    F &= 0x10;
    // check for half carry
    if ((reg & 0xF) + 1 > 0xF)
    {
        // enable half carry flag
        F |= 0x20;
    }
    // increment register and update 0 flag
    if (++reg == 0)
    {
        // enable zero flag
        F |= 0x80;
    }
}

void GB_CPU::DEC(uint8_t& reg)
{
    // DEC effects Z flag, H flag and sets N flag.
    // Set effected flags to 0, so we can update them:
    F &= 0x10;
    // Set N flag as DEC is a type of subtraction
    F |= 0x40;
    // check for half borrow
    if ((reg & 0xF) < 0x01)
    {
        // enable half carry flag
        F |= 0x20;
    }
    // decrement register and update 0 flag
    if (--reg == 0)
    {
        // enable zero flag
        F |= 0x80;
    }
}

void GB_CPU::LD(uint8_t& to, const uint8_t& from)
{
    // LOAD BETWEEN REGISTERS
    to = from;
}
void GB_CPU::LDImmediate(uint8_t& reg)
{
    // LOAD 8 BIT IMMEDIATE VALUE
    reg = bus.read8Bit(PC++);
}
void GB_CPU::LDImmediate(uint16_t& reg)
{
    // LOAD 16 BIT IMMEDIATE
    reg = bus.read16Bit(PC);
    PC += 2;
}

void GB_CPU::LD(uint8_t &reg, uint16_t address)
{
    reg = bus.read8Bit(address);
    PC += 2;
}
void GB_CPU::LD(const uint16_t& address, const uint8_t& reg)
{
    bus.write8Bit(address, reg);
}

// big ol' fuck off switch statement, putting at bottom of file for cleanliness, outta sight, outta mind
unsigned int GB_CPU::decodeAndExecute()
{
    switch (opcode)
    {
        // NOP - does nothing
        case 0x00:
        {
            // fetch and decode still takes 4 clock cycles ( 1 machine cycle )
            return 0x04;
        }
        // ----- LOAD INSTRUCTIONS -----
        // Loads from register to register (e.g. LD A, B -> A = B)
        // LD B, B
        case 0x40:
        {
            // Really a NOP
            LD(B, B);
            return 0x04;
        }
        // LD B, C
        case 0x41:
        {
            LD(B, C);
           return 0x04;
        }
        // LD B, D
        case 0x42:
        {
            LD(B, D);
            return 0x04;
        }
        // LD B, E
        case 0x43:
        {
            LD(B, E);
            return 0x04;
        }
        // LD B, H
        case 0x44:
        {
            LD(B, H);
            return 0x04;
        }
        // LD B, L
        case 0x45:
        {
            LD(B, L);
            return 0x04;
        }
        // LD B, A
        case 0x47:
        {
            LD(B, A);
            return 0x04;
        }
        // LD C, B
        case 0x48:
        {
            LD(C, B);
            return 0x04;
        }
        // LD C, C
        case 0x49:
        {
            // kinda redundant, just a NOP really
            LD(C, C);
            return 0x04;
        }
        // LD C, D
        case 0x4A:
        {
            LD(C, D);
            return 0x04;
        }
        // LD C, E
        case 0x4B:
        {
            LD(C, E);
            return 0x04;
        }
        // LD C, H
        case 0x4C:
        {
            LD(C, H);
            return 0x04;
        }
        // LD C, L
        case 0x4D:
        {
            LD(C, L);
            return 0x04;
        }
        // LD C, A
        case 0x4F:
        {
            LD(C, A);
            return 0x04;
        }
        // LD D, B
        case 0x50:
        {
            LD(D, B);
            return 0x04;
        }
        // LD D, C
        case 0x51:
        {
            LD(D, C);
            return 0x04;
        }
        // LD D, D
        case 0x52:
        {
            LD(D, D);
            return 0x04;
        }
        // LD D, E
        case 0x53:
        {
            LD(D, E);
            return 0x04;
        }
        // LD D, H
        case 0x54:
        {
            LD(D, H);
            return 0x04;
        }
        // LD D, L
        case 0x55:
        {
            LD(D, L);
            return 0x04;
        }
        // LD D, A
        case 0x57:
        {
            LD(D, A);
            return 0x04;
        }
        // LD E, B
        case 0x58:
        {
            LD(E, B);
            return 0x04;
        }
        // LD E, C
        case 0x59:
        {
            LD(E, C);
            return 0x04;
        }
        // LD E, D
        case 0x5A:
        {
            LD(E, D);
            return 0x04;
        }
        // LD E, E
        case 0x5B:
        {
            LD(E, E);
            return 0x04;
        }
        // LD E, H
        case 0x5C:
        {
            LD(E, H);
            return 0x04;
        }
        // LD E, L
        case 0x5D:
        {
            LD(E, L);
            return 0x04;
        }
        // LD E, A
        case 0x5F:
        {
            LD(E, A);
            return 0x04;
        }
        // LD H, B
        case 0x60:
        {
            LD(H, B);
            return 0x04;
        }
        // LD H, C
        case 0x61:
        {
            LD(H, C);
            return 0x04;
        }
        // LD H, D
        case 0x62:
        {
            LD(H, D);
            return 0x04;
        }
        // LD H, E
        case 0x63:
        {
            LD(H, E);
            return 0x04;
        }
        // LD H, H
        case 0x64:
        {
            LD(H, H);
            return 0x04;
        }
        // LD H, L
        case 0x65:
        {
            LD(H, L);
            return 0x04;
        }
        // LD H, A
        case 0x67:
        {
            LD(H, A);
            return 0x04;
        }
        // LD L, B
        case 0x68:
        {
            LD(L, B);
            return 0x04;
        }
        // LD L, C
        case 0x69:
        {
            LD(L, C);
            return 0x04;
        }
        // LD L, D (heh)
        case 0x6A:
        {
            LD(L, D);
            return 0x04;
        }
        // LD L, E
        case 0x6B:
        {
            LD(L, E);
            return 0x04;
        }
        // LD L, H
        case 0x6C:
        {
            LD(L, H);
            return 0x04;
        }
        // LD L, L (lol)
        case 0x6D:
        {
            LD(L, L);
            return 0x04;
        }
        // LD L, A
        case 0x6F:
        {
            LD(L, A);
            return 0x04;
        }
        // LD A, B
        case 0x78:
        {
            LD(A, B);
            return 0x04;
        }
        // LD A, C
        case 0x79:
        {
            LD(A, C);
            return 0x04;
        }
        // LD A, D
        case 0x7A:
        {
            LD(A, D);
            return 0x04;
        }
        // LD A, E
        case 0x7B:
        {
            LD(A, E);
            return 0x04;
        }
        // LD A, H
        case 0x7C:
        {
            LD(A, H);
            return 0x04;
        }
        // LD A, L
        case 0x7D:
        {
            LD(A, L);
            return 0x04;
        }
        // LD A, A
        case 0x7F:
        {
            LD(A, A);
            return 0x04;
        }
        // Loads from pointer to register (e.g. LD A, [HL] -> A = Value stored at HL)
        // LD B, [HL]
        case 0x46:
        {
            LD(B, HL);
            return 0x08;
        }
        // LD C, [HL]
        case 0x4E:
        {
            LD(C, HL);
            return 0x08;
        }
        // LD D, [HL]
        case 0x56:
        {
            LD(D, HL);
            return 0x08;
        }
        // LD E, [HL]
        case 0x5E:
        {
            LD(E, HL);
            return 0x08;
        }
        // LD H, [HL]
        case 0x66:
        {
            LD(H, L);
            return 0x08;
        }
        // LD L, [HL]
        case 0x6E:
        {
            LD(L, HL);
            return 0x08;
        }
        // LD A, [HL]
        case 0x7E:
        {
            LD(A, HL);
            return 0x08;
        }
        default:
        {
            std::cout << "\x1b[1;31mUnknown opcode: " << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned>(opcode) << "\x1b[1;0m" << std::endl;
            printRegisters();
            std::terminate();
        }
    }
}


