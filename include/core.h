#ifndef CORE_H
#define CORE_H

#include "memory.h"
#include <cstdint>
#include "../include/instruction.h"

class Core {
private:
    Memory& memory;
    uint32_t pc; // Program Counter
    int coreID;
    uint32_t baseAddress;
   
public:
    Core(Memory& memRef, int coreId); // Fix constructor declaration
    uint32_t registers[33] = {0};// General-purpose registers
    uint32_t fetch(); // Fetch an instruction from memory
    void execute(const Instruction& instruction); // Execute an instruction
    void run(int numInstructions); // Run the core for a number of instructions
};

#endif // CORE_H
