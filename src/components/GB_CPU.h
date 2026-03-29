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
        union
        {
            struct
            {
                uint8_t C;
                uint8_t B;
            };
            uint16_t BC = 0x00;
        };
        union
        {
            struct
            {
                uint8_t E;
                uint8_t D;
            };
            uint16_t DE = 0x00;
        };
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
        uint16_t PC = 0x100;
        // CPU's bus
        GB_MemoryBus bus;
        // current opcode to be decoded and executed
        uint8_t opcode = 0x0;
        // ---------- METHODS ------------
        // sets opcode to the current byte at PC
        void fetch();
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
        void LD(uint16_t& address, uint8_t& reg);
    public:
        // responsible for the fetch-decode-execute of the next instruction - Returns T-cycles taken for last instruction
        unsigned int step();
        // handy print functions for debugging
        void printRegisters();

};


#endif //GAMEBOY2_GB_CPU_H