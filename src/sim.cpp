#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include "../include/core.h"
#include "../include/memory.h"
#include <thread>
// void executeCore(Core &core, int coreID, int n, const std::vector<std::tuple<std::string, int, int, int, int,std::string>> &instructions)
// {
//     std::cout << "\n[Core " << coreID << "] Initial Register State:" << std::endl;
//     for (int i = 0; i < 32; i++)
//     {
//         std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
//     }

//     std::cout << "\n[Core " << coreID << "] Executing Instructions...\n"
//               << std::endl;

//     for (const auto &inst : instructions)
//     {
//         core.execute(std::get<0>(inst), std::get<1>(inst), std::get<2>(inst), std::get<3>(inst), std::get<4>(inst));
//     };

//     std::cout << "\n[Core " << coreID << "] Final Register State:" << std::endl;
//     for (int i = 0; i < 32; i++)
//     {
//         std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
//     }
// }
std::unordered_map<std::string, int> labelMap;
void executeCore(Core &core, int coreID, int n, const std::vector<std::tuple<std::string, int, int, int, int, std::string>> &instructions) {
    std::cout << "\n[Core " << coreID << "] Initial Register State:\n";
    for (int i = 0; i < 32; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }

    std::cout << "\n[Core " << coreID << "] Executing Instructions...\n";
    while (core.pc < instructions.size()) {
        const auto &inst = instructions[core.pc];
        core.execute(std::get<0>(inst), std::get<1>(inst), std::get<2>(inst), std::get<3>(inst), std::get<4>(inst),std::get<5>(inst),labelMap);
    }

    std::cout << "\n[Core " << coreID << "] Final Register State:\n";
    for (int i = 0; i < 32; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }
}

std::unordered_map<std::string, uint32_t> opcodeTable = {
    {"add", 0b0110011}, // R-type
    {"sub", 0b0110011},
    {"addi", 0b0010011}, // I-type
    {"lw", 0b0000011},   // I-type
    {"sw", 0b0100011},   // S-type
    {"beq", 0b1100011},  // B-type
    {"j", 0b1101111}     // J-type
};
uint32_t encodeRType(const std::string &instr, int rd, int rs1, int rs2)
{
    uint32_t funct3, funct7;
    const uint32_t opcode = 0b0110011; // R-type opcode in RISC-V

    if (instr == "add")
    {
        funct3 = 0b000;
        funct7 = 0b0000000;
    }
    else if (instr == "sub")
    {
        funct3 = 0b000;
        funct7 = 0b0100000;
    }
    else
        return 0; // Consider handling errors differently

    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}
// Function to encode I-type instructions (addi, lw)
uint32_t encodeIType(const std::string &instr, int rd, int rs1, int imm)
{
    uint32_t funct3;

    if (instr == "addi")
        funct3 = 0b000;
    else if (instr == "lw")
        funct3 = 0b010;
    else
        return 0; // Unsupported

    return ((imm & 0xFFF) << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcodeTable[instr];
}
// Function to encode S-type instructions (sw)
uint32_t encodeSType(const std::string &instr, int rs1, int rs2, int imm)
{
    uint32_t funct3 = 0b010; // sw funct3
    uint32_t imm11_5 = (imm >> 5) & 0x7F;
    uint32_t imm4_0 = imm & 0x1F;

    return (imm11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm4_0 << 7) | opcodeTable[instr];
}
// Function to encode B-type instructions (beq)
uint32_t encodeBType(const std::string &instr, int rs1, int rs2, int imm)
{
    uint32_t funct3 = 0b000; // beq funct3
    uint32_t imm12 = (imm >> 12) & 0x1;
    uint32_t imm10_5 = (imm >> 5) & 0x3F;
    uint32_t imm4_1 = (imm >> 1) & 0xF;
    uint32_t imm11 = (imm >> 11) & 0x1;
    return (imm12 << 31) | (imm11 << 7) | (imm10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm4_1 << 8) | opcodeTable[instr];
}

// Function to encode J-type instructions (jump)
uint32_t encodeJType(const std::string &instr, int rd, int imm)
{
    uint32_t imm20 = (imm >> 20) & 0x1;
    uint32_t imm10_1 = (imm >> 1) & 0x3FF;
    uint32_t imm11 = (imm >> 11) & 0x1;
    uint32_t imm19_12 = (imm >> 12) & 0xFF;

    return (imm20 << 31) | (imm19_12 << 12) | (imm11 << 20) | (imm10_1 << 21) | (rd << 7) | opcodeTable[instr];
}
/*
// Function to parse instruction and call encoding function
std::tuple<std::string, int, int, int, int> assembleInstruction(const std::string& line) {
    std::istringstream iss(line);
    std::string instr;
    int rd = 0, rs1 = 0, rs2 = 0, imm = 0;
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

        return {instr, rd, rs1, rs2,imm};
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

        return {instr, rd, rs1, rs2,imm};
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
        return {instr, rd, rs1, rs2,imm};
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
        return {instr, rd, rs1, rs2,imm};
    }

    else if (instr == "sw") {
        std::string rs2Str, rs1Str, immStr;
        char paren1, paren2;

        // Read rs2, imm, and rs1 in the format "sw x5, 16(x2)"
        getline(iss >> std::ws, rs2Str, ',');   // Read "x5"
        getline(iss >> std::ws, immStr, '(');  // Read "16"
        getline(iss >> std::ws, rs1Str, ')');  // Read "x2"

        // Convert to integers
        rs2 = std::stoi(rs2Str.substr(1));   // "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1));   // "x2" -> 2
        imm = std::stoi(immStr);             // "16" -> 16

        return {instr, rd, rs1, rs2,imm};
    }

    else if (instr == "beq") {
        iss >> rs1 >> comma >> rs2 >> comma >> imm;
        return {instr, rd, rs1, rs2,imm};
    }

    else if (instr == "j") {
        iss >> imm;
        return {instr, rd, rs1, rs2,imm};// Jump target address
    }
    return {"",0,0,0,0}; // Unsupported instruction
}
std::vector<std::tuple<std::string, int, int, int, int>> loadProgramFromFile(const std::string &filename) {
    std::vector<std::tuple<std::string, int, int, int, int>> program;
    std::ifstream file(filename);
    std::unordered_map<std::string, int> labelMap;


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
    int currentAddress = 0;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#') continue;

        if (line.back() == ':') {
            labelMap[line.substr(0, line.size() - 1)] = currentAddress;
        } else {
            currentAddress++;
        }
    }

    file.clear();
    file.seekg(0);
    currentAddress = 0;


    while (std::getline(file, line)) {
        // Trim leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#' || line.back() == ':') continue; // Skip empty lines

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

            auto Instr = assembleInstruction(line);
            if (std::get<0>(Instr).empty()) {
                std::cout << "Warning: Unrecognized instruction -> " << line << std::endl;
            } else {
                program.push_back(Instr);
            }
        }
    }

    return program;
}
*/
std::tuple<std::string, int, int, int, int,std::string> assembleInstruction(const std::string &line)
{
    std::istringstream iss(line);
    std::string instr;
    int rd = 0, rs1 = 0, rs2 = 0, imm = 0;
    std::string label;
    char comma;
    iss >> instr;
    if (instr == "add" || instr == "sub")
    {
        std::string rdStr, rs1Str, rs2Str;
        // Read register names as strings
        iss >> rdStr;
        iss.ignore(); // Ignore comma
        iss >> rs1Str;
        iss.ignore(); // Ignore comma
        iss >> rs2Str;

        // Convert register strings to integer IDs
        rd = std::stoi(rdStr.substr(1)); // Remove 'x' and convert
        rs1 = std::stoi(rs1Str.substr(1));
        rs2 = std::stoi(rs2Str.substr(1));

        return {instr, rd, rs1, rs2, imm,""};
    }

    if (instr == "addi")
    {
        std::string rdStr, rs1Str;
        int imm;

        // Read the destination register, source register, and immediate value
        getline(iss >> std::ws, rdStr, ',');  // Read rd (e.g., "x5"), trim spaces
        getline(iss >> std::ws, rs1Str, ','); // Read rs1 (e.g., "x2"), trim spaces
        iss >> imm;                           // Read immediate (e.g., 1000)

        // Convert register strings to integer IDs
        rd = std::stoi(rdStr.substr(1));   // Remove 'x' and convert "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // Remove 'x' and convert "x2" -> 2

        return {instr, rd, rs1, rs2, imm,""};
    }

    if (instr == "mv")
    {
        std::string rdStr, rs1Str;

        // Read destination (rd) and source (rs1) registers
        getline(iss >> std::ws, rdStr, ',');
        getline(iss >> std::ws, rs1Str, ',');

        // Convert register names to integers
        rd = std::stoi(rdStr.substr(1));   // "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // "x8" -> 8

        // Call encodeIType with immediate = 0 (addi x5, x8, 0)
        return {instr, rd, rs1, rs2, imm,""};
    }
    else if (instr == "lw")
    {
        std::string rdStr, rs1Str, immStr;
        char paren1, paren2;
        // Read rd, immediate, and rs1 in the format "lw x5, 0(x2)"
        getline(iss >> std::ws, rdStr, ',');  // Read "x5"
        getline(iss >> std::ws, immStr, '('); // Read "0"
        getline(iss >> std::ws, rs1Str, ')'); // Read "x2"
        // Convert to integers
        rd = std::stoi(rdStr.substr(1));   // "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // "x2" -> 2
        imm = std::stoi(immStr);           // "0"  -> 0
        return {instr, rd, rs1, rs2, imm,""};
    }

    else if (instr == "sw")
    {
        std::string rs2Str, rs1Str, immStr;
        char paren1, paren2;

        // Read rs2, imm, and rs1 in the format "sw x5, 16(x2)"
        getline(iss >> std::ws, rs2Str, ','); // Read "x5"
        getline(iss >> std::ws, immStr, '('); // Read "16"
        getline(iss >> std::ws, rs1Str, ')'); // Read "x2"

        // Convert to integers
        rs2 = std::stoi(rs2Str.substr(1)); // "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // "x2" -> 2
        imm = std::stoi(immStr);           // "16" -> 16

        return {instr, rd, rs1, rs2, imm,""};
    }
    else if (instr == "bne" || instr == "beq") {
        std::string rs1Str, rs2Str, labelStr;
        getline(iss >> std::ws, rs1Str, ',');
        getline(iss >> std::ws, rs2Str, ',');
        iss >> labelStr;

        rs1 = std::stoi(rs1Str.substr(1));
        rs2 = std::stoi(rs2Str.substr(1));
        label = labelStr; // Store label name to resolve later
        return {instr, rd, rs1, rs2, imm, label};
    }
    else if (instr == "j" ) {
        std::string labelStr;
        iss >> labelStr;
        label = labelStr; // Store label name to resolve later
        return {instr, rd, rs1, rs2, imm, label};
    }

    else if (instr == "jal") {
        std::string rdStr, labelStr;
        
        if (!(iss >> rdStr >> labelStr)) {  // Extract rd and label
            std::cerr << "Error: Invalid JAL instruction format!" << std::endl;
            return {};  // Return an empty instruction structure
        }
    
        // Extract register number from "x7"
        if (rdStr[0] == 'x') {
            rdStr = rdStr.substr(1);  // Remove 'x' prefix
        }
    
        int rd = std::stoi(rdStr);  // Convert to integer
        label = labelStr;  // Assign label
    
        return {instr, rd, 0, 0, 0, label};  // JAL does not use rs1, rs2, or immediate
    }
    

    
    else {
        std::cout<<"None"<<std::endl;
        return {instr, rd, rs1, rs2, imm, ""};
    }

    
}

std::vector<std::tuple<std::string, int, int, int, int, std::string>> 
loadProgramFromFile(const std::string &filename) {
    std::vector<std::tuple<std::string, int, int, int, int, std::string>> program;
    std::ifstream file(filename);

    // Check if file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return program;
    } else {
        std::cout << "File opened successfully" << std::endl;
    }

    std::string line;
    bool inTextSection = false;
    int instructionIndex = 0;

    // 🔹 First Pass: Identify Labels
    while (std::getline(file, line)) {
        // Trim leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue; // Skip empty lines

        if (line.find(".text") != std::string::npos) {
            inTextSection = true;
            continue;
        }

        if (inTextSection) {
            // 🔹 Properly handle labels (trim spaces before checking ':')
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string label = line.substr(0, colonPos);
                label.erase(0, label.find_first_not_of(" \t\r\n"));  // Trim spaces
                label.erase(label.find_last_not_of(" \t\r\n") + 1);
                
                if (!label.empty()) {
                    std::cout << "Label found: " << label << " at index " << instructionIndex << std::endl;
                    labelMap[label] = instructionIndex;
                }
                continue; // Skip label-only lines
            }

            // Ignore comments (assuming '#' or '//')
            size_t commentPos = line.find('#');
            if (commentPos == std::string::npos)
                commentPos = line.find("//");
            if (commentPos != std::string::npos)
                line = line.substr(0, commentPos);

            // Trim again after removing comments
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty()) continue; // Skip if only a comment was present
            
            // Parse the instruction
            auto Instr = assembleInstruction(line);
            if (std::get<0>(Instr).empty()) {
                std::cerr << "Warning: Unrecognized instruction -> " << line << std::endl;
            } else {
                program.push_back(Instr);
                instructionIndex++;
            }
        }
    }

    // 🔹 Reset file for second pass
    file.clear();  // Clear EOF flag
    file.seekg(0); // Move read position to the start

    // 🔹 Second Pass: Resolve Labels
    for (auto &instr : program) {
        std::string &label = std::get<5>(instr); // Fetch stored label
        if (!label.empty() && labelMap.find(label) != labelMap.end()) {
            std::get<4>(instr) = labelMap[label]; // Replace label with instruction index
        } else if (!label.empty()) {
            std::cerr << "Error: Undefined label '" << label << "'" << std::endl;
        }
    }

    return program;
}

class Simulator {
    public:
        Memory memory;
        std::vector<Core> cores;
        int clock;
        std::vector<std::tuple<std::string, int, int, int, int,std::string>> program;
    
    public:
        Simulator(const std::string &filename) : clock(0), memory() {
            program = loadProgramFromFile("program.s");
            std::cout << "\n=== Label Map ===\n";
    for (const auto &entry : labelMap) {
        std::cout << "Label: " << entry.first << " -> Instruction Index: " << entry.second << "\n";
    }

                for (int i = 0; i < 4; i++) {
                    cores.emplace_back(memory, program, i); // Directly construct objects in the vector
                }
                std::cout<<"Program"<<std::endl;
                for (const auto &inst : program) {
                    std::cout << std::get<0>(inst) << " " // Instruction name
                              << "x" << std::get<1>(inst) << ", " // Destination register
                              << "x" << std::get<2>(inst) << ", " // Source register 1
                              << "x" << std::get<3>(inst) << ", " // Source register 2 (if applicable)
                              << std::get<4>(inst)  // Immediate value (if applicable)
                              << std::endl;
                }
            }
        
           
            void executeCore(Core &core, int coreID, int instructionIndex) {
                if (instructionIndex < program.size()) {
                    auto &inst = program[instructionIndex];
                    std::cout << "\n[Core " << coreID << "] Executing: " << std::get<0>(inst) << std::endl;
                    core.execute(std::get<0>(inst), std::get<1>(inst), std::get<2>(inst), std::get<3>(inst), std::get<4>(inst),std::get<5>(inst),labelMap);
                }
            }
            void run() {
                int totalInstructions = program.size();
                bool running = true;
                while (running) {
                    running = false;  // Assume all cores are finished
            
                    std::cout << "\n--- Clock Cycle: " << clock << " ---\n";
            
                    for (int i = 0; i < 4; i++) {
                        if (cores[i].pc < totalInstructions) {
                            executeCore(cores[i], i, cores[i].pc);
                            running = true;  // At least one core is still running
                        }
                    }
                    if(clock==20) break;
                    if (!running) break;
                    clock++;
                }
            
                // Print Final State
                std::cout << "\nFinal State of Registers After Execution:\n";
                for (int i = 0; i < 4; i++) {
                    std::cout << "Core " << i << " Registers:\n";
                    cores[i].printRegisters();
                }
            
                std::cout << "\nFinal State of Memory After Execution:\n";
                for (int i = 0; i < 4; i++) {
                    memory.printMem(i);
                }
            }
            
    // Final State Dump

};
int main()
{
    Simulator simulator("program.s");
    simulator.run();
    std::cout<<"No.of clock Cycles : "<<simulator.clock<<std::endl;
    return 0;
}
