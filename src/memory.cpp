#include "../include/memory.h"
#include <iostream>


Memory::Memory() : mem(MEMORY_SIZE, 0) {}

uint32_t Memory::loadWord(uint32_t address) const {
    if (address % 4 != 0 || address + 3 >= MEMORY_SIZE) {
        std::cerr << "[ERROR] Invalid memory access at: 0x" << std::hex << address << std::dec << std::endl;
        return 0;
    }
    return (mem[address]) | (mem[address + 1] << 8) |
           (mem[address + 2] << 16) | (mem[address + 3] << 24);
}

void Memory::storeWord(uint32_t address, uint32_t value) {
    std::cout<<value<<" At addr"<<address<<std::endl;
    if (address % 4 != 0 || address + 3 >= MEMORY_SIZE) {
        std::cerr << "[ERROR] Invalid memory access at: 0x" << std::hex << address << std::dec << std::endl;
        return;
    }
    mem[address] = value & 0xFF;
    mem[address + 1] = (value >> 8) & 0xFF;
    mem[address + 2] = (value >> 16) & 0xFF;
    mem[address + 3] = (value >> 24) & 0xFF;
}