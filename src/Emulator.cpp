//
// Created by Ned on 29/03/2026.
//

#include "Emulator.h"

#include <iostream>
#include <set>

#include "helpers/CircleBuffer.h"


Emulator::Emulator()
{
    cpu.init();
}

void Emulator::step()
{
    if (debug) handleDebugMode();
    unsigned int cycles = cpu.step();
    ppu.step(cycles);
    // MAYBE apu.step(cycles) (sound, dunno if thats in the projects scope)
}

// debug mode logic, formatting and static vars make it HUGE, its not that complex, but keeping it at the bottom, out of
// the way to look nice
// i have to break this shit up into helper mane - later!
void Emulator::handleDebugMode()
{
    // Formatting and functionality based HEAVILY on sameboys debugger
    // ----- STATIC VARIABLES USED IN THE FUNCTION -----
    static const std::set<std::string> validCommands =
    {
        "help", "step", "cont", "continue", "breakpoint", "bp", "delete", "reg"
    };
    // stores the last 5 PC values and their operations last pair is the NEXT one to be executed (not executed yet)
    static CircleBuffer<std::pair<uint16_t, uint8_t>> opBuffer(5);
    // lambda function for printing buffer
    static constexpr auto prnBuffer = []()
    {
        for (int i = 0; i < opBuffer.length(); i++)
        {
            if (i == opBuffer.length() - 1) std::cout << "-->"; else std::cout << "   ";
            std::cout << std::hex << opBuffer.at(i).first << " : " <<
            std::hex << (unsigned)opBuffer.at(i).second << "\n";
        }
    };
    // this is janky, i should change this later
    static GB_MemoryBus mem;
    // ---- FUNCTION LOGIC ----
    // add next cpu instruction to buffer
    auto pc = cpu.getPC();
    opBuffer.insert(std::pair(pc, mem.read8Bit(pc)) );
    // check for breakpoint
    if (breakpoints.contains(cpu.getPC()))
    {
        // clear debug command, effectively freezing until new command is received
        debugCommand = "";
        // print the buffer
        prnBuffer();
    }
    // continue command allows function like normal until a breakpoint is hit
    if (debugCommand != "cont" && debugCommand != "continue")
    {
        // continue command is implicitly handled, it acts as a flag to ignore command handling in debug mode
        // get new debug command
        std::cin >> debugCommand;
        if (!validCommands.contains(debugCommand))
        {
            // ugly ansi code is italics
            std::cout << "invalid command! Type \x1B[3mhelp\x1B[0m to see list of debug commands!\n";
            std::cin >> debugCommand;
        }
        else if (debugCommand == "help")
        {
            // ugly ansi code is bold and italics!!
            std::cout
            <<          "<------------------------| Commands |------------------------>\n"
            << "\x1B[1;3mstep\x1B[0m           ->   steps the program forward by one instruction\n"
            << "\x1B[1;3mcontinue\x1B[0m       ->   steps the program forward until a breakpoint is reached\n"
            << "\x1B[1;3mbreakpoint <x>\x1B[0m ->   sets a breakpoint at memory address <x>\n"
            << "\x1B[1;3mdelete <x>\x1B[0m     ->   deletes (if present) the breakpoint at memory address <x>\n"
            << "\x1B[1;3mreg <x>\x1B[0m        ->   print the values stored in the CPU registers\n";
            // get a new command
            std::cin >> debugCommand;
        }
        else if (debugCommand == "step")
        {
            // print opcode buffer
            prnBuffer();
        }
        else if (debugCommand == "breakpoint" || debugCommand == "bp")
        {
            uint16_t address;
            std::cin >> std::hex >> address;
            breakpoints.insert(address);
        }
        else if (debugCommand == "delete")
        {
            uint16_t address;
            std::cin >> std::hex >> address;
            breakpoints.erase(address);
        }
        else if (debugCommand == "reg")
        {
            cpu.printRegisters();
            std::cin >> debugCommand;
        }
    }
}

// helper functions for debugging