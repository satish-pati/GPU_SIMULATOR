class Memory:
    MEMORY_SIZE = 4096  # Define total memory size
    CORE_BLOCK_SIZE = 1024  # Each core gets 1KB of memory

    def __init__(self):
        self.mem = [0] * self.MEMORY_SIZE

    def load_word(self, address, core_id, is_active):
        base = core_id * self.CORE_BLOCK_SIZE
        if address < base or address >= base + self.CORE_BLOCK_SIZE:
            print(f"Core {core_id} - ERROR: Invalid memory access for lw at address {address}")
            is_active = False
            return 0
        if address % 4 != 0 or address + 3 >= self.MEMORY_SIZE:
            print(f"Core {core_id} - ERROR: Unaligned memory access at {address}")
            is_active = False
            return 0
        return (self.mem[address] | (self.mem[address + 1] << 8) |
                (self.mem[address + 2] << 16) | (self.mem[address + 3] << 24))

    def store_word(self, address, value, core_id, is_active):
        base = core_id * self.CORE_BLOCK_SIZE
        if address < base or address >= base + self.CORE_BLOCK_SIZE:
            print(f"Core {core_id} - ERROR: Memory Access Violation for sw at address {address}")
            is_active = False
            return
        if address % 4 != 0 or address + 3 >= self.MEMORY_SIZE:
            print(f"Core {core_id} - ERROR: Unaligned memory access for sw at {address}")
            return
        self.mem[address] = value & 0xFF
        self.mem[address + 1] = (value >> 8) & 0xFF
        self.mem[address + 2] = (value >> 16) & 0xFF
        self.mem[address + 3] = (value >> 24) & 0xFF

    def print_memory(self, core_id):
        base = core_id * self.CORE_BLOCK_SIZE
        end = base + self.CORE_BLOCK_SIZE
        print(f"\nMemory for Core {core_id}:")
        for i in range(base, end, 16):
            print(f"Addr(Bytes) {i}-{i + 16}: ", end="")
            for j in range(0, 16, 4):
                dummy_active = True
                print(self.load_word(i + j, core_id, dummy_active), end=" ")
            print()
# Inside Memory class
def get_memory_str(self, core_id):
    memory_str = ""
    for addr in range(core_id * 1024, (core_id + 1) * 1024, 4):
        memory_str += f"0x{addr:X}: {self.load_word(addr, core_id)}\n"
    return memory_str
