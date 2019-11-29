/*
    Name 1: Jiahan Zhang
    UTEID 1: jz23745
*/
/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {
    IRD1, IRD0,
    COND3, COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    LD_EXCV,
    LD_PSR,
    LD_OLDPSR,
    LD_EVENT,
    LD_USP,
    INTFLAGCLR,
    CCUPDATE,
    LD_TRAPFLAG,
    TRAPFLAGCLR,
    LD_WFLAG,
    WFLAGCLR,
    LD_VA,
    LD_NS,
    LD_MR,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    GATE_IE,
    GATE_OLDPSR,
    GATE_STACK,
    GATE_VAMUX,
    GATE_MAR,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    MARMUX2,
    PSRMUX1, PSRMUX0,
    STACKADDRMUX,
    STACKMUX1, STACKMUX0,
    EVENTMUX,
    VAMUX,
    NSMUX2, NSMUX1, NSMUX0,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
            CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return((x[IRD1] << 1) + x[IRD0]); }
int GetCOND(int *x)          { return((x[COND3] << 3) + (x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
                                      (x[J3] << 3) + (x[J2] << 2) +
                                      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetLD_EXCV(int *x)       { return(x[LD_EXCV]); }
int GetLD_PSR(int *x)        { return(x[LD_PSR]); }
int GetLD_OLDPSR(int *x)     { return(x[LD_OLDPSR]); }
int GetLD_EVENT(int *x)      { return(x[LD_EVENT]); }
int GetLD_USP(int *x)        { return(x[LD_USP]); }
int GetINTFLAGCLR(int *x)    { return(x[INTFLAGCLR]); }
int GetCCUPDATE(int *x)      { return(x[CCUPDATE]); }
int GetLD_TRAPFLAG(int *x)   { return(x[LD_TRAPFLAG]); }
int GetTRAPFLAGCLR(int *x)   { return(x[TRAPFLAGCLR]); }
int GetLD_WFLAG(int *x)      { return(x[LD_WFLAG]); }
int GetWFLAGCLR(int *x)      { return(x[WFLAGCLR]); }
int GetLD_VA(int *x)         { return(x[LD_VA]); }
int GetLD_NS(int *x)         { return(x[LD_NS]); }
int GetLD_MR(int *x)         { return(x[LD_MR]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetGATE_IE(int *x)       { return(x[GATE_IE]); }
int GetGATE_OLDPSR(int *x)   { return(x[GATE_OLDPSR]); }
int GetGATE_STACK(int *x)    { return(x[GATE_STACK]); }
int GetGATE_VAMUX(int *x)    { return(x[GATE_VAMUX]); }
int GetGATE_MAR(int *x)      { return(x[GATE_MAR]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetMARMUX2(int *x)       { return(x[MARMUX2]); }
int GetPSRMUX(int *x)        { return((x[PSRMUX1] << 1) + x[PSRMUX0]); }
int GetSTACKADDRMUX(int *x)  { return(x[STACKADDRMUX]); }
int GetSTACKMUX(int *x)      { return((x[STACKMUX1] << 1) + x[STACKMUX0]); }
int GetEVENTMUX(int *x)      { return(x[EVENTMUX]); }
int GetVAMUX(int *x)         { return(x[VAMUX]); }
int GetNSMUX(int *x)         { return((x[NSMUX2] << 2) + (x[NSMUX1] << 1) + x[NSMUX0]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); }
int GetLSHF1(int *x)         { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

    int PC,		/* program counter */
            MDR,	/* memory data register */
            MAR,	/* memory address register */
            IR,		/* instruction register */
            N,		/* n condition bit */
            Z,		/* z condition bit */
            P,		/* p condition bit */
            BEN;        /* ben register */

    int READY;	/* ready bit */
    /* The ready bit is also latched as you dont want the memory system to assert it
       at a bad point in the cycle*/

    int REGS[LC_3b_REGS]; /* register file. */

    int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

    int STATE_NUMBER; /* Current State Number - Provided for debugging */

/* For lab 4 */
    int INTV; /* Interrupt vector register */
    int EXCV; /* Exception vector register */
    int SSP; /* Initial value of system stack pointer */
/* MODIFY: You may add system latches that are required by your implementation */
    int PSR;
    int OLDPSR;
    int USP;
    int EVENT;
    int INTFLAG;

    int PTBR; /* This is initialized when we load the page table */
    int VA;   /* Temporary VA register */
    int TRAP;
    int W;
    int NS;
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    //ISR ends at 513
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();
    eval_micro_sequencer();

    if(CYCLE_COUNT == 299){
        NEXT_LATCHES.INTFLAG |= 0x01;
        NEXT_LATCHES.INTV = 0x01;
    }
    CURRENT_LATCHES = NEXT_LATCHES;

    CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
        printf("Error: Can't open micro-code file %s\n", ucode_filename);
        exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
            printf("Error: Too few lines (%d) in micro-code file: %s\n",
                   i, ucode_filename);
            exit(-1);
        }

        /* Put in bits one at a time. */
        index = 0;

        for (j = 0; j < CONTROL_STORE_BITS; j++) {
            /* Needs to find enough bits in line. */
            if (line[index] == '\0') {
                printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                       ucode_filename, i);
                exit(-1);
            }
            if (line[index] != '0' && line[index] != '1') {
                printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                       ucode_filename, i, j);
                exit(-1);
            }

            /* Set the bit in the Control Store. */
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
            index++;
        }

        /* Warn about extra bits in line. */
        if (line[index] != '\0')
            printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    if (is_virtual_base) {
        if (CURRENT_LATCHES.PTBR == 0) {
            printf("Error: Page table base not loaded %s\n", program_filename);
            exit(-1);
        }

        /* convert virtual_base to physical_base */
        virtual_pc = program_base << 1;
        pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) |
              MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

        printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
        if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
            program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
            printf("physical base of program: %x\n\n", program_base);
            program_base = program_base >> 1;
        } else {
            printf("attempting to load a program into an invalid (non-resident) page\n\n");
            exit(-1);
        }
    }
    else {
        /* is page table */
        CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base)
        CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *pagetable_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);

    init_memory();
    load_program(pagetable_filename,0);
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename,1);
        while(*(program_filename++) != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

/* MODIFY: you can add more initialization code HERE */
    CURRENT_LATCHES.PSR = 0x8002; /* Initial value of PSR**/
    CURRENT_LATCHES.PTBR = 0x1000;

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
        printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argv[3], argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int16_t outputMARMUX2();
int16_t outputPSRMUX();

/*
 * Set next state microinstruction based on next state integer
 */
void set_nextMicroInstruction(int16_t J){
    for(int i = 0; i < CONTROL_STORE_BITS; i++){
        NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[J][i];
    }
    NEXT_LATCHES.STATE_NUMBER = J;
}

void eval_micro_sequencer() {

    /*
     * Evaluate the address of the next state according to the
     * micro sequencer logic. Latch the next microinstruction.
     */

    //Get relevant signals
    uint16_t IRD = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);

    uint16_t COND_0 = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0001;
    uint16_t COND_1 = Low16bits((GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0002) >> 1);
    uint16_t COND_2 = Low16bits((GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0004) >> 2);
    uint16_t COND_3 = Low16bits((GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0008) >> 3);

    uint16_t J_0 = GetJ(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0001;
    uint16_t J_1 = Low16bits((GetJ(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0002) >> 1);
    uint16_t J_2 = Low16bits((GetJ(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0004) >> 2);
    uint16_t J_3 = Low16bits((GetJ(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0008) >> 3);
    uint16_t J_4 = Low16bits((GetJ(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0010) >> 4);
    uint16_t J_5 = Low16bits((GetJ(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0020) >> 5);

    uint16_t BEN = CURRENT_LATCHES.BEN & 0x0001;
    uint16_t IR_11 = Low16bits((CURRENT_LATCHES.IR & 0x0800) >> 11);
    uint16_t R = CURRENT_LATCHES.READY & 0x0001;
    uint16_t IR_15_12 = Low16bits((CURRENT_LATCHES.IR & 0xF000) >> 12);

    uint16_t PSR_15 = Low16bits((outputPSRMUX() & 0x8000) >> 15);
    uint16_t MAR_0 = Low16bits( outputMARMUX2() & 0x0001);
    uint16_t unknown = Low16bits((IR_15_12 == 10 ? 1 : 0) | (IR_15_12 == 11 ? 1 : 0));
    uint16_t unaligned_only = Low16bits(MAR_0);
    uint16_t prot_pagefault = Low16bits((((CURRENT_LATCHES.PSR & 0x8000) >> 15) && !(CURRENT_LATCHES.TRAP) && !((CURRENT_LATCHES.MDR & 0x0008) >> 3)) || !((CURRENT_LATCHES.MDR & 0x0004) >> 2));

    uint16_t I = CURRENT_LATCHES.INTFLAG;
    uint16_t E = Low16bits((unknown & !COND_3 & COND_2 & COND_1 & !COND_0) | (unaligned_only & !COND_3 & COND_2 & COND_1 & COND_0) | (prot_pagefault & COND_3 & !COND_2 & !COND_1 & !COND_0));


    //Next state based on signals
    if(IRD == 0){
        //Generate values based on gates
        J_0 = Low16bits(J_0 | E |(IR_11 & !COND_3 & !COND_2 & COND_1 & COND_0));
        J_1 = Low16bits(J_1 | E | (R & !COND_3 &  !COND_2 & !COND_1 & COND_0) | ((R & !COND_3 &  COND_2 & !COND_1 & !COND_0) & (!I & !COND_3 &  COND_2 & !COND_1 & !COND_0)));
        J_2 = Low16bits(J_2 | E | (BEN & !COND_3 & !COND_2 & COND_1 & !COND_0));
        J_3  = Low16bits(J_3 | E | (!COND_3 & !COND_2 & COND_1 & !COND_0 & !BEN & I));
        J_4 = (J_4 | E);
        J_5 = (J_5 | E | ((I & !COND_3 & COND_2 & !COND_1 & COND_0) | ((!COND_3 & COND_2 & !COND_1 & !COND_0 & R) & (!COND_3 & COND_2 & !COND_1 & !COND_0) & I)));

        uint16_t J = Low16bits( (J_5 << 5) | (J_4 << 4) | (J_3 << 3) | (J_2 << 2) | (J_1 << 1) | J_0);

        set_nextMicroInstruction(J);
    }
    //Next state is 0+0+IR[15:12] (Decode state only)
    else if(IRD == 1){
        if(E == 0) {
            set_nextMicroInstruction(IR_15_12);
        }
        else{
            //used for unknown opcode exception
            set_nextMicroInstruction(63);
        }
    }
    //Next state dependent on NS register
    else{
        set_nextMicroInstruction(CURRENT_LATCHES.NS);
    }
}

void cycle_memory() {

    /*
     * This function emulates memory and the WE logic.
     * Keep track of which cycle of MEMEN we are dealing with.
     * If fourth, we need to latch Ready bit at the end of
     * cycle to prepare microsequencer for the fifth cycle.
     */
    static int MEM_CYCLE = 0;

    if(GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        MEM_CYCLE++;
    }

    if(MEM_CYCLE == 4){
        NEXT_LATCHES.READY = 1;
        MEM_CYCLE = -1;
    }
}

/*
 * Performs a sign extension on the given number
 */
int16_t SEXT(int16_t num, int numBits){
    /*
    int16_t temp = num >> msbPosition;
    temp &= 0x0001;

    //Sign extend by 1's;
    if(temp == 1){
        int m = 1U << (numBits - 1);
        i
    }
     */
    int16_t m = Low16bits(1U << (numBits - 1));
    int16_t r = Low16bits((num ^ m) - m);
    return r;
}

/*
 * Functions to calculate outputs of various logic circuits
 */
int16_t outputSR1MUX(){
    if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits((CURRENT_LATCHES.IR & 0x0E00) >> 9);
    }
    else if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return Low16bits((CURRENT_LATCHES.IR & 0x01C0) >> 6);
    }
    else{
        return 6;
    }
}

int16_t outputDRMUX(){
    if(GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits((CURRENT_LATCHES.IR & 0x0E00) >> 9);
    }
    else if(GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return 7;
    }
    else{
        return 6;
    }
}

int16_t outputADDR1MUX(){
    if(GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(CURRENT_LATCHES.PC);
    }
    else{
        return Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()]);
    }
}

int16_t outputADDR2MUX(){
    if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return 0;
    }
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return Low16bits(SEXT(CURRENT_LATCHES.IR & 0x003F, 6));
    }
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return Low16bits(SEXT(CURRENT_LATCHES.IR & 0x01FF, 9));
    }
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3){
        return Low16bits(SEXT(CURRENT_LATCHES.IR & 0x07FF, 11));
    }
}

int16_t outputLSHF1(){
    if(GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return Low16bits(outputADDR2MUX() << 1);
    }
    else{
        return Low16bits(outputADDR2MUX());
    }
}

int16_t outputADDER(){
    return Low16bits(outputLSHF1() + outputADDR1MUX());
}

int16_t outputMARMUX(){
    if(GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(Low16bits(CURRENT_LATCHES.IR & 0x00FF) << 1);
    }
    else{
        return Low16bits(outputADDER());
    }
}

int16_t outputMARMUX2(){
    if(GetMARMUX2(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(BUS);
    }
    else{
        return Low16bits((BUS << 1) + 0x200);
    }
}

int16_t outputPCMUX(){
    if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(CURRENT_LATCHES.PC + 2);
    }
    else if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return Low16bits(BUS);
    }
    else if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return Low16bits(outputADDER());
    }
    else{
        return Low16bits(CURRENT_LATCHES.PC - 2);
    }
}

int16_t outputSR2MUX(){
    int IR_5 = Low16bits((CURRENT_LATCHES.IR & 0x0020) >> 5);
    int16_t SR2;

    if(IR_5 == 0){
        SR2 = Low16bits(CURRENT_LATCHES.IR & 0x0007);
        SR2 = Low16bits(CURRENT_LATCHES.REGS[SR2]);
    }
    else{
        SR2 = Low16bits(CURRENT_LATCHES.IR & 0x001F);
        SR2 = SEXT(SR2, 5);
    }

    return Low16bits(SR2);
}

int16_t outputALU(){
    if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()] + outputSR2MUX());
    }
    else if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()] & outputSR2MUX());
    }
    else if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()] ^ outputSR2MUX());
    }
    else{
        return Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()]);
    }
}

int16_t outputSHF(){
    int16_t s0 = (CURRENT_LATCHES.IR & 0x0010) >> 4;
    int16_t s1 = (CURRENT_LATCHES.IR & 0x0020) >> 5;
    int amt = CURRENT_LATCHES.IR & 0x000F;
    if(s0 == 0) {
        return Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()] << amt);
    }
    else if(s1 == 0){
        uint16_t op1 = Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()]);
        return Low16bits(op1 >> amt);
    }
    else{
        int16_t op1 = Low16bits(CURRENT_LATCHES.REGS[outputSR1MUX()]);
        return Low16bits(op1 >> amt);
    }
}

int16_t outputPSRMUX(){
    if(GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        if ((int16_t) BUS > 0) {
            return Low16bits((CURRENT_LATCHES.PSR | 0x0001) & 0x8001);
        } else if ((int16_t) BUS == 0) {
            return Low16bits((CURRENT_LATCHES.PSR | 0x0002) & 0x8002);
        } else {
            return Low16bits((CURRENT_LATCHES.PSR | 0x0004) & 0x8004);
        }
    }
    else if(GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return Low16bits(CURRENT_LATCHES.PSR & 0x7FFF);
    }
    else if(GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return Low16bits(CURRENT_LATCHES.PSR | 0x8000);
    }
    else{
        return Low16bits(BUS);
    }
}

int16_t outputSTACKADDRMUX(){
    if(GetSTACKADDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(-2);
    }
    else{
        return Low16bits(2);
    }
}

int16_t outputSTACKMUX(){
    if(GetSTACKMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(outputSTACKADDRMUX() + CURRENT_LATCHES.REGS[outputSR1MUX()]);
    }
    else if(GetSTACKMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return Low16bits(CURRENT_LATCHES.USP);
    }
    else{
        return Low16bits(CURRENT_LATCHES.SSP);
    }
}

int16_t outputIEMUX(){
    if(CURRENT_LATCHES.EVENT == 0){
        return Low16bits(CURRENT_LATCHES.INTV);
    }
    else{
        return Low16bits(CURRENT_LATCHES.EXCV);
    }
}

int16_t outputEVENTMUX(){
    if(GetEVENTMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return Low16bits(0);
    }
    else{
        return Low16bits(1);
    }
}

int16_t outputVAMUX(){
    if(GetVAMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        int16_t VA = Low16bits((CURRENT_LATCHES.VA & 0xFE00) >> 9);
        return CURRENT_LATCHES.PTBR + (VA << 1);
    }
    else{
        int16_t VA = Low16bits((CURRENT_LATCHES.VA & 0x01FF));
        int16_t MDR = Low16bits(CURRENT_LATCHES.MDR & 0x3E00);
        return Low16bits(VA + MDR);
    }
}

int16_t outputNSMUX(){
    if(GetNSMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return 29;
    }
    else if(GetNSMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return 25;
    }
    else if(GetNSMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return 23;
    }
    else if(GetNSMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3){
        return 24;
    }
    else if(GetNSMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 4){
        return 33;
    }
    else if(GetNSMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 5){
        return 28;
    }
}

int16_t readMEM(){
    int16_t temp;

    //always read entire word
    int16_t lower = Low16bits(MEMORY[CURRENT_LATCHES.MAR >> 1][0]);
    int16_t upper = Low16bits(MEMORY[CURRENT_LATCHES.MAR >> 1][1]);
    upper = Low16bits(upper << 8);
    temp = Low16bits(upper | lower);

    NEXT_LATCHES.READY = 0;
    return temp;
}

int16_t writeMEM(){
    //BYTE
    if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        int16_t MAR_0 = CURRENT_LATCHES.MAR & 0x0001;
        //Lower byte
        if(MAR_0 == 0) {
            MEMORY[CURRENT_LATCHES.MAR >> 1][0] = Low16bits(CURRENT_LATCHES.MDR);
        }
            //Upper byte
        else{
            MEMORY[CURRENT_LATCHES.MAR >> 1][1] = Low16bits(CURRENT_LATCHES.MDR);
        }
    }
        //WORD
    else{
        MEMORY[CURRENT_LATCHES.MAR >> 1][0] = Low16bits(CURRENT_LATCHES.MDR & 0x00FF);
        MEMORY[CURRENT_LATCHES.MAR >> 1][1] = Low16bits((CURRENT_LATCHES.MDR & 0xFF00) >> 8);
    }

    NEXT_LATCHES.READY = 0;
}

/*
 * Tristate drivers
 */
int16_t Gate_MARMUX = 0;
int16_t Gate_PC = 0;
int16_t Gate_ALU = 0;
int16_t Gate_SHF = 0;
int16_t Gate_MDR = 0;
int16_t Gate_IE = 0;
int16_t Gate_OldPSR = 0;
int16_t Gate_Stack = 0;
int16_t Gate_MAR = 0;
int16_t Gate_VAMUX = 0;

void eval_bus_drivers() {

    /*
     * Datapath routine emulating operations before driving the bus.
     * Evaluate the input of tristate drivers
     *             Gate_MARMUX,
     *		 Gate_PC,
     *		 Gate_ALU,
     *		 Gate_SHF,
     *		 Gate_MDR.
     */
    //tristate drivers
    Gate_MARMUX = outputMARMUX();
    Gate_PC = CURRENT_LATCHES.PC;
    Gate_ALU = outputALU();
    Gate_SHF = outputSHF();
    Gate_IE = outputIEMUX();
    Gate_OldPSR = CURRENT_LATCHES.OLDPSR;
    Gate_Stack = outputSTACKMUX();
    Gate_MAR = CURRENT_LATCHES.MAR;
    Gate_VAMUX = outputVAMUX();

    if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        //BYTE
        int16_t MAR_0 = CURRENT_LATCHES.MAR & 0x0001;
        if(MAR_0 == 0){
            Gate_MDR = Low16bits(SEXT(CURRENT_LATCHES.MDR & 0x00FF, 8));
        }
        else{
            int16_t value = Low16bits((CURRENT_LATCHES.MDR & 0xFF00) >> 8);
            Gate_MDR = Low16bits(SEXT(value, 8));
        }
    }
    else{
        //WORD
        Gate_MDR = Low16bits(CURRENT_LATCHES.MDR);
    }

    //Write to memory if necessary
    if(GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        if(CURRENT_LATCHES.READY == 1) {
            writeMEM();
        }
    }
}

void drive_bus() {

    /*
     * Datapath routine for driving the bus from one of the 5 possible
     * tristate drivers.
     */

    if(GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_MARMUX);
    }
    else if(GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_PC);
    }
    else if(GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_ALU);
    }
    else if(GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_SHF);
    }
    else if(GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_MDR);
    }
    else if(GetGATE_IE(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_IE);
    }
    else if(GetGATE_OLDPSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_OldPSR);
    }
    else if(GetGATE_STACK(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_Stack);
    }
    else if(GetGATE_MAR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_MAR);
    }
    else if(GetGATE_VAMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        BUS = Low16bits(Gate_VAMUX);
    }
    else{
        BUS = 0;
    }
}

void latch_datapath_values() {

    /*
     * Datapath routine for computing all functions that need to latch
     * values in the data path at the end of this cycle.  Some values
     * require sourcing the bus; therefore, this routine has to come
     * after drive_bus.
     */
    //Inputs that require stuff from the bus
    if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.MAR = Low16bits(outputMARMUX2());
    }
    if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            int temp = BUS;
            //BYTE
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
                //Lower byte
                temp = Low16bits(temp & 0x00FF);
            }
            NEXT_LATCHES.MDR = Low16bits(temp);
        } else {
            if (CURRENT_LATCHES.READY == 1) {
                NEXT_LATCHES.MDR = Low16bits(readMEM());
            }
        }
    }
    if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.IR = Low16bits(BUS);
    }
    if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.REGS[outputDRMUX()] = Low16bits(BUS);
    }
    if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        if ((int16_t) BUS > 0) {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        } else if ((int16_t) BUS == 0) {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        } else {
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
    }
    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.PC = Low16bits(outputPCMUX());
    }
    if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        //BEN
        int IR_11 = (CURRENT_LATCHES.IR & 0x0800) >> 11;
        int IR_10 = (CURRENT_LATCHES.IR & 0x0400) >> 10;
        int IR_9 = (CURRENT_LATCHES.IR & 0x0200) >> 9;

        NEXT_LATCHES.BEN = Low16bits((IR_11 & CURRENT_LATCHES.N) | (IR_10 & CURRENT_LATCHES.Z) | (IR_9 & CURRENT_LATCHES.P));
    }
    if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.USP = BUS;
    }
    if(GetLD_EXCV(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        uint16_t IR_15_12 = Low16bits((CURRENT_LATCHES.IR & 0xF000) >> 12);
        uint16_t MAR_0 = Low16bits( outputMARMUX2() & 0x0001);
        uint16_t unknown = Low16bits((IR_15_12 == 10 ? 1 : 0) | (IR_15_12 == 11 ? 1 : 0));
        uint16_t unaligned_only = Low16bits(MAR_0);
        uint16_t protection = Low16bits((((CURRENT_LATCHES.PSR & 0x8000) >> 15) && !(CURRENT_LATCHES.TRAP) && !((CURRENT_LATCHES.MDR & 0x0008) >> 3)));
        if(unknown){
            NEXT_LATCHES.EXCV = 0x05;
        }
        else if(unaligned_only){
            NEXT_LATCHES.EXCV = 0x03;
        }
        else if(protection){
            NEXT_LATCHES.EXCV = 0x04;
        }
        else{
            NEXT_LATCHES.EXCV = 0x02;
        }
    }
    if (GetLD_OLDPSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.OLDPSR = Low16bits(CURRENT_LATCHES.PSR);
    }
    if (GetLD_EVENT(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.EVENT = Low16bits(outputEVENTMUX());
    }
    if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.PSR = Low16bits(outputPSRMUX());
    }
    if (GetINTFLAGCLR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.INTFLAG = 0;
    }
    if(GetCCUPDATE(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        int16_t PSR = Low16bits(outputPSRMUX());
        NEXT_LATCHES.N = PSR & 0x0004;
        NEXT_LATCHES.Z = PSR & 0x0002;
        NEXT_LATCHES.P = PSR & 0x0001;
    }
    if(GetLD_TRAPFLAG(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.TRAP = 0x0001;
    }
    if(GetTRAPFLAGCLR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.TRAP = 0;
    }
    if(GetLD_WFLAG(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.W = 1;
    }
    if(GetWFLAGCLR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.W = 0;
    }
    if(GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.VA = Low16bits(BUS);
    }
    if(GetLD_NS(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.NS = outputNSMUX();
    }
    if(GetLD_MR(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        NEXT_LATCHES.MDR = Low16bits(CURRENT_LATCHES.MDR | (CURRENT_LATCHES.W << 1) | (0x0001));
    }
}