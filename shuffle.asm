        	.ORIG x3000
        	LEA R1,ADDRESS
LOOP    	ADD R0,R0,#0 ;test for if any more words exist
        	BRz DONE
        	LDB R2,R1,#0 ;first get upper byte
        	LDW R3,R1,#0 ;get entire 2 bytes and left shift 8 times to move lower byte
        	LSHF R3,R3,#8
        	ADD R2,R2,R3 ;combine the 2 bytes, store in R2
        	STW R2,R1,#0 ;store it back into R1
        	ADD R1,R1,#2 ;update offset and total number of values
        	ADD R0,R0,#-1
        	BR LOOP
DONE    	HALT
ADDRESS	.FILL x3050
       	.END