#include <iostream>

#include "Emulator.h"
#include "components/GB_CPU.h"

int main()
{
    GB_MemoryBus::loadRom("C:/Users/Ned/Downloads/Tetris (World) (Rev 1)/Tetris (World) (Rev 1).gb");
    Emulator emulator;
    for (;;)
    {
        emulator.step();
    }
}
