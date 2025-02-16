#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include "../include/core.h"
#include "../include/memory.h"
#include <thread>


void executeCore(Core &core, int coreID,int n) {
    std::cout << "\n[Core " << coreID << "] Initial Register State:" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }

    std::cout << "\n[Core " << coreID << "] Executing Instructions...\n" << std::endl;
    
    core.run(n);  

    std::cout << "\n[Core " << coreID << "] Final Register State:" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }
}
std::unordered_map<std::string, uint32_t> opcodeTable = {
    {"add",  0b0110011},  // R-type
    {"sub",  0b0110011},
    {"addi", 0b0010011},  // I-type
    {"lw",   0b0000011},  // I-type
    {"sw",   0b0100011},  // S-type
    {"beq",  0b1100011},  // B-type
    {"j",    0b1101111}   // J-type
};

uint32_t encodeRType(const std::string& instr, int rd, int rs1, int rs2) {
    uint32_t funct3, funct7;
    const uint32_t opcode = 0b0110011; // R-type opcode in RISC-V

    if (instr == "add")  { funct3 = 0b000; funct7 = 0b0000000; }
    else if (instr == "sub") { funct3 = 0b000; funct7 = 0b0100000; }
    else return 0; // Consider handling errors differently

    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}
uint32_t encodeIType(const std::string& instr, int rd, int rs1, int imm) {
    uint32_t funct3;
    
    if (instr == "addi") funct3 = 0b000;
    else if (instr == "lw") funct3 = 0b010;
    else return 0; // Unsupported instruction

    uint32_t imm12 = imm & 0xFFF;  // Extract lower 12 bits
    if (imm < 0) imm12 |= 0xFFFFF000; // Sign-extend if negative

    return (imm12 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcodeTable[instr];
}
uint32_t encodeSType(const std::string& instr, int rs1, int rs2, int imm) {
    uint32_t funct3 = 0b010; // funct3 for `sw`
    uint32_t opcode = 0b0100011; // Opcode for `sw`
    
    uint32_t imm11_5 = imm  & 0x7F; // Extract upper 7 bits (bits 11-5)
    //std::cout<<"7-bit Immediate : "<<imm11_5<<std::endl;
    uint32_t imm4_0 = imm & 0x1F;         // Extract lower 5 bits (bits 4-0)
    //std::cout<<"4-bit Immediate : "<<imm4_0<<std::endl;
    return (imm11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm4_0 << 7) | opcode;
}
// // Function to encode B-type instructions (bne)
// uint32_t encodeBType(const std::string& instr, int rs1, int rs2, int imm) {
//     uint32_t funct3 = 0b001; // beq funct3
//     uint32_t imm12 = (imm >> 12) & 0x1;
//     uint32_t imm10_5 = (imm >> 5) & 0x3F;
//     uint32_t imm4_1 = (imm >> 1) & 0xF;
//     uint32_t imm11 = (imm >> 11) & 0x1;

//     return (imm12 << 31) | (imm11 << 7) | (imm10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm4_1 << 8) | opcodeTable[instr];
// }

// uint32_t encodeBType(const std::string& instr, int rs1, int rs2, int imm) {
//     uint32_t funct3;

//     if (instr == "beq") funct3 = 0b000;
//     else if (instr == "bne") funct3 = 0b001;
//     else return 0; // Unsupported branch instruction

//     uint32_t opcode = 0b1100011; // Fixed opcode for all branch instructions

//     // Extract the correct bit fields
//     uint32_t imm12 = (imm >> 12) & 0x1;  // Bit 12 (MSB)
//     uint32_t imm10_5 = (imm >> 5) & 0x3F; // Bits 10-5
//     uint32_t imm4_1 = (imm >> 1) & 0xF;   // Bits 4-1
//     uint32_t imm11 = (imm >> 11) & 0x1;   // Bit 11

//     // Construct the 32-bit instruction
//     return (imm12 << 31) | (imm11 << 7) | (imm10_5 << 25) | (rs2 << 20) |
//            (rs1 << 15) | (funct3 << 12) | (imm4_1 << 8) | (opcode);
// }
uint32_t encodeBType(const std::string& instr, int rs1, int rs2, int imm) {
    uint32_t funct3;

    if (instr == "beq") funct3 = 0b000;
    else if (instr == "bne") funct3 = 0b001;
    else return 0; // Unsupported branch instruction

    uint32_t opcode = 0b1100011; // Fixed opcode for all branch instructions

    // Extract the correct bit fields
    uint32_t imm12 = (imm >> 12) & 0x1;  // Bit 12 (MSB)
    uint32_t imm10_5 = (imm >> 5) & 0x3F; // Bits 10-5
    uint32_t imm4_1 = (imm >> 1) & 0xF;   // Bits 4-1
    uint32_t imm11 = (imm >> 11) & 0x1;   // Bit 11

    // Construct the 32-bit instruction in the correct order
    return (imm12 << 31) | (imm11 << 7) | (imm10_5 << 25) | (rs2 << 20) |
           (rs1 << 15) | (funct3 << 12) | (imm4_1 << 8) | (opcode);
}

// Function to encode J-type instructions (jump)
uint32_t encodeJType(const std::string& instr, int rd, int imm) {
    uint32_t imm20 = (imm >> 20) & 0x1;
    uint32_t imm10_1 = (imm >> 1) & 0x3FF;
    uint32_t imm11 = (imm >> 11) & 0x1;
    uint32_t imm19_12 = (imm >> 12) & 0xFF;

    return (imm20 << 31) | (imm19_12 << 12) | (imm11 << 20) | (imm10_1 << 21) | (rd << 7) | opcodeTable[instr];
}

// Function to parse instruction and call encoding function
uint32_t assembleInstruction(const std::string& line) {
    std::istringstream iss(line);
    std::string instr;
    int rd, rs1, rs2, imm;
    char comma;
    iss >> instr;
    if (instr == "add" || instr == "sub") {
        std::string rdStr, rs1Str, rs2Str;
        
        // Read register names as strings
        iss >> rdStr;
        iss.ignore();  // Ignore comma
        iss >> rs1Str;
        iss.ignore();  // Ignore comma
        iss >> rs2Str;
    
        // Convert register strings to integer IDs
        rd = std::stoi(rdStr.substr(1));  // Remove 'x' and convert
        rs1 = std::stoi(rs1Str.substr(1));
        rs2 = std::stoi(rs2Str.substr(1));
        //std::cout<<"rd "<<rd<<" rs1 "<<rs1<<"rs2 "<<rs2<<std::endl;
        return encodeRType(instr, rd, rs1, rs2);
    }
    
    if (instr == "addi") {
        std::string rdStr, rs1Str;
        int imm;
    
        // Read the destination register, source register, and immediate value
        getline(iss >> std::ws, rdStr, ',');  // Read rd (e.g., "x5"), trim spaces
        getline(iss >> std::ws, rs1Str, ','); // Read rs1 (e.g., "x2"), trim spaces
        iss >> imm;  // Read immediate (e.g., 1000)
    
        // Convert register strings to integer IDs
        rd = std::stoi(rdStr.substr(1));  // Remove 'x' and convert "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // Remove 'x' and convert "x2" -> 2
       // std::cout<<"rd "<<rd<< "rs1 "<<rs1<<" imm"<<imm<<std::endl;
        return encodeIType(instr, rd, rs1, imm);
    }
    
    if (instr == "mv") {
        std::string rdStr, rs1Str;
    
        // Read destination (rd) and source (rs1) registers
        getline(iss >> std::ws, rdStr, ',');
        getline(iss >> std::ws, rs1Str, ',');
    
        // Convert register names to integers
        rd = std::stoi(rdStr.substr(1));   // "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // "x8" -> 8
    
        // Call encodeIType with immediate = 0 (addi x5, x8, 0)
        return encodeIType("addi", rd, rs1, 0);
    }
    else if (instr == "lw") {
        std::string rdStr, rs1Str, immStr;
        char paren1, paren2;
    
        // Read rd, immediate, and rs1 in the format "lw x5, 0(x2)"
        getline(iss >> std::ws, rdStr, ',');   // Read "x5"
        getline(iss >> std::ws, immStr, '(');  // Read "0"
        getline(iss >> std::ws, rs1Str, ')');  // Read "x2"
    
        // Convert to integers
        rd = std::stoi(rdStr.substr(1));   // "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // "x2" -> 2
        imm = std::stoi(immStr);           // "0"  -> 0
    
        std::cout<<"rd :: "<<rd<<" rs1 :: "<<rs1<<" imm :"<<imm<<std::endl;
        return encodeIType("lw", rd, rs1, imm);
    }
    else if (instr == "sw") {
        std::string rs2Str, rs1Str, immStr;
        char paren1, paren2;
    
        // Read rs2, imm, and rs1 in the format "sw x5, 16(x2)"
        getline(iss >> std::ws, rs1Str, ',');   // Read "x5"
        getline(iss >> std::ws, immStr, '(');  // Read "16"
        getline(iss >> std::ws, rs2Str, ')');  // Read "x2"
    
        // Convert to integers
        rs2 = std::stoi(rs2Str.substr(1));   // "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1));   // "x2" -> 2w
        imm = std::stoi(immStr);             // "16" -> 16
    
        //std::cout<<"Immediate value : "<<imm<<std::endl;

        return encodeSType("sw", rs1, rs2, imm);
    }    
    
    else if (instr == "bne") {
        std::string rs1Str, rs2Str, immStr;

        // Read rs1, rs2, and offset (e.g., "bne x5, x6, 16")
        getline(iss >> std::ws, rs1Str, ',');
        getline(iss >> std::ws, rs2Str, ',');
        iss >> imm;

        // Convert to integers
        rs1 = std::stoi(rs1Str.substr(1)); // "x5" -> 5
        rs2 = std::stoi(rs2Str.substr(1)); // "x6" -> 6

        return encodeBType("bne", rs1, rs2, imm);
    }

    else if (instr == "j") {
        iss >> imm;
        return encodeJType(instr, 0, imm); // Jump target address
    }

    return 0; // Unsupported instruction
}
std::vector<uint32_t> loadProgramFromFile(const std::string &filename) {
    std::vector<uint32_t> program;
    std::ifstream file(filename);

    // Check if file opened successfully
    if (!file.is_open()) {
        std::cout << "Error: Unable to open file " << filename << std::endl;
        return program;
    }
    else
    {
        std::cout<<"File opened successfully"<<std::endl;
    }

    std::string line;
    bool inTextSection = false;

    while (std::getline(file, line)) {
        // Trim leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue; // Skip empty lines

        // Handle `.text` section
        if (line.find(".text") != std::string::npos) {
            inTextSection = true;
            continue;
        }

        if (inTextSection) {
            // Ignore comments (assuming '#' or '//')
            size_t commentPos = line.find('#');
            if (commentPos == std::string::npos) commentPos = line.find("//");
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }

            // Trim again after removing comments
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty()) continue; // Skip if only a comment was present

            uint32_t hexInstr = assembleInstruction(line);
            if (hexInstr == 0) {
                std::cout << "Warning: Unrecognized instruction in line -> " << line << std::endl;
            } else {
                program.push_back(hexInstr);
            }
        }
    }

    return program;
}

int main() {
    Memory memory;
    std::vector<uint32_t> program = loadProgramFromFile("program.s");

    for (int core = 0; core < 4; core++) {
        for (size_t i = 0; i < program.size(); i++) {
            memory.storeWord(core * 1024 + i * 4, program[i]);
        }
    }

    Core cores[4] = { Core(memory, 0), Core(memory, 1), Core(memory, 2), Core(memory, 3) };

    // for (int i = 0; i < 4; i++) {
    //     cores[i].registers[2] = 2; // x1 = 10
    //     cores[i].registers[3] = 33;//at adreess 0-2130355 is stored in above inst loading it-FOR LOAD 
    //     cores[i].registers[5] = 55;//STORED RANDOM ADDRESS IN X6
    //     cores[i].registers[6] = 8;
    //     cores[i].registers[7] = 22;
    //     cores[i].registers[8] = 11;
    // }
    for (int i = 0; i < 4; i++) {
        executeCore(cores[i], i, program.size());
    }

    return 0;
}   
