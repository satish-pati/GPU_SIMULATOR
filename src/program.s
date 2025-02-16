.text
  .text
   ADD x32, x0, x32     #cannot write to x32
      ADD   x0,  x32, x32     # can read 
   ADD   x1,  x0, x32    
   ADDI x2, x0, 20     # x2 = 20
    ADDI x3, x0, 3000  # x3 = Address (assume 0x100 as base memory address)
    sw x1, 80(x3)
    sw x1, 0(x3)       
    sw x2, 4(x3) 
        lw x4 ,80(x3)  #if trying to access a invalid mem through load  then reg value rd remains same as before   
    lw x4, 0(x3)        
    LW x5, 4(x3)  
    addi x3,x0,1028 
    sw  x1,0(x3) 
    lw x5, 0(x3)   
    add x6, x4, x5      
    sub x7, x5, x4
    sw    x7,1020(x3)
    sw    x7,3064(x3)
    # comment    
    sw x6, 8(x3)       
    sw x7, 12(x3)      
    lw x8, 8(x3)        
    lw x9, 12(x3)       
