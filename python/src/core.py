class Core:
    def __init__(self, memory, program, core_id):
        self.memory = memory
        self.program = program
        self.pc = 0
        self.core_id = core_id
        self.base_address = core_id * 1024
        self.is_active = True
        self.registers = [0] * 33
        self.registers[32] = core_id  # Core ID stored in register x32

    def execute(self, instruction, rd, rs1, rs2, imm, label, label_map):
        lst=[True]
        if rd == 32:
            print("Cannot write to X32, it is read-only & contains core ID")
            self.pc += 1
            return
        
        should_increment_pc = True
        
        if instruction.lower() == "add":
            if rd == 0:
                print("X0 is hardwired to 0 & contains x0=0")
                self.pc += 1
                return
            self.registers[rd] = self.registers[rs1] + self.registers[rs2]
             #print(f"Core {self.core_id} - ADD: x{rd} = {self.registers[rs1]} + {self.registers[rs2]} = {self.registers[rd]}")
            self.pc += 1

        
        elif instruction.lower() == "sub":
            if rd == 0:
                print("X0 is hardwired to 0 & contains x0=0")
                self.pc += 1
                return
            self.registers[rd] = self.registers[rs1] - self.registers[rs2]
             #print(f"Core {self.core_id} - SUB: x{rd} = {self.registers[rs1]} - {self.registers[rs2]} = {self.registers[rd]}")
            self.pc += 1
        elif instruction.lower() == "lw":
            if rd == 0:
                print("X0 is hardwired to 0 & contains x0=0")
                self.pc += 1
                return
            address = self.registers[rs1] + imm
            temp = self.registers[rd]
            self.registers[rd] = self.memory.load_word(address, self.core_id, self.is_active,lst)

            if lst[0]:
                 self.is_active=True
                 #print(f"Core {self.core_id} - LW: x{rd} loaded with {self.registers[rd]} from address {address}")
            else:
                 #print(f"x{rd} contains its previously loaded value (if not loaded contains 0)")
                self.registers[rd] = temp
                lst[0] = [True]
            self.pc += 1
        elif instruction.lower() == "la":
            if rd == 0:
                print("X0 is hardwired to 0 & contains x0=0")
                self.pc += 1
                return
            self.registers[rd] = self.core_id * 1024 + imm
             #print(f"Core {self.core_id} - LA: x{rd} loaded with {self.registers[rd]} from address containing val {self.memory.load_word(self.core_id * 1024 + imm, self.core_id, self.is_active)}")
            self.pc += 1
        elif instruction.lower() == "sw":
            address = self.registers[rs1] + imm
            self.memory.store_word(address, self.registers[rs2], self.core_id, self.is_active,lst)
            
            if lst[0]:
                self.is_active=True
                 #print(f"Core {self.core_id} - SW: Stored {self.registers[rs2]} at address {address}")
            else:
                lst[0] = [True]
            self.pc += 1
        elif instruction.lower() == "bne":
            if self.registers[rs1] != self.registers[rs2]:
                 #print(f"Core {self.core_id} - BNE: x{rs1} != x{rs2} (Jumping to {label})")
                if label:
                 should_increment_pc = False
                 self.pc = label_map[label]
                else :
                 self.pc+=imm
            else:
                self.pc += 1
                print("In BNE Instruction if condition not taken")
        
        elif instruction.lower() == "beq":
            if self.registers[rs1] == self.registers[rs2]:
                 #print(f"Core {self.core_id} - BEQ: x{rs1} == x{rs2} (Jumping to {label})")
                if label:
                 should_increment_pc = False
                 self.pc = label_map[label]
                else :
                 self.pc+=imm
                
            else:
                self.pc += 1
                 #print("In BEQ Instruction if condition not taken")
        
        elif instruction.lower() == "ble":
            if self.registers[rs1] <= self.registers[rs2]:
                 #print(f"Core {self.core_id} - BLE: x{rs1} <= x{rs2} (Jumping to {label})")
                if label:
                 should_increment_pc = False
                 self.pc = label_map[label]
                else :
                 self.pc+=imm
                
            else:
                self.pc += 1
                 #print("In BLE Instruction if condition not taken")
        
        elif instruction.lower() == "addi":
            if rd == 0:
                self.pc += 1
                print("X0 is hardwired to 0 & contains x0=0")
                return
             #print(f"Core {self.core_id} - ADDI: Register x{rd} = {self.registers[rs1]} + {imm}")
            self.registers[rd] = self.registers[rs1] + imm
            self.pc += 1
        elif instruction.lower() == "mv":
            if rd == 0:
                self.pc += 1
                print("X0 is hardwired to 0 & contains x0=0")
                return
             #print(f"Core {self.core_id} - MV: Register x{rd} = {self.registers[rs1]} + {imm}")
            self.registers[rd] = self.registers[rs1] + imm
            self.pc += 1
        elif instruction == "jal" or instruction == "JAL":
            #print(f"Core {self.core_id}- JAL: Saving return address in x{rd}, jumping to {label}")
           self.registers[rd] = self.pc + 1  # Save return address
           if label:
               shouldIncrementPC = False
               self.pc = label_map[label]  # Jump to label
           else:
               self.pc  += imm  # Fallback
        
        elif instruction == "j" or instruction == "J":
             #print(f"Core {self.core_id} - JUMP: Jumping to {label}")
            shouldIncrementPC = False
            self.pc = label_map[label] if label else self.pc + imm

   

        self.registers[0] = 0  # Ensure x0 is always zero
        self.registers[32] = self.core_id  # Ensure x32 is always core ID
        
        #if should_increment_pc:
    
    def print_registers(self):
        print(f"\nRegister Dump for Core {self.core_id}:")
        for i in range(33):
            print(f"x{i} = {self.registers[i]}")

 # Inside Core class
def get_registers_str(self):
    return "\n".join([f"X{i}: {self.registers[i]}" for i in range(len(self.registers))])
