#include "../include/core.h"
#include <iostream>
#include <unordered_map>
Core::Core(Memory &memRef, std::vector<std::tuple<std::string, int, int, int, int,std::string>> &prog ,int coreId)
    : memory(memRef), program(prog), pc(0), coreID(coreId), baseAddress(coreId * 1024) ,isActive(true){
    std::fill(std::begin(registers), std::end(registers), 0);
    registers[32] = coreId; //  core ID to register x32
}
/*std::tuple<std::string, int, int, int, int> Core::fetch() {
    if (pc < program.size()) {
        return program[pc];  // No pc++ here
    }
    return {"",0,0,0,0}; 
}
bool shouldIncrementPC = true; 
void Core::execute(const Instruction &instruction) {
    uint8_t opcode = instruction.getOpcode();
    uint8_t funct3 = instruction.getFunct3();
    uint8_t funct7 = instruction.getFunct7();
    uint8_t rd = instruction.getRd();
    uint8_t rs1 = instruction.getRs1();
    uint8_t rs2 = instruction.getRs2();
    int32_t imm = instruction.getImm();
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
            pc += (imm/4);
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
    registers[rd] = memory.loadWord(address,coreID );
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
    memory.storeWord(address, registers[rs2],coreID);
    std::cout << "Core " << coreID << " - SW: Stored " << registers[rs2] 
              << " at memory address " << address 
              << std::endl;
              registers[0]=0;
    break;
}


case 0x6F: // JAL (Jump and Link)
{
    registers[rd] = pc+1 ; // Storing return address
    pc += (imm/4);  // Jump to target address
    std::cout << "Core " << coreID << " - JAL: Jumping to " << pc 
              << ", storing return address in x" << (int)rd << std::endl;
              registers[0]=0;
              shouldIncrementPC = false;

    break;
}

/*case 0x67: // JALR (Jump and Link Register)
{
    if (funct3 == 0x0) {
        uint32_t temp = pc + 4; // Store return address
        pc = (registers[rs1] + imm) & ~1; // Compute target address, ensuring LSB is 0
        registers[rd] = temp; // Write return address to rd
        shouldIncrementPC = false;

        std::cout << "Core " << coreID << " - JALR: Jumping to " << pc
                  << ", storing return address in x" << (int)rd << std::endl;
    } else {
        std::cerr << "[ERROR] Core " << coreID << " - Unknown JALR funct3: 0x"
                  << std::hex << (int)funct3 << std::dec << std::endl;
    }

    registers[0] = 0; // Ensure x0 is always zero
    break;
}*/
// case 0x67: // JALR (Jump and Link Register)
// {
//     if(funct3 == 0x0){
//     uint32_t temp = pc+4 ;
//     pc = (registers[rs1] + imm) & ~1; // Jump to (rs1 + imm), ensuring LSB is 0
//     registers[rd] = temp; // Store return address
//     std::cout << "Core " << coreID << " - JALR: Jumping to " << pc 
//               << ", storing return address in x" << (int)rd << std::endl;
//     }
//     else {
//         std::cerr << "[ERROR] Core " << coreID << " - Unknown JALR funct3: 0x" << std::hex << (int)funct3 << std::dec << std::endl;
//         }
//         registers[0]=0;
//     break;
// }
/*case 0x13: // I-type Instructions (Immediate ALU)
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
        pc += 1;
    }
}

void Core::run(int numInstructions) {
    for (int i = 0; i < numInstructions; i++) {
        std::cout << "ITERATION: " << i << std::endl;
        uint32_t rawInst = fetch();
        if (rawInst == 0) break; 
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


// #include "../include/core.h"
// #include <iostream>
// #include <algorithm>

// Core::Core(Memory &memRef, int coreId)
//     : memory(memRef), pc(0), coreID(coreId), baseAddress(coreId * 1024) {
//     std::fill(std::begin(registers), std::end(registers), 0);
//     registers[32] = coreId; //  core ID in register x32
// }

// uint32_t Core::fetch() {
//     uint32_t instruction = memory.loadWord(baseAddress + pc);
//     if (instruction == 0) {
//         std::cerr << "[ERROR] Core " << coreID << " - Invalid instruction (0x0) at address: "
//                   << std::hex << baseAddress + pc << std::dec << std::endl;
//     }
//     std::cout << "Core " << coreID << " - Fetched Instruction: 0x"
//               << std::hex << instruction << std::dec
//               << " at PC = " << pc << " (Address: " << baseAddress + pc << ")" 
//               << std::endl;
//     pc += 4;
//     return instruction;
// }

// void Core::execute(const Instruction &instruction) {
//     uint8_t opcode = instruction.getOpcode();
//     uint8_t funct3 = instruction.getFunct3();
//     uint8_t funct7 = instruction.getFunct7();
//     uint8_t rd = instruction.getRd();
//     uint8_t rs1 = instruction.getRs1();
//     uint8_t rs2 = instruction.getRs2();

//     int32_t imm = instruction.getImm(); // Sign-extended immediate


//     switch (opcode) {
//     case 0x33: // R-type instructions
//         if (funct3 == 0x0) {
//             if (funct7 == 0x00) { // ADD
//                 registers[rd] = registers[rs1] + registers[rs2];
//                 std::cout << "Core " << coreID << " - ADD: x" 
//                           << (int)rd << " = " << registers[rs1] 
//                           << " + " << registers[rs2] << " = " << registers[rd] 
//                           << std::endl;
//                           registers[0]=0;
//             } 
//             else if (funct7 == 0x20) { // SUB
//                 registers[rd] = registers[rs1] - registers[rs2];

//                 registers[0]=0;
//                 std::cout << "Core " << coreID << " - SUB: Register x" 

              
//                           << (int)rd << " = " << registers[rd] << std::endl;
//             } 
//         }
//         break;


//         case 0x63: // B-type Instructions (BNE)
//         if (funct3 == 0x0) { // BNE
//             if (registers[rs1] != registers[rs2]) {
//                 pc = pc - 4 + imm;  // Adjust for pre-incremented PC
//             }
//             std::cout << "Core " << coreID << " - BNE: Comparing x" 
//                       << (int)rs1 << " (" << registers[rs1] << ") with x" 
//                       << (int)rs2 << " (" << registers[rs2] 
//                       << "), PC = " << pc << std::endl;

             
//         }
//            registers[0]=0;
         
//         break;


//     case 0x03: // LW
//         {
//             uint32_t address = registers[rs1] + imm;
//             if (address % 4 != 0) {
//                 std::cerr << "[ERROR] Unaligned memory access at: 0x" 
//                           << std::hex << address << std::dec << std::endl;
//                 break;
//             }
//             registers[rd] = memory.loadWord(address);
//             std::cout << "Core " << coreID << " - LW: x" << (int)rd 
//                       << " loaded with value " << registers[rd] 
//                       << " from memory address 0x" << std::hex << address 
//                       << std::dec << std::endl;
//         }
//           registers[0]=0;
//     break;
        

// case 0x6F: // JAL (Jump and Link)
// {
//     registers[rd] = pc ; 
//     pc-=4; // Store return address which is address of next Instruction
//     pc += imm;  // Jump to target address
//     std::cout << "Core " << coreID << " - JAL: Jumping to " << pc 
//               << ", storing return address in x" << (int)rd << std::endl;
//               registers[0]=0;
//     break;
// }
// =======

//     case 0x23: // SW
//         {
//             uint32_t address = registers[rs1] + imm;
//             if (address % 4 != 0) {
//                 std::cerr << "[ERROR] Unaligned memory access at: 0x" 
//                           << std::hex << address << std::dec << std::endl;
//                 break;
//             }
//             memory.storeWord(address, registers[rs2]);
//             std::cout << "Core " << coreID << " - SW: Stored " << registers[rs2] 
//                       << " at memory address 0x" << std::hex << address 
//                       << std::dec << std::endl;
   
//         }
//              registers[0]=0;

//         break;

// case 0x67: // JALR (Jump and Link Register)
// {
//     if(funct3 == 0x0){
//     uint32_t temp = pc ;
//     pc-=4;
//     pc = (registers[rs1] + imm) & ~1; // Jump to (rs1 + imm), ensuring LSB is 0
//     registers[rd] = temp; // Store return address
//     std::cout << "Core " << coreID << " - JALR: Jumping to " << pc 
//               << ", storing return address in x" << (int)rd << std::endl;
//     }
//     else {
//         std::cerr << "[ERROR] Core " << coreID << " - Unknown JALR funct3: 0x" << std::hex << (int)funct3 << std::dec << std::endl;
//         }
//         registers[0]=0;
//     break;
// }
// case 0x13: // I-type Instructions (Immediate ALU)
// {
//     if (funct3 == 0x0) { // ADDI
//         std::cout<<"just for fun"<<std::dec<<imm<<std::endl;
//         std::cout << "Core " << coreID << " - ADDI: Register x" << (int)rd 
//                   << " = " << registers[rs1];
//         registers[rd] = registers[rs1] + imm;
//         std::cout << " + " << imm 
//                   << " = " << registers[rd] << std::endl;
//     } 
//     else {
//         std::cerr << "[ERROR] Core " << coreID << " - Unknown I-type funct3: 0x" 
//                   << std::hex << (int)funct3 << std::dec << std::endl;
//     }
//     registers[0]=0;
//     break;
// }
//     default:
//         std::cerr << "[ERROR] Core " << coreID << " - Unknown instruction: 0x" 
//                   << std::hex << (int)opcode << std::dec << std::endl;
//                   registers[0]=0;
//         break;
//     }
//      registers[0] = 0;
// }

// void Core::run(int numInstructions) {
//     for (int i = 0; i < numInstructions; i++) {
//         std::cout<<"ITERATION                  :"<<i<<std::endl;
//         uint32_t rawInst = fetch();
//         Instruction inst(rawInst);
//         inst.decode();
//         execute(inst);
//     }
    
// }
// // uint32_t Core::getRegister(int reg) const {
// //     return registers[reg]; // Return the register value
// // }
// // void Core::setRegister(int reg, uint32_t value) {
// //     if (reg != 0) { // Prevent writing to x0 (registers[0])
// //         registers[reg] = value;
// //     }
// // }

*/
void Core::execute(const std::string& instruction, int rd, int rs1, int rs2, int imm, const std::string &label, std::unordered_map<std::string, int> &labelMap) {
   // if (!isActive) return; // Stop execution if core is inactive
   if (rd == 32) {
    std::cout<<"Cannot write to X32,it is read -only & contains core ID"<<std::endl;
    pc += 1;

    return; // Prevent modifying x32 (always core ID)
   }

    bool shouldIncrementPC = true;
    if (instruction == "add"||instruction == "ADD") {
       // if (isActive) {
       if(rd==0){
        std::cout<<" X0 is hardwired to 0 & contains x0=0"<<std::endl;
        pc += 1;
        return;
       }
        registers[rd] = registers[rs1] + registers[rs2];
        std::cout << "Core " << coreID << " - ADD: x" << rd << " = "
                  << registers[rs1] << " + " << registers[rs2] 
                  << " = " << registers[rd] << std::endl;
    } 
//}
    else if (instruction == "sub"||instruction == "SUB") {
        //if (isActive) {
            if(rd==0){
                std::cout<<" X0 is hardwired to 0 & contains x0=0"<<std::endl;
                pc += 1;

                return;
               }
        registers[rd] = registers[rs1] - registers[rs2];
        std::cout << "Core " << coreID << " - SUB: x" << rd << " = "
                  << registers[rs1] << " - " << registers[rs2] 
                  << " = " << registers[rd] << std::endl;}
   // }
    else if (instruction == "lw"||instruction == "LW") {
        
        if(rd==0){
            std::cout<<" X0 is hardwired to 0 & contains x0=0"<<std::endl;
            pc += 1;

            return;
           }
        uint32_t address = (rs1 == -1) ? imm : (registers[rs1] + imm);
        //uint32_t address = registers[rs1] + imm;
        auto temp=registers[rd];
        registers[rd] = memory.loadWord(address,coreID,isActive);
       if (isActive) {
        std::cout << "Core " << coreID << " - LW: x" << rd << " loaded with "
                  << registers[rd] << " from address " << address << std::endl;
    }
    else {
        std::cout<<'x'<<rd<<" contains its previously loaded value(if not loaded contains 0)"<<std::endl;
        registers[rd]=temp;//if invalid memory set rd to its previous value only
        isActive=true;
    }

}
else if (instruction == "la"||instruction == "LA") {
        
    if(rd==0){
        std::cout<<" X0 is hardwired to 0 & contains x0=0"<<std::endl;
        pc += 1;
        return;
       }
   
    registers[rd] = coreID*1024+imm;
   if (isActive) {
    std::cout << "Core " << coreID << " - La: x" << rd << " loaded with "
              << registers[rd] << " from address containing val " <<memory.loadWord(coreID*1024+imm,coreID,isActive) << std::endl;
}
}
    else if (instruction == "sw"||instruction == "SW") {
       
        uint32_t address = registers[rs1] + imm;
        memory.storeWord(address, registers[rs2],coreID,isActive);
        if (isActive) {
        std::cout << "Core " << coreID << " - SW,: Stored " << registers[rs2] 
                  << " at address " << address << std::endl;
    }
    else isActive=true;

}
//     else if (instruction == "bne"||instruction == "BNE") {
//         //if (isActive) {
//         if (registers[rs1] != registers[rs2]) {
//             pc += (imm/4);
//         }
//         shouldIncrementPC=false;
//         std::cout << "Core " << coreID << " - BEQ: Comparing x" << rs1 
//                   << " and x" << rs2 << ", PC = " << pc << std::endl;
//    // }
//     }
//     else if (instruction == "beq"||instruction == "BEQ") {
//        // if (isActive) {
//         if (registers[rs1] == registers[rs2]) {
//             pc += (imm/4);
//         }
//         shouldIncrementPC=false;
//         std::cout << "Core " << coreID << " - BEQ: Comparing x" << rs1 
//                   << " and x" << rs2 << ", PC = " << pc << std::endl;
//  //   }
// }
        else if (instruction == "bne"||instruction == "BNE") {
        if (registers[rs1] != registers[rs2]) {
            std::cout << "Core " << coreID << " - BNE: x" << rs1 << " != x" << rs2 
                      << " (Jumping to " << label << ")" << std::endl;
            if (!label.empty()) {
                pc = labelMap[label];  // Jump to label index
            } else {
                pc += imm;  // Fallback if label missing
            }
            shouldIncrementPC = false;
        }
        else{
            std::cout<<"In BNE Instruction if condition not taken"<<std::endl;
        }
    }
    else if (instruction == "beq"||instruction == "BEQ") {
        if (registers[rs1] == registers[rs2]) {
            std::cout << "Core " << coreID << " - BEQ: x" << rs1 << " == x" << rs2 
                      << " (Jumping to " << label << ")" << std::endl;
            if (!label.empty()) {
                pc = labelMap[label];  // Jump to label index
            } else {
                pc += imm;  // Fallback if label missing
                
            }
            shouldIncrementPC = false;
        }
        else{
            std::cout<<"In BEQ Instruction if condition is not taken"<<std::endl;
        }
    }
    else if(instruction == "addi"||instruction == "ADDI"){
        //if (isActive) {
            if(rd==0){
                std::cout<<" X0 is hardwired to 0 & contains x0=0"<<std::endl;
                return;
               }
        std::cout << "Core " << coreID << " - ADDI: Register x" << (int)rd 
        << " = " << registers[rs1];
registers[rd] = registers[rs1] + imm;
std::cout << " + " << imm 
        << " = " << registers[rd] << std::endl;
    }
    else if(instruction == "mv"||instruction == "MV"){
        if(rd==0){
            std::cout<<" X0 is hardwired to 0 & contains x0=0"<<std::endl;
            return;
           }
        std::cout << "Core " << coreID << " - mv: Register x" << (int)rd 
        << " = " << registers[rs1];
registers[rd] = registers[rs1] + imm;
std::cout << " + " << imm 
        << " = " << registers[rd] << std::endl;
    }  
    else if (instruction == "jal"||instruction == "JAL") {
        std::cout << "Core " << coreID << " - JAL: Saving return address in x" << rd 
                  << ", jumping to " << label << std::endl;
        registers[rd] = pc + 1;  // Save return address
        if (!label.empty()) {
            pc = labelMap[label];  // Jump to label
        } else {
            pc += imm;  // Fallback
        }
        shouldIncrementPC = false;
    }
    else if (instruction == "j"||instruction == "J") {
        std::cout << "Core " << coreID << " - JUMP: Jumping to " << label << std::endl;
        if (!label.empty()) {
            pc = labelMap[label];  // Jump to label
        } else {
            pc += imm;  // Fallback
        }
        shouldIncrementPC = false;
    }
    
registers[0] = 0; // Ensure x0 is always zero
registers[32] = coreID; // Ensure x32 is always core ID
    if (shouldIncrementPC) {
        pc += 1;
    }
}
void Core::printRegisters() const {
    std::cout << "\nRegister Dump for Core " << coreID << ":\n";
    for (int i = 0; i < 33; i++) {
        std::cout << "x" << i << " = " << registers[i] << std::endl;
    }
} 


