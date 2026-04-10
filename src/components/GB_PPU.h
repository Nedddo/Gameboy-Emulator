//
// Created by Ned on 29/03/2026.
//

#ifndef GAMEBOY2_GB_PPU_H
#define GAMEBOY2_GB_PPU_H
#include "GB_MemoryBus.h"


class GB_PPU
{
    private:
        // clock cycle split between ly (current scanline) and dot (cycle for this scan line)
        unsigned int ly = 0x90; // 0->153 (154 total)
        unsigned int dot = 0; // 0->455 (456 total)
        // mode, a number between 0-3 which dictates the PPUs behaviour and what memory can be accessed by other components
        // after boot rom is executed, ly = 144, the start of vblank - we're hardcoding this for now
        unsigned int mode = 1;
        // PPU needs to write directly to memory, the CPU avoids this because my CPU architecture was stupid
        // in retrospect, CPU would have worked similar but its too late to change that now!
        GB_MemoryBus memory;

    public:
        // takes the current no. of cycles elapsed in the frame, gets the delta between its
        void step(unsigned int ticks);
};


#endif //GAMEBOY2_GB_PPU_H