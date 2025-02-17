.data
array:   .word 10, 20, 30, 40
value:   .word 100

.text
    la x10, array       # Load base address of 'array' into t0
    la x11, value       # Load address of 'value' into t1
    lw x1, 0(x10)       # Load first value (10) of 'array' into x1
    lw x2, 4(x10)       # Load 'value' (100) into x2
    lw x3, 8(x10)       # Load second value (20) of 'array' into x3
    lw x4, 12(x10)       # Load second value (20) of 'array' into x3
