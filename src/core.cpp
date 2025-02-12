#include "../include/core.h"
#include <iostream>
#include <algorithm>

Core::Core(Memory &memRef, int coreId)
    : memory(memRef), pc(0), coreID(coreId), baseAddress(coreId * 1024) {
    std::fill(std::begin(registers), std::end(registers), 0);
    registers[32] = coreId; //  core ID in register x32
}

uint32_t Core::fetch() {
    uint32_t instruction = memory.loadWord(baseAddress + pc);
    if (instruction == 0) {
        std::cerr << "[ERROR] Core " << coreID << " - Invalid instruction (0x0) at address: "
                  << std::hex << baseAddress + pc << std::dec << std::endl;
    }
    std::cout << "Core " << coreID << " - Fetched Instruction: 0x"
              << std::hex << instruction << std::dec
              << " at PC = " << pc << " (Address: " << baseAddress + pc << ")" 
              << std::endl;
    pc += 4;
    return instruction;
}

void Core::execute(const Instruction &instruction) {
    uint8_t opcode = instruction.getOpcode();
    uint8_t funct3 = instruction.getFunct3();
    uint8_t funct7 = instruction.getFunct7();
    uint8_t rd = instruction.getRd();
    uint8_t rs1 = instruction.getRs1();
    uint8_t rs2 = instruction.getRs2();
    int32_t imm = instruction.getImm(); // Sign-extended immediate

    switch (opcode) {
    case 0x33: // R-type instructions
        if (funct3 == 0x0) {
            if (funct7 == 0x00) { // ADD
                registers[rd] = registers[rs1] + registers[rs2];
                std::cout << "Core " << coreID << " - ADD: x" 
                          << (int)rd << " = " << registers[rs1] 
                          << " + " << registers[rs2] << " = " << registers[rd] 
                          << std::endl;
            } 
            else if (funct7 == 0x20) { // SUB
                registers[rd] = registers[rs1] - registers[rs2];
                std::cout << "Core " << coreID << " - SUB: x" 
                          << (int)rd << " = " << registers[rd] << std::endl;
            } 
        }
        break;

        case 0x63: // B-type Instructions (BNE)
        if (funct3 == 0x0) { // BNE
            if (registers[rs1] != registers[rs2]) {
                pc = pc - 4 + imm;  // Adjust for pre-incremented PC
            }
            std::cout << "Core " << coreID << " - BNE: Comparing x" 
                      << (int)rs1 << " (" << registers[rs1] << ") with x" 
                      << (int)rs2 << " (" << registers[rs2] 
                      << "), PC = " << pc << std::endl;
        }
        break;
    

    case 0x03: // LW
        {
            uint32_t address = registers[rs1] + imm;
            if (address % 4 != 0) {
                std::cerr << "[ERROR] Unaligned memory access at: 0x" 
                          << std::hex << address << std::dec << std::endl;
                break;
            }
            registers[rd] = memory.loadWord(address);
            std::cout << "Core " << coreID << " - LW: x" << (int)rd 
                      << " loaded with value " << registers[rd] 
                      << " from memory address 0x" << std::hex << address 
                      << std::dec << std::endl;
        }
        break;

    case 0x23: // SW
        {
            uint32_t address = registers[rs1] + imm;
            if (address % 4 != 0) {
                std::cerr << "[ERROR] Unaligned memory access at: 0x" 
                          << std::hex << address << std::dec << std::endl;
                break;
            }
            memory.storeWord(address, registers[rs2]);
            std::cout << "Core " << coreID << " - SW: Stored " << registers[rs2] 
                      << " at memory address 0x" << std::hex << address 
                      << std::dec << std::endl;
        }
        break;

    default:
        std::cerr << "[ERROR] Core " << coreID << " - Unknown instruction: 0x" 
                  << std::hex << (int)opcode << std::dec << std::endl;
        break;
    }
     registers[0] = 0;
}

void Core::run(int numInstructions) {
    for (int i = 0; i < numInstructions; i++) {
        uint32_t rawInst = fetch();
        Instruction inst(rawInst);
        inst.decode();
        execute(inst);
    }
}
