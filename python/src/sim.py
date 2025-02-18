import sys
from core import Core
from memory import Memory
import re

class Simulator:
    def __init__(self, filename):
        self.clock = 0
        self.memory = Memory()
        self.label_map = {}
        self.data_labels = {}
        self.program = self.load_program_from_file(filename)
         # 🔹 Print parsed program
        print("\nParsed Program Instructions:")
        for i, instr in enumerate(self.program):
            print(f"Instruction {i}: {instr}")
        self.cores = [Core(self.memory, self.program, i) for i in range(4)]
    def assemble_instruction(self, line):
     parts = line.replace(',', ' ').split()  # Ensure proper tokenization
     if not parts:
        return None  # Handle empty lines safely

     instr = parts[0].lower()
     rd, rs1, rs2, imm, label = 0, 0, 0, 0, None  # Label is now None by default

     if instr in {"add", "sub"}:
        if len(parts) >= 4:
            rd, rs1, rs2 = map(lambda x: int(x[1:]), parts[1:4])
        return instr, rd, rs1, rs2, imm, label

     elif instr == "addi":
        if len(parts) >= 4:
            rd, rs1, imm = int(parts[1][1:]), int(parts[2][1:]), int(parts[3])
        return instr, rd, rs1, rs2, imm, label

     elif instr == "mv":
        if len(parts) >= 3:
            rd, rs1 = int(parts[1][1:]), int(parts[2][1:])
        return "addi", rd, rs1, 0, imm, label

     elif instr == "lw":
        if len(parts) >= 3:
            rd, imm_rs1 = parts[1], parts[2]
            rd = int(rd[1:])
            imm, rs1 = imm_rs1.split('(')
            imm = int(imm)
            rs1 = int(rs1[:-1][1:])
        return instr, rd, rs1, rs2, imm, label

     elif instr == "la":
        if len(parts) >= 3:
            rd = int(parts[1][1:])
            imm_label = parts[2]
            if imm_label in self.data_labels:
                imm = self.data_labels[imm_label]
            else:
                print(f"Error: Undefined data label '{imm_label}'")
                return None
        return instr, rd, 0, 0, imm, imm_label  # Store label correctly

     elif instr == "sw":
        if len(parts) >= 3:
            rs2, imm_rs1 = parts[1], parts[2]
            rs2 = int(rs2[1:])
            imm, rs1 = imm_rs1.split('(')
            imm = int(imm)
            rs1 = int(rs1[:-1][1:])
        return instr, 0, rs1, rs2, imm, label

     elif instr in {"bne", "beq", "ble"}:
        if len(parts) >= 4:
            rs1, rs2, label = int(parts[1][1:]), int(parts[2][1:]), parts[3]
        return instr, 0, rs1, rs2, imm, label  # Store actual label

     elif instr == "j":
        if len(parts) > 1:
             
            label = parts[1]  # Store label correctly
        return instr, 0, 0, 0, imm, label  # Store jump label

     else:
        print(f"Error: Unknown instruction '{instr}'")
        return None  # Handle unknown instructions gracefully


    def load_program_from_file(self, filename):
        program = []
        try:
            with open(filename, 'r') as file:
                print("File opened successfully")
                
                in_text_section = False
                in_data_section = False
                data_address = 0x0
                instruction_index = 0
                
                lines = file.readlines()
                
                # First Pass: Identify Labels
                for line in lines:
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                    
                    if ".data" in line:
                        in_data_section = True
                        continue
                    if ".text" in line:
                        in_data_section = False
                        in_text_section = True
                        continue
                    
                    if in_data_section:
                        tokens = line.split()
                        if not tokens:
                            continue
                        
                        if ':' in tokens[0]:
                            # label = tokens[0].replace(':', '').strip()
                            # self.data_labels[label] = data_address
                            # tokens = tokens[1:]
                            label, *rest = tokens[0].split(':', 1)  # Split label and the rest
                            label = label.strip()
                            self.data_labels[label] = data_address
                            tokens = rest + tokens[1:]  # Merge back the rest with other tokens
                            
                        # for token in tokens:
                        #     token = token.strip(',')
                        #     if token.lstrip('-').isdigit():
                        #         value = int(token)
                        #         for core_id in range(4):
                        #             self.memory.store_word(core_id * 1024 + data_address, value, core_id, True)
                        #         data_address += 4
                    values=[]
                        if tokens and tokens[0] == ".word":  # Ensure it's a .word declaration
                           if len(tokens) > 1: 
                              values = re.split(r'[,\s]+', " ".join(tokens[1:]))  # Handle spaces and commas
                              values = [v for v in values if v]  # Remove empty strings

                        for value in values:
                            try:
                                num = int(value)  # Ensure it's a valid integer
                                for core_id in range(4):
                                    self.memory.store_word(core_id * 1024 + data_address, num, core_id, True)
                                data_address += 4
                            except ValueError:
                                print(f"Warning: Invalid integer '{value}' in .word declaration")
                                
                    elif in_text_section:
                        label_match = re.match(r'^(\w+):', line)
                        if label_match:
                            label = label_match.group(1)
                            self.label_map[label] = instruction_index
                            line = line[label_match.end():].strip()
                            if not line:
                                continue
                        
                        comment_match = re.search(r'(#|//)', line)
                        if comment_match:
                            line = line[:comment_match.start()].strip()
                        
                        if not line:
                            continue
                        
                        instr = self.assemble_instruction(line)
                        if instr is None:
                            print(f"Error: Failed to parse instruction -> {line}")
                            continue
                        else:
                            program.append(instr)
                            instruction_index += 1
                
                # Second Pass: Resolve Labels
                for i, instr in enumerate(program):
                    label = instr[5]
                    if label and label in self.label_map:
                        program[i] = (*instr[:4], self.label_map[label], label)
                    elif label and label in self.data_labels:
                        program[i] = (*instr[:4], self.data_labels[label], label)
                    elif label:
                        print(f"Error: Undefined label '{label}'")
        
        except FileNotFoundError:
            print(f"Error: Unable to open file {filename}")
        
        return program
    def execute_core(self,core, core_id, instruction_index, program, label_map):
     if instruction_index < len(program):
        inst = program[instruction_index]
        print(f"\n[Core {core_id}] Executing: {inst[0]}")
        core.execute(inst[0], inst[1], inst[2], inst[3], inst[4], inst[5], label_map)


    def run(self):
     total_instructions = len(self.program)
     running = True

     while running:
        running = False  # Assume all cores are finished

        print(f"\n--- Clock Cycle: {self.clock} ---")

        for i in range(4):
            if self.cores[i].pc < total_instructions:
                self.execute_core(self.cores[i], i, self.cores[i].pc, self.program, self.label_map)
                running = True  # At least one core is still running

        if not running:
            break
        
        self.clock += 1
     print("\nFinal State of Registers After Execution:")
     for i, core in enumerate(self.cores):
            print(f"Core {i} Registers:")
            core.print_registers()
        
     print("\nFinal State of Memory After Execution:")
     for i in range(4):
            self.memory.print_memory(i)


        
        

if __name__ == "__main__":
    simulator = Simulator("program.s")
    simulator.run()
    print(f"No. of clock cycles: {simulator.clock}")
