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
        // this is the CPUs interrupt master enabler, if it is set to false, no matter what NO interrupts will be handled
        bool IME = false;
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
        // --> these are some memory address constants for RST and interrupts
        static constexpr uint16_t
        // --> RST memory addresses
        RST0 = 0x00, RST1 = 0x08, RST2 = 0x10, RST3 = 0x18,
        RST4 = 0x20, RST5 = 0x28, RST6 = 0x30, RST7 = 0x38,
        // --> Interrupt vectors
        VBLANK = 0x40, STAT   = 0x48,
        TIMER  = 0x50, SERIAL = 0x58,
        JOYPAD = 0x60;
        // ---------- METHODS ------------
        // --> Flag methods, unnecessary ig, just cleaner (and clearer!) than bitwise operations
        // returns true if flag is set
        bool isFlagSet(const decltype(Z) flag) const { return (F & flag) == flag; }
        // sets given flag to 1
        void setFlag(const decltype(Z) flag) { F |= flag; }
        // sets specified flag to off
        void clearFlag(const decltype(Z) flag) { F &= ~flag; }
        // sets F register to zero
        void clearFlags() { F = 0x00; }
        // sets all flags except for specified flag to 0
        void keepFlag(const decltype(Z) flag) { F &= flag; }
        // sets opcode to the current byte at PC
        // --------- HELPER FUNCTIONS FOR THE MAIN STEP LOGIC -------------
        void handleInterrupts();
        void fetch();
        /*** decodes and executes... duh. Easier to put these together as one method - Returns the T-cycles for the
        entire fetch-decode-execute, this is obviously a simplification, but calculating this seems unnecessary */
        unsigned int decodeAndExecute();
        // --------- GENERIC CPU INSTRUCTIONS ------------
        // most instructions in the Game Boy aren't unique, many do the same thing just with different operands
        // for example, there's something like >90 LD instructions - which are more or less the same
        // --> ARITHMETIC OPERATIONS
        /// adds specified regisater to A
        void ADD(uint8_t n);
        /// subtracts specified register from A
        void SUB(uint8_t n);
        /// essentially a subtraction where the result isn't stored, only updates flags
        void CP(uint8_t n);
        /// increment specified register
        void INC(uint8_t &reg);
        /// decrement specified register
        void DEC(uint8_t& reg);
        // --> LOGICAL OPERATIONS
        void XOR(uint8_t value);
        void OR(uint8_t value);
        void AND(uint8_t value);
        // --> LOADS
        /// load from one register to another
        void LD(uint8_t& to, const uint8_t& from);
        /// load an immediate to a register
        void LDImmediate(uint8_t& reg);
        /// load an immediate 16 bit number to a register pair
        void LDImmediate(uint16_t& reg);
        // load from/to memory to/from a register
        void LD(uint8_t& reg, uint16_t address);
        void LD(const uint16_t& address, const uint8_t& reg);
        // --> JUMPS
        /// Jumps to immediate 16-Bit value
        void JP();
        /// Jump relative, adds an immediate signed 8 bit value to PC
        void JR();
    public:
        /** set initial register values to their post boot rom values, the reason this isnt in the constructor is
        that in future, I may want to emulate the actual boot rom */
        void init();
        /// responsible for the fetch-decode-execute of the next instruction - Returns T-cycles taken for last instruction
        unsigned int step();
        /// handy print functions for debugging
        void printRegisters() const;
        /// returns PC value
        uint16_t getPC() const { return PC; }

};


#endif //GAMEBOY2_GB_CPU_H