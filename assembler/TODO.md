# assembler
Assembler component of cerebellum brainf*** compiler

## needed operations
intel syntax instructions needed:  
 - mov r, r - initialize stack
 - mov r/m, r - print
 - mov r, r/m
 - mov r/m, imm8 
 - add EAX, imm32
 - push imm32
 - push sym
 - push r
 - call sym
 - cmp r/m, imm8 - compare with zero
 - jz rel32