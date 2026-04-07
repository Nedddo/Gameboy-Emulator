cmake_minimum_required(VERSION 4.0)
project(Gameboy2)

set(CMAKE_CXX_STANDARD 20)

add_executable(Gameboy2 src/main.cpp
        src/Emulator.cpp
        src/Emulator.h
        src/components/GB_MemoryBus.cpp
        src/components/GB_MemoryBus.h
        src/components/GB_CPU.cpp
        src/components/GB_CPU.h
        src/components/GB_PPU.cpp
        src/components/GB_PPU.h
        src/helpers/Utils.cpp
        src/helpers/CircleBuffer.cpp
        src/helpers/CircleBuffer.h)
