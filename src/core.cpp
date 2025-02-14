#include "../include/core.h"
#include <iostream>
Core::Core(Memory &memRef, int coreId)
    : memory(memRef), pc(0), coreID(coreId), baseAddress(coreId * 1024) {
    std::fill(std::begin(registers), std::end(registers), 0);
    registers[32] = coreId; //  core ID to register x32
}
uint32_t Core::fetch() {
    uint32_t instruction = memory.loadWord(baseAddress + pc);
    if (instruction == 0) {
        std::cerr << "[ERROR] Core " << coreID << " - Attempted to fetch invalid instruction (0x0) at address: " 
                  << baseAddress + pc << std::endl;
    }
    std::cout << "Core " << coreID << " Fetched Instruction: 0x" 
              << std::hex << instruction << std::dec 
              << " at PC = " << pc << " (Address: " << baseAddress + pc << ")" 
              << std::endl;
    return instruction;
}
void Core::execute(const Instruction &instruction) {
    uint8_t opcode = instruction.opcode;
    uint8_t funct3 = instruction.funct3;
    uint8_t funct7 = instruction.funct7;
    uint8_t rd = instruction.rd;
    uint8_t rs1 = instruction.rs1;
    uint8_t rs2 = instruction.rs2;
    int32_t imm = instruction.imm;
    bool shouldIncrementPC = true; 
    switch (opcode) {
    case 0x33: // R-type instructions
    {
        if (funct3 == 0x0) {
            if (funct7 == 0x00) { // ADD
                registers[rd] = registers[rs1] + registers[rs2];
                std::cout << "Core " << coreID << " - ADD: Register x" 
                          << (int)rd << " = " << registers[rs1] 
                          << " + " << registers[rs2] << " = " << registers[rd] 
                          << std::endl;
                          registers[0]=0;
            } 
            else if (funct7 == 0x20) { // SUB
                registers[rd] = registers[rs1] - registers[rs2];
                registers[0]=0;
                std::cout << "Core " << coreID << " - SUB: Register x" 
                          << (int)rd << " = " << registers[rd] << std::endl;
            } 
        }
        break;
    } 
    case 0x63: // B-type Instructions
{
    if (funct3 == 0x1) { // BNE 
        if (registers[rs1] != registers[rs2]) {
            pc += imm;
            shouldIncrementPC = false;
            registers[0]=0;

        }

        std::cout << "Core " << coreID << " - BNE: Comparing x" 
                  << (int)rs1 << " (" << registers[rs1] << ") with x" 
                  << (int)rs2 << " (" << registers[rs2] 
                  << "), PC = " << pc << std::endl;
    }
    break;
}
case 0x03: // LW 
{
    uint32_t address = registers[rs1] + static_cast<int32_t>(imm); // Ensure correct sign extension
    if (address % 4 != 0) {
        std::cerr << "[ERROR] Unaligned memory access at: 0x" 
                  << std::hex << address << std::dec << std::endl;
        break;
    }
    registers[rd] = memory.loadWord(address);
    std::cout << "Core " << coreID << " - LW: Register x" << (int)rd 
              << " loaded with value " << registers[rd] 
              << " from memory address " << address 
              << std::endl;
              registers[0]=0;
    break;
}

case 0x23: // SW 
{
    uint32_t address = registers[rs1] + static_cast<int32_t>(imm); // Ensure correct sign extension
    if (address % 4 != 0) {
        std::cerr << "[ERROR] Unaligned memory access at: 0x" 
                  << std::hex << address << std::dec << std::endl;
        break;
    }
    memory.storeWord(address, registers[rs2]);
    std::cout << "Core " << coreID << " - SW: Stored " << registers[rs2] 
              << " at memory address " << address 
              << std::endl;
              registers[0]=0;
    break;
}

case 0x6F: // JAL (Jump and Link)
{
    registers[rd] = pc ; 
    pc += imm;  // Jump to target address
    std::cout << "Core " << coreID << " - JAL: Jumping to " << pc 
              << ", storing return address in x" << (int)rd << std::endl;
              registers[0]=0;
        shouldIncrementPC = false;  // Don't increment PC normally

    break;
}

case 0x67: // JALR (Jump and Link Register)
{
    if(funct3 == 0x0){
    uint32_t temp = pc ;
    pc = (registers[rs1] + imm) & ~1; // Jump to (rs1 + imm), ensuring LSB is 0
    registers[rd] = temp; // Store return address
    shouldIncrementPC = false;  
    std::cout << "Core " << coreID << " - JALR: Jumping to " << pc 
              << ", storing return address in x" << (int)rd << std::endl;
    }
    else {
        std::cerr << "[ERROR] Core " << coreID << " - Unknown JALR funct3: 0x" << std::hex << (int)funct3 << std::dec << std::endl;
        }
        registers[0]=0;
    break;
}
case 0x13: // I-type Instructions (Immediate ALU)
{
    if (funct3 == 0x0) { // ADDI
        std::cout<<"just for fun"<<std::dec<<imm<<std::endl;
        std::cout << "Core " << coreID << " - ADDI: Register x" << (int)rd 
                  << " = " << registers[rs1];
        registers[rd] = registers[rs1] + imm;
        std::cout << " + " << imm 
                  << " = " << registers[rd] << std::endl;
    } 
    else {
        std::cerr << "[ERROR] Core " << coreID << " - Unknown I-type funct3: 0x" 
                  << std::hex << (int)funct3 << std::dec << std::endl;
    }
    registers[0]=0;
    break;
}
    default:
        std::cerr << "[ERROR] Core " << coreID << " - Unknown instruction: 0x" 
                  << std::hex << (int)opcode << std::dec << std::endl;
                  registers[0]=0;
        break;
    }
    if (shouldIncrementPC) {
        pc += 4;
    }
}

void Core::run(int numInstructions) {
    for (int i = 0; i < numInstructions; i++) {
        std::cout<<"ITERATION                  :"<<i<<std::endl;
        uint32_t rawInst = fetch();
        Instruction inst(rawInst);
        inst.decode();
        execute(inst);
    } 
}
// uint32_t Core::getRegister(int reg) const {
//     return registers[reg]; // Return the register value
// }
// void Core::setRegister(int reg, uint32_t value) {
//     if (reg != 0) { // Prevent writing to x0 (registers[0])
//         registers[reg] = value;
//     }
// }

