#include "../include/memory.h" 
#include <iostream>
#include <climits>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
static constexpr int maxRRPV = 3;
Cache::Cache(int size, int blockSize, int associativity, int accessLatency, ReplacementPolicy policy,std::string type)
    : size(size), blockSize(blockSize), associativity(associativity),
      accessLatency(accessLatency), policy(policy), currentCycle(0),
      accesses(0), misses(0),type(type)
{
    numSets = size / (blockSize * associativity);
    sets.resize(numSets, std::vector<CacheLine>(associativity, CacheLine(blockSize)));
}
/*
bool Cache::access(uint32_t address)
{
    accesses++;
    currentCycle++;
    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag = blockAddr / numSets;
    for (auto &line : sets[setIndex]) {
        if (line.valid && line.tag == tag) {
            line.lastUsed = currentCycle;
            return true;
        }
    }
    misses++;
    for (auto &line : sets[setIndex]) {
        if (!line.valid) {
            line.valid = true;
            line.tag = tag;
            line.insertCycle = currentCycle;
            line.lastUsed = currentCycle;
            line.dirty = false;
            return false;
        }
    }
    int replaceIndex = 0;
    if (policy == LRU) {
        int minUsed = INT_MAX;
        for (int i = 0; i < associativity; i++) {
            if (sets[setIndex][i].lastUsed < minUsed) {
                minUsed = sets[setIndex][i].lastUsed;
                replaceIndex = i;
            }
        }
    } else { 
        while (true) {
            for (int i = 0; i < associativity; i++) {
                if (!sets[setIndex][i].valid){
                 replaceIndex= i;        // empty slot wins
                 break;
                }
                if (sets[setIndex][i].rrpv == maxRRPV) {
                    replaceIndex= i;
                    break;
                }
            }
            if(replaceIndex != -1) break;
            // 2) Otherwise, increment everyone’s RRPV and retry
            for (auto &line : sets[setIndex]) {
                if (line.rrpv < maxRRPV) line.rrpv++;
            }
        }
    }
    sets[setIndex][replaceIndex].tag = tag;
    sets[setIndex][replaceIndex].lastUsed = currentCycle;
    sets[setIndex][replaceIndex].insertCycle = currentCycle;
    sets[setIndex][replaceIndex].dirty = false;
    return false;
}
    */

// function to read a block from the cache
std::vector<uint8_t> Cache::readBlock(uint32_t address)  {
    ++currentCycle;
    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag      = blockAddr / numSets;

    for (auto &line : sets[setIndex]) {
        if (line.valid && line.tag == tag) {
            if (policy == ReplacementPolicy::LRU) {
            line.lastUsed = currentCycle;
} else if (policy == ReplacementPolicy::SRRIP) {
           line.rrpv = 0;
}
            return line.data;
        }
    }
    throw std::runtime_error("Cache::readBlock: block not found");
}
// function to check if a block is in the cache
bool Cache::contains(uint32_t address) {
    ++currentCycle;

    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag = blockAddr / numSets;
    for ( auto &line : sets[setIndex]) {
        if (line.valid && line.tag == tag){
            if (policy == ReplacementPolicy::LRU) {
    line.lastUsed = currentCycle;
} else if (policy == ReplacementPolicy::SRRIP) {
         line.rrpv = 0;
}
            return true;
        }
    }
    return false;
}
// function to update a block in the cache
std::pair<bool,std::pair<uint32_t,std::vector<uint8_t>>> Cache::updateBlock(uint32_t address, const std::vector<uint8_t>& block) {
    ++currentCycle;
    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag = blockAddr / numSets;
    int replaceIndex = -1;

    // First, try to find an invalid line
    
    for (int i = 0; i < associativity; i++) {
        if (!sets[setIndex][i].valid) { 
            replaceIndex = i; 
            break; 
        }
    }
    std::pair<bool, std::pair<uint32_t, std::vector<uint8_t>>> result = {false, {0, {}}};
    // If all lines are valid, we need to select a victim
    if (replaceIndex == -1) {
        if (policy == LRU) { 
            int minUsed = INT_MAX;
            for (int i = 0; i < associativity; i++) {
                if (sets[setIndex][i].lastUsed < minUsed) {
                    minUsed = sets[setIndex][i].lastUsed;
                    replaceIndex = i;
                }
            }
        } else { 
             while (true) {
            for (int i = 0; i < associativity; i++) {
                if (!sets[setIndex][i].valid){
                 replaceIndex= i;      
                 break;
                }
                if (sets[setIndex][i].rrpv == maxRRPV) {
                    replaceIndex= i;
                    break;
                }
            }
            if(replaceIndex != -1) break;
            // increment everyone’s RRPV and retry
            for (auto &line : sets[setIndex]) {
                if (line.rrpv < maxRRPV) line.rrpv++;
            }
        }
        }

        if (sets[setIndex][replaceIndex].dirty) {
            uint32_t victimBlockAddr = ((sets[setIndex][replaceIndex].tag * numSets) + setIndex) * blockSize;
            result = {true, {victimBlockAddr, sets[setIndex][replaceIndex].data}};
        }
    }
   sets[setIndex][replaceIndex].data = block;
    sets[setIndex][replaceIndex].valid = true;
    sets[setIndex][replaceIndex].tag = tag;
   if (policy == LRU){
       sets[setIndex][replaceIndex].lastUsed = currentCycle;
           sets[setIndex][replaceIndex].insertCycle = currentCycle;
     }
     if (policy == SRRIP)
        sets[setIndex][replaceIndex].rrpv = maxRRPV - 1;

    sets[setIndex][replaceIndex].dirty = false;
    return result;
    }

    // function to read a word from the cache
uint32_t Cache::readWord(uint32_t address, Memory &memory) {

    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag = blockAddr / numSets;
    int offset = address % blockSize;
    currentCycle++;
            uint32_t word = 0;

    for (auto &line : sets[setIndex]) {
        if (line.valid && line.tag == tag) {
               
            if (policy == ReplacementPolicy::LRU) {
    line.lastUsed = currentCycle;
} else if (policy == ReplacementPolicy::SRRIP) {
         line.rrpv = 0;
}
    word = 0;
            for (int i = 0; i < 4; i++) {
                word |= (line.data[offset + i] << (8 * i));
            }
            return word;
        }
    }

            return word;

}


// function to write a word to the cache
void Cache::writeWord(uint32_t address, uint32_t value, Memory &memory) {
    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag = blockAddr / numSets;
    int offset = address % blockSize;
    currentCycle++;
    for (auto &line : sets[setIndex]) {
        if (line.valid && line.tag == tag) {
            if (policy == ReplacementPolicy::LRU) {
    line.lastUsed = currentCycle;
} else if (policy == ReplacementPolicy::SRRIP) {
         line.rrpv = 0;
}
  
            for (int i = 0; i < 4; i++) {
                line.data[offset + i] = (value >> (8 * i)) & 0xFF;
            }
           
            line.dirty = true;
            return;
        }
    }
    
}

//function to print the cache contents
void Cache::printCacheContents(const std::string& name) const {
    std::cout << "----- " << name << " Cache Contents (All Blocks) -----\n";
    for (int setIdx = 0; setIdx < numSets; ++setIdx) {
        for (int way = 0; way < associativity; ++way) {
            const auto& line = sets[setIdx][way];
            if (line.valid) {
                std::cout << "Set " << std::dec<<setIdx << ", Way " << way << ", Tag " << line.tag << ": ";
                for (int offset = 0; offset < line.data.size(); offset += 4) {
                    uint32_t word = 0;
                    for (int k = 0; k < 4; ++k) {
                        word |= (line.data[offset + k] << (8 * k));
                    }
                    std::cout << std::dec << word << " ";
                }
                std::cout << std::dec << std::endl;
            }
        }
    }
    std::cout << "----- " << name << "Cache End -----\n";
}

// function to update the final memory with dirty blocks
void Cache::updateFinalMemory(Memory &memory) {
    for (int setIdx = 0; setIdx < numSets; ++setIdx) {
        for (int way = 0; way < associativity; ++way) {
            CacheLine &line = sets[setIdx][way];
            if (line.valid && line.dirty) {
                uint32_t blockAddr = (line.tag * numSets + setIdx) * blockSize;
                memory.writeBack(blockAddr, line.data);
                line.dirty = false; 
            }
        }
    }
}

// function to load instruction from memory
int Memory::loadinstructionWord(uint32_t address) {
        uint32_t totalLatency = 0;
    if (L1I->contains(address)) {
        L1I->accesses++;
                totalLatency+=L1I->accessLatency;

        return L1I->accessLatency;
    }
    else if (L2->contains(address)) {
         L1I->accesses++;
            L2->accesses++;
             L1I->misses++;
                totalLatency+=L1I->accessLatency+L2->accessLatency;

        uint32_t word = L2->readWord(address, *this);
        uint32_t blockStartAddr = (address / L1I->blockSize) * L1I->blockSize;
        std::vector<uint8_t> block = L2->readBlock(address);
        auto writebackInfo = L1I->updateBlock(address, block);
        if (writebackInfo.first) {
                        totalLatency+=L2->accessLatency;

              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                // update in L2 
                auto writebackInfo2= L2->updateBlock(writebackInfo.second.first, writebackInfo.second.second);
               if (writebackInfo2.first) {
                        totalLatency+=L2->accessLatency;
                  if (L2->contains(writebackInfo2.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo2.second.first, writebackInfo2.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                //writeback to main memory
                writeBack(writebackInfo2.second.first, writebackInfo2.second.second);

            }
        }
    }
}
        return totalLatency;
    }
    else {
         L1I->accesses++;
            L2->accesses++;
             L1I->misses++;
             L2->misses++;
        totalLatency+=L1I->accessLatency+L2->accessLatency+mainMemoryLatency;
        uint32_t blockStartAddr = (address / L2->blockSize) * L2->blockSize;
        std::vector<uint8_t> block = loadBlock(blockStartAddr, L2->blockSize);
        auto writebackInfo = L2->updateBlock(address, block);
          if (writebackInfo.first) {
                totalLatency+=mainMemoryLatency;
            writeBack(writebackInfo.second.first, writebackInfo.second.second);
        }
        uint32_t l1BlockStartAddr = (address / L1I->blockSize) * L1I->blockSize;
        std::vector<uint8_t> l1Block = loadBlock(l1BlockStartAddr, L1I->blockSize);
        writebackInfo = L1I->updateBlock(address, l1Block);
         if (writebackInfo.first) {
                        totalLatency+=L2->accessLatency;

            //writeBack(writebackInfo.second.first, writebackInfo.second.second);
              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                auto writebackInfo2= L2->updateBlock(writebackInfo.second.first, writebackInfo.second.second);
               if (writebackInfo2.first) {
                        totalLatency+=L2->accessLatency;
                  if (L2->contains(writebackInfo2.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo2.second.first, writebackInfo2.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                //L2->writeBlockL2(writebackInfo.second.first, writebackInfo.second.second,*this);
                writeBack(writebackInfo2.second.first, writebackInfo2.second.second);
            }
        }
    }
    }
        int offset = address % L1I->blockSize;
        uint32_t word = 0;
        for (int i = 0; i < 4; i++) {
            word |= (l1Block[offset + i] << (8 * i));
        }
        return totalLatency;
    }
}

// fucntion to store a instruction in memory
int Memory::storeinstructionWord(uint32_t address, uint32_t value) {
    int totalLatency = 0;
    if (L1I->contains(address)) {
        L1I->accesses++;
        totalLatency += L1I->accessLatency;
        L1I->writeWord(address,value,*this);
            return totalLatency;
    }
    else if (L2->contains(address)) {
        L2->accesses++;
        L1I->accesses++;
        L1I->misses++;
        totalLatency += L2->accessLatency+L1I->accessLatency;
        uint32_t blockStartAddr = (address / L1I->blockSize) * L1I->blockSize;
        std::vector<uint8_t> block = L2->readBlock(address);
        auto writebackInfo = L1I->updateBlock(address, block);
         if (writebackInfo.first) {
            totalLatency+=L2->accessLatency;
            //writeBack(writebackInfo.second.first, writebackInfo.second.second);
              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                auto writebackInfo2= L2->updateBlock(writebackInfo.second.first, writebackInfo.second.second);
               if (writebackInfo2.first) {
                        totalLatency+=L2->accessLatency;
                  if (L2->contains(writebackInfo2.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo2.second.first, writebackInfo2.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                //L2->writeBlockL2(writebackInfo.second.first, writebackInfo.second.second,*this);
                writeBack(writebackInfo2.second.first, writebackInfo2.second.second);

            }
        }
    }
}
   
        L1I->writeWord(address, value, *this);
        totalLatency += L1I->accessLatency;
            return totalLatency;
    }
    else {
        
         L1I->accesses++;
            L2->accesses++;
             L1I->misses++;
             L2->misses++;
       totalLatency += L1I->accessLatency+ L2->accessLatency+ mainMemoryLatency;
        uint32_t blockStartAddr = (address / L2->blockSize) * L2->blockSize;
        std::vector<uint8_t> block = loadBlock(blockStartAddr, L2->blockSize);
        auto writebackInfo = L2->updateBlock(address, block);
        if (writebackInfo.first) {
            totalLatency +=mainMemoryLatency;
            writeBack(writebackInfo.second.first, writebackInfo.second.second);
        }
        
        uint32_t l1BlockStartAddr = (address / L1I->blockSize) * L1I->blockSize;
        std::vector<uint8_t> l1Block = loadBlock(l1BlockStartAddr, L1I->blockSize);
        writebackInfo = L1I->updateBlock(address, l1Block);
         if (writebackInfo.first) {
                        totalLatency+=L2->accessLatency;

            //writeBack(writebackInfo.second.first, writebackInfo.second.second);
              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                auto writebackInfo2= L2->updateBlock(writebackInfo.second.first, writebackInfo.second.second);
               if (writebackInfo2.first) {
                        totalLatency+=L2->accessLatency;
                  if (L2->contains(writebackInfo2.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo2.second.first, writebackInfo2.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                //L2->writeBlockL2(writebackInfo.second.first, writebackInfo.second.second,*this);
                writeBack(writebackInfo2.second.first, writebackInfo2.second.second);

            }
        }
    }
}         
        L1I->writeWord(address, value, *this);
        totalLatency += L1I->accessLatency;
        return totalLatency;
    }
}
// fucntion to calculate miss rate
double Cache::missRate() const
{
    return (accesses > 0) ? (static_cast<double>(misses) / accesses) : 0;
}


// function for replacing an existing cache line 
void Cache::replaceLine(uint32_t address, const std::vector<uint8_t>& block) {
    ++currentCycle;
    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag = blockAddr / numSets;
    for (auto &line : sets[setIndex]) {
        if (line.valid && line.tag == tag) {
            line.data = block;
           if(policy==LRU){
               line.lastUsed = currentCycle;
           }
           else {
            line.rrpv=0;
           }
            line.dirty = true;  
            return;
        }
    }
    throw std::runtime_error("Attempted to replace a non-existent cache line");
}
//function for writing a block to L2
void Cache::writeBlockL2(uint32_t address, const std::vector<uint8_t> &data,Memory &memory) {
    uint32_t blockAddr = address / blockSize;
    int setIndex = blockAddr % numSets;
    uint32_t tag = blockAddr / numSets;
    currentCycle++;
    int replaceIndex = -1;
    for (int i = 0; i < associativity; i++) {
        if (!sets[setIndex][i].valid) {
            replaceIndex = i;
            break;
        }
    }

    // If all lines are valid, apply replacement policy
    if (replaceIndex == -1) {
        if (policy == LRU) {
            int minUsed = INT_MAX;
            for (int i = 0; i < associativity; i++) {
                if (sets[setIndex][i].lastUsed < minUsed) {
                    minUsed = sets[setIndex][i].lastUsed;
                    replaceIndex = i;
                }
            }
        } else { // SRRIP
             while (true) {
            for (int i = 0; i < associativity; i++) {
                if (!sets[setIndex][i].valid){
                 replaceIndex= i;        
                 break;
                }

                if (sets[setIndex][i].rrpv == maxRRPV) {
                    replaceIndex= i;
                    break;
                }
            }
            if(replaceIndex != -1) break;
            //  Otherwise, increment everyone’s RRPV and retry
            for (auto &line : sets[setIndex]) {
                if (line.rrpv < maxRRPV) line.rrpv++;
            }
        }
        }

        // If dirty, write back to main memory
        if (sets[setIndex][replaceIndex].dirty) {
            uint32_t victimBlockAddr = (sets[setIndex][replaceIndex].tag * numSets + setIndex) * blockSize;
            memory.writeBack(victimBlockAddr, sets[setIndex][replaceIndex].data);
        }
    }
    sets[setIndex][replaceIndex].data = data;
    sets[setIndex][replaceIndex].valid = true;
    sets[setIndex][replaceIndex].dirty = false;
    sets[setIndex][replaceIndex].tag = tag;
    if (policy == LRU){
       sets[setIndex][replaceIndex].lastUsed = currentCycle;
           sets[setIndex][replaceIndex].insertCycle = currentCycle;
     }
     if (policy == SRRIP)
        sets[setIndex][replaceIndex].rrpv = maxRRPV - 1;
}

Memory::Memory()
    : mem(MEMORY_SIZE, 0), totalMemoryStalls(0), scratchpad(SPM_SIZE, 0)
{
    // Default cache configuration.
    int defaultL1Size = 1024;      // bytes
    int defaultL2Size = 4096;      // bytes
    int defaultBlockSize = 64;     // bytes
    int defaultAssociativity = 4;
    int defaultL1Latency = 1;
    int defaultL2Latency = 10;
    mainMemoryLatency = 50;        // cycles
    L1I = new Cache(defaultL1Size, defaultBlockSize, defaultAssociativity, defaultL1Latency, LRU,"l1I");
    L1D = new Cache(defaultL1Size, defaultBlockSize, defaultAssociativity, defaultL1Latency, LRU,"l1D");
    L2   = new Cache(defaultL2Size, defaultBlockSize, defaultAssociativity, defaultL2Latency, SRRIP,"l2");
}

Memory::~Memory()
{
    delete L1I;
    delete L1D;
    delete L2;
}
// updating cache paramters from User
void Memory::configureCaches(int l1ISize,int l1Size, int l2Size, int blockSize, int associativity,
                             int l1Latency, int l2Latency, int memLatency)
{
    delete L1I;
    delete L1D;
    delete L2;
    L1I = new Cache(l1ISize, blockSize, associativity, l1Latency, LRU,"l1I");
    L1D = new Cache(l1Size, blockSize, associativity, l1Latency, LRU,"l1D");
    L2   = new Cache(l2Size, blockSize, associativity, l2Latency, SRRIP,"l2");
    mainMemoryLatency = memLatency;
    scratchpad.resize(l1Size);
}
/*
int Memory::getLatency(uint32_t address, bool isInstruction)
{
    Cache *l1 = isInstruction ? L1I : L1D;
    int latency = l1->accessLatency;
    bool hitL1 = l1->access(address);
    if (!hitL1) {
        latency += L2->accessLatency;
        bool hitL2 = L2->access(address);
        if (!hitL2) {
            latency += mainMemoryLatency;
        }
    }
    return latency;
}
*/

// function for loading word from cache 
 std::pair<uint32_t,int> Memory::loadWord(uint32_t address, int coreID, bool &isActive) {
        uint32_t totalLatency = 0;
    if (L1D->contains(address)) {
             L1D->accesses++;
        totalLatency+=L1D->accessLatency;
        return {L1D->readWord(address, *this),totalLatency};
    }
    else if (L2->contains(address)) {
            L1D->accesses++;
            L2->accesses++;
             L1D->misses++;

        totalLatency+=L1D->accessLatency+L2->accessLatency;
        uint32_t word = L2->readWord(address, *this);
        uint32_t blockStartAddr = (address / L1D->blockSize) * L1D->blockSize;
        std::vector<uint8_t> block = L2->readBlock(address);
        auto writebackInfo = L1D->updateBlock(address,block);
        if (writebackInfo.first) {
                        totalLatency+=L2->accessLatency;

              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                auto writebackInfo2= L2->updateBlock(writebackInfo.second.first, writebackInfo.second.second);
               if (writebackInfo2.first) {
                        totalLatency+=L2->accessLatency;
                  if (L2->contains(writebackInfo2.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo2.second.first, writebackInfo2.second.second);
            } else {
                writeBack(writebackInfo2.second.first, writebackInfo2.second.second);

            }
        }
    }
    }
        return {word,totalLatency}; 
    }
    else {
         L1D->accesses++;
            L2->accesses++;
             L1D->misses++;
             L2->misses++;
                totalLatency+=L1D->accessLatency+L2->accessLatency+mainMemoryLatency;
        uint32_t blockStartAddr = (address / L2->blockSize) * L2->blockSize;
        std::vector<uint8_t> block =loadBlock(blockStartAddr, L2->blockSize);
        auto writebackInfo = L2->updateBlock(address, block);
        if (writebackInfo.first) {
            totalLatency+=mainMemoryLatency;
            writeBack(writebackInfo.second.first, writebackInfo.second.second);
        }
        
        uint32_t l1BlockStartAddr = (address / L1D->blockSize) * L1D->blockSize;
        std::vector<uint8_t> l1Block = loadBlock(blockStartAddr, L2->blockSize);
        writebackInfo = L1D->updateBlock(address, l1Block);
        if (writebackInfo.first) {
                        totalLatency+=L2->accessLatency;

            //writeBack(writebackInfo.second.first, writebackInfo.second.second);
              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                auto writebackInfo2= L2->updateBlock(writebackInfo.second.first, writebackInfo.second.second);
               if (writebackInfo2.first) {
                        totalLatency+=L2->accessLatency;
                  if (L2->contains(writebackInfo2.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo2.second.first, writebackInfo2.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                //L2->writeBlockL2(writebackInfo.second.first, writebackInfo.second.second,*this);
                writeBack(writebackInfo2.second.first, writebackInfo2.second.second);

            }
        }
    }
    }
        int offset = address % (L1D->blockSize);
        uint32_t word = 0;
        for (int i = 0; i < 4; i++) {
            word |= (l1Block[offset + i] << (8 * i));
        }
        return {word,totalLatency}; //return word;
    }
}

// function to storeword to cache
int Memory::storeWord(uint32_t address, uint32_t value, int coreID, bool &isActive) {
    //std::cout<<"storeWord"<<address<<std::endl;
    uint32_t totalLatency = 0;
    if (L1D->contains(address)) {
                     L1D->accesses++;
        L1D->writeWord(address, value, *this);
                totalLatency+=L1D->accessLatency;//read
             return totalLatency;
    }
    else if (L2->contains(address)) {
         L1D->accesses++;
            L2->accesses++;
             L1D->misses++;
        totalLatency+=L1D->accessLatency+L2->accessLatency;
        uint32_t blockStartAddr = (address / L1D->blockSize) * L1D->blockSize;
        std::vector<uint8_t> block = L2->readBlock(address);
        auto writebackInfo = L1D->updateBlock(address, block);
        if (writebackInfo.first) {
                            totalLatency+=L2->accessLatency;
              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                L2->writeBlockL2(writebackInfo.second.first, writebackInfo.second.second,*this);
                
            }
        }   
        L1D->writeWord(address, value, *this);
                        totalLatency+=L1D->accessLatency;

        return totalLatency;
    }
    else {
          L1D->accesses++;
            L2->accesses++;
             L1D->misses++;
             L2->misses++;
            totalLatency+=L1D->accessLatency+L2->accessLatency+mainMemoryLatency;

        uint32_t blockStartAddr = (address / L2->blockSize) * L2->blockSize;
        std::vector<uint8_t> block = loadBlock(blockStartAddr, L2->blockSize);
        auto writebackInfo = L2->updateBlock(address, block);
        if (writebackInfo.first) {
            totalLatency+=mainMemoryLatency;
            writeBack(writebackInfo.second.first, writebackInfo.second.second);
        }
        uint32_t l1BlockStartAddr = (address / L1D->blockSize) * L1D->blockSize;
        std::vector<uint8_t> l1Block = loadBlock(l1BlockStartAddr, L1D->blockSize);
        writebackInfo = L1D->updateBlock(address, l1Block);
         if (writebackInfo.first) {
                        totalLatency+=L2->accessLatency;

            //writeBack(writebackInfo.second.first, writebackInfo.second.second);
              // If L1D had to evict a dirty line, we need to update L2
            if (L2->contains(writebackInfo.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo.second.first, writebackInfo.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                auto writebackInfo2= L2->updateBlock(writebackInfo.second.first, writebackInfo.second.second);
               if (writebackInfo2.first) {
                        totalLatency+=L2->accessLatency;
                  if (L2->contains(writebackInfo2.second.first)) {
                // Directly replace the content in L2
                L2->replaceLine(writebackInfo2.second.first, writebackInfo2.second.second);
                //writeBack(writebackInfo.second.first, writebackInfo.second.second);
            } else {
                //L2->writeBlockL2(writebackInfo.second.first, writebackInfo.second.second,*this);
                writeBack(writebackInfo2.second.first, writebackInfo2.second.second);

            }
        }
    }
    }
        totalLatency+=L1D->accessLatency;
        L1D->writeWord(address, value, *this);
        return totalLatency;
    }
}

//helper for printing vector
void printVector(const std::vector<uint8_t>& vec) {
    for (size_t i = 0; i < vec.size(); i += 4) {
        uint32_t word = 0;
        for (int j = 0; j < 4 && (i + j) < vec.size(); j++) {
            word |= (vec[i + j] << (8 * j));
        }
        std::cout << std::hex << word << " ";
    }
    std::cout << std::dec << std::endl;
}
// fucntion for fetching blocks from memory to cache
std::vector<uint8_t> Memory::loadBlock(uint32_t addr, int blockSize)
{
    
    std::vector<uint8_t> block(blockSize, 0);
    for (int i = 0; i < blockSize; i++) {
        if (addr + i < mem.size())
            block[i] = mem[addr + i];
        else
            block[i] = 0;
    }
    
    return block;
}

// fucntion for writing back blocks from cache to memory
void Memory::writeBack(uint32_t addr, const std::vector<uint8_t>& blockData)
{
    for (int i = 0; i < blockData.size(); i++) {
        if (addr + i < mem.size())
            mem[addr + i] = blockData[i];
    }
}

// fucntion for loading word from scratchpad
uint32_t Memory::loadWordSPM(uint32_t address) const
{
    if (address + 3 >= scratchpad.size())
        throw std::out_of_range("SPM load out of bounds");
    return (scratchpad[address]) |
           (scratchpad[address + 1] << 8) |
           (scratchpad[address + 2] << 16) |
           (scratchpad[address + 3] << 24);
}
// function for storing word to scratchpad
void Memory::storeWordSPM(uint32_t address, uint32_t value)
{
    if (address + 3 >= scratchpad.size())
        throw std::out_of_range("SPM store out of bounds");
    scratchpad[address] = value & 0xFF;
    scratchpad[address + 1] = (value >> 8) & 0xFF;
    scratchpad[address + 2] = (value >> 16) & 0xFF;
    scratchpad[address + 3] = (value >> 24) & 0xFF;
}
// function for printing memory
void Memory::printMem(int coreID)
{
    uint32_t base = 1008 + coreID * CORE_BLOCK_SIZE;
    uint32_t end = base + CORE_BLOCK_SIZE;
    for (uint32_t i = base; i < end; i += 16) {
        
        bool nonZeroLine = false;
        std::ostringstream lineStream;

        for (int j = 0; j < 16 && (i + j) < end; j += 4) {
            bool dummyActive = true;
            auto wordpair = loadWord(i + j, coreID, dummyActive);
            auto word = wordpair.first;
            if (word != 0) nonZeroLine = true;
            lineStream << word << " ";
        }
        if (nonZeroLine) {
            std::cout << "Addr(Bytes) " << i << "-" << (i + 16) << ": " << lineStream.str() << std::endl;
        }

        std::cout << std::endl;
    }
    L1D->printCacheContents("L1 Data");
    //L1I->printCacheContents("L1 Instruction");
    L2->printCacheContents("L2");

}
// function to update final memory
void Memory::update(){
     L1D->updateFinalMemory(*this);
    L1I->updateFinalMemory(*this);
    L2->updateFinalMemory(*this);
}
// fucntion for printing scratchpad
void Memory::printSPM() const
{
    std::cout << "----- Scratchpad Memory (SPM) Contents -----\n";
    for (uint32_t i = 0; i < scratchpad.size(); i += 16) {
        std::cout << "SPM Addr(Bytes) " << i << "-" << (i + 15) << ": ";
        for (int j = 0; j < 16 && (i + j + 3) < scratchpad.size(); j += 4) {
            try {
                uint32_t word = loadWordSPM(i + j);
                std::cout << word << " ";
            } catch (const std::out_of_range& e) {
                std::cout << "ERR ";
            }
        }
        std::cout << std::endl;
    }
}
// to get L1D latency
int Memory::getL1DLatency() const
{
    return L1D->accessLatency;
}

// function for printing cache stats
void Memory::printCacheStats() const
{
    std::cout << "\n------------------ CACHE STATISTICS ------------------\n";
    std::cout << "L1 Instruction Cache Miss Rate: " << L1I->missRate() << std::endl;
    std::cout << "L1 Data Cache Miss Rate       : " << L1D->missRate() << std::endl;
    std::cout << "L2 Unified Cache Miss Rate    : " << L2->missRate() << std::endl;
    std::cout << "------------------------------------------------------\n";
}

// function for storing word to memory  before program starts
void Memory::sstoreWord(uint32_t address, uint32_t value, int coreID, bool &isActive)
{
    mem[address] = value & 0xFF;
    mem[address + 1] = (value >> 8) & 0xFF;
    mem[address + 2] = (value >> 16) & 0xFF;
    mem[address + 3] = (value >> 24) & 0xFF;
}
// function for storing word to memory  before program starts
uint32_t Memory::lloadWord(uint32_t address, int coreID, bool &isActive) 
{
    return (mem[address]) | (mem[address + 1] << 8) |
           (mem[address + 2] << 16) | (mem[address + 3] << 24);
}

// function for printing complete memory
void Memory::printCompleteMemoryContents() const {
    //std::cout << "----- Complete Memory Contents -----\n";
    for (uint32_t addr = DATA_BASE_ADDRESS; addr <=MEMORY_SIZE; addr += 16) {
        std::cout << "Addr: 0x" << std::dec << addr << "-0x" << addr + 15 << ": ";
        for (int offset = 0; offset < 16; offset += 4) {
            if (addr + offset + 3 >= mem.size()) break;
            uint32_t word = 0;
            for (int i = 0; i < 4; ++i) {
                word |= (mem[addr + offset + i] << (8 * i));
            }
            std::cout << std::dec << word << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "----- End of Memory Contents -----\n";
    L1D->printCacheContents("L1 Data");
    L1I->printCacheContents("L1 Instruction");
    L2->printCacheContents("L2 Data");
}
