// pipeline.h
#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>

// A structure to hold an instruction (converted from your tuple representation)
struct Instruction {
    std::string opcode;
    int rd;
    int rs1;
    int rs2;
    int imm;
    std::string label;
    int index; // The program index (for PC matching)
};

// Pipeline stages enumeration
enum Stage {
    STAGE_INVALID,
    STAGE_IF,
    STAGE_ID,
    STAGE_EX,
    STAGE_MEM,
    STAGE_WB,
    STAGE_COMPLETE
};

// Pipeline register to hold an instruction and additional info per stage
struct PipelineRegister {
    bool valid;             // Indicates if the register holds a valid instruction
    Instruction instr;      // The instruction
    Stage stage;            // Current stage of the instruction
    int remainingLatency;   // For variable-latency instructions in the EX stage
    int computedResult;     // To hold result from EX stage (if applicable)

    PipelineRegister() : valid(false), stage(STAGE_INVALID), remainingLatency(0), computedResult(0) {}
};

#endif

