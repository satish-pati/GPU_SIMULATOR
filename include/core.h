#ifndef CORE_H
#define CORE_H

#include <vector>
#include <tuple>
#include <string>
#include <array>
#include "Memory.h"  

class Core {
public:
    Core(Memory &memRef, 
         const std::vector<std::tuple<std::string, int, int, int, int, std::string>> &prog, 
         int coreId);

         void printRegisters()const;
    // ALU operation based on the instruction and operands.
    int ALUOperation(const std::string &instruction, int rs1Val, int rs2Val, int imm);

    // Determines if a branch instruction should be taken.
    bool isBranchTaken(const std::string &instruction, int rs1Val, int rs2Val,int rdVal,bool bnenum);

    // Write back the result to a register, ensuring registers x0 and x32 are read-only.
    void writeBack(int rd, int value);

    // Read the value of a register.
    int readRegister(int reg);

    // Reset all registers (except x32 which holds the core ID).
    void reset();

private:
    Memory &memory;   // Reference to the memory module.
    const std::vector<std::tuple<std::string, int, int, int, int, std::string>> program;  // Program instructions.
    int coreID;       // Core identifier.
    std::array<int, 33> registers; // Register file: x0 to x32 (33 registers total).
};

#endif 
