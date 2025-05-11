.data
arr: .word 1:64     # 64 words = 256B, smaller than L2

.text
main:
    li x5, 0
    li x6, 0
    li x7, 4         #loop 4 times over same elements

outer:
    li x6, 0
    li x8, 64
inner:
    la x10, arr
    li x11, 4
    mul x12, x6, x11
    add x10, x10, x12
    lw x13, 0(x10)
    add x5, x5, x13
    addi x6, x6, 1
    blt x6, x8, inner

    addi x7, x7, -1
    bne x7, x0, outer

    ecall
