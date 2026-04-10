//
// Created by Ned on 29/03/2026.
//

#include "GB_PPU.h"

#include <iostream>
// Clock Cycles (dots) per frame
constexpr unsigned int DOTS_PER_FRAME = 70224;
// the amound of dots( clock ticks) per scanline
constexpr uint16_t SCANLINE_LENGTH = 456;
// length (in dots) that we scan OAM for
constexpr uint8_t OAM_SCAN_LENGTH = 80;
// minimum length (without penalties besides 12 dot minimum penalty, in dots) that we draw pixels for
constexpr uint8_t MINIMUM_DRAW_LENGTH = 172;
// vertical resolution of the gameboy
constexpr uint8_t GB_Y = 144;
// number of vertical scanlines
constexpr uint8_t TOTAL_SCANLINES = 154;

void GB_PPU::step(const unsigned int ticks)
{
for (int i = 0; i < ticks; i++)
    {
        // write LY to the relevant byte in the IO region of memory
        memory.write8Bit(0xFF44, ly);
        switch (mode)
        {
            // OAM Scan
            case 2:
            {
                if (dot == OAM_SCAN_LENGTH - 1)
                {
                    mode = 3;
                }
                // per dot logic for OAM scan
                break;
            }
            // Drawing Pixels
            case 3:
            {
                // TO DO: actually calculate penalty (probably best to do this before switching to mode 3)!
                unsigned int penalty = 0;
                if (dot == OAM_SCAN_LENGTH - 1 + MINIMUM_DRAW_LENGTH + penalty)
                {
                    mode = 0;
                }

                break;
            }
            // HBlank
            case 0:
            {
                // switch modes on last dot of the mode so next dot processes correctly
                if (dot == SCANLINE_LENGTH - 1)
                {
                    if (ly >= GB_Y - 1)
                    {
                        mode = 1;
                    }
                    else
                    {
                        mode = 2;
                    }

                }
                break;
            }
            // VBlank
            case 1:
            {
                // VBlank, handle interrupts, wait
                break;
            }
            default:
            {
                std::cout << "ERROR, INVALID PPU MODE - TERMINATING" << std::endl;
                terminate();
            }
        }
        dot++;
        // wrap dot and ly values
        if (dot == SCANLINE_LENGTH)
        {
            ly++;
            dot = 0;
            if (ly == TOTAL_SCANLINES)
            {
                // frame end!
                // exit VBlank
                mode = 2;
                //wrap ly
                ly = 0;
            }
        }
    }
}
