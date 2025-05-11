.data
arr:     .word 1:10000
sums:   .word 0, 0, 0, 0
.text
main:
    li x10, 100          # X = 100 (cache size / 4)  ← Not used, consider removing?
    la x6, arr          # base of array
    li x14, 100          # total loop count (moved earlier)
    li x20, 400          # stride
    add x11, x6, x0      # addr = &array[i*X]
    li x13, 0            # SPM base index
    li x8, 0             # i = 0
fill_loop:
    lw x12, 0(x11)
    sw_spm x12, 0(x13)
    addi x13, x13, 4     # SPM pointer increment
    add x11, x11, x20    # array pointer increment
    addi x8, x8, 1
    blt x8, x14, fill_loop

    li x5, 0             # sum[CID]
    li x7, 100           # outer loop counter
    li x13, 400          # total bytes in SPM (100 * 4)
    
outer_loop_spm:
    li x8, 0             # i = 0
    li x9, 0             # SPM address

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
    sw x5, 0(x15)
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

end_spm:
    ecall