#include "../include/instruction.h"
#include <iostream>
Instruction::Instruction(uint32_t rawInstruction) {
    opcode = (rawInstruction & 0x7F);
    rd = (rawInstruction >> 7) & 0x1F; 
    funct3 = (rawInstruction >> 12) & 0x07;
    rs1 = (rawInstruction >> 15) & 0x1F;
    rs2 = (rawInstruction >> 20) & 0x1F;
    funct7 = (rawInstruction >> 25) & 0x7F;
if (opcode == 0x63) { // B-type 
    imm = ((rawInstruction >> 7) & 0x1) << 11 |   // bit [11]
          ((rawInstruction >> 8) & 0xF) << 1 |    // bits [4:1]
          ((rawInstruction >> 25) & 0x3F) << 5 |  // bits [10:5]
          ((rawInstruction >> 31) ? 0xFFFFF000 : 0); // Sign-extend
    imm <<= 1; 
}
    else if (opcode == 0x03 || opcode == 0x13) { // Load & Immediate
        imm = static_cast<int32_t>(rawInstruction) >> 20; // sign-extend
        std::cout << "IMM: " << imm << std::endl;
    } 
    else if (opcode == 0x23) { // Store (SW)
       imm = ((rawInstruction >> 7) & 0x1F) | ((rawInstruction >> 25) << 5);
if (rawInstruction & 0x80000000) imm |= 0xFFFFF000; // Sign-extend
    } 
    else {
        imm = 0;
    }
}
void Instruction::decode() const {
    std::cout << "Core Decoded: Opcode = 0x" << std::hex << (int)opcode
              << ", RD = " << std::dec << (int)rd
              << ", RS1 = " << (int)rs1
              << ", RS2 = " << (int)rs2
              << ", FUNCT3 = 0x" << std::hex << (int)funct3
              << ", FUNCT7 = 0x" << std::hex << (int)funct7
              << ", IMM = " << std::dec << imm
              << std::endl;
}
