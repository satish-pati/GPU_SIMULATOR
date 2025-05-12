# RISC-V Multi-Core Simulator

## Overview
This project is a multi-core simulator inspired by [Ripes](https://github.com/mortbopet/Ripes). The simulator models a **4-core RISC-V processor**, that execute instructions concurrently while sharing memory. This is **Phase 1** of the project, currently developed in **Python and C++**, with a future goal to extend one based on feasibility and convert the cores into GPU cores.



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
 
## Phase 3
#### Report : https://hackmd.io/@uUsQvP7mRR-VkcGLNOg8Zw/SJc9dOkWxe
# Test Cases 
# Part 1
## Assembly code Used 
```Riscv=

.data
arr: .word 1:10000
sums:   .word 0, 0, 0, 0
.text
main:
    addi x5,x0, 0             #x5 = sum[CID]
    addi x7,x0, 100        # count loop
    addi x10,x0,25    #count 
    la x6,arr     # base of array
    addi x31,x0,4          # bytes per word
    li  x20, 400
    mul x30,x20,x10
    mul x30,x30,x32
    add x30,x30,x6
outer_loop:
    addi x8,x0,0             # i = 0
    addi x11,x30,0
inner_loop:
    lw x12,0(x11)       # load a[i*X]
    add x5,x5,x12      #sum[CID] += a[i*X]
    addi x8,x8,1
    add  x11, x11, x20      # update pointer to next element position in the inner stride
    blt  x8,x10,inner_loop
    addi x7, x7, -1
    bne x7,x0, outer_loop
    #Save partial sum in memory
sum:
    la x14, sums
    addi x15,x0,4
    mul x16, x32, x15
    add x14, x14, x16
    sw x5,0(x14)
    sync
    #Combine if CID == 1
    bne cid, 1, end
    la x14, sums
    lw x5, 0(x14)
    lw x6, 4(x14)
    lw x7, 8(x14)
    lw x8, 12(x14)
    add x5, x5, x6
    add x5, x5, x7
    add x5, x5, x8
    sw x5,4(x14)
    # x5 now contains total sum
end:
    li x10,100
```

## Parameters given


```
Enter L1D Cache Size in bytes: 400
Enter L1I Cache Size in bytes: 400
Enter L2 Cache Size in bytes: 800
Enter Cache Block Size in bytes: 4
Enter Cache Associativity: 1
Enter L1 Cache Access Latency (cycles): 2
Enter L2 Cache Access Latency (cycles): 4
Enter Main Memory Access Time (cycles): 6
Enable data forwarding? (y/n): y
Enter latency for ADD (cycles): 1
Enter latency for SUB (cycles): 1
Enter latency for ADDI (cycles): 1
Enter latency for MUL (cycles): 1

```
## Testing of  a direct mapped vs equivalent SPM


### Algorithm 1: Implementation without using scratchpad memory.


```
      

otal Clock Cycles: 56195
Core 0 Final Results:
   Instructions Committed  : 12916
   Stall Cycles            : 45774
   IPC (Instructions/Cycle): 0.229843
Core 1 Final Results:
   Instructions Committed  : 12925
   Stall Cycles            : 45765
   IPC (Instructions/Cycle): 0.230003
Core 2 Final Results:
   Instructions Committed  : 12916
   Stall Cycles            : 45774
   IPC (Instructions/Cycle): 0.229843
Core 3 Final Results:
   Instructions Committed  : 12916
   Stall Cycles            : 45773
   IPC (Instructions/Cycle): 0.229843
-------------------------------------------------------------

------------------ CACHE STATISTICS ------------------
L1 Instruction Cache Miss Rate: 0.00985797      
L1 Data Cache Miss Rate       : 0.9995
L2 Unified Cache Miss Rate    : 1
------------------------------------------------------
```

### Algorithm 2: Implementation with using scratchpad memory.
## Assembly code Used 
```Riscv=
.data
arr:     .word 1:10000
sums:   .word 0, 0, 0, 0
.text
main:
    la x6, arr          # base of array
    li x14,25         # total loop count (moved earlier)
    li x20, 400          # stride
    li x13, 0            # SPM base index
    li x8, 0             # i = 0
    li x10,25
    mul x30,x20,x10
    mul x30,x30,x32
    add x30,x30,x6
    li x29,100
    mul x28,x32,x29
    mv x13,x28
    add x11, x30, x0     # addr = &array[i*X]
fill_loop:
    lw x12, 0(x11)
    sw_spm x12, 0(x13)
    addi x13,x13,4     # SPM pointer increment
    add x11, x11, x20  # array pointer increment
    addi x8, x8, 1     
    blt x8, x14, fill_loop
    li x5, 0             # sum[CID]
    li x7,100           # outer loop counter
outer_loop_spm:
    li x8,0             #i = 0
    mv x9,x28          #SPM address
inner_loop:
    lw_spm x12, 0(x9)
    add x5, x5, x12
    addi x9, x9, 4
    addi x8, x8, 1
    blt x8, x14, inner_loop
    addi x7, x7, -1
    bne x7, x0, outer_loop_spm
    # Save partial sum
    la x15, sums
    li x16, 4
    mul x17, x32, x16
    add x15, x15, x17
    sw x5,0(x15)
    sync
    # Combine if CID == 1
    bne cid, 1, end_spm
    la x15, sums
    lw x5, 0(x15)
    lw x6, 4(x15)
    lw x7, 8(x15)
    lw x8, 12(x15)
    add x5, x5, x6
    add x5, x5, x7
    add x5, x5, x8
    sw x5,4(x15)
end_spm:
    ecall
```
```
Total Clock Cycles: 31925
Core 0 Final Results:
   Instructions Committed  : 13072
   Stall Cycles            : 18897
   IPC (Instructions/Cycle): 0.40946
Core 1 Final Results:
   Instructions Committed  : 13081
   Stall Cycles            : 18888
   IPC (Instructions/Cycle): 0.409742
Core 2 Final Results:
   Instructions Committed  : 13072
   Stall Cycles            : 18897
   IPC (Instructions/Cycle): 0.40946
Core 3 Final Results:
   Instructions Committed  : 13072
   Stall Cycles            : 18896
   IPC (Instructions/Cycle): 0.40946
-------------------------------------------------------------

  
```

### Analysis

A direct-mapped cache works much slower than a scratchpad memory (SPM). Without SPM, the system takes 221,159 clock cycles to complete, whereas with SPM, it only takes 123,641 cycles—which is significantly faster. The IPC (Instructions Per Cycle) also increases from about 0.23 to 0.41, meaning the system performs more useful work in less time. The main reason for the poor performance of the direct-mapped cache is its very high miss rate, caused by strided memory access patterns. In such cases, multiple elements map to the same cache location, so the cache often can't find the needed data—resulting in nearly every data cache access missing. In contrast, SPM avoids these issues by allowing the programmer to manually manage data placement, ensuring faster access and better reuse of the needed data.



# Part 2
## Parameters given
```
Enter L1D Cache Size in bytes: 400
Enter L1I Cache Size in bytes: 400
Enter L2 Cache Size in bytes: 800
Enter Cache Block Size in bytes: 4
Enter Cache Associativity: 100
Enter L1 Cache Access Latency (cycles): 2
Enter L2 Cache Access Latency (cycles): 4
Enter Main Memory Access Time (cycles): 6
Enable data forwarding? (y/n): y
Enter latency for ADD (cycles): 1
Enter latency for SUB (cycles): 1
Enter latency for ADDI (cycles): 1
Enter latency for MUL (cycles): 1

```
## Testing of Fully associative vs equivalent SPM


### Algorithm 1: Implementation without using scratchpad memory.

```
Total Clock Cycles: 31445
Core 0 Final Results:
   Instructions Committed  : 12916
   Stall Cycles            : 21024
   IPC (Instructions/Cycle): 0.410749
Core 1 Final Results:
   Instructions Committed  : 12925
   Stall Cycles            : 21015
   IPC (Instructions/Cycle): 0.411035
Core 2 Final Results:
   Instructions Committed  : 12916
   Stall Cycles            : 21024
   IPC (Instructions/Cycle): 0.410749
Core 3 Final Results:
   Instructions Committed  : 12916
   Stall Cycles            : 21023
   IPC (Instructions/Cycle): 0.410749
-------------------------------------------------------------

------------------ CACHE STATISTICS ------------------
L1 Instruction Cache Miss Rate: 0.00985797      
L1 Data Cache Miss Rate       : 0.0103906       
L2 Unified Cache Miss Rate    : 1
------------------------------------------------------

```

### Algorithm 2: Implementation with using scratchpad memory.

```
Total Clock Cycles: 31925
Core 0 Final Results:
   Instructions Committed  : 13072
   Stall Cycles            : 18897
   IPC (Instructions/Cycle): 0.40946
Core 1 Final Results:
   Instructions Committed  : 13081
   Stall Cycles            : 18888
   IPC (Instructions/Cycle): 0.409742
Core 2 Final Results:
   Instructions Committed  : 13072
   Stall Cycles            : 18897
   IPC (Instructions/Cycle): 0.40946
Core 3 Final Results:
   Instructions Committed  : 13072
   Stall Cycles            : 18896
   IPC (Instructions/Cycle): 0.40946
-------------------------------------------------------------

  
```
### Analysis
The performance of the fully associative cache and the scratchpad memory (SPM) is almost the same. The system with the fully associative cache takes 31,445 clock cycles, while the one using SPM takes 31,925 cycles, which is a very small difference. Both setups also have a similar IPC of around 0.41, meaning they do about the same amount of useful work per cycle.  SPM gives similar performance by letting the programmer control data placement. Overall, in this case, both methods perform nearly equally well, so using a fully associative cache can be just as effective as using SPM.


# Part 2 
# Algorithm 1: Implementation without using scratchpad memory.
## Assembly code Used 
```Riscv=


.data
arr: .word 1:20000
sums:   .word 0, 0, 0, 0
.text
main:
    addi x5,x0, 0             #x5 = sum[CID]
    addi x7,x0, 100        # count loop
    addi x10,x0,50    #count 
    la x6,arr     # base of array
    addi x31,x0,4          # bytes per word
    li  x20, 400
    mul x30,x20,x10
    mul x30,x30,x32
    add x30,x30,x6
outer_loop:
    addi x8,x0,0             # i = 0
    addi x11,x30,0
inner_loop:
    lw x12,0(x11)       # load a[i*X]
    add x5,x5,x12      #sum[CID] += a[i*X]
    addi x8,x8,1
    add  x11, x11, x20      # update pointer to next element position in the inner stride
    blt  x8,x10,inner_loop
    addi x7, x7, -1
    bne x7,x0, outer_loop
    
    #Save partial sum in memory
sum:
    la x14, sums
    addi x15,x0,4
    mul x16, x32, x15
    add x14, x14, x16
    sw x5, 0(x14)
    sync
    #Combine if CID == 1
    bne cid, 1, end
    la x14, sums
    lw x5, 0(x14)
    lw x6, 4(x14)
    lw x7, 8(x14)
    lw x8, 12(x14)
    add x5, x5, x6
    add x5, x5, x7
    add x5, x5, x8
    sw x5,4(x14)
    # x5 now contains total sum
end:
    li x10,100
    

```



## Fully associative

```
Total Clock Cycles: 111195
Core 0 Final Results:
   Instructions Committed  : 25416
   Stall Cycles            : 90774
   IPC (Instructions/Cycle): 0.228571
Core 1 Final Results:
   Instructions Committed  : 25425
   Stall Cycles            : 90765
   IPC (Instructions/Cycle): 0.228652
Core 2 Final Results:
   Instructions Committed  : 25416
   Stall Cycles            : 90774
   IPC (Instructions/Cycle): 0.228571
Core 3 Final Results:
   Instructions Committed  : 25416
   Stall Cycles            : 90773
   IPC (Instructions/Cycle): 0.228571
-------------------------------------------------------------

------------------ CACHE STATISTICS ------------------
L1 Instruction Cache Miss Rate: 0.00581873      
L1 Data Cache Miss Rate       : 0.99975
L2 Unified Cache Miss Rate    : 1
------------------------------------------------------
```

# Algorithm 2: Implementation with using scratchpad memory.

## Assembly code Used 
```Riscv=

.data
arr:     .word 1:20000
sums:   .word 0, 0, 0, 0
.text
main:
    la x6, arr          # base of array
    li x14,25         # total loop count (moved earlier)
    li x20, 400          # stride
    li x13, 0            # SPM base index
    li x8, 0             # i = 0
    li x10,25
    mul x27,x20,x10
    mul x30,x27,x32
    add x30,x30,x6
    li x29,100
    mul x28,x32,x29
    mv x13,x28
    li x19,50
    add x11,x30,x0     #addr = &array[i*X]
    addi x26,x30,40000
fill_loop:
    lw x12, 0(x11)
    sw_spm x12, 0(x13)
    addi x13,x13,4     # SPM pointer increment
    add x11, x11, x20  # array pointer increment
    addi x8, x8, 1     
    blt x8, x14, fill_loop
    li x5, 0             # sum[CID]
    li x7,100           # outer loop counter
outer_loop_spm:
    li x8,0             #i = 0
    mv x9,x28          #SPM address
    mv x25,x26
inner_loop:
    blt x8,x10, lw_from_spm
    lw x12,0(x25)
    addi x25,x25,400
    addi x8, x8, 1
    j calsum
lw_from_spm:
    lw_spm x12, 0(x9)
    addi x9,x9,4
    addi x8, x8, 1
calsum:
    add x5, x5, x12
    blt x8,x19,inner_loop
    addi x7, x7, -1
    bne x7,x0, outer_loop_spm
    # Save partial sum
    la x15,sums
    li x16,4
    mul x17, x32, x16
    add x15, x15, x17
    sw x5,0(x15)
    sync
    # Combine if CID == 1
    bne cid, 1, end_spm
    la x15, sums
    lw x5, 0(x15)
    lw x6, 4(x15)
    lw x7, 8(x15)
    lw x8, 12(x15)
    add x5, x5, x6
    add x5, x5, x7
    add x5, x5, x8
    sw x5,4(x15)
end_spm:
    ecall


```
# Parameters Given
```

Total Clock Cycles: 87401
Core 0 Final Results:
   Instructions Committed  : 33174
   Stall Cycles            : 54535
   IPC (Instructions/Cycle): 0.379561
Core 1 Final Results:
   Instructions Committed  : 33183
   Stall Cycles            : 54526
   IPC (Instructions/Cycle): 0.379664
Core 2 Final Results:
   Instructions Committed  : 33174
   Stall Cycles            : 54535
   IPC (Instructions/Cycle): 0.379561
Core 3 Final Results:
   Instructions Committed  : 33174
   Stall Cycles            : 54534
   IPC (Instructions/Cycle): 0.379561
-------------------------------------------------------------


```
### Analysis

- At first in previous test case, it might look like scratchpad memory (SPM) and fully associative caches give similar performance when they’re the same size. 
- But from our new results, **Algorithm 2** (which uses SPM) clearly performs better than **Algorithm 1** (which uses a cache) in terms of speed and IPC.
- We doubled the number of elements being accessed to 200 but only store 100 in the SPM—**Algorithm 2** still does better.
- The main reason is that with SPM, the program controls exactly what gets stored, so every access hits the correct data with no guessing or data being replaced. 
- In this Testcase,we double our loop to touch 200 distinct elements but only preload 100 words into the SPM. Every access to those 100 words is a guaranteed hit, whereas a fully associative cache will begin evicting and reloading data once its capacity is exceeded—causing  misses even though it is **Fully associative**
-  This means that with a larger stride (200 elements), the cache’s miss rate rises sharply and stalls far more, while the SPM continues to deliver steady, predictable hits on its 100 preloaded entries. Thus, even under equal latency assumptions, SPM offers a clear performance edge for workloads whose access patterns can be partitioned to fit its manually managed storage.

# Latencies (and power requirements) of SPMs are far lower than caches

- Scratchpad memories (SPMs) are much faster and use less energy than regular caches because they are built in a simpler way. 
- They use basic memory blocks with one or two access ports and don’t include the extra circuits that caches need, like those for checking addresses or deciding what data to replace. 
- Caches must compare tags, keep track of sharing between processors, and run a replacement process every time data is accessed or loaded. These extra steps add more layers of logic, increase wiring needs, and use more power. 
- SPMs, on the other hand, are accessed using fixed addresses managed by the programmer or compiler, and these go straight to the memory with no need for comparisons or replacement logic. 
- This simple setup makes SPMs quicker (fewer steps and shorter delays) and more power-efficient (less unnecessary activity), so they perform better and save energy compared to caches of the same size.

# Implementation of SYNC 

## Sync Instruction Handling in Simulator

### Detection in EX Stage

When a core encounters a `sync` instruction in the **Execute (EX)** stage:

```cpp
if (ps.EX.instr.opcode == "sync") {
  if (!ps.waitingForSync) {
    ps.waitingForSync = true;
    syncCounter++;
  }
  if (syncCounter < 4) {
    ps.stallCount++;
    continue;  // core stalls here, stays in EX
  } else {
    // all cores have reached the barrier
    for (int j = 0; j < 4; j++) {
      pipelineStates[j].EX.valid = false;
      pipelineStates[j].waitingForSync = false;
    }
    syncCounter = 0;
    continue;  // release all cores
  }
}
```

- `ps.waitingForSync`: Ensures a core only contributes to `syncCounter` once.
- `syncCounter`: Global variable tracking how many cores have hit the barrier.
- If `syncCounter < 4`, the core **stalls in EX**, incrementing `stallCount`.
- Once `syncCounter == 4`, all cores:
  - Invalidate their EX stage (`EX.valid = false`)
  - Reset their `waitingForSync` flags
  - `syncCounter` is reset to 0

---

### Effect on Pipeline

- **Barrier semantics**: No core can move past `sync` until all others have reached it.
- The instruction **stays in EX** and does not advance through MEM/WB.
- On release:
  - All cores' EX stages are flushed
  - Program counters advance in **lockstep**
  - Next cycle: Each core fetches the instruction after `sync`

---

### Design Choices 

- ✅ Avoiding Fixed Delays
- The design **does not use hardcoded delays** like “stall for 100 cycles.”
- Instead, it **dynamically detects** when all cores arrive at the barrier:
  - Cores stall in EX until `syncCounter == num_cores`
- ✅ **Minimal pipeline changes**: Reuses EX stage logic (`valid`, `waitingForSync`).
- ✅ **Scalable**: `syncCounter` logic can extend to any number of cores.
- ✅ **Flush approach**: Ensures synchronized PC advancement after barrier.
- ✅**Zero-cost semantic**:
The sync does no arithmetic or memory access; it only performs the barrier. All register-and-cache side-effects are NOP.
- Ensured strong barrier behavior without introducing new pipeline complexity.

### Hardware Mapping of design 

## Hardware Relation for `sync` Instruction


- **Global Barrier Counter**  
  - Implemented as a simple register (`SYNC_COUNT`) that all cores can increment.  
  - Each core has a small “arrived” flag in its own status register to avoid double‐counting.

- **Per‐Core Waiting Flag**  
  - A single bit (`WAIT_FOR_SYNC`) in each core’s control register.  
  - Set when the core first executes `sync`, preventing repeated increments.

- **Stall Control in EX Stage**  
  - A hardware comparator checks: `if (SYNC_COUNT < NUM_CORES)`.  
  - If true, the core’s EX stage is held (its pipeline register enable is gated off).  
  - Stall signal increments a per‐core stall counter for performance stats.

- **Barrier Release Logic**  
  - Once `SYNC_COUNT == NUM_CORES`, a broadcast clears:  
    - All cores’ EX‐valid bits (flush the `sync` instruction)  
    - All `WAIT_FOR_SYNC` flags  
    - The `SYNC_COUNT` register  
  - A single clock edge resumes all EX stages together, advancing each PC in lockstep.

- **Minimal Extra Hardware**  
  - No new pipeline stages—just a few extra bits and simple comparator logic in EX.  
  - No timers or fixed delays—hardware stalls until the counter condition is met.

- **Scalability**  
  - Replace the single counter with a tree of counters or a ring network to support more cores without one hot‐spot register.
  - This hardware view shows how a small amount of extra control logic in the EX stage can implement a full barrier without major pipeline redesign. 




## Meeting Minutes
---

#### **Meeting 8 (May 12, 2025)**  
**Focus:** Final testing and reporting.  
**Members:** Satish, Niharika  
**Decisions:**  
- Perform comprehensive testing with all test cases.
- Compile final report with stalls, cache miss rate, and IPC.  

**Tasks :**  
- **Satish:**
   - Implemented new functions for latency cacaluclations `storeWordlatency`,`loadWordlatency`,`loadInstructionWordlatency` to correct Latency caluculation.
   - Wrote new test cases as per ProjectDoc and analysed results & performance .
   - Modified Simulator class and pipeline Implementation to correctly insert stalls due to the memory access Latencies.
   - Compiled results from testcases to Report &  updated readme file.
- **Niharika:**
  - readme file.
  - checked complete code for any errors.
  - checked with different test cases
---

#### **Meeting 7 (May 10, 2025)**  
**Focus:** Finalize SPM integration and test with remaining SPM test cases.  
**Members:** Satish, Niharika  
**Decisions:**  
- Complete SPM operations with cache and SYNC in the pipeline.  
- Test SPM, cache, and SYNC with `testcase3spm.s`.  

**Tasks :**  
- **Satish:**  
  - Modified `Simulator::run()` to ensure SPM operations are correctly simulated.  
  - Modified `Memory::loadWord()` to integrate SPM access with cache hierarchy.  
  - Modified `Memory::storeWord()` to integrate SPM access with cache hierarchy.  
  - Implemented `Memory::update()` to handle SPM and cache .
  - Modifed L1I cache to Correctly work and add latencies with the instruction fetch stage of pipeline
   
- **Niharika:**  
  - Modified `Simulator::cycle()` to ensure SPM, cache, and SYNC work together.   
  - Implemented `Memory::sstoreWord()` to support SPM test case setup.  
  - Implemented test case `testcase3spm.s` to verify SPM, cache, and SYNC with larger array access.  
---

#### **Meeting 6 (May 8, 2025)**  
**Focus:** Start SPM implementation and test with an SPM test case.  
**Members:** Satish, Niharika  
**Decisions:**  
- Begin SPM integration with `lw_spm` and `sw_spm` instructions.  
- Test SPM functionality with `testcase1spm.s`.  

**Tasks :**  
- **Satish:**  
  - Implemented `Memory::loadWordSPM()` to load words from SPM (`lw_spm` instruction).  
  - Implemented `Memory::storeWordSPM()` to store words in SPM (`sw_spm` instruction).  
  - Modified `Memory::Memory()` to include SPM initialization.  
  - Modified `Memory::~Memory()` to clean up SPM resources.
  - Implemented test case `testcase1spm.s` to verify SPM behavior with strided array addition. 
- **Niharika:**  
  - Modified `assembleInstruction()` in `sim2.cpp` to parse `lw_spm` and `sw_spm` instructions.  
  - Modified `Simulator::cycle()` to handle SPM instructions with correct latency.  
  - Implemented `Memory::lloadWord()` to load words for test case setup (SPM-related).  
  - Implemented `Memory::sstoreWord()` to store words for test case setup (SPM-related).   
---

#### **Meeting 5 (April 27, 2025)**  
**Focus:** Complete SYNC implementation and test with a cache-only test case.  
**Members:** Satish, Niharika  
**Decisions:**  
- Finalize SYNC instruction handling in the pipeline.  
- Test SYNC and cache functionality with `testcase3cache.s` (cache-only test case).  
- Prepare for SPM implementation in the next meeting.  

**Tasks :**  
- **Satish:**  
  - Modified `Simulator::cycle()` to finalize SYNC implementation for all cores.  
  - Modified `Simulator::run()` to ensure SYNC behavior in multi-core simulation.  
  - Modified `Core::isBranchTaken()` to support CID-based branching for test cases.  
  - Implemented `Simulator::pipelinesEmpty()` to ensure all pipelines completed.
  - Added new Lw_spm ,sw_spm instructions for SPM memory.
- **Niharika:**    
  - checked complete implemenatation of SYNC for any errors and fixes.
  - Implemented test case `testcase3cache.s` to verify cache and SYNC behavior with larger array access.  
---

#### **Meeting 4 (April 20, 2025)**  
**Focus:** Start SYNC implementation and test with cache-only test cases.  
**Members:** Satish, Niharika  
**Decisions:**  
- Begin implementing SYNC instruction for multi-core synchronization.  
- Test cache functionality with `testcase1.s` and `program.s` (cache-only test cases).  

**Tasks :**  
- **Satish:**  
  - Modified `Simulator::hazardDetected()` to handle stalls with SYNC and cache latency.  
  - Modified `Simulator::cycle()` to implement SYNC by waiting for all cores to execute it.  
  - Implemented `Memory::updateFinalMemory()` to update dirty cache blocks to the memory at the end.  
  - Implemented test case `testcase1.s` to verify cache behavior with strided array addition.
  -  Added new No-Op instruction for SYNC instruction
- **Niharika:**  
  - Implemented `Simulator::pipelinesEmpty()` to check pipeline state for SYNC synchronization.
  - Modified `Simulator::run()` to ensure SYNC behavior in multi-core simulation.  
  - final check of complete cache implementation with test cases.
  - Implemented test case `program.s` to verify cache behavior with strided array addition.  
---

#### **Meeting 3 (April 13, 2025)**  
**Focus:** Finalize cache implementation and test with a cache-focused test case.  
**Members:** Satish, Niharika  
**Decisions:**  
- Complete cache operations and integrate them into the pipeline.  
- Test cache functionality with `l2cacheutilise.s` to verify L2 cache utilization.  

**Tasks :**  
- **Satish:**  
  - Modified `Simulator::hazardDetected()` to account for stalls due to cache miss latency.  
  - Implemented `Simulator::run()` to compute stalls, cache miss rate, and IPC at the end.  
  - Implemented `Memory::getL1DLatency()` to support variable latency in loads/stores.  
  - Implemented `Memory::missRate()` to calculate cache miss rates for L1I, L1D, and L2.
  - Implemented test case `l2cacheutilise.s` to verify L2 cache utilization. 
- **Niharika:**
  - Implemented `Memory::writeBack()` to write dirty cache blocks to main memory.  
  - Implemented `Memory::update()` to flush dirty cache blocks to main memory.  
  - Implemented `Memory::lloadWord()` to load words for test case setup (cache-related).  
  - Implemented `Memory::sstoreWord()` to store words for test case setup (cache-related).   
---

#### **Meeting 2 (April 10, 2025)**  
**Focus:** Continue cache implementation.  
**Members:** Satish, Niharika  
**Decisions:**  
- Implement cache operations for instruction and data access.  
- Prepare for SYNC implementation in the next meeting.  

**Tasks :**  
- **Satish:**
  - Redesigned Memory class and Implemented accordingly .
  - Implemented `Memory::loadinstructionWord()` to fetch instructions from L1I, L2, or main memory.  
  - Implemented `Memory::storeinstructionWord()` to store instructions in the cache.  
  - Implemented `Memory::loadWord()` to load data from L1D, L2, or main memory with cache latency.  
  - Implemented `Memory::storeWord()` to store data with cache .
    
- **Niharika:**  
  - Implemented `Cache::updateBlock()` to update cache blocks with LRU/SRRIP policies.  
  - Implemented `Cache::writeBlockL2()` to update L2 cache with evicted L1 lines.  
  - Implemented `Cache::replaceLine()` to replace cache lines using LRU/SRRIP.  
  - Implemented `Memory::loadBlock()` to fetch blocks from main memory on cache misses.  

---

#### **Meeting 1 (April 3, 2025)**  
**Focus:** Start cache implementation.  
**Members:** Satish, Niharika  
**Decisions:**  
- Review project requirements for cache implementation.  
- Set timeline for incremental development, starting with caches.  
- Divide initial tasks for cache design and configuration.  

**Tasks :**  
- **Satish:**
  - Designed and implemented  new Cache class .
  - Implemented `Cache::Cache()` to set up cache parameters for L1I, L1D, and L2.  
  - Implemented `Memory::Memory()` to initialize L1I, L1D, and L2.  
  - Implemented `Memory::~Memory()` to clean up cache resources.  
  - Implemented `Memory::configureCaches()` to set up cache parameters from input.  
- **Niharika:**  
  - Implemented `Cache::readWord()` to read words from the cache.  
  - Implemented `Cache::writeWord()` to write words to the cache.  
  - Implemented `Cache::contains()` to check if a block exists in the cache.  
  - Implemented `Cache::readBlock()` to retrieve blocks from the cache.  

---
PHASE-3
---
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
**Note** : we decided to continue development only in c++    language. `.cpp`  & assumed branches outcomes are known in executioon Stage only


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
  - Testing of bubble sort && Debugging and fixing of errors.
       
### **Meeting 6 (9-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Implementation of `MUL` intruction as it is needed.
  - Modifying Hazards detection,data forwarding & without forwarding to work with latencies.
  - Implemenatation of `li` and `ecall` Instructions.
  - Modifying ReadmeFile with all information For phase2
  - Implemenation of Caluculation of final results .
- **Tasks**:
     - ### Satish:
    -  Modifying Hazards detection,data forwarding & without forwarding to work with latencies.
    -  Implemenation of Caluculation of final results.
    -  Implementation of `MUL` intruction as it is needed.
       - ### Niharika:
       - Implemenatation of `li` and `ecall` Instructions.
       - Assembly code Implementation for  - for loop execution as specified. .
       - Modifying ReadmeFile with new information For phase2.
### **Meeting 5 (8-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Branch Instruction Implementation with CID specification.
  - Implementation fo Global PC and  local PC for cores and handle takeup correct Instructions across all Cores.
  - Implementation of shared data & Instruction memory as per Phase 2 requirements.
  - Implemented user-configurable instruction latencies for arithmetic Instructions.
  - Assembly code Implementation for for loop execution as specified.
- **Tasks**:
     ###  Satish:
  - Implementation of Hazards detection with data forwarding.
  - Implemented user-configurable instruction latencies for arithmetic Instructions.
  - Implementation fo Global PC and  local PC for cores and handle takeup correct Instructions across all Cores.
      ### Niharika:
  - Implement Branch Instruction Implementation with CID specification
  - Implementation of shared data & Instruction memory as per Phase 2 requirements.
  -  Improvement of pipelinehazard detection & Debugging and fixing of errors.

    
       

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
  - Implement proper instruction fetching for each core.
  - Improvement of Checking if all present instructions are working with and without forwarding.
       
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
  - Implementation of `la` ,`lw` insturctions & Checking if hazards are correctly detected manually.
    

### **Meeting 2 (2-March-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Implemenation of Basic Structure of  State of pipeline Using PipelineRegsiters.
  - Implementation of pipeline with stages  `IF`,`ID`,`EX`,`MEM`,`WB`.
  - Modify core class so that it suits with pipelining .
  - Debug and testing of pipeline.
- **Tasks**:
   ### Satish:
   - Implemenatation of pipeline with stages  `IF`,`ID`,`EX`,`MEM`,`WB`.
   - Implement pipeline control logic for stage transitions.
   - Debug and testing of pipeline implementation.
      
   ###  Niharika:
  - Modify Core class and execution of cores to work with Pipelining.
  - Design and implement the fetch unit into the pipeline.
  - Modify and implement instruction parsing as per pipeline.
  
  
       
  
### **Meeting 1 (28-Feb-2025)**
- **Members**: Satish,Niharika
- **Decisions**:
  - Decided to just focus on implementation using `cpp`.
  - analyzed the complete phase 2 project details and decided on how to proceed.
  - Transforming phase 1 files to be prepared for extension and enhancement in phase 2.
  - Implementation of basic Structure of PipelineRegisters.
  - Modifying instruction parsing using proper Stucture required for Pipelining Stages.
  - Understand and study about how pipelining ,Hazards,latencies works.
- **Tasks**:
   ### Satish:
  - Design and Implementation of basic pipeline architecture.
  - Implementation of Pipeline Registers.
  - Understanding and study about how pipelining, hazards and latencies works.
   ### Niharika:
  
   - Transform the phase 1 files to be prepared for extension and enhancement in phase 2.
   - Analyse phase 2 project details and plan how to proceed.
   - Study about  how pipelining, hazards and latencies works.
     
      
# Phase 1


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
