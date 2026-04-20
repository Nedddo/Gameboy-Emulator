//
// Created by Ned on 12/04/2026.
//

#ifndef GAMEBOY2_GB_TIMER_H
#define GAMEBOY2_GB_TIMER_H
#include <cstdint>

#include "GB_MemoryBus.h"

class GB_Timer
{
    private:
        GB_MemoryBus bus;
        bool enabled = false;
        // counts gameboy clock ticks for each register we need to increment
        unsigned int DIVC = 0;
        unsigned int TIMAC = 0;
        // Registers
        uint8_t DIV;
        uint8_t TIMA;
        uint8_t TMA;
        uint8_t TAC;
    public:
        void update(unsigned int ticks);
};


#endif //GAMEBOY2_GB_TIMER_H