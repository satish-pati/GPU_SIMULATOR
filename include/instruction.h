#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>

class Instruction {
private:
    uint32_t rawInstruction;
    uint8_t opcode, funct3, funct7, rd, rs1, rs2;
    int32_t imm;
public:
    Instruction(uint32_t rawInstruction);
    void decode() const;
    uint8_t getOpcode() const { return opcode; }
    uint8_t getRd() const { return rd; }
    uint8_t getRs1() const { return rs1; }
    uint8_t getRs2() const { return rs2; }
        int32_t getImm() const { return imm; }
        uint8_t getFunct3() const { return funct3; }
        uint8_t getFunct7() const { return funct7; }

};

#endif // INSTRUCTION_H
