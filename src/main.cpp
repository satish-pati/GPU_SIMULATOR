#include "../include/core.h"
// #include "Memory.h"
#include "../include/memory.h"
#include <thread>
#include <iostream>
#include <vector>

void executeCore(Core &core, int coreID,int n) {
    std::cout << "\n[Core " << coreID << "] Initial Register State:" << std::endl;
    for (int i = 0; i < 31; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }

    std::cout << "\n[Core " << coreID << "] Executing Instructions...\n" << std::endl;
    
    core.run(n);  

    std::cout << "\n[Core " << coreID << "] Final Register State:" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }
}
int main() {
    Memory memory;
    std::vector<uint32_t> program = {
      //  0xFE209CE3,// BNE x1, x2, -8
        0x002081b3, // ADD x3, x1, x2  (x3 = x1 + x2)
        0x40208233, // SUB x4, x1, x2  (x4 = x1 - x2) 
        0x08432423,//sw x4, 136(x5)
        0x00432283, // lW x5, 4(x6)   
        0x00412283 , // LW x5, 4(x2)    (Load x5 from address x2+4)
        0x0080006F,
        0x00518193,
        0x00518193,
        0x3E800F93,
        0x00A08293,
        0x02A28293,
        0x01033063,

    };
    // Load instructions into memory for all cores
for (int core = 0; core < 4; core++) {
    for (size_t i = 0; i < program.size(); i++) {
        memory.storeWord(core * 1024 + i * 4, program[i]);
    }
}
    // Create 4 cores with shared memory
    Core cores[4] = { Core(memory, 0), Core(memory, 1), Core(memory, 2), Core(memory, 3) };
    // Initialize Registers for Each Core
    for (int i = 0; i < 4; i++) {
        cores[i].registers[1] = 10; // x1 = 10
        cores[i].registers[2] = 3088;//at adreess 3084- 4268675 (instrns) is stored 
         cores[i].registers[6] = 3088;//STORED RANDOM ADDRESS IN X6

    }
    for (int i = 0; i < 4; i++) {
    executeCore(cores[i], i,program.size());  // Run cores 
}
    return 0;
}
