#include "../include/core.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>

Core::Core(Memory &memRef, const std::vector<std::tuple<std::string, int, int, int, int, std::string>> &prog, int coreId)
    : memory(memRef), program(prog), coreID(coreId)
{
    reset();
}
//function for printing reg values
void Core::printRegisters() const {
    std::cout << "\nRegister Dump for Core " << coreID << ":\n";
    for (int i = 0; i < 33; i++) {
        std::cout << "x" <<std::dec<< i << " = " << registers[i] << std::endl;
    }
}
//function for arithmetic operations
int Core::ALUOperation(const std::string &instruction, int rs1Val, int rs2Val, int imm) {
    if (instruction == "add" || instruction == "ADD") {
        return rs1Val + rs2Val;
    } else if (instruction == "sub" || instruction == "SUB") {
        return rs1Val - rs2Val;
    } else if (instruction == "mul" || instruction == "MUL") {
        return rs1Val*rs2Val;
    } 
    else if (instruction == "addi" || instruction == "ADDI") {
        return rs1Val + imm;
    } else if (instruction == "mv" || instruction == "MV") {
        return rs1Val;
    }
    return 0;
}

// function for checking branch operations
bool Core::isBranchTaken(const std::string &instruction, int rs1Val, int rs2Val,int rdVal,bool bnenum) {
    if (instruction == "beqcid" || instruction == "BEQCID" ) {
       
            if(coreID == rs2Val)
            {
               
                return true;
            }
            return false;
        }
        else if (instruction == "bnecid" || instruction == "BNECID" ) {
       
            if(coreID != rs2Val)
            {
               
                return true;
            }
            return false;
        }
        else if (instruction == "blecid" || instruction == "BLECID" ) {
       
            if(coreID <= rs2Val)
            {
               
                return true;
            }
            return false;
        }
        else if (instruction == "bgecid" || instruction == "BGECID" ) {
       
            if(coreID >= rs2Val)
            {
               
                return true;
            }
            return false;
        }
        else if (instruction == "bltcid" || instruction == "BLTCID" ) {
       
            if(coreID < rs2Val)
            {
                
                return true;
            }
            return false;
        }

    
   else  if (instruction == "bne" || instruction == "BNE") {
        
        
            if(rs1Val!=rs2Val)
            {
                
                return true;
            }
            return false;

            
    } else if (instruction == "beq" || instruction == "BEQ") {
       
        
            if(rs1Val==rs2Val)
            {
                return true;
            }
            return false;

            
        }
     else if (instruction == "ble" || instruction == "BLE") {
        
        if(rs1Val<=rs2Val)
        {
            return true;
        }
        return false;

       
    } else if (instruction == "blt" || instruction == "BLT") {
        
        if(rs1Val<rs2Val)
        {
            return true;
        }
        return false;

        
       
    } else if (instruction == "bge" || instruction == "BGE") {
        if(rs1Val>=rs2Val)
        {
            return true;
        }
        return false;

       
    }
    return false;
}


// Helper fucntion for writing back
void Core::writeBack(int rd, int value) {
    if (rd == 0) {
        return;
    }
    if (rd == 32) {
        return;
    }
    registers[rd] = value;
}

//Helper function for reading
int Core::readRegister(int reg) {
    if (reg == 0)
        return 0;
    if (reg == 32)
        return coreID;
    return registers[reg];
}

 // Keep core ID in x32
void Core::reset() {
    std::fill(std::begin(registers), std::end(registers), 0);
    registers[32] = coreID;
}
