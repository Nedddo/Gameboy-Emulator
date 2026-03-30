//
// Created by Ned on 29/03/2026.
//

#include "Emulator.h"

Emulator::Emulator()
{
    cpu.init();
}

void Emulator::step()
{
    unsigned int cycles = cpu.step();
}
