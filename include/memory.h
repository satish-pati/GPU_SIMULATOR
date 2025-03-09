#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <cstdint>
#include <iostream>

class Memory {
private:
    static constexpr int MEMORY_SIZE = 4096;  // 4KB shared memory
    static constexpr int CORE_BLOCK_SIZE = 1024; // 1KB per core
    std::vector<uint8_t> mem; // Byte-addressable memory

public:
    Memory(); // Constructor initializes memory to zero
    // Load word (4 bytes) from memory
    uint32_t loadWord(uint32_t address,int coreID,bool &isactive) const;
    // Store word (4 bytes) in memory
    void storeWord(uint32_t address, uint32_t value, int coreID,bool &isactive);
    // Print memory 
    void printMem(int coreID) const;
};

#endif 
