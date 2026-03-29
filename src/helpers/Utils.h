//
// Created by Ned on 23/03/2026.
//

#ifndef GB_EMU_UTILS_H
#define GB_EMU_UTILS_H
#include <string>

std::string get_filename(std::string path);

// contains a couple functions to separate and combine 16-bit numbers
namespace ByteHelpers
{
    inline uint16_t combined(const uint8_t LOW, const uint8_t HIGH) { return HIGH << 8 | LOW; }
    inline uint8_t high(const uint16_t x) { return x >> 8; }
    inline uint8_t low(const uint16_t x) { return x & 0xFF; }
}

#endif //GB_EMU_UTILS_H