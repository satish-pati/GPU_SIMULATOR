#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include <map>
#include "../include/core.h"
#include "../include/memory.h"
#include <thread>
#include "../include/pipeline.h"
#include <climits>
#include <string>
using namespace std;

std::unordered_map<std::string, int> labelMap;
std::map<std::string, int> dataLabels;
bool sp= false;
bool bnenum= false;

// Converts a line (already parsed as a tuple) into an Instruction object.
Instruction tupleToInstruction(const std::tuple<std::string, int, int, int, int, std::string> &t, int index)
{
    Instruction instr;
    instr.opcode = std::get<0>(t);
    instr.rd = std::get<1>(t);
    instr.rs1 = std::get<2>(t);
    instr.rs2 = std::get<3>(t);
    instr.imm = std::get<4>(t);
    instr.label = std::get<5>(t);
    instr.index = index;
    return instr;
}
// function for assembly instruction 
std::tuple<std::string, int, int, int, int, std::string> assembleInstruction(const std::string &line)
{

    std::istringstream iss(line);
    std::string instr;
    int rd = 0, rs1 = 0, rs2 = 0, imm = 0;
    std::string label;
    char comma;
    iss >> instr;
    if (instr == "sync" || instr == "SYNC")
    {
        return {instr, 0, 0, 0, 0, ""};
    }

    if (instr == "add" || instr == "sub" || instr == "ADD" || instr == "SUB" || instr == "mul" || instr == "MUL")
    {

        std::string rdStr, rs1Str, rs2Str;
        // Read register names as strings
        std::getline(iss, rdStr, ',');
        std::getline(iss, rs1Str, ',');
        std::getline(iss, rs2Str);

        auto trim = [](std::string &str)
        {
            str.erase(0, str.find_first_not_of(" \t\r\n"));
            str.erase(str.find_last_not_of(" \t\r\n") + 1);
        };

        trim(rdStr);
        trim(rs1Str);
        trim(rs2Str);
        // std::cout << "Decoded registers: " << rdStr << ", " << rs1Str << ", " << rs2Str << std::endl;
        //  Convert register strings to integer IDs
        // std::cout << rdStr << " " << rs1Str << " " << rs2Str << std::endl;
        rd = std::stoi(rdStr.substr(1)); // Remove 'x' and convert
        rs1 = std::stoi(rs1Str.substr(1));
        rs2 = std::stoi(rs2Str.substr(1));

        return {instr, rd, rs1, rs2, imm, ""};
    }
// ... inside assembleInstruction() ...

    if (instr == "addi" || instr == "ADDI")
    {
        std::string rdStr, rs1Str;
        int imm;

        // Check if the input uses commas or spaces as separators
        if (line.find(',') != std::string::npos)
        {
            // Comma-separated format
            getline(iss >> std::ws, rdStr, ',');  // Read rd (e.g., "x5"), trim spaces
            getline(iss >> std::ws, rs1Str, ','); // Read rs1 (e.g., "x2"), trim spaces
            iss >> imm;                           // Read immediate (e.g., 1000)
        }
        else
        {
            // Space-separated format
            iss >> rdStr >> rs1Str >> imm; // Read rd, rs1, and immediate
        }

        // Convert register strings to integer IDs
        rd = std::stoi(rdStr.substr(1));   // Remove 'x' and convert "x5" -> 5
        rs1 = std::stoi(rs1Str.substr(1)); // Remove 'x' and convert "x2" -> 2

        return {instr, rd, rs1, rs2, imm, ""};
    }

    if (instr == "mv" || instr == "MV")
    {
        std::string rdStr, rs1Str;
        int imm = 0; // imm is always 0 for `mv`

        // Check if the input uses commas or spaces as separators
        if (line.find(',') != std::string::npos)
        {
            // Comma-separated format (e.g., mv x3,x4)
            getline(iss >> std::ws, rdStr, ',');  // Read rd (e.g., "x3")
            getline(iss >> std::ws, rs1Str, ','); // Read rs1 (e.g., "x4")
        }
        else
        {
            // Space-separated format (e.g., mv x3 x4)
            iss >> rdStr >> rs1Str; // Read rd and rs1
        }

        // Convert register strings to integer IDs
        rd = std::stoi(rdStr.substr(1));   // Remove 'x' and convert "x3" -> 3
        rs1 = std::stoi(rs1Str.substr(1)); // Remove 'x' and convert "x4" -> 4

        // Call encodeIType with immediate = 0 (mv is equivalent to addi with imm = 0)
        return {instr, rd, rs1, rs2, imm, ""};
    }
    else if (instr == "lw" || instr == "LW")
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
        return {instr, rd, rs1, rs2, imm, ""};
    }
    else if (instr == "la" || instr == "lA")
    {
        std::string rdStr, rs1Str, immStr;
        char paren1, paren2;

        // Check if comma is present in the input
        if (line.find(',') != std::string::npos)
        {
            // Comma-separated format (e.g., la x2, array)
            getline(iss >> std::ws, rdStr, ',');  // Read "x2"
            getline(iss >> std::ws, immStr, '('); // Read "array" or immediate
            getline(iss >> std::ws, rs1Str, ')'); // Read "x2" (if present)
        }
        else
        {
            // Space-separated format (e.g., la x2 array)
            iss >> rdStr;  // Read "x2"
            iss >> immStr; // Read "array" (label or immediate)
        }

        // Convert the destination register to integer
        rd = std::stoi(rdStr.substr(1)); // "x2" -> 2

        // Check if immStr is a number (immediate addressing)
        if (isdigit(immStr[0]) || (immStr[0] == '-' && isdigit(immStr[1])))
        {
            // Immediate addressing
            imm = std::stoi(immStr);
            rs1 = 0; // Base register is zero when using immediate addressing
        }
        else
        {
            // Label-based addressing
            if (dataLabels.find(immStr) != dataLabels.end())
            {
                imm = dataLabels[immStr];
                rs1 = 0; // Base register is zero when using label-based addressing
            }
            else
            {
                std::cerr << "Error: Undefined data label '" << immStr << "'" << std::endl;
                return {};
            }
        }

        return {instr, rd, rs1, rs2, imm, ""};
    }

    else if (instr == "sw" || instr == "SW")
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

        return {instr, rd, rs1, rs2, imm, ""};
    }
    else if (instr == "lw_spm" || instr == "LW_SPM") {
        std::string rdStr, rs1Str, immStr;
        // Format: lw_spm rd, offset(rs1)
        getline(iss >> std::ws, rdStr, ',');
        getline(iss >> std::ws, immStr, '(');
        getline(iss >> std::ws, rs1Str, ')');
        rd = std::stoi(rdStr.substr(1));   // Remove 'x' and convert
        rs1 = std::stoi(rs1Str.substr(1)); // Remove 'x' and convert
        imm = std::stoi(immStr);           // Convert offset
        sp=true;
        return {instr, rd, rs1, rs2, imm, ""};
    }
    else if (instr == "sw_spm" || instr == "SW_SPM") {
        std::string rs2Str, rs1Str, immStr;
        // Format: sw_spm rs2, offset(rs1)
        getline(iss >> std::ws, rs2Str, ',');
        getline(iss >> std::ws, immStr, '(');
        getline(iss >> std::ws, rs1Str, ')');
        rs2 = std::stoi(rs2Str.substr(1)); // Remove 'x' and convert
        rs1 = std::stoi(rs1Str.substr(1)); // Remove 'x' and convert
        imm = std::stoi(immStr);           // Convert offset
        sp=true;
        return {instr, rd, rs1, rs2, imm, ""};
    }
    
    else if (instr == "li" || instr == "LI")
    {
        std::string rdStr;
        int imm;

        // Handle both comma and space-separated formats
        if (line.find(',') != std::string::npos)
        {
            getline(iss >> std::ws, rdStr, ',');
            iss >> imm;
        }
        else
        {
            iss >> rdStr >> imm;
        }

        rd = std::stoi(rdStr.substr(1)); // Extract register number

        // Translate `li` into `addi rd, x0, imm`
        return {"addi", rd, 0, 0, imm, ""};
    }
    else if (instr == "bne" || instr == "beq" || instr == "ble" || instr == "BNE" || instr == "BEQ" || instr == "BLE" || instr == "BLT" || instr == "blt" || instr == "BGE" || instr == "bge")
    {
        std::string rs1Str, rs2Str, labelStr;

        // Check if the input uses commas or spaces as separators
        if (line.find("cid") != std::string::npos || line.find("CID") != std::string::npos)
        {
            std::string cidDummy, labelStr;
            int cidValue;
            char comma;
            char ch;

            if (line.find(',') != std::string::npos)
            {
                // Read `cidDummy` character by character until a comma appears
                while (iss.get(ch))
                {
                    if (ch == ',')
                        break; // Stop reading when a comma is encountered
                    cidDummy += ch;
                }

                // Trim any spaces in `cidDummy`
                auto trim = [](std::string &str)
                {
                    str.erase(0, str.find_first_not_of(" \t\r\n"));
                    str.erase(str.find_last_not_of(" \t\r\n") + 1);
                };
                trim(cidDummy);
                // Comma-separated format: bne cid, 2, label
                char comma;
                iss >> cidValue >> comma >> labelStr;
            }
            else
            {
                // Space-separated format: bne cid 2 label
                iss >> cidDummy >> cidValue >> labelStr;
            }

            if (cidDummy == "cid" || cidDummy == "CID")
            {
                imm = 0;
                label = labelStr;
                if (cidValue > 3 || cidValue < 0)
                {
                    rd = 0;
                    cout << "wrong immediate given in bne instruction and immediate should be between 0 and 3 only" << endl;
                    //bnenum = false;
                }
                else
                {
                    rd = 0;
                   // bnenum = true;
                }
                rs1 = 0;        // will change to coreID in sim classs---for now 0,no need of rd
                rs2 = cidValue; // given in code ...just compare cid,core id---> rs1,rs2
            }
            if (instr == "bne" || instr == "BNE")
                instr = "bnecid";
            if (instr == "beq" || instr == "BEQ")
                instr = "beqcid";
            if (instr == "ble" || instr == "BLE")
                instr = "blecid";
            if (instr == "BGE" || instr == "bge")
                instr = "bgecid";
            if (instr == "BLT" || instr == "blt")
                instr = "bltcid";

            // cout << "cidDummy:" << cidDummy << "    cidValue:" << cidValue << endl;
            // std::cout << "BNE CID Parsed: instr=" << instr
            //           << " rd=" << rd << " rs1=" << rs1
            //          << " rs2=" << rs2 << " label=" << label << "ended" << std::endl;
            // std::cout << "bnenum:" << bnenum << std::endl;
            return {instr, rd, rs1, rs2, imm, label};
        }
        if (line.find(',') != std::string::npos)
        {
            // Comma-separated format (e.g., bne x2, x3, label)
            getline(iss >> std::ws, rs1Str, ','); // Read "x2"
            getline(iss >> std::ws, rs2Str, ','); // Read "x3"
            iss >> labelStr;                      // Read "label"
        }
        else
        {
            // Space-separated format (e.g., bne x2 x3 label)
            iss >> rs1Str >> rs2Str >> labelStr; // Read "x2", "x3", and "label"
        }

        // Convert registers to integers
        rs1 = std::stoi(rs1Str.substr(1)); // "x2" -> 2
        rs2 = std::stoi(rs2Str.substr(1)); // "x3" -> 3
        label = labelStr;                  // Store label name to resolve later

        return {instr, rd, rs1, rs2, imm, label};
    }

    else if (instr == "j" || instr == "J")
    {
        std::string labelStr;
        iss >> labelStr;
        label = labelStr; // Store label name to resolve later
        return {instr, rd, rs1, rs2, imm, label};
    }

    else if (instr == "jal" || instr == "JAL")
    {
        std::string rdStr, labelStr;

        // Check if the input uses commas or spaces as separators
        if (line.find(',') != std::string::npos)
        {
            // Comma-separated format (e.g., jal x7, label)
            if (!(iss >> rdStr >> labelStr))
            { // Extract rd and label
                std::cerr << "Error: Invalid JAL instruction format!" << std::endl;
                return {}; // Return an empty instruction structure
            }
        }
        else
        {
            // Space-separated format (e.g., jal x4 label)
            iss >> rdStr >> labelStr; // Extract rd and label
        }

        // Extract register number from "x7"
        if (rdStr[0] == 'x')
        {
            rdStr = rdStr.substr(1); // Remove 'x' prefix
        }

        int rd = std::stoi(rdStr); // Convert to integer
        label = labelStr;          // Assign label

        return {instr, rd, 0, 0, 0, label};
    }

    else
    {
        // std::cout << "None" << std::endl;
        return {instr, rd, rs1, rs2, imm, ""};
    }
}

// function to load program from file
std::vector<std::tuple<std::string, int, int, int, int, std::string>>
loadProgramFromFile(const std::string &filename, Memory &memory)
{
    std::vector<std::tuple<std::string, int, int, int, int, std::string>> program;
    std::ifstream file(filename);

    // Check if file opened successfully
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return program;
    }
    else
    {
        std::cout << "File opened successfully" << std::endl;
    }

    std::string line;
    bool inTextSection = false;
    bool inDataSection = false;
    int dataAddress = DATA_BASE_ADDRESS;
    ; // Start of the data section
    // int memoryAddress = 0x10010000; // Starting address for .data section
    int instructionIndex = 0;

    // 🔹 First Pass: Identify Labels
    while (std::getline(file, line))
    {
        // Trim leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty())
            continue; // Skip empty lines
        if (line.find(".data") != std::string::npos)
        {
            inDataSection = true;
            continue;
        }
        if (line.find(".text") != std::string::npos)
        {
            inDataSection = false;
            inTextSection = true;
            continue;
        }

        if (inDataSection)
        {
            std::istringstream iss(line);
            std::string token;
            iss >> token; // Read first token
            bool is = true;
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos)
            {
                line = line.substr(0, commentPos); // Ignore the comment part
            }

            // Trim leading and trailing spaces from the line
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            iss.clear();   // Clear the stringstream state
            iss.str(line); // Reinitialize the stringstream with the trimmed line

            // Read the first token to check for a label (e.g., arr:)
            iss >> token;

            size_t colonPos = token.find(':');
            if (colonPos != std::string::npos)
            {
                // It's a label
                std::string label = token.substr(0, colonPos);
                label.erase(0, label.find_first_not_of(" \t\r\n"));
                label.erase(label.find_last_not_of(" \t\r\n") + 1);

                if (!label.empty())
                {
                   // std::cout << "Data Label found: " << label << " at address " << dataAddress << std::endl;
                    dataLabels[label] = dataAddress;
                }

                // Read the remaining values in the line (e.g., `.word 10, 20, 30`)
                while (iss >> token)
                {
                    size_t rangePos = token.find(':');
                    if (rangePos != std::string::npos && rangePos > 0 && rangePos < token.length() - 1) {
                        // Handle range format
                        try {
                            int start = std::stoi(token.substr(0, rangePos));
                            int end = std::stoi(token.substr(rangePos + 1));
                            
                            if (start <= end) {
                                for (int i = start; i <= end; i++) {
                                    memory.sstoreWord(dataAddress, i, 0, is);
                                    dataAddress += 4;
                                }
                            } else {
                                for (int i = start; i >= end; i--) {
                                    memory.sstoreWord(dataAddress, i, 0, is);
                                    dataAddress += 4;
                                }
                            }
                            continue; // Skip the normal comma processing for this token
                        } catch (...) {
                            // If conversion fails, treat it as normal token
                        }
                    }
                    size_t commaPos;
                    while ((commaPos = token.find(',')) != std::string::npos)
                    {
                        std::string valueStr = token.substr(0, commaPos);
                        valueStr.erase(0, valueStr.find_first_not_of(" \t\r\n"));
                        valueStr.erase(valueStr.find_last_not_of(" \t\r\n") + 1);

                        if (!valueStr.empty() && (isdigit(valueStr[0]) || (valueStr[0] == '-' && isdigit(valueStr[1]))))
                        {
                            int value = std::stoi(valueStr);
                            // Store the value in all cores' memory
                           
                                memory.sstoreWord( dataAddress, value, 0, is);
                            
                            dataAddress += 4;
                        }

                        token = token.substr(commaPos + 1); // Update the token to the part after the comma
                    }

                    // After the last value in the token, process it
                    token.erase(0, token.find_first_not_of(" \t\r\n"));
                    token.erase(token.find_last_not_of(" \t\r\n") + 1);

                    if (!token.empty() && (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))))
                    {
                        int value = std::stoi(token);
                        // Store the value in all cores' memory
                       /* for (int coreID = 0; coreID < 4; coreID++)
                        {
                            memory.storeWord(coreID * 1024 + dataAddress, value, coreID, is);
                        }*/
                        memory.sstoreWord( dataAddress, value, 0, is);
                        dataAddress += 4;
                    }
                }
            }
            else
            {
                // Line without a label, possible continuation of `.word` values
                while (iss >> token)
                {
                    // Token might be something like "10, 20, 30" so split it by commas
                    size_t commaPos;
                    while ((commaPos = token.find(',')) != std::string::npos)
                    {
                        std::string valueStr = token.substr(0, commaPos);
                        valueStr.erase(0, valueStr.find_first_not_of(" \t\r\n"));
                        valueStr.erase(valueStr.find_last_not_of(" \t\r\n") + 1);

                        if (!valueStr.empty() && (isdigit(valueStr[0]) || (valueStr[0] == '-' && isdigit(valueStr[1]))))
                        {
                            int value = std::stoi(valueStr);
                            // Store the value in all cores' memory
                           /* for (int coreID = 0; coreID < 4; coreID++)
                            {
                                memory.storeWord(coreID * 1024 + dataAddress, value, coreID, is);
                            }*/
                           // Store the value in shared memory only once.
                           memory.sstoreWord(dataAddress, value, 0, is);
                            dataAddress += 4;
                        }
                        token = token.substr(commaPos + 1); // Update the token to the part after the comma
                    }

                    // After the last value in the token, process it
                    token.erase(0, token.find_first_not_of(" \t\r\n"));
                    token.erase(token.find_last_not_of(" \t\r\n") + 1);

                    if (!token.empty() && (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))))
                    {
                        int value = std::stoi(token);
                        // Store the value in all cores' memory
                       /*for (int coreID = 0; coreID < 4; coreID++)
                        {
                            memory.storeWord(coreID * 1024 + dataAddress, value, coreID, is);
                        }*/
                        memory.sstoreWord(dataAddress, value, 0, is);
                        dataAddress += 4;
                    }
                }
            }
        }

          
            

        if (inTextSection)
        {
            // 🔹 Properly handle labels (trim spaces before checking ':')
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos)
            {
                std::string label = line.substr(0, colonPos);
                label.erase(0, label.find_first_not_of(" \t\r\n")); // Trim spaces
                label.erase(label.find_last_not_of(" \t\r\n") + 1);

                if (!label.empty())
                {
                    //  std::cout << "Label found: " << label << " at index " << instructionIndex << std::endl;
                    labelMap[label] = instructionIndex;
                }
                // After the label, check if there's an instruction
                line = line.substr(colonPos + 1);
                line.erase(0, line.find_first_not_of(" \t\r\n")); // Trim spaces after the label

                if (line.empty())
                    continue; // If only label was present, skip line
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

            if (line.empty())
                continue; // Skip if only a comment was present

            // Parse the instruction
            auto Instr = assembleInstruction(line);
            if (std::get<0>(Instr).empty())
            {
                std::cerr << "Warning: Unrecognized instruction -> " << line << std::endl;
            }
            else
            {
                program.push_back(Instr);
                instructionIndex++;
            }
        }
    }

    // 🔹 Reset file for second pass
    file.clear();  // Clear EOF flag
    file.seekg(0); // Move read position to the start

    // 🔹 Second Pass: Resolve Labels
    for (auto &instr : program)
    {
        std::string &label = std::get<5>(instr); // Fetch stored label
        if (!label.empty() && labelMap.find(label) != labelMap.end())
        {
            std::get<4>(instr) = labelMap[label]; // Replace label with instruction index
        }
        else if (!label.empty() && dataLabels.find(label) != dataLabels.end())
        {
            // Data Label Resolution (Replace with Memory Address)
            std::get<4>(instr) = dataLabels[label];
            std::cout << "Resolved Data Label '" << label << "' to Address: " << dataLabels[label] << std::endl;
        }
        else if (!label.empty())
        {
            std::cerr << "Error: Undefined label '" << label << "'" << std::endl;
        }
    }

    return program;
}
// convert instruction to string
std::string instrToString(const Instruction &instr)
{
    return "Index=" + std::to_string(instr.index) +
           " Opcode=" + instr.opcode +
           " rd=" + std::to_string(instr.rd) +
           " rs1=" + std::to_string(instr.rs1) +
           " rs2=" + std::to_string(instr.rs2) +
           " imm=" + std::to_string(instr.imm) +
           " label=" + instr.label;
}
// --- Utility Functions ---
Instruction createNopInstruction()
{
    Instruction nop;
    nop.opcode = "nop";
    nop.rd = 0;
    nop.rs1 = 0;
    nop.rs2 = 0;
    nop.imm = 0;
    nop.index = -1;
    return nop;
}
vector<vector<tuple<string, int>>> consoleOutput = vector<vector<tuple<string, int>>>(4);
// --- Pipeline Simulation Structures for each Core ---
struct PipelineState
{
    PipelineRegister IF;
    PipelineRegister ID;
    PipelineRegister EX;
    PipelineRegister MEM;
    PipelineRegister WB;
    int localPC;         // Next expected instruction index for this core
    int stallCount;      // Count of stall cycles
    int completedInstr;  // Number of instructions that reached WB (committed)
    bool previf;         // Flag to ignore fetched instruction (after branch taken)
    bool waitingForSync; // Flag to indicate SYNC instruction is waiting

    PipelineState() : localPC(0), stallCount(0), completedInstr(0), previf(false), waitingForSync(false) {}
};

// --- Simulator Class with Pipeline Implementation ---
class Simulator
{
public:
    Memory memory;
    std::vector<Core> cores;          // Four compute units
    std::vector<Instruction> program; // Converted program
    int globalPC;                     // Single fetch unit pointer
    int clockCycles;
    bool forwardingEnabled;
    std::unordered_map<std::string, int> latencyConfig; //  {"ADD":1, "SUB":1, "ADDI":1, "MUL":3}
    std::vector<PipelineState> pipelineStates;          // One per core
    int syncCounter;                                    //  counter for SYNC barrier.
public:
    Simulator(const std::string &filename) : clockCycles(0), globalPC(0),syncCounter(0)
    {
        int l1Size,l1Isize, l2Size, blockSize, associativity, l1Latency, l2Latency, memLatency;
        std::cout << "Enter L1D Cache Size in bytes: ";
        std::cin >> l1Size;
        std::cout << "Enter L1I Cache Size in bytes: ";
        std::cin >> l1Isize;
        std::cout << "Enter L2 Cache Size in bytes: ";
        std::cin >> l2Size;
        std::cout << "Enter Cache Block Size in bytes: ";
        std::cin >> blockSize;
        std::cout << "Enter Cache Associativity: ";
        std::cin >> associativity;
        std::cout << "Enter L1 Cache Access Latency (cycles): ";
        std::cin >> l1Latency;
        std::cout << "Enter L2 Cache Access Latency (cycles): ";
        std::cin >> l2Latency;
        std::cout << "Enter Main Memory Access Time (cycles): ";
        std::cin >> memLatency;
        memory.configureCaches(l1Isize,l1Size, l2Size, blockSize, associativity, l1Latency, l2Latency, memLatency);
        std::cout<<"Instruction "<<std::endl;
        auto progTuples = loadProgramFromFile(filename, memory);
                    std::cout<<"Instruction "<<std::endl;
        // Convert tuples into Instruction objects
        for (size_t i = 0; i < progTuples.size(); i++)
        {
           // std::cout<<"Instruction "<<i<<std::endl;
            auto isactive=true;
            memory.sstoreWord(i*4,i,0,isactive);
            program.push_back(tupleToInstruction(progTuples[i], i));
        }

        for (int i = 0; i < 4; i++)
        {
            cores.emplace_back(memory, progTuples, i);
            pipelineStates.push_back(PipelineState());
        }
        // User configuration for forwarding and instruction latencies
        char fwdChoice;
        std::cout << "Enable data forwarding? (y/n): ";
        std::cin >> fwdChoice;
        forwardingEnabled = (fwdChoice == 'y' || fwdChoice == 'Y');
        std::cout << "Enter latency for ADD (cycles): ";
        int lat;
        // std::cin >> lat;
        
        /*for(int i=1;i<=20000;i++)   {
            uint32_t word=(1008+((i-1)*4));
            bool k=true;
            for(int j=0;j<4;j++){
                memory.storeWord(word,i ,j,k);
            }
        }*/
        do
        {
            std::cin >> lat;
            if (lat < 1)
            {
                std::cout << "Error: Latency must be at least 1. Please try again." << std::endl;
            }
        } while (lat < 1);
        latencyConfig["add"] = lat;
        latencyConfig["ADD"] = lat;
        std::cout << "Enter latency for SUB (cycles): ";
        // std::cin >> lat;
        do
        {
            std::cin >> lat;
            if (lat < 1)
            {
                std::cout << "Error: Latency must be at least 1. Please try again." << std::endl;
            }
        } while (lat < 1);
        latencyConfig["sub"] = lat;
        latencyConfig["SUB"] = lat;
        // std::cin >> lat;
        std::cout << "Enter latency for ADDI (cycles): ";

        do
        {

            std::cin >> lat;
            if (lat < 1)
            {
                std::cout << "Error: Latency must be at least 1. Please try again." << std::endl;
            }
        } while (lat < 1);
        latencyConfig["addi"] = lat;
        latencyConfig["ADDI"] = lat;
        std::cout << "Enter latency for MUL (cycles): ";
        // std::cin >> lat;
        do
        {
            std::cin >> lat;
            if (lat < 1)
            {
                std::cout << "Error: Latency must be at least 1. Please try again." << std::endl;
            }
        } while (lat < 1);
        latencyConfig["mul"] = lat;
        latencyConfig["MUL"] = lat;
        // --- Prompt for cache configuration ---
        
        if(l1Latency==1&&!sp) clockCycles=-10000;
        for (int cid = 0; cid < 4; cid++) {
            pipelineStates[cid].stallCount = 0;
            pipelineStates[cid].waitingForSync = false;
        }
    }

    // Hazard detection in the ID stage for a given core.
    bool hazardDetected(const PipelineState &ps, const Instruction &instr)
    { // data hazard --> RAW
        int src1 = instr.rs1;
        int src2 = instr.rs2;
        auto regNeedsStall = [&](int reg, const PipelineRegister &pr) -> bool
        {
            if (!pr.valid)
                return false;       // Skip invalid pipeline stages
            int dest = pr.instr.rd; // Get destination register of previous instruction
            if (dest == 0 || dest == 32)
                return false; // Skip x0 and x32 (CID)

            if (forwardingEnabled)
            {
                // Even with forwarding, if a load is in EX, its data isn't ready until MEM.
                if ((pr.instr.opcode == "lw" || pr.instr.opcode == "LW" || pr.instr.opcode == "la" || pr.instr.opcode == "LA") && pr.stage == STAGE_MEM && dest == reg)
                    return true;
                // For other instructions, stall only if in EX and still busy.
                if (pr.stage == STAGE_EX && pr.remainingLatency > 0 && dest == reg)
                    return true;
                return false;
            }

            // Without forwarding, stall if any stage (EX, MEM, WB) has the required rd
            return (dest == reg) && (pr.stage == STAGE_EX || pr.stage == STAGE_MEM || pr.stage == STAGE_WB);
        };

        if (regNeedsStall(src1, ps.EX) || regNeedsStall(src1, ps.MEM) || regNeedsStall(src1, ps.WB))
            return true;
        if (regNeedsStall(src2, ps.EX) || regNeedsStall(src2, ps.MEM) || regNeedsStall(src2, ps.WB))
            return true;
        return false;
    }

    
    // Simulate one clock cycle.
   void cycle()
    {
        
        //  Adjust globalPC if any core's localPC is behind (branch taken backward)
        int minLocalPC = INT_MAX;
        for (int cid = 0; cid < 4; cid++)
        {
            if (pipelineStates[cid].localPC < minLocalPC)
                minLocalPC = pipelineStates[cid].localPC;
        }

        globalPC = minLocalPC;

        // Fetch Stage: Broadcast one instruction from global instruction memory 
        Instruction fetchedInstr;
        bool fetchedValid = false;
        if (globalPC < (int)program.size())
        {
            fetchedInstr = program[globalPC];
            fetchedValid = true;
            /*std::cout << "Global Fetch: Fetched instruction "
                      << instrToString(fetchedInstr) << " at globalPC " << globalPC << std::endl;*/
            globalPC++; // Increment global fetch pointer after fetching
        }
        // Process pipeline stages for each core in order: WB -> MEM -> EX -> ID -> IF
        for (int cid = 0; cid < 4; cid++)
        {
            // bool stallThisCycle = false;
            bool stallCounted = false;
            PipelineState &ps = pipelineStates[cid];
            Core &core = cores[cid];
            bool idbusy = ps.ID.valid; // Flag to indicate that an instruction has just finished EX this cycle
            bool exBusy = ps.EX.valid;
            //  WriteBack Stage
            if (ps.WB.valid)
            {
                if (ps.WB.instr.opcode != "nop")
                {
                    // Check if the instruction actually writes to a register.
                    // For arithmetic, load, move, load address, and jal, update destination.
                    if (ps.WB.instr.opcode == "add" || ps.WB.instr.opcode == "ADD" ||
                        ps.WB.instr.opcode == "sub" || ps.WB.instr.opcode == "SUB" ||
                        ps.WB.instr.opcode == "addi" || ps.WB.instr.opcode == "ADDI" ||
                        ps.WB.instr.opcode == "mul" || ps.WB.instr.opcode == "MUL" ||
                        ps.WB.instr.opcode == "mv" || ps.WB.instr.opcode == "MV" ||
                        ps.WB.instr.opcode == "lw" || ps.WB.instr.opcode == "LW" ||
                        ps.WB.instr.opcode == "la" || ps.WB.instr.opcode == "LA" ||
                        ps.WB.instr.opcode == "jal" || ps.WB.instr.opcode == "JAL"
                    ||ps.WB.instr.opcode == "lw_spm" || ps.WB.instr.opcode == "LW_SPM" 
                )
                    {
                        core.writeBack(ps.WB.instr.rd, ps.WB.computedResult);
                    }
                    // For branch, store, jump (J) and other control instructions,
                    // no register update is needed but we still count the instruction.
                    ps.completedInstr++;
                    
                    if (ps.WB.instr.opcode == "sw" || ps.WB.instr.opcode == "SW" ||
                        ps.WB.instr.opcode == "bne" || ps.WB.instr.opcode == "BNE" ||
                        ps.WB.instr.opcode == "beq" || ps.WB.instr.opcode == "BEQ" ||
                        ps.WB.instr.opcode == "ble" || ps.WB.instr.opcode == "BLE" ||
                        ps.WB.instr.opcode == "blt" || ps.WB.instr.opcode == "BLT" ||
                        ps.WB.instr.opcode == "bge" || ps.WB.instr.opcode == "BGE" ||
                        ps.WB.instr.opcode == "j" || ps.WB.instr.opcode == "J")
                    {
                        // std::cout << "(No writeback needed)";
                    }
                }
                ps.WB.valid = false;
            }
            // --- Memory Stage ---
            if (ps.MEM.valid)
            {
                // If lw or sw and still waiting on latency, decrement remainingLatency.
                if ((ps.MEM.instr.opcode == "lw" || ps.MEM.instr.opcode == "LW" ||
                     ps.MEM.instr.opcode == "sw" || ps.MEM.instr.opcode == "SW"||ps.MEM.instr.opcode == "sw_spm" || ps.MEM.instr.opcode == "SW_SPM"||ps.MEM.instr.opcode == "lw_spm" || ps.MEM.instr.opcode == "LW_SPM") &&
                    (ps.MEM.remainingLatency > 0))
                {
                    ps.MEM.remainingLatency--;
                }
                else if (ps.MEM.instr.opcode == "lw_spm" || ps.MEM.instr.opcode == "LW_SPM")
        {
         uint32_t addr = ps.MEM.computedResult;
         int value = memory.loadWordSPM(addr);
        // std::cout << "Load from SPM: " << addr <<"value: " <<value<<std::endl;

         ps.MEM.computedResult = value;
         ps.WB = ps.MEM;
         ps.WB.stage = STAGE_WB;
         ps.MEM.valid = false;
    }
    // New branch for SPM store
    else if (ps.MEM.instr.opcode == "sw_spm" || ps.MEM.instr.opcode == "SW_SPM")
    {
         auto getVal = [&](int reg) -> int {
             if (forwardingEnabled) {
                 if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                     return ps.WB.computedResult;
                 if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                     return ps.MEM.computedResult;
             }
             return core.readRegister(reg);
         };
         uint32_t addr = ps.MEM.computedResult;
         int value = getVal(ps.MEM.instr.rs2);
         memory.storeWordSPM(addr, value);
         ps.WB = ps.MEM;
         ps.WB.stage = STAGE_WB;
         ps.MEM.valid = false;
    }
                // Once latency count reaches zero,perform memory access.
else if (ps.MEM.instr.opcode == "lw" || ps.MEM.instr.opcode == "LW")
{
                    bool isActive = true;
                    uint32_t addr = ps.MEM.computedResult;
                    auto memdatapair = memory.loadWord(addr, cid, isActive);
                    int memData = memdatapair.first;
                    ps.MEM.computedResult = memData;
                    ps.WB = ps.MEM;
                    ps.WB.stage = STAGE_WB;
                    ps.MEM.valid = false;
                }
                else if (ps.MEM.instr.opcode == "sw" || ps.MEM.instr.opcode == "SW")
                {
                    bool isActive = true;
                    uint32_t addr = ps.MEM.computedResult;
                    auto getVal = [&](int reg) -> int
                    {
                        if (forwardingEnabled)
                        {
                            if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                                return ps.WB.computedResult;
                            if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                                return ps.MEM.computedResult;
                        }
                        return core.readRegister(reg);
                    };
                    int value = getVal(ps.MEM.instr.rs2);
                    memory.storeWord(addr, value, cid, isActive);
                    ps.WB = ps.MEM;
                    ps.WB.stage = STAGE_WB;
                    ps.MEM.valid = false;
                }
                else
                {
                    ps.WB = ps.MEM;
                    ps.WB.stage = STAGE_WB;
                    ps.MEM.valid = false;
                }
            }

            // --- Execute Stage ---
            if (ps.EX.valid)
            {

                
                 if (ps.EX.remainingLatency > 0)
                {
                    const std::string &op = ps.EX.instr.opcode;
                   
                    ps.EX.remainingLatency--;
                }
                else if (ps.EX.remainingLatency == 0)
                {
                    
                 if (ps.EX.instr.opcode == "sync" || ps.EX.instr.opcode == "SYNC")
                {
                    // If this core hasn't yet flagged itself as waiting, do so and update syncCounter.
                    if (!ps.waitingForSync)
                    {
                        ps.waitingForSync = true;
                        syncCounter++;
                       
                    }
                    // If not all cores have reached SYNC, stall this core.
                    if (syncCounter < 4)
                    {                   
                        ps.stallCount++;
                        continue; // Remain in EX until barrier condition is met.
                    }
                    else
                    {                 
                        // For all cores, flush the SYNC instruction and advance localPC.
                        for (int j = 0; j < 4; j++)
                        {
                            pipelineStates[j].EX.valid = false;
                            pipelineStates[j].waitingForSync = false;
                            // Advance the localPC to move past the SYNC instruction.
                          
                        }
                        syncCounter = 0; 
                        continue;
                    }
                }
                else if  (ps.MEM.valid )
                {
                    // MEM stage is busy; stall the current instruction in EX.
                    ps.stallCount++;
                    // Do not clear or modify EX, let the instruction remain until MEM becomes free.
                }
                    else
                    {
                        const std::string &op = ps.EX.instr.opcode;
                        if (op == "lw" || op == "LW" || op == "sw" || op == "SW")
                        {
                            // Compute effective memory address.
                            auto getVal = [&](int reg) -> int
                            {
                                if (forwardingEnabled)
                                {
                                    if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                                        return ps.WB.computedResult;
                                    if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                                        return ps.MEM.computedResult;
                                }
                                return core.readRegister(reg);
                            };
                            int base = getVal(ps.EX.instr.rs1);
                            int addr = base + ps.EX.instr.imm;
                            ps.EX.computedResult = addr;
                            // Transfer to MEM stage and set memory latency.
                           
                            ps.MEM = ps.EX;
                            ps.MEM.stage = STAGE_MEM;
                            int memLat;
                            bool isActive = true;
                            if (op == "lw" || op == "LW" ){
                                 auto memdatapair = memory.loadWord(addr, cid, isActive);
                                 memLat = memdatapair.second;
                            }
                            else {
                                    int value = getVal(ps.MEM.instr.rs2);
                                auto memLat = memory.storeWord(addr,value,cid,isActive);
                            }
                            ps.MEM.remainingLatency = (memLat > 0 ? memLat - 1 : 0);
                            

                            ps.EX.valid = false;
                        }
                        else if (op == "lw_spm" || op == "LW_SPM") {
                            // Compute effective address: base register + immediate
                            auto getVal = [&](int reg) -> int {
                                if (forwardingEnabled) {
                                    if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                                        return ps.WB.computedResult;
                                    if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                                        return ps.MEM.computedResult;
                                }
                                return core.readRegister(reg);
                            };
                            int base = getVal(ps.EX.instr.rs1);
                            int addr = base + ps.EX.instr.imm;
                            ps.EX.computedResult = addr;
                            // Transfer to MEM stage with SPM latency (assumed equal to L1D access latency)
                            ps.MEM = ps.EX;
                            ps.MEM.stage = STAGE_MEM;
                            int spmLatency = memory.getL1DLatency();
                             
                            ps.MEM.remainingLatency = (spmLatency > 0 ? spmLatency - 1 : 0);

                            ps.EX.valid = false;
                        }
                        // New branch for SPM store
                        else if (op == "sw_spm" || op == "SW_SPM") {
                            auto getVal = [&](int reg) -> int {
                                if (forwardingEnabled) {
                                    if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                                        return ps.WB.computedResult;
                                    if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                                        return ps.MEM.computedResult;
                                }
                                return core.readRegister(reg);
                            };
                            int base = getVal(ps.EX.instr.rs1);
                            int addr = base + ps.EX.instr.imm;
                            ps.EX.computedResult = addr;
                            // Transfer to MEM stage with SPM latency
                            ps.MEM = ps.EX;
                            ps.MEM.stage = STAGE_MEM;
                            int spmLatency = memory.getL1DLatency();
                            ps.MEM.remainingLatency = (spmLatency > 0 ? spmLatency - 1 : 0);
                            //ps.MEM.remainingLatency=0;
                            ps.EX.valid = false;
                        }
                        else
                        {
                            if (op == "add" || op == "ADD" ||
                                op == "sub" || op == "SUB" ||
                                op == "addi" || op == "ADDI" || op == "mul" || op == "MUL" ||
                                op == "mv" || op == "MV")
                            {
                                // If forwarding is enabled, check WB and MEM for a more recent value.
                                auto getVal = [&](int reg) -> int
                                {
                                    if (forwardingEnabled)
                                    {
                                        if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                                            return ps.WB.computedResult;
                                        if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                                            return ps.MEM.computedResult;
                                    }
                                    return core.readRegister(reg);
                                };
                                int rs1Val = getVal(ps.EX.instr.rs1);
                                int rs2Val = getVal(ps.EX.instr.rs2);
                                // std::cout<<"rs1Val: updated "<<rs1Val<<endl;
                                // std::cout<<"rs2Val:  updated "<<rs2Val<<endl;
                                int result = core.ALUOperation(op, rs1Val, rs2Val, ps.EX.instr.imm);
                                ps.EX.computedResult = result;
                                // std::cout << "Core " << cid << " - EX: "
                                //         << instrToString(ps.EX.instr) << " result = " << result << std::endl;
                            }

                            else if (op == "la" || op == "LA")
                            {
                                if (ps.EX.instr.rd == 0)
                                {
                                    // std::cout << "Core " << cid << " - La: X0 is hardwired to 0 and always 0" << std::endl;
                                }
                                else
                                {
                                    int computedValue = ps.EX.instr.imm;
                                    ps.EX.computedResult = computedValue;
                                }
                            }
                            else if (op == "jal" || op == "JAL")
                            {
                                // For JAL, compute the link (address of the next instruction)
                                int link = ps.EX.instr.index + 1;
                                ps.EX.computedResult = link;
                                // std::cout << "Core " << cid << " - EX: JAL encountered, writing link "
                                //         << link << " and jumping to index " << ps.EX.instr.imm << std::endl;
                                if (ps.ID.valid)
                                {
                                    ps.stallCount++;
                                }
                                ps.localPC = ps.EX.instr.imm;
                                ps.ID.valid = false;
                                ps.IF.valid = false; // Flush IF stage as well.
                            }
                            else if (op == "j" || op == "J")
                            {
                                if (ps.ID.valid)
                                {
                                    ps.stallCount++;
                                }

                                ps.localPC = ps.EX.instr.imm;
                                ps.ID.valid = false;
                                ps.IF.valid = false; // Flush IF stage as well.
                            }
                            else if (op == "bnecid" || op == "BNECID" || op == "beqcid" || op == "BEQCID" || op == "blecid" || op == "BLECID" || op == "bgecid" || op == "BGECID" || op == "bltcid" || op == "BLTCID")
                            {
                                int rs1Val = core.readRegister(ps.EX.instr.rs1);
                                int rs2Val = ps.EX.instr.rs2;
                                int rdVal = ps.EX.instr.rd;

                                bool taken = core.isBranchTaken(op, rs1Val, rs2Val, rdVal, bnenum);

                                if (taken)
                                {
                                   // std::cout << "Core " << cid << " - EX: Branch "
                                    //         << instrToString(ps.EX.instr)
                                    //         << " taken, jumping to index " << ps.EX.instr.imm << std::endl;
                                    if (ps.ID.valid)
                                    {
                                        ps.stallCount++;
                                    }
                                    //  std::cout<<"inc stall"<<endl;} // for IF down
                                    ps.localPC = ps.EX.instr.imm;
                                    ps.ID.valid = false;
                                    ps.IF.valid = false; // Flush IF stage as well.
                                }
                                else
                                {
                                  //  std::cout << "Core " << cid << " - EX: Branch "
                                             // << instrToString(ps.EX.instr) << " not taken" << std::endl;
                                }
                            }
                            else if (op == "bne" || op == "BNE" ||
                                     op == "beq" || op == "BEQ" ||
                                     op == "ble" || op == "BLE" ||
                                     op == "blt" || op == "BLT" ||
                                     op == "bge" || op == "BGE")
                            {
                                auto getVal = [&](int reg) -> int
                                {
                                    if (forwardingEnabled)
                                    {
                                        if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                                            return ps.WB.computedResult;
                                        if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                                            return ps.MEM.computedResult;
                                    }
                                    return core.readRegister(reg);
                                };
                                int rs1Val = getVal(ps.EX.instr.rs1);
                                int rs2Val = getVal(ps.EX.instr.rs2);
                                // if(bnenum) rs1Val=core.readRegister(32);//need to change
                                int rdVal = ps.EX.instr.rd;

                                bool taken = core.isBranchTaken(op, rs1Val, rs2Val, rdVal, bnenum);

                                if (taken)
                                {
                                    if (ps.ID.valid)
                                    {
                                        ps.stallCount++;
                                    }

                                    ps.localPC = ps.EX.instr.imm;
                                    ps.ID.valid = false;
                                    ps.IF.valid = false; // Flush IF stage as well.
                                }
                                else
                                {
                                }
                            }
                            else if (ps.EX.instr.opcode == "ecall")
                            {

                                bool isActive = true;
                                auto getVal = [&](int reg) -> int
                                {
                                    if (forwardingEnabled)
                                    {
                                        if (ps.WB.valid && ps.WB.instr.rd == reg && reg != 0 && reg != 32)
                                            return ps.WB.computedResult;
                                        if (ps.MEM.valid && ps.MEM.instr.rd == reg && reg != 0 && reg != 32)
                                            return ps.MEM.computedResult;
                                    }
                                    return core.readRegister(reg);
                                };
                                int x10 = getVal(10);
                                int x17 = getVal(17);

                                if (x17 == 1)
                                { // Print integer
                                    int value = x10;
                                    consoleOutput[cid].emplace_back("", value); // Store in the core's section
                                }
                                std::cout <<"Cloclk Cycle: "<< clockCycles << " | Core " << cid << " | Executing ECALL" << "  x10=" << x10 << std::endl;
                                std::cout << " | Core " << cid
                                      << " | Executing ECALL" << "  x10=" << x10 << std::endl;
                            }

                            ps.MEM = ps.EX;
                            ps.MEM.remainingLatency = 0;
                            ps.MEM.stage = STAGE_MEM;
                            ps.EX.valid = false;
                        }
                    }
                }
            }
            // --- Decode Stage ---
            if (ps.ID.valid)
            {
                if (ps.EX.valid && ps.EX.remainingLatency > 0)
                {
                    if (!stallCounted)
                    {
                        ps.stallCount++;
                        stallCounted = true;
                    }
                }

                else if (hazardDetected(ps, ps.ID.instr) && ps.ID.instr.opcode != "bnecid" && ps.ID.instr.opcode != "beqcid" && ps.ID.instr.opcode != "BNECID" && ps.ID.instr.opcode != "BEQCID" && ps.ID.instr.opcode != "blecid" && ps.ID.instr.opcode != "BLECID" && ps.ID.instr.opcode != "bgecid" && ps.ID.instr.opcode != "BLTCID" && ps.ID.instr.opcode != "bltcid" && ps.ID.instr.opcode != "BGECID")
                {
                    if (!stallCounted)
                    {
                        ps.stallCount++;
                        stallCounted = true;
                    }
                }
                else if (ps.EX.valid)
                {
                    if (!stallCounted)
                    {
                        ps.stallCount++;
                        stallCounted = true;
                    }
                }
                else
                {
                    ps.EX = ps.ID;
                    ps.EX.stage = STAGE_EX;
                    std::string op = ps.EX.instr.opcode;
                  
                    // std::cout<< ps.IF.instr.opcode<<endl;
          
                    
                    // std::cout << "Core " << cid << " - ID: Decoding instruction "
                    //  << instrToString(ps.ID.instr) << std::endl;
                    if (latencyConfig.find(op) != latencyConfig.end())
                        ps.EX.remainingLatency = latencyConfig[op] - 1;
                    else
                        ps.EX.remainingLatency = 0;
                    ps.ID.valid = false;
                }
            }

            // --- Fetch Stage (IF) ---
            // IF Stage: Fetch instruction and load into IF register if conditions are met.
            if (fetchedValid)
            {
                //
                if (ps.localPC == fetchedInstr.index)
                {
                   // std::cout<<"Fetched"<<std::endl;

                    // Only load a new instruction into IF if IF is not already occupied.
                    if (!ps.IF.valid&& !ps.waitingForSync)
                    {
                        ps.IF.valid = true;
                        ps.IF.instr = fetchedInstr;
                        ps.IF.instr.index = fetchedInstr.index;
                        ps.IF.stage = STAGE_IF;
                        // Simulate cache latency for instruction fetch.
                        uint32_t instrAddress = fetchedInstr.index * 4;
                        int latency = memory.loadinstructionWord(instrAddress);
                        // Set remainingLatency in IF so that the instruction waits here.
                        ps.IF.remainingLatency = (latency > 0 ? latency - 1 : 0);
                        std::string op = ps.IF.instr.opcode;
                        

                    }
                    else
                    {
                    }
                }
                else
                {
                    // The fetched instruction index does not match the expected one.
                    ps.IF.valid = false;
                    // ps.stallCount++;
                    if (!stallCounted && ps.localPC < program.size())
                    {
                        ps.stallCount++;
                        stallCounted = true;
                    }
                }
            }
            // Advance IF -> ID if the ID stage is free.
            if (ps.IF.valid)
            {
                if (ps.IF.remainingLatency > 0&& !ps.waitingForSync)
                {
                    // Decrement the fetch latency counter; do not move to ID until it reaches 0.
                    ps.IF.remainingLatency--;
                    ps.stallCount++;
                }
                else if (!ps.ID.valid&& !ps.waitingForSync)
                {
                    std::string op = ps.IF.instr.opcode;
                    if (op == "sync")
                    {                   // cout<<"CLOCK CYCLE: "<<clockCycles<<endl;
                       // std::cout << "Core " << cid << " - ID: Fetched  instruction  moved to ID "
                            //      << instrToString(ps.IF.instr) << clockCycles << std::endl;
                    }
                    ps.ID = ps.IF;
                    ps.ID.stage = STAGE_ID;
                   
                    ps.IF.valid = false;
                    ps.localPC++;
                }
                else
                {
                    // ID stage is busy so the instruction stays in IF; count stall.
                    // ps.stallCount++;
                    if (!stallCounted)
                    {
                        ps.stallCount++;
                        stallCounted = true;
                        // std::cout << "Core " << cid << " - IF: ID stage busy; stall counted" << std::endl;
                    }
                }
            }
        }
        clockCycles++;
        // std::cout << "---- End of Cycle " << clockCycles << " ----" << std::endl;
    }
    // Check if all pipelines are empty and no new instructions will be fetched.
    bool pipelinesEmpty()
    {
        for (int cid = 0; cid < 4; cid++)
        {
            PipelineState &ps = pipelineStates[cid];
            if (ps.ID.valid || ps.EX.valid || ps.MEM.valid || ps.WB.valid)
                return false;
            if (ps.localPC < (int)program.size())
                return false;
        }
        return true;
    }

    void run()
    {
        while (!pipelinesEmpty())
        {
            cycle();
        }
        memory.update();

        

        
       
        memory.printCompleteMemoryContents();
         std::cout << "-------------------------------------------------------------" << std::endl;

        std::cout << "\n================== FINAL STATE OF REGISTERS ==================\n"
                  << std::endl;
        for (int i = 0; i < 4; i++)
        {
            std::cout << "Core " << i << " Registers:" << std::endl;
            cores[i].printRegisters();
            std::cout << "-------------------------------------------------------------" << std::endl;
        }
        std::cout << "\n================== FINAL SIMULATION RESULTS ==================\n"
                  << std::endl;

        std::cout << "\n====================================\n"
                  << std::endl;
        std::cout << "Total Clock Cycles: " << clockCycles << std::endl;
for (int cid = 0; cid < 4; cid++)
        {
            int committed = pipelineStates[cid].completedInstr;
            int stalls = pipelineStates[cid].stallCount;
            double ipc = (clockCycles > 0) ? static_cast<double>(committed) / clockCycles : 0;

            std::cout << "Core " << cid << " Final Results:" << std::endl;
            std::cout << "   Instructions Committed  : " << committed << std::endl;
            std::cout << "   Stall Cycles            : " << stalls << std::endl;
            std::cout << "   IPC (Instructions/Cycle): " << ipc << std::endl;
        }
        /*std::cout << "\n================== FINAL STATE OF MEMORY =====================\n"
                  << std::endl;
        for (int i = 0; i < 4; i++)
        {
            // std::cout << "Memory for Core " << i << ":" << std::endl;
            std::cout << "-------------------------------------------------------------" << std::endl;

            memory.printMem(i);
        }*/
        memory.printSPM();
     //memory.printCompleteMemoryContents();
    }
};

int main()
{
    Simulator simulator("program.s");
    simulator.run();
    return 0;
}
