/*
    Name 1: Jiahan Zhang
    UTEID 1: jz23745
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

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
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
            N,		/* n condition bit */
            Z,		/* z condition bit */
            P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
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
/* Purpose   : Simulate the LC-3b until HALTed                 */
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

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

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
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
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
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

uint16_t MAR = 0x0000;
uint16_t MDR = 0x0000;
uint16_t IR = 0x0000;
uint8_t BEN = 0x0000;
uint8_t OPCODE = 0x0000;

//num must be shifted right
//Performs a sign extension function
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

//Sets condition codes depending on result
void setCC(int result){
    int16_t masked = Low16bits(result);
    if(masked == 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if(masked > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
    else{
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
}

//Fetch instruction
void fetch(){
    MAR = Low16bits(CURRENT_LATCHES.PC) >> 1;
    NEXT_LATCHES.PC += 2;
    //upper byte
    MDR = 0;
    MDR |= Low16bits(MEMORY[MAR][1] << 8);
    //lower byte
    MDR |= Low16bits(MEMORY[MAR][0]);

    IR = MDR;
}

//Decode instruction
void decode(){
    //Isolate bit fields for IR[11], IR[10], IR[9], IR[15:12]
    int IR_11 = (IR & 0x0800) >> 11;
    int IR_10 = (IR & 0x0400) >> 10;
    int IR_9 = (IR & 0x0200) >> 9;
    int IR_15_12 = (IR & 0xF000) >> 12;

    BEN = (IR_11 & CURRENT_LATCHES.N) | (IR_10 & CURRENT_LATCHES.Z) | (IR_9 & CURRENT_LATCHES.P);
    OPCODE = IR_15_12;
};

void add(){
    uint16_t DR = (IR & 0x0E00) >> 9;
    uint16_t SR1 = (IR & 0x01C0) >> 6;
    uint16_t bit_5 = (IR & 0x0020) >> 5;
    if (bit_5 == 0) {
        uint16_t SR2 = (IR & 0x0007);
        NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR1]) + Low16bits(CURRENT_LATCHES.REGS[SR2]));
    }
    else{
        int16_t imm5 = (IR & 0x001F);
        imm5 = SEXT(imm5, 5);
        NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR1]) + Low16bits(imm5));
    }

    setCC(NEXT_LATCHES.REGS[DR]);
}

void and(){
    uint16_t DR = (IR & 0x0E00) >> 9;
    uint16_t SR1 = (IR & 0x01C0) >> 6;
    uint16_t bit_5 = (IR & 0x0020) >> 5;
    if (bit_5 == 0) {
        uint16_t SR2 = (IR & 0x0007);
        NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR1]) & Low16bits(CURRENT_LATCHES.REGS[SR2]));
    }
    else{
        int16_t imm5 = (IR & 0x001F);
        imm5 = SEXT(imm5, 5);
        NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR1]) & Low16bits(imm5));
    }

    setCC(NEXT_LATCHES.REGS[DR]);
}

void br(){
    int16_t PCoffset9 = (IR & 0x01FF);
    PCoffset9 = SEXT(PCoffset9, 9);

    if(BEN){
        NEXT_LATCHES.PC = Low16bits(Low16bits(NEXT_LATCHES.PC) + Low16bits(PCoffset9 << 1));
    }
}

void jmp(){
    uint16_t BaseR = (IR & 0x01C0) >> 6;

    NEXT_LATCHES.PC = Low16bits(Low16bits(CURRENT_LATCHES.REGS[BaseR]));
}

void jsr(){
    int temp = Low16bits(NEXT_LATCHES.PC);

    uint16_t bit_11 = (IR & 0x0800) >> 11;
    uint16_t BaseR = (IR & 0x01C0) >> 6;
    int16_t PCoffset11 = (IR & 0x07FF);
    PCoffset11 = SEXT(PCoffset11, 11);

    if(bit_11 == 0){
        NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[BaseR]);
    }
    else{
        NEXT_LATCHES.PC = Low16bits(Low16bits(NEXT_LATCHES.PC) + Low16bits(PCoffset11 << 1));
    }

    NEXT_LATCHES.REGS[7] = Low16bits(temp);
}

void ldb(){
    uint16_t DR = (IR & 0x0E00) >> 9;
    uint16_t BaseR = (IR & 0x01C0) >> 6;
    int16_t boffset6 = IR & 0x003F;
    boffset6 = SEXT(boffset6, 6);

    uint16_t result = Low16bits(Low16bits(CURRENT_LATCHES.REGS[BaseR]) + Low16bits(boffset6));
    //odd address - find corresponding address and take [1]
    if(result%2 != 0) {
        uint16_t correspondingMem = Low16bits((result - 1) >> 1);
        NEXT_LATCHES.REGS[DR] = Low16bits(SEXT(Low16bits(MEMORY[correspondingMem][1]), 8));
    }
    //even address
    else{
        uint16_t correspondingMem = Low16bits(result >> 1);
        NEXT_LATCHES.REGS[DR] = Low16bits(SEXT(Low16bits(MEMORY[correspondingMem][0]), 8));
    }

    setCC(NEXT_LATCHES.REGS[DR]);
}

void ldw(){
    uint16_t DR = (IR & 0x0E00) >> 9;
    uint16_t BaseR = (IR & 0x01C0) >> 6;
    int16_t boffset6 = (IR & 0x003F);
    boffset6 = SEXT(boffset6, 6);

    uint16_t result = Low16bits(Low16bits(CURRENT_LATCHES.REGS[BaseR]) + Low16bits(boffset6 << 1));

    //even address - aligned access
    uint16_t correspondingMem = Low16bits(result >> 1);
    NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(MEMORY[correspondingMem][1]) << 8);
    NEXT_LATCHES.REGS[DR] |= Low16bits(MEMORY[correspondingMem][0]);

    setCC(NEXT_LATCHES.REGS[DR]);
}

void lea(){
    uint16_t DR = (IR & 0x0E00) >> 9;
    int16_t PCoffset9 = IR & 0x01FF;
    PCoffset9 = SEXT(PCoffset9, 9);

    NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(NEXT_LATCHES.PC) + Low16bits(PCoffset9 << 1));
}

void shf(){
    uint16_t DR = (IR & 0x0E00) >> 9;
    uint16_t SR = (IR & 0x01C0) >> 6;
    uint16_t amount4 = IR & 0x000F;
    uint16_t bit_4 = (IR & 0x0010) >> 4;
    uint16_t bit_5 = (IR & 0x0020) >> 5;

    if(bit_4 == 0){
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] << amount4);
    }
    else if(bit_5 == 0){
        //make unsigned for logical shift
        uint16_t unsignedSR = Low16bits(CURRENT_LATCHES.REGS[SR]);
        NEXT_LATCHES.REGS[DR] = Low16bits(unsignedSR >> amount4);
    }
    else{
        //make signed for arithmetic shift
        int16_t signedSR = Low16bits(CURRENT_LATCHES.REGS[SR]);
        NEXT_LATCHES.REGS[DR] = Low16bits(signedSR >> amount4);
    }

    setCC(NEXT_LATCHES.REGS[DR]);
}

void stb(){
    uint16_t SR = (IR & 0x0E00) >> 9;
    uint16_t BaseR = (IR & 0x01C0) >> 6;
    int16_t boffset6 = IR & 0x003F;
    boffset6 = SEXT(boffset6, 6);

    uint16_t result = Low16bits(Low16bits(CURRENT_LATCHES.REGS[BaseR]) + Low16bits(boffset6));
    //odd address - find corresponding address and take [1]
    if(result%2 != 0) {
        uint16_t correspondingMem = Low16bits((result - 1) >> 1);

        uint8_t value = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR]) & 0x00FF);
        MEMORY[correspondingMem][1] = value;
    }
    //even address
    else{
        uint16_t correspondingMem = Low16bits(result >> 1);

        uint8_t value = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR]) & 0x00FF);
        MEMORY[correspondingMem][0] = value;
    }
}

void stw(){
    uint16_t SR = (IR & 0x0E00) >> 9;
    uint16_t BaseR = (IR & 0x01C0) >> 6;
    int16_t boffset6 = (IR & 0x003F);
    boffset6 = SEXT(boffset6, 6);

    uint16_t result = Low16bits(Low16bits(CURRENT_LATCHES.REGS[BaseR]) + Low16bits(boffset6 << 1));
    //even address - aligned access
    uint16_t correspondingMem = Low16bits(result >> 1);

    uint8_t lower = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR]) & 0x00FF);
    uint8_t upper = Low16bits((Low16bits(CURRENT_LATCHES.REGS[SR]) & 0xFF00) >> 8);
    MEMORY[correspondingMem][0] = lower;
    MEMORY[correspondingMem][1] = upper;
}

void trap(){
    uint16_t trapvect8 = IR & 0x00FF;
    NEXT_LATCHES.REGS[7] = Low16bits(NEXT_LATCHES.PC);

    uint16_t result = Low16bits(trapvect8 << 1);
    uint16_t correspondingMem = result >> 1;

    uint16_t lower = Low16bits(MEMORY[correspondingMem][0] & 0x00FF);
    uint16_t upper = Low16bits(MEMORY[correspondingMem][1] & 0x00FF);

    uint16_t total = Low16bits((upper << 8) | lower);
    NEXT_LATCHES.PC = Low16bits(total);
}

void xor(){
    uint16_t DR = (IR & 0x0E00) >> 9;
    uint16_t SR1 = (IR & 0x01C0) >> 6;
    uint16_t bit_5 = (IR & 0x0020) >> 5;
    if (bit_5 == 0) {
        uint16_t SR2 = IR & 0x0007;
        NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR1]) ^ Low16bits(CURRENT_LATCHES.REGS[SR2]));
    }
    else{
        int16_t imm5 = IR & 0x001F;
        imm5 = SEXT(imm5, 5);
        NEXT_LATCHES.REGS[DR] = Low16bits(Low16bits(CURRENT_LATCHES.REGS[SR1]) ^ Low16bits(imm5));
    }

    setCC(NEXT_LATCHES.REGS[DR]);
}

//Execute instruction
void execute(){
    //Execute depending on opcode

    //ADD
    if(OPCODE == 1){
        add();
    }
    //AND
    else if(OPCODE == 5) {
        and();
    }
    //BR
    else if(OPCODE == 0){
        br();
    }
    //JMP/RET
    else if(OPCODE == 12){
        jmp();
    }
    //JSR(R)
    else if(OPCODE == 4){
        jsr();
    }
    //LDB
    else if(OPCODE == 2){
        ldb();
    }
    //LDW
    else if(OPCODE == 6){
        ldw();
    }
    //LEA
    else if(OPCODE == 14){
        lea();
    }
    //LSHF/RSHFL/RSHFA
    else if(OPCODE == 13){
        shf();
    }
    //STB
    else if(OPCODE == 3){
        stb();
    }
    //STW
    else if(OPCODE == 7){
        stw();
    }
    //TRAP
    else if(OPCODE == 15){
        trap();
    }
    //NOT/XOR
    else if(OPCODE == 9){
        xor();
    }
}

void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */

    fetch();
    decode();
    execute();
}