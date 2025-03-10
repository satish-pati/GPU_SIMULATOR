.data
array:         .word 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100
partialsums:   .word 0, 0, 0, 0
.text
main:
    mv    x5, x32         # x5 = CID
    addi    x6,x0,25      # x6 = 25
    mul   x5, x5, x6      # x5 = CID * 25
    addi  x31,x0,4
    mul   x5,x5,x31       # x5 = (CID * 25) * 4 (byte offset into array)
    mv    x7, x5         # x7 now holds the base byte offset for this compute unit
    addi  x8,x0,0         # x8 = i = 0
    addi  x9,x0,0         # x9 = local sum = 0
loop:
    addi  x11,x0,25       # x11 = 25 (number of elements to sum)
    beq   x8, x11, loopend  # if i == 25, exit loop
    mul  x12, x8,x31    # x12 = i * 4 (byte offset for current element)
    add   x13, x7, x12  # x13 = base offset + (i * 4)
    la    x14, array    # x14 = base address of array
    add   x15, x14, x13 # x15 = address of the current element in array
    lw    x16, 0(x15)   # x16 = array element
    add   x9, x9, x16   # local sum += element
    addi  x8, x8, 1     # i++
    j     loop
loopend:
    la    x5, partialsums   # x5 = base address of partialsums
    addi    x7,x0,4           # x7 = 4 (byte size per word)
    mul   x6, x32, x7       # x6 = cid * 4 (compute byte offset for this core)
    add   x5, x5, x6        # x5 now points to partialsums[cid]
    sw    x9, 0(x5)         # store the local sum (x9) into the appropriate slot
    bne   cid, 0, end   # if core ID (x32) is not 0, skip final reduction          
finish:
    la x5,partialsums
    lw  x21,0(x5) #partialsums[0]
    lw x18,4(x5) #partialsums[1]
    lw x19,8(x5) #partialsums[2]
    lw  x20,12(x5) #partialsums[3]
    add x21,x21,x18
    add x21,x21,x19
    add x21,x21,x20 #x21 has final sum
    mv x10,x21
    addi x17,x0,1 #x17 =1 integer
    ecall
end:
#complete
