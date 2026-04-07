//
// Created by Ned on 29/03/2026.
//

#ifndef GAMEBOY2_EMULATOR_H
#define GAMEBOY2_EMULATOR_H
#include <unordered_set>

#include "components/GB_CPU.h"
#include "components/GB_PPU.h"

/// Exists more or less as a container which handles the interoperation of the various components
class Emulator
{
    private:
        GB_CPU cpu;
        GB_PPU ppu;
        // --> debug mode variables
        bool debug = false;
        // list of breakpoints
        std::unordered_set<uint16_t> breakpoints;
        std::string debugCommand;
        // extract debug mode behaviour into a seperate function, keeps step functionm clean
        void handleDebugMode();
    public:
        Emulator();
        // progresses the emulator by one instruction
        void step();
        // enables emulators debug functions
        void debugEnable() { debug = true; }
};


#endif //GAMEBOY2_EMULATOR_H