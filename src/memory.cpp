#include "../include/memory.h"
#include <iostream>


Memory::Memory() : mem(MEMORY_SIZE, 0) {}
uint32_t Memory::loadWord(uint32_t address, int coreID,bool &isActive) const {
    uint32_t base = coreID * CORE_BLOCK_SIZE;
    /* if (address < base || address >= base + CORE_BLOCK_SIZE) {
        std::cerr << "Core " << coreID << " - ERROR: Invalid memory access for lw at address " 
                   << address << std::endl;
                   isActive = false;         
        return 0;
    }
    if (address % 4 != 0 || address + 3 >= MEMORY_SIZE) {
        std::cerr << "Core " << coreID << " - ERROR: Invalid memory access for lw at address" 
        << address << std::endl;
        isActive = false;
        return 0;
    }*/
    return (mem[address]) | (mem[address + 1] << 8) |
           (mem[address + 2] << 16) | (mem[address + 3] << 24);
} 
void Memory::storeWord(uint32_t address, uint32_t value, int coreID,bool &isActive) {
    uint32_t base = coreID * CORE_BLOCK_SIZE;
    // Ensure access is within core's allocated 1KB memory
   /* if (address < base || address >= base + CORE_BLOCK_SIZE) {
        std::cerr << "ERROR: Memory Access Violation for sw  by Core " << coreID <<  "at address " << address << std::endl;
        isActive = false;
        return;
    }
    std::cout<<value<<" At addr"<<address<<std::endl;
    if (address % 4 != 0 || address + 3 >= MEMORY_SIZE) {
        std::cerr << "ERROR: Memory Access Violation for sw  by Core " << address <<  std::endl;
        return;
    }*/
    mem[address] = value & 0xFF;
    mem[address + 1] = (value >> 8) & 0xFF;
    mem[address + 2] = (value >> 16) & 0xFF;
    mem[address + 3] = (value >> 24) & 0xFF;
}

void Memory::printMem(int coreID) const {
    uint32_t base = coreID * CORE_BLOCK_SIZE;
    uint32_t end = base + CORE_BLOCK_SIZE;
    std::cout << "\nMemory for Core " << coreID << ":" << std::endl;
    for (uint32_t i = base; i < end; i += 16) { // Print 4 words per line
        std::cout << "Addr(Bytes) " << i<<'-'<<(i+16) << ": ";
        for (int j = 0; j < 16 && (i + j) < end; j += 4) {
            bool dummyActive = true;
            std::cout << loadWord(i + j, coreID, dummyActive) << " ";
        }
        std::cout << std::endl;
    }
}
