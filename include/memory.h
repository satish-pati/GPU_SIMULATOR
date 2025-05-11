#ifndef MEMORY_H
#define MEMORY_H
#include <string>  // ✅ Must be added at the top

#include <cstdint>
#include <vector>
#include <climits>

// --- Configuration Macros ---
#define MEMORY_SIZE 131072      // 128KB Total memory
#define CORE_BLOCK_SIZE 1024    // Per-core block size
#define DATA_BASE_ADDRESS 1008
constexpr int SPM_SIZE = 400;  // Scratchpad memory size

// --- Replacement Policy Enum ---
enum ReplacementPolicy
{
    LRU,
    FIFO,
    SRRIP
};

// --- Cache Class Declaration ---
class Memory; // Forward declaration to use in Cache

class Cache
{
public:
    // --- Cache Line Structure ---
    struct CacheLine
    {

        bool valid;
        uint32_t tag;
        int lastUsed;
        int insertCycle;
        bool dirty;
        std::vector<uint8_t> data;
            int rrpv; // 0 = “most recently used”, larger = less likely to be re-used

        CacheLine(int blockSize)
            : valid(false), tag(0), lastUsed(0),
              insertCycle(0), dirty(false), data(blockSize, 0),rrpv(3) {}
    };

    // --- Cache Parameters ---
    int size;
    int blockSize;
    int associativity;
    int accessLatency;
    ReplacementPolicy policy;
    int numSets;
    int currentCycle;
    int accesses;
    int misses;
std::string type;
    // 2D structure: [set][line]
    std::vector<std::vector<CacheLine>> sets;

    // --- Constructor ---
    Cache(int size, int blockSize, int associativity, int accessLatency, ReplacementPolicy policy,std::string type);
    bool contains(uint32_t address)  ;
    std::pair<bool,std::pair<uint32_t,std::vector<uint8_t>>> updateBlock(uint32_t address, const std::vector<uint8_t>& block) ;
    // --- Core Methods ---
        std::pair<bool,std::pair<uint32_t,std::vector<uint8_t>>> updateSWBlock(uint32_t address,uint32_t value, const std::vector<uint8_t>& block,bool load); ;
    //bool access(uint32_t address); // For latency-only access
    uint32_t readWord(uint32_t address, Memory &memory);
    void writeWord(uint32_t address, uint32_t value, Memory &memory);
    double missRate() const;
    void updateFinalMemory(Memory &memory) ;
    void writeBlockL2(uint32_t address, const std::vector<uint8_t>& block,Memory &memory);
    void printCacheContents(const std::string& name) const;
    void replaceLine(uint32_t address, const std::vector<uint8_t>& block);
    std::vector<uint8_t> readBlock(uint32_t address);
int findVictimIndex(int setIndex);
};

// --- Memory Class Declaration ---
class Memory
{
private:
    std::vector<uint8_t> mem;      // Main memory
    int totalMemoryStalls;
    int mainMemoryLatency;
    std::vector<uint8_t> scratchpad;


public:

    Cache *L1I;
    Cache *L1D;
    Cache *L2;
    // Constructor / Destructor
    Memory();
    ~Memory();

    // Configuration
    void configureCaches(int l1ISize,int l1Size, int l2Size, int blockSize, int associativity,
                         int l1Latency, int l2Latency, int memLatency);

    // Access latency calculator
    int getLatency(uint32_t address, bool isInstruction);
 int loadinstructionWord(uint32_t address);
    int storeinstructionWord(uint32_t address, uint32_t value);
    // Main memory accessors via cache
    std::pair<uint32_t,int> loadWord(uint32_t address, int coreID, bool &isActive);
    int storeWord(uint32_t address, uint32_t value, int coreID, bool &isActive);
    uint32_t lloadWord(uint32_t address, int coreID, bool &isActive);
    void sstoreWord(uint32_t address, uint32_t value, int coreID, bool &isActive);

    // SPM accessors
    uint32_t loadWordSPM(uint32_t address) const;
    void storeWordSPM(uint32_t address, uint32_t value);

    // Block-level memory interface
    std::vector<uint8_t> loadBlock(uint32_t addr, int blockSize);
    void writeBack(uint32_t addr, const std::vector<uint8_t> &blockData);
    std::vector<uint8_t> getBlockFromL2(uint32_t address, int blockSize);

    // Debug and statistics
    void printMem(int coreID) ;
    void printSPM() const;
    void printCacheStats() const;
    void update();

    int getL1DLatency() const;
    void printCompleteMemoryContents() const;
};

#endif // MEMORY_H  
