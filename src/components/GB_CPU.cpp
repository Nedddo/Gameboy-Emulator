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


void GB_CPU::handleInterrupts()
{
    // The IE register -> what interrupts should we handle
    const uint8_t IE = bus.read8Bit(0xFFFF);
    // The IF register -> what interrupts are being requested
    const uint8_t IF = bus.read8Bit(0xFF0F);
    // of the requested interrupts, which are enabled
    const uint8_t handle = IE & IF;
    if (handle)
    {
        // DISABLE IME, this needs to be done in a specific way. Ignoring for now
        if (handle & 0x01)
        {
            // checks if bit 0 is set, this corresponds to VBLANK (highest priority)
            PC = VBLANK;
            return;
        }
        if (handle & 0x02)
        {
            // checks if bit 1 is set, this corresponds to STAT
            PC = STAT;
            return;
        }
        if (handle & 0x04)
        {
            // checks if bit 2 is set, this corresponds to TIMER
            PC = TIMER;
            return;
        }
        if (handle & 0x08)
        {
            // checks if bit 3 is set, this corresponds to SERIAL
            PC = SERIAL;
            return;
        }
        if (handle & 0x10)
        {
            // checks if bit 2 is set, this corresponds to JOYPAD (lowest priority)
            PC = JOYPAD;
            return;
        }
    }
}

void GB_CPU::fetch()
{
    opcode = bus.read8Bit(PC++);
}

unsigned int GB_CPU::step()
{
    if (IME) handleInterrupts();
    fetch();
    return decodeAndExecute();
}


void GB_CPU::printRegisters() const
{
    std::cout << "Registers:\n";
    std::cout << "\nAF:    0x" << std::hex << std::setw(4) << std::setfill('0') << AF;
    std::cout << "\nBC:    0x" << std::setw(4) << std::setfill('0') << BC;
    std::cout << "\nDE:    0x" << std::setw(4) << std::setfill('0') << DE;
    std::cout << "\nHL:    0x" << std::setw(4) << std::setfill('0') << HL;

    std::cout << "\n\nPC:    0x" << std::setw(4) << std::setfill('0') << PC;
    std::cout << "\nSP:    0x" << std::setw(4) << std::setfill('0') << SP;
}

// ---- CPU INSTRUCTIONS ----
void GB_CPU::ADD(const uint8_t n)
{
    // Addition updates all flags, so we 0 the F register
    clearFlags();
    // check for carry
    if (0xFF - n < A)
    {
        // set carry flag
        setFlag(CY);
    }
    // check for half carry
    if ((A & 0xF) + (n & 0xF) > 0xF)
    {
        // enable half carry flag
        setFlag(HC);
    }
    // technically better to do this AFTER updating A, (then we can just do A == 0) but looks nicer
    if (A + n == 0)
    {
        // enable Z flag
        setFlag(Z);
    }
    // update accumulator register
    A += n;
}

void GB_CPU::SUB(const uint8_t n)
{
    // Subtraction sets updates all flags
    clearFlags();
    // set N flag as subtraction is, in fact subtraction
    setFlag(N);
    // check for carry
    if (n > A)
    {
        // set carry flag
        setFlag(CY);
    }
    // check for half carry
    if ((n & 0xF) > (A & 0xF))
    {
        // enable half carry flag
        setFlag(HC);
    }
    // technically better to do this AFTER updating A, (then we can just do A == 0) but looks nicer
    if (A - n == 0)
    {
        // enable Z flag
        setFlag(Z);
    }
    // update accumulator register
    A -= n;
}

void GB_CPU::CP(const uint8_t n)
{
    // Comparisons are basically subtractions where we throw away the result
    // Subtraction sets updates all flags
    clearFlags();
    // set N flag as subtraction is, in fact subtraction
    setFlag(N);
    // check for carry
    if (n > A)
    {
        // set carry flag
        setFlag(CY);
    }
    // check for half carry
    if ((n & 0xF) > (A & 0xF))
    {
        // enable half carry flag
        setFlag(HC);
    }
    // technically better to do this AFTER updating A, (then we can just do A == 0) but looks nicer
    if (A - n == 0)
    {
        // enable Z flag
        setFlag(Z);
    }
}

void GB_CPU::INC(uint8_t& reg)
{
    // INC effects Z flag, HC flag and unsets N flag.
    // keep CY as its previous value, zero all else
    keepFlag(CY);
    // check for half carry
    if ((reg & 0xF) + 1 > 0xF)
    {
        // enable half carry flag
        setFlag(HC);
    }
    // increment register and update 0 flag
    if (++reg == 0)
    {
        // enable zero flag
        setFlag(Z);
    }
}

void GB_CPU::DEC(uint8_t& reg)
{
    // DEC effects Z flag, H flag and sets N flag.
    // Set effected flags to 0, so we can update them:
    keepFlag(CY);
    // Set N flag as DEC is a type of subtraction
    setFlag(N);
    // check for half borrow
    if ((reg & 0xF) < 0x01)
    {
        // enable half carry flag
        setFlag(HC);
    }
    // decrement register and update 0 flag
    if (--reg == 0)
    {
        // enable zero flag
        setFlag(Z);
    }
}

void GB_CPU::XOR(uint8_t value)
{
    clearFlags();
    A ^= value;
    if (A == 0) setFlag(Z);
}

void GB_CPU::OR(uint8_t value)
{
    clearFlags();
    A |= value;
    if (A == 0) setFlag(Z);
}

void GB_CPU::AND(uint8_t value)
{
    clearFlags();
    // for whatever reason AND sets half carry flag
    setFlag(HC);
    A &= value;
    if (A == 0) setFlag(Z);
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

void GB_CPU::JP()
{
    PC = bus.read16Bit(PC);
}

void GB_CPU::JR()
{
    // JR uses a signed immediate (lets us go backwards!!!)
    PC += static_cast<int8_t>(bus.read8Bit(PC));
}

unsigned int GB_CPU::decodeAndExecute()
{
    // big ol' fuck off switch statement, putting at bottom of file for cleanliness, outta sight, outta mind
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
        // --> Loads from pointer to register (e.g. LD A, [HL] -> A = Value stored at HL)
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
        // --> Loads to registers from immediate values
        // LD A, u8
        case 0x3E:
        {
            LDImmediate(A);
            return 0x08;
        }
        // LD B, u8
        case 0x06:
        {
            LDImmediate(B);
            return 0x08;
        }
        // LD C, u8
        case 0x0E:
        {
            LDImmediate(C);
            return 0x08;
        }
        // LD D, u8
        case 0x16:
        {
            LDImmediate(D);
            return 0x08;
        }
        // LD E, u8
        case 0x1E:
        {
            LDImmediate(E);
            return 0x08;
        }
        // LD H, u8
        case 0x26:
        {
            LDImmediate(H);
            return 0x08;
        }
        // LD L, u8
        case 0x2E:
        {
            LDImmediate(L);
            return 0x08;
        }
        // --> Loads to 16 bit register pairs from immediate values
        // LD BC, u16
        case 0x01:
        {
            LDImmediate(BC);
            return 0x0C;
        }
        // LD DE, u16
        case 0x11:
        {
            LDImmediate(DE);
            return 0x0C;
        }
        // LD HL, u16
        case 0x21:
        {
            LDImmediate(HL);
            return 0x0C;
        }
        // LD SP, u16
        case 0x31:
        {
            LDImmediate(SP);
            return 0x0C;
        }
        // --> Loads to memory from register A
        // LD [BC], A
        case 0x02:
        {
            LD(BC, A);
            return 0x08;
        }
        // LD [DE], A
        case 0x12:
        {
            LD(DE, A);
            return 0x08;
        }
        // LD [HL++], A
        case 0x22:
        {
            LD(HL++, A);
            return 0x08;
        }
        // LD [HL--}, A
        case 0x32:
        {
            LD(HL--, A);
            return 0x08;
        }
        // ----- ARITHMETIC AND LOGICAL OPERATIONS -----
        // --> INC's
        // INC A
        case 0x3C:
        {
            INC(A);
            return 0x04;
        }
        // INC B
        case 0x04:
        {
            INC(B);
            return 0x04;
        }
        // INC C
        case 0x0C:
        {
            INC(C);
            return 0x04;
        }
        // INC D
        case 0x14:
        {
            INC(D);
            return 0x04;
        }
        // INC E
        case 0x1C:
        {
            INC(E);
            return 0x04;
        }
        // INC H
        case 0x24:
        {
            INC(H);
            return 0x04;
        }
        // INC L
        case 0x2C:
        {
            INC(L);
            return 0x04;
        }
        // INC [HL]
        case 0x34:
        {
            // This opcode is too specific and one off to really bother making a generic function for it
            bus.write8Bit(HL, bus.read8Bit(HL) + 1);
            return 0x0C;
        }
        // --> DEC's
        // DEC A
        case 0x3D:
        {
            DEC(A);
            return 0x04;
        }
        // DEC B
        case 0x05:
        {
            DEC(B);
            return 0x04;
        }
        // DEC C
        case 0x0D:
        {
            DEC(C);
            return 0x04;
        }
        // DEC D
        case 0x15:
        {
            DEC(D);
            return 0x04;
        }
        // DEC E
        case 0x1D:
        {
            DEC(E);
            return 0x04;
        }
        // DEC H
        case 0x25:
        {
            DEC(H);
            return 0x04;
        }
        // DEC L
        case 0x2D:
        {
            DEC(L);
        }
        // DEC [HL]
        case 0x35:
        {
            // again, too specific to make a specialized function for
            bus.write8Bit(HL, bus.read8Bit(HL) - 1);
            return 0x0C;
        }
        // --> ADD's
        // ADD A, B
        case 0x80:
        {
            ADD(B);
            return 0x04;
        }
        // ADD A, C
        case 0x81:
        {
            ADD(C);
            return 0x04;
        }
        // ADD A, D
        case 0x82:
        {
            ADD(D);
            return 0x04;
        }
        // ADD A, E
        case 0x83:
        {
            ADD(E);
            return 0x04;
        }
        // ADD A, H
        case 0x84:
        {
            ADD(H);
            return 0x04;
        }
        // ADD A, L
        case 0x85:
        {
            ADD(L);
            return 0x04;
        }
        // ADD A, [HL]
        case 0x86:
        {
            ADD(bus.read8Bit(HL));
            return 0x08;
        }
        // ADD A, A
        case 0x87:
        {
            ADD(A);
            return 0x04;
        }
        // --> ADC's : Addition + Carry flag :]
        // not worth making a whole new function out of so we make use of bool -> int implicit conversion
        // ADC A, B
        case 0x88:
        {
            ADD(B + isFlagSet(CY));
            return 0x04;
        }
        // ADC A, C
        case 0x89:
        {
            ADD(C + isFlagSet(CY));
            return 0x04;
        }
        // ADC A, D
        case 0x8A:
        {
            ADD(D + isFlagSet(CY));
            return 0x04;
        }
        // ADC A, E
        case 0x8B:
        {
            ADD(E + isFlagSet(CY));
            return 0x04;
        }
        // ADC A, H
        case 0x8C:
        {
            ADD(H + isFlagSet(CY));
            return 0x04;
        }
        // ADC A, L
        case 0x8D:
        {
            ADD(L + isFlagSet(CY));
            return 0x04;
        }
        // ADC A, [HL]
        case 0x8E:
        {
            ADD(bus.read8Bit(HL) + isFlagSet(CY));
            return 0x08;
        }
        // ADC A, A
        case 0x8F:
        {
            ADD(A + isFlagSet(CY));
            return 0x04;
        }
        // --> SUB's
        // SUB A, B
        case 0x90:
        {
            SUB(B);
            return 0x04;
        }
        // SUB A, C
        case 0x91:
        {
            SUB(C);
            return 0x04;
        }
        // SUB A, D
        case 0x92:
        {
            SUB(D);
            return 0x04;
        }
        // SUB A, E
        case 0x93:
        {
            SUB(E);
            return 0x04;
        }
        // SUB A, H
        case 0x94:
        {
            SUB(H);
            return 0x04;
        }
        // SUB A, L
        case 0x95:
        {
            SUB(L);
            return 0x04;
        }
        // SUB A, [HL]
        case 0x96:
        {
            SUB(bus.read8Bit(HL));
            return 0x08;
        }
        // SUB A, A
        case 0x97:
        {
            SUB(A);
            return 0x04;
        }
        // --> SBC's : Subtraction - Carry flag :]
        // SBC A, B
        case 0x98:
        {
            SUB(B + isFlagSet(CY));
            return 0x04;
        }
        // SBC A, C
        case 0x99:
        {
            SUB(C + isFlagSet(CY));
            return 0x04;
        }
        // SBC A, D
        case 0x9A:
        {
            SUB(D + isFlagSet(CY));
            return 0x04;
        }
        // SBC A, E
        case 0x9B:
        {
            SUB(E + isFlagSet(CY));
            return 0x04;
        }
        // SBC A, H
        case 0x9C:
        {
            SUB(H + isFlagSet(CY));
            return 0x04;
        }
        // SBC A, L
        case 0x9D:
        {
            SUB(L + isFlagSet(CY));
            return 0x04;
        }
        // SBC A, [HL]
        case 0x9E:
        {
            SUB(bus.read8Bit(HL) + isFlagSet(CY));
            return 0x08;
        }
        // SBC A, A
        case 0x9F:
        {
            SUB(A + isFlagSet(CY));
            return 0x04;
        }
        // --> AND's
        // AND A, B
        case 0xA0:
        {
            AND(B);
            return 0x04;
        }
        // AND A, C
        case 0xA1:
        {
            AND(C);
            return 0x04;
        }
        // AND A, D
        case 0xA2:
        {
            AND(D);
            return 0x04;
        }
        // AND A, E
        case 0xA3:
        {
            AND(E);
            return 0x04;
        }
        // AND A, H
        case 0xA4:
        {
            AND(H);
            return 0x04;
        }
        // AND A, L
        case 0xA5:
        {
            AND(L);
            return 0x04;
        }
        // AND A, [HL]
        case 0xA6:
        {
            AND(bus.read8Bit(HL));
            return 0x08;
        }
        // AND A, A
        case 0xA7:
        {
            AND(A);
            return 0x04;
        }
        // --> XOR's
        // XOR A, B
        case 0xA8:
        {
            XOR(B);
            return 0x04;
        }
        // XOR A, C
        case 0xA9:
        {
            XOR(C);
            return 0x04;
        }
        // XOR A, D
        case 0xAA:
        {
            XOR(D);
            return 0x04;
        }
        // XOR A, E
        case 0xAB:
        {
            XOR(E);
            return 0x04;
        }
        // XOR A, H
        case 0xAC:
        {
            XOR(H);
            return 0x04;
        }
        // XOR A, L
        case 0xAD:
        {
            XOR(L);
            return 0x04;
        }
        // XOR A, [HL]
        case 0xAE:
        {
            XOR(bus.read8Bit(HL));
            return 0x08;
        }
        // XOR A, A
        case 0xAF:
        {
            XOR(A);
            return 0x04;
        }
        // --> OR's
        // OR A, B
        case 0xB0:
        {
            OR(B);
            return 0x04;
        }
        // OR A, C
        case 0xB1:
        {
            OR(C);
            return 0x04;
        }
        // OR A, D
        case 0xB2:
        {
            OR(D);
            return 0x04;
        }
        // OR A, E
        case 0xB3:
        {
            OR(E);
            return 0x04;
        }
        // OR A, H
        case 0xB4:
        {
            OR(H);
            return 0x04;
        }
        // OR A, L
        case 0xB5:
        {
            OR(L);
            return 0x04;
        }
        // OR A, [HL]
        case 0xB6:
        {
            OR(bus.read8Bit(HL));
            return 0x08;
        }
        // OR A, A
        case 0xB7:
        {
            OR(A);
            return 0x04;
        }
        // --> CP's
        // CP A, B
        case 0xB8:
        {
            CP(B);
            return 0x04;
        }
        // CP A, C
        case 0xB9:
        {
            CP(C);
            return 0x04;
        }
        // CP A, D
        case 0xBA:
        {
            CP(D);
            return 0x04;
        }
        // CP A, E
        case 0xBB:
        {
            CP(E);
            return 0x04;
        }
        // CP A, H
        case 0xBC:
        {
            CP(H);
            return 0x04;
        }
        // CP A, L
        case 0xBD:
        {
            CP(L);
            return 0x04;
        }
        // CP A, [HL]
        case 0xBE:
        {
            CP(bus.read8Bit(HL));
            return 0x08;
        }
        // CP A, A
        case 0xBF:
        {
            CP(A);
            return 0x04;
        }
        // ----- JUMP INSTRUCTIONS -----
        // (we'll include CALLS and RST's here too)
        // JP u16
        case 0xC3:
        {
            JP();
            return 0x10;
        }
        // JP NZ, u16
        case 0xC2:
        {
            // Jumps if Z flag isn't set (is this comment really necessary?)
            if (!isFlagSet(Z))
            {
                JP();
                return 0x10;
            }
            return 0x0C;
        }
        // JP NC, u16
        case 0xD2:
        {
            // Jumps if CY flag isn't set
            if (!isFlagSet(CY))
            {
                JP();
                return 0x10;
            }
            return 0x0C;
        }
        // JP Z, u16
        case 0xCA:
        {
            // Jumps if Z flag IS set
            if (isFlagSet(Z))
            {
                JP();
                return 0x10;
            }
            return 0x0C;
        }
        // JP C, u16
        case 0xDA:
        {
            // Jumps if Z flag IS set
            if (isFlagSet(CY))
            {
                JP();
                return 0x10;
            }
            return 0x0C;
        }
        // --> Jump relatives
        // JR, s8
        case 0x18:
        {
            JR();
            return 0x0C;
        }
        // JR NZ, s8
        case 0x20:
        {
            // Jumps if Z flag isn't set (is this comment really necessary?)
            if (!isFlagSet(Z))
            {
                JR();
                return 0x0C;
            }
            return 0x08;
        }
        // JR NC, s8
        case 0x30:
        {
            // Jumps if CY flag isn't set
            if (!isFlagSet(CY))
            {
                JR();
                return 0x0C;
            }
            return 0x08;
        }
        // JR Z, s8
        case 0x28:
        {
            // Jumps if Z flag IS set
            if (isFlagSet(Z))
            {
                JR();
                return 0x0C;
            }
            return 0x08;
        }
        // JR C, s8
        case 0x38:
        {
            // Jumps if Z flag IS set
            if (isFlagSet(CY))
            {
                JR();
                return 0x0C;
            }
            return 0x08;
        }

        default:
        {
            std::cout << "\x1b[1;31mUnknown opcode: " << std::setw(2) << std::setfill('0')<< std::hex <<
                            static_cast<unsigned>(opcode) << "\x1b[1;0m" << std::endl;
            // PC gets incremented everytime BEFORE the instruction gets executed, to acutually reflect what line
            // the program stopped on, we decrement PC
            PC--;
            printRegisters();
            std::terminate();
        }
    }
}


