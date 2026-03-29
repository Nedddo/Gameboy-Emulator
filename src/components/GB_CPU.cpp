//
// Created by Ned on 29/03/2026.
//

#include "GB_CPU.h"

#include <iomanip>
#include <iostream>

void GB_CPU::fetch()
{
    opcode = bus.read8Bit(PC++);
}

unsigned int GB_CPU::step()
{
    fetch();
    return decodeAndExecute();
}


void GB_CPU::printRegisters()
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
void GB_CPU::LD(uint16_t& address, uint8_t& reg)
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
            return 0x04;
        }
        // ----- LOAD INSTRUCTIONS -----
        // Loads from register to register (e.g. LD A, B -> A = B)
        // LD B, B
        case 0x40:
        {
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
        }
        // Loads from pointer to register (e.g. LD A, [HL] -> A = Value stored at HL)
        // LD B, HL
        case 0x46:
        {
            LD(B, HL);
            return 0x08;
        }
        default:
        {
            std::cout << "\x1b[1;31mUnknown opcode: " << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned>(opcode) << "\x1b[1;0m" << std::endl;
            printRegisters();
            std::terminate();
        }
    }
    return 0x00;
}


