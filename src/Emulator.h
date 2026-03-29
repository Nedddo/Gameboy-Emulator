//
// Created by Ned on 29/03/2026.
//

#ifndef GAMEBOY2_EMULATOR_H
#define GAMEBOY2_EMULATOR_H
#include "components/GB_CPU.h"
#include "components/GB_PPU.h"

// Exists more or less as a container which handles the interoperation of the various components
class Emulator
{]
    private:
        GB_CPU cpu;
        GB_PPU ppu;
    public:
        // progresses the emulator by one instruction
        void step();
};


#endif //GAMEBOY2_EMULATOR_H