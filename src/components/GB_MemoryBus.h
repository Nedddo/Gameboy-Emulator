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
        static inline bool romLoaded = false;
        // static memory (non-switchable)
        static uint8_t VRAM[0x2000];
        static uint8_t RAM[0x2000];
        static uint8_t OAM[0x00A0];
        static uint8_t IO[0x004C];
        static uint8_t HRAM[0x7F];
        static uint8_t interruptReg;
        // ---- Members relating to Memory reservation ----
        // pointers to the PPUS bus IF they are claiming VRAM/OAM, else nullptr
        static GB_MemoryBus* VRAMClaimer;
        static GB_MemoryBus* OAMClaimer;

    public:
        static void loadRom(const std::string& path);
        // --- Read Instructions ---
        uint8_t read8Bit(uint16_t address);
        uint16_t read16Bit(uint16_t address);
        // --- Write Instructions ---
        void write8Bit(uint16_t address, uint8_t value);
        void write16Bit(uint16_t addr, uint16_t value);
        // --- MEMORY RESERVATION ---
        void claimVRAM();
        void releaseVRAM();
        void claimOAM();
        void releaseOAM();
        // the PPU, depending on the mode can claim memory, hence the static memory with non-static accessor objects
};


#endif //GAMEBOY2_GB_MEMORYBUS_H