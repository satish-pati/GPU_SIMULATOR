#include "../include/core.h"
// #include "Memory.h"
#include "../include/memory.h"
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
void executeCore(Core &core, int coreID,int n,const std::vector<std::tuple<std::string, int, int, int, int>>& instructions) {
    std::cout << "\n[Core " << coreID << "] Initial Register State:" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }

    std::cout << "\n[Core " << coreID << "] Executing Instructions...\n" << std::endl;
    
    for (const auto& inst : instructions) {
        core.execute(std::get<0>(inst), std::get<1>(inst), std::get<2>(inst), std::get<3>(inst), std::get<4>(inst));
    };  

    std::cout << "\n[Core " << coreID << "] Final Register State:" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "[Core " << coreID << "] x" << i << " = " << core.registers[i] << std::endl;
    }
}

int main() {
    Memory memory;
    std::vector<std::tuple<std::string, int, int, int, int>> program = loadProgramFromFile("program.s");
    Memory sharedMemory; // 4KB total, 1KB per core
    Core cores[4] = { Core(sharedMemory, program, 0), 
                      Core(sharedMemory, program, 1),
                      Core(sharedMemory, program, 2), 
                      Core(sharedMemory, program, 3) };
    for (int i = 0; i < 4; i++) {
        executeCore(cores[i], i, program.size(),program);
    }

    return 0;
}
