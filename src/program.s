.text
   
   addi x2,x0,25
   mv x31,x2
  # addi x3,x0,33
   mv x30,x3
   addi x5,x0,300
   mv x29,x5
   addi x6,x0,88
   mv x28,x6
   addi x7,x0,22
   mv x27,x7
   addi x8,x0,11
   lw x5, 8(x2)
    add x10, x2, x3  
    bne x2, x3, 8
    sub x11, x5, x6  
    sw x5, 16(x2) 
    add x7, x8, x9
    addi x22,x11,777
