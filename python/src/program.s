
.data
arr: .word 3, 2, 0, 5, 1, 9, 4, 6, 8, 7, 11, 10

.text

addi x2,x0,12   # no. of elements
addi x1,x2,-1  # n-1      
addi x4,x0,0   # outer loop i value

Loop1:
beq x4, x1, exit1
    sub x5, x1, x4            # n-1-i
    addi x6, x0, 0            # inner loop j value
    la x3, arr               # reset x3 to the start of the array at the beginning of each outer loop 

Loop2:lw x7, 0(x3)
    lw x8, 4(x3)
    beq x6, x5, exit2
    ble x7, x8, skip
    sw x8, 0(x3)
    sw x7, 4(x3)
skip:
    addi x3, x3,4   # move to the next element in the array
    addi x6, x6, 1   # increment inner loop counter
    j Loop2

exit2:
    addi x4, x4,1   # increment outer loop counter
    j Loop1

exit1:
