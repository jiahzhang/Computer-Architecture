.ORIG x3000
        ;add first 20 numbers
        LEA R0, ADDRESS1
        LDW R0, R0, #0
        AND R1, R1, #0
        AND R2, R2, #0
        ADD R2, R2, #10
	    ADD R2, R2, #10
LOOP    LDB R3, R0, #0
        LDB R4, R0, #1
        ADD R1, R1, R3
        ADD R1, R1, R4
        ADD R0, R0, #2
        ADD R2, R2, #-2
        BRp LOOP

        ;store at xC014
        STW R1, R0, #0

        ;store at xC017 - unaligned exception
        ;ADD R0, R0, #3
        ;STW R1, R0, #0

        ;store at x0000 - protection exception
        ;AND R0, R0, #0
        ;STW R1, R0, #0

        ;store at x0001 - unaligned/protect but protection exception has higher priority
        ;AND R0, R0, #0
        ;ADD R0, R0, #1
        ;STW R1, R0, #0

        ;unknown opcode
        ;.FILL xA000
        ;.FILL xB000
        JMP R1
TRAP x25
ADDRESS1 .FILL xC000
.END