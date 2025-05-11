.data
arr: 1:20000
sums: .word 0, 0, 0, 0

.text
main:
    li x10, 100            # X = 100 (cache size / 4)
    la x6, arr            # base address of arr[]
    li x31, 4             # word size = 4 bytes
    li x13, 0              # SPM pointer for storing
    li x20, 400            # stride = X * word size
    li x8, 0               # i = 0
    li x14, 100            # loop bound for fill
    la x11, arr            # pointer to arr[i*X]
fill_loop:
    lw x12, 0(x11)         # load arr[i*X]
    sw_spm x12, 0(x13)     # store to SPM[i]
    add x11, x11, x20      # next arr[i+1*X]
    addi x13, x13, 4       # move SPM pointer
    addi x8, x8, 1
    blt x8, x14, fill_loop
    li x5, 0               # sum[CID]
    li x7, 100             # count loop
    li x8, 0               # i = 0
    li x9, 0               # SPM load pointer
    li x18, 400            # stride (X * 4)
    li x19, 200            # total inner loop iterations
outer_loop_spm:
    li x8, 0               # reset i for inner loop
    li x9, 0               # reset SPM pointer
inner_loop:
    blt x8, x10, lw_from_spm
    # access from DRAM
    mul x27, x8, x18       # offset = i * X * 4
    add x27, x6, x27       # addr = base + offset
    lw x12, 0(x27)
    j calculate_sum
lw_from_spm:
    lw_spm x12, 0(x9)      # load from SPM[i]
    addi x9, x9, 4         # increment SPM pointer
calculate_sum:
    add x5, x5, x12        # sum[CID] += value
    addi x8, x8, 1
    blt x8, x19, inner_loop
    addi x7, x7, -1
    bne x7, x0, outer_loop_spm
    # Save partial sum
    la x15, sums
    mul x17, x32, x31     # offset = CID * 4
    add x15, x15, x17
    sw x5, 0(x15)
    sync
    # Combine only if CID == 1
    bne cid, 1, end_spm
    lw x5, 0(x15)
    lw x6, 4(x15)
    lw x7, 8(x15)
    lw x8, 12(x15)
    add x5, x5, x6
    add x5, x5, x7
    add x5, x5, x8
end_spm:
    ecall
