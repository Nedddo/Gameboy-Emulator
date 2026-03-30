//
// Created by Ned on 29/03/2026.
//

#ifndef GAMEBOY2_GB_CPU_H
#define GAMEBOY2_GB_CPU_H
#include <cstdint>

#include "GB_MemoryBus.h"


class GB_CPU
{
    private:
        // ----------- REGISTERS -------------
        // Union means that AF and the struct share the same space in memory
        // AF (Accumulator and Flags register pair)
        union
        {
            // 16-bit Anonymous struct containing 2 8bit registers
            struct
            {
                uint8_t F;
                uint8_t A;
            };
            // 16-bit integer containing the structs pair
            uint16_t AF = 0x00;
        };
        // BC
        union
        {
            struct
            {
                uint8_t C;
                uint8_t B;
            };
            uint16_t BC = 0x00;
        };
        // DE
        union
        {
            struct
            {
                uint8_t E;
                uint8_t D;
            };
            uint16_t DE = 0x00;
        };
        // HL (often used as a pointer)
        union
        {
            struct
            {
                uint8_t L;
                uint8_t H;
            };
            uint16_t HL = 0x00;
        };
        // specialized 16-bit registers: stack pointer and program counter
        uint16_t SP = 0x00;
        uint16_t PC = 0x00;
        // CPU's bus
        GB_MemoryBus bus;
        // current opcode to be decoded and executed
        uint8_t opcode = 0x00;
        // ------ CONSTANTS ------
        // --> Bitmasks for the F register:
        // anonymous enum because Flags::Z would be chopped and make flag checks MORE cumbersome, which i dont want
        // does make function syntax a bit chopped but ah well, such is life!
        enum : uint8_t
        {
        // set if last operation resulted in 0
        Z  = 0x80,
        // set if last operation was a subtraction
        N  = 0x40,
        // set if last operation had a half carry
        HC = 0x20,
        // set if last operation had a carry
        CY = 0x10
        };
        // ---------- METHODS ------------
        // --> Flag methods, unnecessary ig, just cleaner (and clearer!) than bitwise operations
        // returns true if flag is set
        bool isFlagSet(const decltype(Z) flag) const { return (F & flag) == F; }
        // sets given flag to 1
        void setFlag(const decltype(Z) flag) { F |= flag; }
        // sets specified flag to off
        void clearFlag(const decltype(Z) flag) { F &= ~flag; }
        // sets F register to zero
        void clearFlags() { F = 0x00; }
        // sets all flags except for specified flag to 0
        void keepFlag(const decltype(Z) flag) { F &= flag; }
        // sets opcode to the current byte at PC
        void fetch();
        // helper function to see if a flag is set
        // decodes and executes... duh. Easier to put these together as one method - Returns the T-cycles for the
        // entire fetch-decode-execute, this is obviously a simplification, but calculating this seems unnecessary
        unsigned int decodeAndExecute();
        // --------- GENERIC CPU INSTRUCTIONS ------------
        // most instructions in the Game Boy aren't unique, many do the same thing just with different operands
        // for example, there's something like >90 LD instructions - which are more or less the same
        void ADD(uint8_t n);
        void SUB(uint8_t n);
        void CP(uint8_t n);
        void INC(uint8_t &reg);
        void DEC(uint8_t& reg);
        // load from one register to another
        void LD(uint8_t& to, const uint8_t& from);
        // load an immediate to a register
        void LDImmediate(uint8_t& reg);
        void LDImmediate(uint16_t& reg);
        // load from/to memory to/from a register
        void LD(uint8_t& reg, uint16_t address);
        void LD(const uint16_t& address, const uint8_t& reg);
        // --> JUMPS
        // Jumps to immediate 16-Bit value
        void JP();
        // Jump relative, adds an immediate signed 8 bit value to PC
        void JR();
    public:
        // set initial register values to their post boot rom values, the reason this isnt in the constructor is
        // that in future, I may want to emulate the actual boot rom
        void init();
        // responsible for the fetch-decode-execute of the next instruction - Returns T-cycles taken for last instruction
        unsigned int step();
        // handy print functions for debugging
        void printRegisters() const;

};


#endif //GAMEBOY2_GB_CPU_H