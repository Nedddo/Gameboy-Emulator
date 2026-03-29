//
// Created by Ned on 29/03/2026.
//

#include "GB_MemoryBus.h"
#include "../helpers/Utils.h"

#include <fstream>
#include <iostream>

void GB_MemoryBus::loadRom(const std::string &path)
{
    std::ifstream is;
    is.open(path, std::ios::binary);
    // check that the file was opened correctly before continuing
    if (!is.is_open())
    {
        std::cout << "Path not opened: " << path << "\n Is the formatting of the path valid? Does the file exist there?";
        return;
    }
    // get size of rom
    is.seekg(0, std::ios::end);
    ROMSize = is.tellg();
    is.seekg(0, std::ios::beg);
    const std::string name = get_filename(path);
    std::cout << "Loading ROM: " << get_filename(path) << "(" << ROMSize << " Bytes)" << "\n";
    // allocate memory for the ROM and copy it over
    ROM = new uint8_t[ROMSize];
    is.read(reinterpret_cast<char *>(ROM), ROMSize);

    std::cout << name << " Loaded!\n";
}

uint8_t GB_MemoryBus::read8Bit(uint16_t address)
{
    // handle reads using the gameboys memory map
    if (address <= 0x3FFF)
    {
        // 'bank' 0 rom
        return ROM[address];
    }
    if (address <= 0x7FFF)
    {
        // switchable rom bank, logic for bank switching isnt implemented yet so bank will always be 1
        uint16_t offset = address - 0x4000;
        return ROM[ROMBank * 0x4000 + offset];
    }
    if (address <= 0x8000)
    {
        // 0x8000 - 0x9FFF is vram
        // if vram is reserved by this bus OR if its not claimed at all
        if (VRAMClaimer == this || VRAMClaimer == nullptr)
        {
            return VRAM[address - 0x8000];
        }
        // otherwise return garbage value
        return 0xFF;
    }
    if (address <= 0xA000)
    {
        // this is the switchable ram bank, we have not implemented this yet. Should not be neccessary for now
        std::cout << "Switchable ram bank accessed, it may be time to implement this!\n";
        // garbage return
        return 0xFF;
    }
    if (address >= 0xC000 && address < 0xE000)
    {
        // THIS IS INTERNAL RAM, the region 0xE000 - 0xFDFF is unusable
        return RAM[address - 0xC000];
    }
    if (address >= 0xFE00 && address < 0xFE9F)
    {
        // OAM ATTRIBUTES
        //
        if (OAMClaimer == this || OAMClaimer == nullptr)
        {
            return OAM[address - 0xFE00];
        }
        // return garbage value
        return 0xFF;
    }
    if (address >= 0xFF00 && address < 0xFF4B)
    {
        // this is the memory mapped to I/O
        return IO[address - 0xFF00];
    }
    if (address >= 0xFF80 && address < 0xFFFE)
    {
        return HRAM[address - 0xFF80];
    }
    if (address == 0xFFFF)
    {
        // the final byte of memory is designated as the gameboys interrupt register
        return interruptReg;
    }

    std::cout << "Invalid Address! Are you attempting to read from unusable regions? Address: " << address << "\n";
    return 0xFF;
}

void GB_MemoryBus::write8Bit(uint16_t address, uint8_t value)
{
    // handle reads using the gameboys memory map
    if (address <= 0x3FFF)
    {
        // 'bank' 0 rom
        std::cout << "ERROR: Writing to Read-Only Memory at address: " << std::hex << address << "\n";
        return;
    }
    if (address <= 0x7FFF)
    {
        // switchable rom bank, logic for bank switching isnt implemented yet so bank will always be 1
        std::cout << "BANK SWITCHING NOT YET IMPLEMENTED, attempted to 'write' at address: " << std::hex << address << "\n";
        return;
    }
    // 0x8000 - 0x9FFF is VRAM
    if (address <= 0x9FFF)
    {
        if (VRAMClaimer == this || VRAMClaimer == nullptr)
        {
            // if the VRAM is claimed by this bus or claimed by NO bus, writing occurs
            VRAM[address - 0x8000] = value;
        }
        return;
    }
    if (address <= 0xBFFF)
    {
        std::cout << "Trying to write to switchable cartridge RAM - this is not yet implemented. Perhaps its time to do ts";
        return;
    }
    if (address >= 0xC000 && address <= 0xE000)
    {
        // THIS IS INTERNAL RAM, the region 0xE000 - 0xFDFF is unusable
        // THIS IS INTERNAL RAM, the region 0xE000 - 0xFDFF is unusable
        RAM[address - 0xC000] = value;
        return;
    }
    if (address >= 0xFE00 && address <= 0xFE9F)
    {
        // sprite attributes
        if (OAMClaimer == this || OAMClaimer == nullptr)
        {
            // if the OAM is claimed by this bus or claimed by NO bus, writing occurs
            VRAM[address - 0xFE00] = value;
        }
        return;
    }
    if (address >= 0xFF00 && address <= 0xFF4B)
    {
        // this is the memory mapped to I/O
        IO[address - 0xFF00] = value;
        return;
    }
    if (address >= 0xFF80 && address <= 0xFFFE)
    {
        HRAM[address - 0xFF80] = value;
        return;
    }
    if (address == 0xFFFF)
    {
        // the final byte of memory is designated as the gameboys interrup register
        interruptReg = value;
        return;
    }
    std::cout << "Invalid Address! Are you attempting to write to unusable regions? Address: " << std::hex << address << "\n";
}

// 16 Bit read and writes are simply consecutive 8-Bit read and writes, the gameboy is little endian, meaning the lower
// byte is stored in the lower memory address, with the upper being in the higher adress
uint16_t GB_MemoryBus::read16Bit(const uint16_t address)
{
    return ByteHelpers::combined(read8Bit(address), read8Bit(address + 1));
}

void GB_MemoryBus::write16Bit(const uint16_t addr, const uint16_t value)
{
    write8Bit(addr, ByteHelpers::low(value));
    write8Bit(addr + 1, ByteHelpers::high(value));
}




// --------- MEMORY RESERVATION IMPLEMENTATIONS ---------
void GB_MemoryBus::claimVRAM()
{
    VRAMClaimer = this;
}

void GB_MemoryBus::releaseVRAM()
{
    if (VRAMClaimer == this)
    {
        VRAMClaimer = nullptr;
    }
    else
    {
        std::cout << "ERROR: VRAM cannot be released by bus which is not claiming VRAM\n";
    }
}

void GB_MemoryBus::claimOAM()
{
    OAMClaimer = this;
}

void GB_MemoryBus::releaseOAM()
{
    if (VRAMClaimer == this)
    {
        VRAMClaimer = nullptr;
    }
    else
    {
        std::cout << "ERROR: OAM cannot be released by bus which is not claiming OAM\n";
    }
}
