.ORIG x3000
    ADD R0,R0,#1
    LSHF R0,R0,#7
    LSHF R0,R0,#7
loop    ADD R0,R0,#-1
    BRp loop
    TRAP x25
.END