# RISC-V Multi-Core Simulator

## Overview
This project is a multi-core simulator inspired by [Ripes](https://github.com/mortbopet/Ripes). The simulator models a **4-core RISC-V processor**, that execute instructions concurrently while sharing memory. This is **Phase 1** of the project, currently developed in **Python and C++**, with a future goal to extend one based on feasibility and convert the cores into GPU cores.

### Steps to Run
## Python Version
1. Clone the repository:
   ```sh
   git clone https://github.com/satish-pati/SIMULATOR_GROUP_28.git
   
   cd python/src
   ```
2. Compile and run:
   ```sh
   python sim.py  
   ```
3. Provide an **assembly file** named **program.s** as input.(currently **program.s** has bubblesort algorithm)
4. View **register and memory outputs** after execution.
## CPP Version
1. Clone the repository:
   ```sh
   git clone https://github.com/satish-pati/SIMULATOR_GROUP_28.git
   
   cd src
   ```
2. Compile and run:
   ```sh
   g++ -o simulator core.cpp memory.cpp sim.cpp Instruction.cpp
   
   ./simulator
   ```
3. Provide an **assembly file** named **program.s** as input.(currently **program.s** has bubblesort algorithm)
4. View **register and memory outputs** after execution

## Features
- **4-Core Simulation**: Each core operates independently but shares memory.
- - **Memory Architecture**:
  - **4KB memory**, divided equally among cores.
  - Core 0 → First 1KB, Core 1 → Next 1KB, etc.
- **Supported RISC-V Instructions**:
  - `ADD`, `SUB`
  - `BNE`, `JAL`
  - `LW`, `SW`
  - **Additional instruction of choice**
  - `ADDI`,`MV`
  - `BEQ`,`BLE`
  - `J`,`JALR`
- **Additional (Optional) Features**:
  - Graphical interface in python version.
- **Execution Model**:
  - Reads assembly file.
  - Executes instructions across all cores.
  - Displays **final register and memory state**.

## Bubble Sort Implementation
The simulator is capable of running a **bubble sort program** across all cores.


## Meeting Minutes
### **Meeting 1 (28-Jan-2025)**
- **Members**: Alice, Bob, Charlie, Dave
- **Decisions**:
  - Chose Python for implementation.
  - Decided to support only basic instructions in Phase 1.
- **Tasks**:
  - Alice: Memory model implementation (Deadline: 5-Feb-2025)
  - Bob: Instruction parsing and execution (Deadline: 7-Feb-2025)
  - Charlie: Register implementation (Deadline: 6-Feb-2025)
  - Dave: Assembly file reading (Deadline: 8-Feb-2025)
- **Next Steps**:
  - Integrate components.
  - Run test cases.


