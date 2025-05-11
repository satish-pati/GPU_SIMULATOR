.data
arr: 1:20000
sums:   .word 0, 0, 0, 0
.text
main:
    addi x5,x0, 0             #x5 = sum[CID]
    addi x7,x0, 100        #count loop
    addi x10,x0 ,100     #X = 100(cache_size / 4)
    li x6,1008     #base of array
    addi x31,x0,4          #4 bytes per word
    li    x20, 400
outer_loop:
    addi x8,x0,0             # i = 0
    addi x11,x6,0
inner_loop:
    lw x12, 0(x11)       # load a[i*X]
    add x5, x5, x12      # sum[CID] += a[i*X]
    addi x8, x8, 1
    add  x11, x11, x20      # update pointer to next element position in the inner stride
    addi x13,X0,200
    blt x8, x13, inner_loop
    addi x7, x7, -1
    bne x7,x0, outer_loop
    #Save partial sum in memory
    la x14, sums
    addi x15,x0, 4
    mul x16, x32, x15
    add x14, x14, x16
    sw x5, 0(x14)
    sync
    # Combine if CID == 1
    bne cid, 1, end
    la x14, sums
    lw x5, 0(x14)
    lw x6, 4(x14)
    lw x7, 8(x14)
    lw x8, 12(x14)
    add x5, x5, x6
    add x5, x5, x7
    add x5, x5, x8
    # x5 now contains total sum
end:
    ecall