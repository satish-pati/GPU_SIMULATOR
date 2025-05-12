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
