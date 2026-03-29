//
// Created by Ned on 29/03/2026.
//

#ifndef GAMEBOY2_GB_MEMORYBUS_H
#define GAMEBOY2_GB_MEMORYBUS_H
#include <cstdint>
#include <string>

// contains the gameboy's memory as static members, with the individual "bus" objects managing access
class GB_MemoryBus
{    private:
        // pointer to rom, as size is not known in advance
        static uint8_t* ROM;
        static uint8_t ROMBank;
        static uint32_t ROMSize;
        // static memory (non-switchable)
        static uint8_t VRAM[0x2000];
        static uint8_t RAM[0x2000];
        static uint8_t spriteAttributes[0x00A0];
        static uint8_t IO[0x004C];
        static uint8_t HRAM[0x7F];
        static uint8_t interruptReg;
    public:
        static void initMemory();
        static void loadRom(const std::string& path);
};


#endif //GAMEBOY2_GB_MEMORYBUS_H