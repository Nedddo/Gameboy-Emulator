//
// Created by Ned on 12/04/2026.
//

#include "GB_Timer.h"

#include <iostream>
#include <ostream>

void GB_Timer::update(unsigned int ticks)
{
    DIVC += ticks;
    TIMAC += ticks;
    if (DIVC >= 256)
    {
        DIVC -= 256;
        DIV++;
    }

    TAC = bus.read8Bit(0xFF07);
    TMA = bus.read8Bit(0xFF06);

    bool enable = TAC & 0b100;
    uint8_t mode = TAC & 0b11;

    bool inc = false;
    if (enable)
    {
        switch (mode)
        {
            case 0b00:
            {
                if (TIMAC >= 4096)
                {
                    TIMAC -= 4096;
                    inc = true;
                }
                break;
            }
            case 0b01:
            {
                if (TIMAC >= 262144)
                {
                    TIMAC -= 262144;
                    inc = true;
                }
                break;
            }
            case 0b10:
            {
                if (TIMAC >= 65536)
                {
                    TIMAC -= 65536;
                    inc = true;
                }
                break;
            }
            case 0b11:
            {
                if (TIMAC >= 16384)
                {
                    TIMAC -= 16384;
                    inc = true;
                }
                break;
            }
            default:
            {
                std::cout << "INVALID TIMER MODE: " << (int)mode << std::endl;
                std::terminate();
            }
        }
    }
    if (inc)
    {
        if (TIMA == 0xFF)
        {
            TIMA = TMA;
            bus.write8Bit(0xFF0F, bus.read8Bit(0xFF0F) | mode);
        }
        else
        {
            TIMA++;
        }
    }
    bus.write8Bit(0xFF04, DIV);
    bus.write8Bit(0xFF05, TIMA);
}