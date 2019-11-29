.ORIG x1200
        ADD R6, R6, #-2
        STW R0, R6, #0
        ADD R6, R6, #-2
        STW R1, R6, #0
        ADD R6, R6, #-2
        STW R2, R6, #0
        ADD R6, R6, #-2
        STW R3, R6, #0

        LEA R0, MASK
        LDW R3, R0, #0
        LEA R0, ADDRESS
        LDW R0, R0, #0
        AND R2, R2, #0
        ADD R2, R2, #8
        LSHF R2, R2, #4
LOOP    ADD R2, R2, #0
        BRz DONE
        LDW R1, R0, #0
        AND R1, R1, R3
        STW R1, R0, #0
        ADD R0, R0, #2
        ADD R2, R2, #-1
        BRnzp LOOP

DONE    LDW R3, R6, #0
        ADD R6, R6, #2
        LDW R2, R6, #0
        ADD R6, R6, #2
        LDW R1, R6, #0
        ADD R6, R6, #2
        LDW R0, R6, #0
        ADD R6, R6, #2
        RTI
ADDRESS .FILL x1000
MASK .FILL xFFFE
.END