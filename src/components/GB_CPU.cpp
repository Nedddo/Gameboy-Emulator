//
// Created by Ned on 29/03/2026.
//

#include "GB_CPU.h"

#include <iomanip>
#include <iostream>

void GB_CPU::fetch()
{
}

unsigned int GB_CPU::step()
{
    return 0x00;
}


void GB_CPU::printRegisters()
{
    std::cout << "Registers:\n";
    std::cout << "\nAF:    0x" << std::hex << std::setw(4) << std::setfill('0') << AF;
    std::cout << "\nBC:    0x" << std::setw(4) << std::setfill('0') << BC;
    std::cout << "\nDE:    0x" << std::setw(4) << std::setfill('0') << DE;
    std::cout << "\nHL:    0x" << std::setw(4) << std::setfill('0') << DE;

    std::cout << "\n\nPC:    0x" << std::setw(4) << std::setfill('0') << PC;
    std::cout << "\nSP:    0x" << std::setw(4) << std::setfill('0') << SP;
}

// ---- CPU INSTRUCTIONS ----

// big ol' fuck off switch statement, putting at bottom of file for cleanliness, outta sight, outta mind
unsigned int GB_CPU::decodeAndExecute()
{
    return 0x00;
}


