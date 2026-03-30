#include <iostream>

#include "Emulator.h"
#include "components/GB_CPU.h"

int main()
{
    // fuck it dude i dont give a shit this is stupid as fuck
    // ill implement actual file input + commandline args, but for rn, this is fine
    std::string macPath = "/Users/neddo/Downloads/Tetris (World) (Rev 1)/Tetris (World) (Rev 1).gb";
    std::string windowsPath = "C:/Users/Ned/Downloads/Tetris (World) (Rev 1)/Tetris (World) (Rev 1).gb";
    GB_MemoryBus::loadRom(macPath);
    Emulator emulator;
    for (;;)
    {
        emulator.step();
    }
}
