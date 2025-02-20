# RISC-V Multi-Core Simulator

## Overview
This project is a multi-core simulator inspired by [Ripes](https://github.com/mortbopet/Ripes). The simulator models a **4-core RISC-V processor**, that execute instructions concurrently while sharing memory. This is **Phase 1** of the project, currently developed in **Python and C++**, with a future goal to extend one based on feasibility and convert the cores into GPU cores.

### Steps to Run
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
  - Graphical User interface in python version.
- **Execution Model**:
  - Reads assembly file.
  - Executes instructions across all cores.
  - Displays **final register and memory state**.

## Bubble Sort Implementation
The simulator is capable of running a **bubble sort program** across all cores.


## Meeting Minutes
### **Meeting 1 (19-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Finalize the README file for submission .
  - Ensure all instructions execute correctly without errors.
  - Perform a final review of the documentation and code.
  - Validate instruction parsing and execution one last time.
  - All the tasks should be completed before 20-Feb-2025 & submit the project.
- **Tasks**:(done by 20-Feb-2025)
  - Satish:
     -  Made the README file and ensured clarity .
     -  Performed final testing to confirm everything works as expected .
       
     -  Submitted the Project
  - Niharika:
     - Revalidated instruction parsing and execution .
     - Perform a final review of the code and documentation.
     - Submitted the Project

### **Meeting 1 (18-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Add GUI for Python Version.(18-Feb-2025)
  - Implement the Simulator Class in python.
  - Test the excecution of BubbleSort in both version.
  - Final Error solving & reviewing code in both version
- **Next Steps**:
  - Satish:
       - Implement the Simulator Class in python (by 16-Feb-2025)
       - Test the excecution of BubbleSort in both version(by 17-Feb-2025)
  - Niharika:
       - Add GUI for Python Version(by 17-Feb-2025)
       - Final Error solving & reviewing code in both versions(by 17-Feb-2025)
 
### **Meeting 1 (17-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to test all instructions implemented till now with test cases.
  - Implement the core Class in Python.
  - Fix the parsing issuess  .data and  .text sections in assembly file reading.
  - Implement the code for bubbleSort in RISCV to test.
- **Next Steps**:
  - Satish:
       - Implement the Core Class in python (by 16-Feb-2025)
       - Worked on assembly code of BubbleSort (by 17-Feb-2025)      
  - Niharika:
       - Fix the parsing issuess  .data and  .text sections in assembly file reading(by 17-Feb-2025)
       - Testing of all instructions implemented till now with test cases(by 17-Feb-2025)
  
### **Meeting 1 (15-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to also make simluator in python and extend it based on feasibility in phase2.
  - Implement the logic to handle data Section in assembly file(CPP)
  - Implement the logic to handle labels in assembly file.(CPP)
  - Add the additional instructions BEQ,MV in CPP version.
- **Tasks**:
  - Satish:
       - Implement the .data section handling in CPP (by 16-Feb-2025)
       - Reimplement the Memory class in Python(by 17-Feb-2025)
  - Niharika:
       - Implement the labels  handling in CPP(by 17-Feb-2025)
       - Add the additional instructions BEQ,MV. (by 16-Feb-2025)

### **Meeting 1 (13-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to switch from hexcodes to simpler decoding based on words of instruction.
  - Add and implement the instructions SW,BLE,J,JALR,JAL in new format.
  - Reimplement the previous instructions(hexcodes format) to normal decoding and execution.
- **Tasks**:
  - Satish:
       - Reimplement all the previous instructions in the new format.(by 11-Feb-2025)
       - Add and implement the instructions SW,BLE.(by 11-Feb-2025)
  - Niharika:
       - Implement the execution of instructions J,JALR,JAL.(by 11-Feb-2025)
       - Implement the proper reading of .text in Assembly file.(by 11-Feb-2025)
### **Meeting 1 (11-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  -Decided to Implemement the simulator class .
  -Implement the basic Primary Instructions ADD,SUB,LW,ADDI,BNE,JAL.
  -Implement the logic for parsing and reading from Assembly file.
- **Next Steps**:
   - Satish:
       - Implement the Simulator class .(by 11-Feb-2025)
       - Implement the execution of instructions ADD,SUB,LW.(by 11-Feb-2025)  
  - Niharika:
       - Implement the execution of instructions ADDI,BNE,JAL.(by 11-Feb-2025)
       - Implement the logic for parsing and reading from Assembly File.(by 11-Feb-2025)

### **Meeting 1 (9-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  -Use hex codes for instruction decoding.
  -Implement basic Core, Memory, and Instruction classes.
  -Decoding hex codes into instructions.
- **Next Steps**:
   - Satish:
       - Implement basic Core, Memory, and Instruction classes.(by 11-Feb-2025)
       - Implement the logic for fetching instructions & execution of decoded instructions .(by 11-Feb-2025)
  - Niharika:
       - Implement the parsing of instructions &  converting them to hexcodes .(by 11-Feb-2025)
       - Implemente encoding & Decoding of hexcodes to match corresponding instructions.(by 11-Feb-2025)

  
### **Meeting 1 (7-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
-Decided to use C++ for implementation of Simulator.
-Designed the initial project structure and identified necessary files.
-Analyzed the given project architecture (RISCV) for better understanding.
-Decided on the set of basic instructions to be implemented in Phase 1.
-Assigned initial tasks  for development.
- **Next Steps**:
  - Satish:
       - Create a github Repo and intial project files setup.(by 9-Feb-2025)
       - Design the classes needed for implementation.(by 9-Feb-2025)    
   - Niharika:
       - Study how cores and memory interact, instruction handling, and assembly parsing.(by 9-Feb-2025)
       - Understand the execution logic for basic instructions.(by 9-Feb-2025)




