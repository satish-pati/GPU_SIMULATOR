#ifndef CORE_H
#define CORE_H

#include "memory.h"
#include <cstdint>
#include "../include/instruction.h"
#include <unordered_map>

class Core {
private:
    Memory& memory;
    int coreID;
    uint32_t baseAddress;
    bool isActive; // Flag to check if the core should continue execution
    //uint32_t registers[33] = {0};
   std::vector<std::tuple<std::string, int, int, int, int,std::string>> &program ;// Reference to loaded instructions
public:
    uint32_t pc; // Program Counter
    Core(Memory& memRef,std::vector<std::tuple<std::string, int, int, int, int,std::string>>& program, int coreId); // Fix constructor declaration
     uint32_t registers[33] = {0};
     
    uint32_t fetch(); // Fetch an instruction from memory
    void run(int numInstructions); // Run the core for a number of instructions
    void execute( const std::string &instruction, int rd, int rs1, int rs2, int imm, const std::string &label, std::unordered_map<std::string, int> &labelMap); // Execute an instruction

void printRegisters()const;

    //---------------added lines ---//
    // uint32_t getRegister(int reg) const;
    // void setRegister(int reg, uint32_t value);
    //----------------end here-----//
};

#endif // CORE_H
