# RISC-V Multi-Core Simulator

## Overview
This project is a multi-core simulator inspired by [Ripes](https://github.com/mortbopet/Ripes). The simulator models a **4-core RISC-V processor**, that execute instructions concurrently while sharing memory. This is **Phase 1** of the project, currently developed in **Python and C++**, with a future goal to extend one based on feasibility and convert the cores into GPU cores.


---



![Screenshot 2025-02-20 155636](https://github.com/user-attachments/assets/ebebad1a-c1d5-48b3-86e6-b05d03200218)

---



![Screenshot 2025-02-20 175130](https://github.com/user-attachments/assets/f4c66400-72e1-4c78-b66a-193cfb8043fe)




### Steps to Run
## Python Version
1. Clone the repository:
   ```sh
   git clone https://github.com/satish-pati/SIMULATOR_GROUP_28.git
   
   ```
    ```sh
      cd python/src
     ```

2. Compile and run:
   ```sh
   python gui.py  
   ```
    or
   
   ```sh
   python sim.py  
   ```
4. Provide an **assembly file** named **program.s** as input.(currently **program.s** has bubblesort algorithm)
5. View **register and memory outputs** after execution.


## CPP Version
1. Clone the repository:
   ```sh
   git clone https://github.com/satish-pati/SIMULATOR_GROUP_28.git
   
   ```
    ```sh
      cd src
      ```

2. Compile and run:
   ```sh
   g++ -o simulator core.cpp memory.cpp sim.cpp Instruction.cpp

   or
   
   g++ -static -o simulator.exe core.cpp memory.cpp sim.cpp Instruction.cpp
    
   ./simulator
   ```
3. Provide an **assembly file** named **program.s** as input.(currently **program.s** has bubblesort algorithm)
4. View **register and memory outputs** after execution
 

## Features
- **4-Core Simulation**: Each core operates independently but shares memory.
- - **Memory Architecture**:
  - **4KB memory**, divided equally among cores.
  - Core 0 → First 1KB, Core 1 → Next 1KB, Core 2 → Next 1KB,Core 3 → Next 1KB
- **Supported RISC-V Instructions**:
  - `ADD`, `SUB`
  - `BNE`, `JAL`
  - `LW`, `SW`
  - **Additional instruction of choice**
  - `ADDI`,`MV`
  - `BEQ`,`BLE`
  - `BLT`,`LA`
  - `J`,`BGE`,
- **Additional (Optional) Features**:
  - Graphical User interface in python version.
- **Execution Model**:
  - Reads assembly file.
  - Executes instructions across all cores.
  - Displays **final register and memory state**.

## Bubble Sort Implementation
The simulator is capable of running a **bubble sort program** across all cores.


## Meeting Minutes
### **Meeting 9 (20-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Finalize the README file for submission .
  - Ensure all instructions execute correctly without errors.
  - Add Instructions BGE,BLT in both versions
  - Perform a final review of the documentation and code.
  - Validate instruction parsing and execution one last time.
  - All the tasks should be completed before 20-Feb-2025 & submit the project.
- **Tasks**:(done by 20-Feb-2025)
  - Satish:
     -  Made the README file and ensured clarity .
     -  Added instructions BGE,BLT in both versions & Performed final testing to confirm everything works as expected .
     -  Submitted the Project
  - Niharika:
     - Revalidated instruction parsing and execution .
     - Perform a final review of the code and documentation.
     - Submitted the Project

### **Meeting 8 (19-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Modify UI for Python version.
  - Test the excecution of BubbleSort in Python version.
  - Final Error solving & reviewing code in Python version
  - Fix parsing issues in  python vesrion.
  - Fix some logical errors in cpp Version

- **Tasks**:
  - Satish:
     - Modifiy GUI for Python version .( by 20-Feb-2025)
     - Fix errors in CPP version && Test the excecution of BubbleSort & other instructions in Python version.( by 20-Feb-2025)
  - Niharika:
     - Fix parsing issues in  python version  .( by 20-Feb-2025)
     - Final Error solving & reviewing code in Python version( by 20-Feb-2025)
     
### **Meeting 7 (18-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Add GUI for Python Version.
  - Implement the Simulator Class in python.
  - Test the excecution of BubbleSort in CPP version.
  - Final Error solving & reviewing code in CPP version
- **Tasks**:
  - Satish:
       - Implement the Simulator Class in python (by 19-Feb-2025)
       - Test the excecution of BubbleSort in CPP version(by 19-Feb-2025)
  - Niharika:
       - Add  GUI for Python Version(by 19-Feb-2025)
       - Final Error solving & reviewing code in CPP versions(by 19-Feb-2025)
 
### **Meeting 6 (17-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to test all instructions implemented till now with test cases.
  - Implement the core Class in Python.
  - Fix the parsing issuess  .data and  .text sections in assembly file reading.
  - Implement the code for bubbleSort in RISCV to test.
- **Tasks**:
  - Satish:
       - Implement the Core Class in python (by 18-Feb-2025)
       - Work on assembly code of BubbleSort (by 18-Feb-2025)      
  - Niharika:
       - Fix the parsing issuess  .data and  .text sections in assembly file reading(by 18-Feb-2025)
       - Testing of all instructions implemented till now with test cases(by 18-Feb-2025)
  
### **Meeting 5 (15-Feb-2025)**
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

### **Meeting 4 (13-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to switch from hexcodes to simpler decoding based on words of instruction.
  - Add and implement the instructions SW,BLE,J,JALR,JAL,LA in new format.
  - Reimplement the previous instructions(hexcodes format) to normal decoding and execution.
- **Tasks**:
  - Satish:
       - Reimplement all the previous instructions in the new format.(by 15-Feb-2025)
       - Add and implement the instructions SW,BLE,LA.(by 15-Feb-2025)
  - Niharika:
       - Implement the execution of instructions J,JALR,JAL.(by 15-Feb-2025)
       - Implement the proper reading of .text in Assembly file.(by 15-Feb-2025)
### **Meeting 3 (11-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to Implemement the simulator class .
  - Implement the basic Primary Instructions ADD,SUB,LW,ADDI,BNE,JAL.
  - Implement the logic for parsing and reading from Assembly file.
- **Tasks**:
   - Satish:
       - Implement the Simulator class with header file & connect Cores ,memory and simulator class. .(by 13-Feb-2025)
       - Implement the execution of instructions ADD,SUB,LW.(by 13-Feb-2025)  
  - Niharika:
       - Implement the execution of instructions ADDI,BNE,JAL.(by 13-Feb-2025)
       - Implement the logic for parsing and reading from Assembly File.(by 13-Feb-2025)

### **Meeting 2 (9-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Use hex codes for instruction decoding.
  - Implement basic Core, Memory, and Instruction classes.
  - Decoding hex codes into instructions.
- **Tasks**:
   - Satish:
       - Implement the Core, Memory, and Instruction classes & Header files .(by 11-Feb-2025)
       - Implement the logic for fetching instructions & execution of decoded instructions .(by 11-Feb-2025)
  - Niharika:
       - Implement the parsing of instructions &  converting them to hexcodes .(by 11-Feb-2025)
       - Implemente encoding & Decoding of hexcodes to match corresponding instructions.(by 11-Feb-2025)

  
### **Meeting 1 (7-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
- Decided to use C++ for implementation of Simulator.
  - Designed the initial project structure and identified necessary files.
  - Analyzed the given project architecture (RISCV) for better understanding.
  - Decided on the set of basic instructions to be implemented in Phase 1.
  - Assigned initial tasks  for development.
- **Tasks**:
  - Satish:
       - Create a github Repo and intial project files setup.(by 9-Feb-2025)
       - Design and understand the Core,memory,Simulator classes needed for implementation.(by 9-Feb-2025)    
   - Niharika:
       - Study how cores and memory interact, instruction handling, and assembly parsing.(by 9-Feb-2025)
       - Understand the execution logic for basic instructions.(by 9-Feb-2025)
---
# PHASE-2
---
**Note** : we decided to continue development only in c++    language. `.cpp`

### Features
---
#### Pipelining with 5 stages:

- **Instruction Fetch (IF)**: Fetches the instruction from memory. Global fetch unit shared by all cores, each core fetches instructions simultaneously.
- **Instruction Decode (ID)**: Decodes the instruction, reads register values, and prepares operands.
- **Execute (EX)**: Performs arithmetic, logical and  memory address calculations.
- **Memory Access (MEM)**: Handles loading and storing data by interacting with shared memory.
- **Write Back (WB)**: Stores the computed results back into registers.

#### Data Forwarding :
- Eliminates unnecessary stalls by forwarding results from later pipeline stages directly to earlier ones.
- Ensures that dependent instructions do not have to wait for data to be written back.

#### Hazard Management:

- Data Hazards: Fully resolved using data forwarding, reducing performance penalties.
- Stall cycles introduced only when absolutely necessary, ensuring minimal delays.
#### Configurable Instruction Latencies:

- Each arithmetic instruction can be assigned a custom execution cycle count.Example: ADD = 1 cycle, MUL = 3 cycles, ensuring flexibility in execution modeling.

#### Merged Compute Units for Efficiency:

- A single global fetch unit services all cores, ensuring synchronized instruction execution.
- Each core maintains its own independent decode, execute, memory, and writeback stages.
- This setup balances resource sharing with parallel execution efficiency.

#### CID-Based Conditional Execution (Only for Branch Instructions):

- All cores fetch the same instruction, but branch execution depends on the Core ID (CID) register.
- Enables selective branching per core while maintaining a unified instruction stream.

#### Parallel Array Summation Across Cores:

- A 100-element array is divided across four cores.

- Each core computes a partial sum (first 25, next 25, etc.), with only Core 0 printing the final sum.

#### Instructions Implemented:

- `MUL`,`li`,`ecall`

## Meeting Minutes
---
### **Meeting 7 (10-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  -Finalize the README file for submission .
  - Ensure all instructions execute correctly without errors.
  - Verify the Excecution of test cases with sample assembly codes for correct Results.
  - Verification of Stalls,Clock cycles,IPC Caluculation manually.
  - Perform a final review of the documentation and code.
  - Validate instruction parsing and execution one last time.
- **Tasks**:
    ###  Satish:
  - Verification of Stalls,Clock cycles,IPC Caluculation manually.
  - Finalize the README file for submission.
  - Modification of Assembly code test case for submission.
  - Verify the Excecution of test cases with sample assembly codes for correct Results.
    ###  Niharika:
  - Ensure all instructions execute correctly without errors.
  - Perform a final review of the documentation and code.
  - Validate instruction parsing and execution one last time.
  - Debugging and fixing of errors.
       
### **Meeting 6 (9-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Implementation of `MUL` intruction as it is needed.
  - Modifying Hazards detection,data forwarding & without forwarding to work with latencies.
  - Implemenatation of `li` and `ecall` Instructions.
  - Modifying ReadmeFile with all information For phase2
  - Implemenation of Caluculation of final results .
- **Tasks**:
    ### Satish:
    - Modifying Hazards detection,data forwarding & without forwarding to work with latencies.
    - Implemenation of Caluculation of final results.
    - Implementation of `MUL` intruction as it is needed.
      ### Niharika:
      - Implemenatation of `li` and `ecall` Instructions.
      - Assembly code Implementation for  - for loop execution as specified. .
      - Modifying ReadmeFile with new information For phase2.
### **Meeting 5 (8-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  -Branch Instruction Implementation with CID specification.
  -Implementation fo Global PC and  local PC for cores and handle takeup correct Instructions across all Cores.
  -Implementation of shared data & Instruction memory as per Phase 2 requirements.
  -Implemented user-configurable instruction latencies for arithmetic Instructions.
  -Assembly code Implementation for for loop execution as specified.
- **Tasks**:
     ###  Satish:
  - Implementation of Hazards detection with data forwarding.
  - Implemented user-configurable instruction latencies for arithmetic Instructions.
  - Implementation fo Global PC and  local PC for cores and handle takeup correct Instructions across all Cores.
      ### Niharika:
  - Branch Instruction Implementation with CID specification
  - Implementation of shared data & Instruction memory as per Phase 2 requirements.
  - Debugging and fixing of errors.

    
       

### **Meeting 4 (6-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Implementaion of Data forwarding.
  - Modifying the stages accoridngly whether to use forwarding or not.
  - caluculation of stalls for cycles affected by hazards.
  - Checking if all present instructions are working with and without forwarding.
- **Tasks**:
    ### Satish:
  - Modification of pipeline with and without Data forwarding.
  - Implementation of all Instructions as per pipeline.
  - Modifying the pipeline stages transitions based on whether to use forwarding or not.
     ###  Niharika:
  - Implementation  of caluculation stalls for cycles affected by hazards.
  - Ensure proper instruction fetching for each core.
  - Checking if all present instructions are working with and without forwarding.
       
### **Meeting 3 (4-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Implemenatation of hazard detection.
  - Checking if hazards are correctly detected manually.
  - Modify pipeline Stages to check for Hazards and insert Stalls.
  - Implemenation of Single fetch Unit and pipeline Stages accordingly.
  - Implementation of all Instructions as per pipeline.


- **Tasks**:
     ### Satish:
  - Implement hazard detection logic for different types of hazards.
  - Modify pipeline execution flow to insert stalls when needed.
  - Implementation the instructions as per pipeline new structure.
       
    ### Niharika:
  - Implement memory access handling logic.
  - Implementation of Branch and jump instructions as per new design. 
  - Checking if hazards are correctly detected manually.
    

### **Meeting 2 (2-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Implemenation of Basic Structure of  State of pipeline Using PipelineRegsiters.
  - Implemenatation of pipeline with stages  `IF`,`ID`,`EX`,`MEM`,`WB`.
  - Modify core class so that it suits with pipelining .
  - Understand and study about how pipelining ,Hazards,latencies works.

- **Tasks**:
   ### Satish:
   - Implemenatation of pipeline with stages  `IF`,`ID`,`EX`,`MEM`,`WB`.
   - Implement pipeline control logic for stage transitions.
   - Modify Core class and execution of cores to work with Pipelining.
      
   ###  Niharika:
  - Design and implement the fetch unit into the pipeline.
  - Modify the instruction parsing as per pipeline.
  - Debug and test pipeline implementation.
  
       
  
### **Meeting 1 (28-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to just focus on implementation using `cpp`.
  - analyzed the complete phase 2 project details and decided on how to proceed.
  - Transforming phase 1 files to be prepared for extension and enhancement in phase 2.
  - Implementation of basic Structure of PipelineRegisters.
  - Modifying instruction parsing using proper Stucture required for Pipelining Stages.
  - Assigned  tasks and deadlines for development.
- **Tasks**:
   ### Satish:
  - Design and Implementation of basic pipeline architecture.
  - Implementation of Pipeline Registers.
  - Understanding and study about how pipelining, hazards and latencies works.
   ### Niharika:
   - Transform the phase 1 files to be prepared for extension and enhancement in phase 2.
   - Study about pipelining, hazards and latencies works.
      



