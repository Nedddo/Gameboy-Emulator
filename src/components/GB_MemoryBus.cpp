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
    std::string name = get_filename(path);
    std::cout << "Loading ROM: " << get_filename(path) << "(" << ROMSize << " Bytes)" << "\n";
    // allocate memory for the ROM and copy it over
    ROM = new uint8_t[ROMSize];
    is.read(reinterpret_cast<char *>(ROM), ROMSize);

    std::cout << name << " Loaded!\n";
}
