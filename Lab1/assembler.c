/*
	Name 1: Jiahan Zhang
	UTEID 1: jz23745
*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
#include <stdint.h>
#include <math.h>

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
//TODO: toNum returns hex values in unsigned decimal (makes sense need x- to indicate negative value)
#define SIGNED_MAX_16 32767
#define SIGNED_MIN_16 -32768
#define UNSIGNED_MAX_16 65535
#define UNSIGNED_MIN_16 0
#define SHF_MAX 15
#define SHF_MIN 0
#define TRAP_MAX 255
#define TRAP_MIN 0

typedef struct {
    uint16_t address;
    char label[MAX_LABEL_LEN + 1];	/* Question for the reader: Why do we need to add 1? */
} TableEntry;

enum{ DONE, OK, EMPTY_LINE };

TableEntry symbolTable[MAX_SYMBOLS];
int numSymbols = 0;

int isValidLabel(char* lPtr){
    //Label cannot be a register
    if(!strcmp(lPtr,"r0") || !strcmp(lPtr,"r1") || !strcmp(lPtr,"r2") || !strcmp(lPtr,"r3") || !strcmp(lPtr,"r4") || !strcmp(lPtr,"r5") || !strcmp(lPtr,"r6") || !strcmp(lPtr,"r7")){
        return -1;
    }
    //Label cannot start with x or a number or be in,out,getc,puts
    if(lPtr[0] == 'x' || lPtr[0] == '1' || lPtr[0] == '2' || lPtr[0] == '3' || lPtr[0] == '4' || lPtr[0] == '5' || lPtr[0] == '6' || lPtr[0] == '7' || lPtr[0] == '8' || lPtr[0] == '9' || lPtr[0] == '0'){
        return -1;
    }
    else if(!strcmp(lPtr, "in")){
        return -1;
    }
    else if(!strcmp(lPtr, "out")){
        return -1;
    }
    else if(!strcmp(lPtr, "getc")){
        return -1;
    }
    else if(!strcmp(lPtr, "puts")){
        return -1;
    }
    int length = 0;
    while(lPtr[length] != 0){
        //Characters must be alphanumeric
        if(isalnum(lPtr[length]) == 0){
            return -1;
        }
        length++;
    }

    //Label cannot be longer than 20 characters
    if(length > 20){
        return -1;
    }

    return 0;
}
int isOpcode(char* lPtr){
    //Tests if the opcode is valid
    if(!strcmp(lPtr, "add")){
        return 0;
    }
    else if(!strcmp(lPtr, "and")){
        return 0;
    }
    else if(!strcmp(lPtr, "brn")){
        return 0;
    }
    else if(!strcmp(lPtr, "brp")){
        return 0;
    }
    else if(!strcmp(lPtr, "brnp")){
        return 0;
    }
    else if(!strcmp(lPtr, "br")){
        return 0;
    }
    else if(!strcmp(lPtr, "brz")){
        return 0;
    }
    else if(!strcmp(lPtr, "brnz")){
        return 0;
    }
    else if(!strcmp(lPtr, "brzp")){
        return 0;
    }
    else if(!strcmp(lPtr, "brnzp")){
        return 0;
    }
    else if(!strcmp(lPtr, "halt")){
        return 0;
    }
    else if(!strcmp(lPtr, "jmp")){
        return 0;
    }
    else if(!strcmp(lPtr, "jsr")){
        return 0;
    }
    else if(!strcmp(lPtr, "jsrr")){
        return 0;
    }
    else if(!strcmp(lPtr, "ldb")){
        return 0;
    }
    else if(!strcmp(lPtr, "ldw")){
        return 0;
    }
    else if(!strcmp(lPtr, "lea")){
        return 0;
    }
    else if(!strcmp(lPtr, "nop")){
        return 0;
    }
    else if(!strcmp(lPtr, "not")){
        return 0;
    }
    else if(!strcmp(lPtr, "ret")){
        return 0;
    }
    else if(!strcmp(lPtr, "lshf")){
        return 0;
    }
    else if(!strcmp(lPtr, "rshfl")){
        return 0;
    }
    else if(!strcmp(lPtr, "rshfa")){
        return 0;
    }
    else if(!strcmp(lPtr, "rti")){
        return 0;
    }
    else if(!strcmp(lPtr, "stb")){
        return 0;
    }
    else if(!strcmp(lPtr, "stw")){
        return 0;
    }
    else if(!strcmp(lPtr, "trap")){
        return 0;
    }
    else if(!strcmp(lPtr, "xor")){
        return 0;
    }
    else{
        return -1;
    }
}

uint16_t addLabel(uint16_t inst, uint32_t bits, uint16_t currentAddress, char* label){
    //Left shift the inst by n bits
    inst = inst << bits;
    //Find address of label
    uint16_t labelAddress = 0;
    int flag = 0;
    for(int i = 0; i < numSymbols; i++){
        if(!strcmp(symbolTable[i].label,label)){
            labelAddress = symbolTable[i].address;
            flag = 1;
        }
    }
    if(flag == 0){
        printf("Label not found: %s\n", label);
        exit(1);
    }
    else{
        int16_t diff = labelAddress - currentAddress;
        //account for PC
        diff -= 2;
        diff /= 2;
        if((diff > pow(2, bits-1) - 1) || (diff < -pow(2, bits-1))){
            printf("Label is out of range : %s %d %d", label, labelAddress, currentAddress);
            exit(4);
        }
        else{
            //Only want last n bits of diff
            unsigned mask;
            mask = (1 << bits) - 1;
            inst |= (diff & mask);

            return inst;
        }
    }
}

uint16_t addImm(uint16_t inst, uint32_t bits, int imm){
    //Left shift the inst by n bits
    inst = inst << bits;

    //Test if out of range
    if((imm > pow(2,bits-1)-1) || (imm < -pow(2,bits-1))){
        printf("IMM is out of range\n");
        exit(3);
    }

    unsigned mask;
    mask = (1 << bits) - 1;
    inst |= (imm & mask);

    return inst;
}

uint16_t addUIMM(uint16_t inst, uint32_t bits, int imm){
    //Left shift the inst by n bits
    inst = inst << bits;

    //Test if out of range
    if((imm > pow(2,bits)-1)){
        printf("IMM is out of range\n");
        exit(3);
    }

    unsigned mask;
    mask = (1 << bits) - 1;
    inst |= (imm & mask);

    return inst;
}

uint16_t addRegister(uint16_t inst, uint32_t bits, char* reg){
    //Left shift the inst by n bits
    inst = inst << bits;
    //Only R0-R7 are valid DR/SR registers
    if(!strcmp(reg,"r0")){
        inst |= 0;
    }
    else if(!strcmp(reg,"r1")){
        inst |= 1;
    }
    else if(!strcmp(reg,"r2")){
        inst |= 2;
    }
    else if(!strcmp(reg,"r3")){
        inst |= 3;
    }
    else if(!strcmp(reg,"r4")){
        inst |= 4;
    }
    else if(!strcmp(reg,"r5")){
        inst |= 5;
    }
    else if(!strcmp(reg,"r6")){
        inst |= 6;
    }
    else if(!strcmp(reg,"r7")){
        inst |= 7;
    }
    else{
        printf("Invalid operand (expected R0-R7)\n");
        exit(4);
    }

    return inst;
}

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
){
    //Parse through the file, will change char values for opcode,labels,args, etc.
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );

    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' &&
           *lPtr != '\n' )
        lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
        return( EMPTY_LINE );

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }

    *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg1 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}

int toNum( char * pStr ){
    //Used to convert hex and # values to integers
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )                                /* decimal */
    {
        pStr++;
        if( *pStr == '-' )                                /* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr))
            {
                printf("Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )        /* hex     */
    {
        pStr++;
        if( *pStr == '-' )                                /* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isxdigit(*t_ptr))
            {
                printf("Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else
    {
        printf( "Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}

int main(int argc, char* argv[]) {
    char *prgName   = NULL;
    char *iFileName = NULL;
    char *oFileName = NULL;

    prgName   = argv[0];
    iFileName = argv[1];
    oFileName = argv[2];

    printf("program name = '%s'\n", prgName);
    printf("input file name = '%s'\n", iFileName);
    printf("output file name = '%s'\n", oFileName);

    char lLine[MAX_LINE_LENGTH + 1],*lLabel, *lOpcode, *lArg1,
            *lArg2, *lArg3, *lArg4;

    int lRet;

    FILE * lInfile;
    FILE * lOutfile;

    lInfile = fopen(iFileName, "r" );        /* open the input file */
    lOutfile = fopen(oFileName, "w");         /* open the output file*/

    int16_t startAddress = 0x3000;
    //Set flags if .orig and .end exist in program
    int flagORIG = 0;
    int flagEND = 0;

    //In first pass, generate symbol table
    do
    {
        lRet = readAndParse( lInfile, lLine, &lLabel,
                             &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

        if( lRet != DONE && lRet != EMPTY_LINE )
        {
            if(!strcmp(lOpcode,".orig")){
                //test if .ORIG has been used before
                if(flagORIG != 0){
                    printf(".ORIG has been used before\n");
                    exit(4);
                }
                flagORIG = 1;
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at .ORIG\n");
                    exit(4);
                }
                //check value in .ORIG
                if(toNum(lArg1) > UNSIGNED_MAX_16 || (toNum(lArg1) % 2 != 0) || toNum(lArg1) < SIGNED_MIN_16){
                    printf("Operand 1 is too large or not even for .ORIG\n");
                    exit(3);
                }
                else{
                    startAddress = toNum(lArg1);
                }
            }
            //Test if .END pseudo-op
            else if (!strcmp(lOpcode,".end")){
                if(flagORIG != 1){
                    printf(".ORIG has not been used before .END\n");
                    exit(4);
                }
                flagEND = 1;
                if(strlen(lArg1) != 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at .END\n");
                    exit(4);
                }
                break;
            }
            //Found a label
            else if(strlen(lLabel) != 0){
                if(flagORIG != 1){
                    printf(".ORIG has not been used before label declaration\n");
                    exit(4);
                }
                //Test if label is valid
                if(isValidLabel((lLabel)) == -1){
                    printf("Invalid label: %s\n", lLabel);
                    exit(4);
                }
                for (int i = 0; i < numSymbols; i++){
                    if(!strcmp(symbolTable[i].label,lLabel)){
                        printf("Label already exists\n");
                        exit(4);
                    }
                }
                //Add to symbol table
                printf("New label: %s at %d\n", lLabel, startAddress);
                symbolTable[numSymbols].address = startAddress;
                strcpy(symbolTable[numSymbols].label, lLabel);
                numSymbols++;

                //Test if there is an opcode/.FILL following the label
                if(strlen(lOpcode) == 0 && strcmp(lOpcode,".fill") != 0){
                    //There is a number, label, or register after the label
                    printf("Empty label or illegal opcode\n");
                    exit(2);
                }

                startAddress += 2;
            }
            //Blank line
            else if (strlen(lLabel) == 0 && strlen(lOpcode) == 0){
                continue;
            }
            //Regular opcode
            else{
                if(flagORIG != 1){
                    printf(".ORIG has not been used before opcode or pseudocode\n");
                    exit(4);
                }
                startAddress += 2;
            }
            //Reset all values
            lLabel[0] = '\0';
            lOpcode[0] = '\0';
            lArg1[0] = '\0';
            lArg2[0] = '\0';
            lArg3[0] = '\0';
            lArg4[0] = '\0';
        }
    } while( lRet != DONE );

    //Test if both .ORIG and .END exist
    if(flagORIG == 0 || flagEND == 0){
        printf("Missing either .ORIG or .END\n");
        exit(4);
    }

    //Reset file pointer
    rewind(lInfile);

    //In second pass, generate machine code
    do
    {
        lRet = readAndParse( lInfile, lLine, &lLabel,
                             &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

        if( lRet != DONE && lRet != EMPTY_LINE )
        {
            //Will test for missing/extra operands
            //Test if .ORIG pseudo-op
            if(!strcmp(lOpcode,".orig")){
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at .ORIG\n");
                    exit(4);
                }
                if(toNum(lArg1) > UNSIGNED_MAX_16 || (toNum(lArg1) % 2 != 0) || toNum(lArg1) < SIGNED_MIN_16){
                    printf("Operand 1 is too large or not even for .ORIG\n");
                    exit(3);
                }
                else{
                    startAddress = toNum(lArg1);
                    printf("The starting address is %d\n", startAddress);
                    uint16_t inst = 0;
                    inst = addUIMM(inst, 16, startAddress);
                    fprintf( lOutfile, "0x%.4X\n", inst);
                }
            }
            //Test if .END pseudo-op
            else if (!strcmp(lOpcode,".end")){
                if(strlen(lArg1) != 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at .END\n");
                    exit(4);
                }
                break;
            }
            //Test if .FILL pseudo-op
            else if(!strcmp(lOpcode,".fill")){
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at .FILL\n");
                    exit(4);
                }
                int amt = toNum(lArg1);
                //TODO: test this
                if(amt > UNSIGNED_MAX_16 || amt < SIGNED_MIN_16){
                    printf(".FILL out of range: %d\n", amt);
                    exit(3);
                }
                uint16_t inst = amt;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "add")){
                //0001 DR SR1 0 00 SR2
                //0001 DR SR1 1 imm5
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x1;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);

                //Register
                if(lArg3[0] == 'r'){
                    inst = inst << 3;
                    inst = addRegister(inst,3,lArg3);
                }
                //Immediate
                else if(lArg3[0] == 'x' || lArg3[0] == '#'){
                    inst = inst << 1;
                    inst |= 1;
                    inst = addImm(inst,5,toNum(lArg3));
                }
                //None of the above
                else{
                    printf("add has an unexpected operand\n");
                    exit(4);
                }
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "and")){
                //0101 DR SR1 0 00 SR2
                //0101 DR SR1 1 imm5
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x5;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);

                //Register
                if(lArg3[0] == 'r'){
                    inst = inst << 3;
                    inst = addRegister(inst,3,lArg3);
                }
                //Immediate
                else if(lArg3[0] == 'x' || lArg3[0] == '#'){
                    inst = inst << 1;
                    inst |= 1;
                    inst = addImm(inst,5,toNum(lArg3));
                }
                //None of the above
                else{
                    printf("and has an unexpected operand\n");
                    exit(4);
                }

                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "brn")){
                //0000 n PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 4;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "brp")){
                //0000 p PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 1;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "brnp")){
                //0000 np PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 5;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "br")){
                //0000 nzp PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 7;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "brz")){
                //0000 z PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 2;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "brnz")){
                //0000 nz PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 6;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "brzp")){
                //0000 zp PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 3;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "brnzp")){
                //0000 nzp PCOffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0;
                inst = inst << 3;
                inst |= 7;
                inst = addLabel(inst, 9, startAddress, lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "halt")){
                //11110000 x25
                if(strlen(lArg1) != 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xF0;
                inst = inst << 8;
                inst |= 0x25;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "jmp")){
                //1100 000 BaseR 0000000
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xA;
                inst = inst << 3;
                inst = addRegister(inst,3,lArg1);
                inst = inst << 6;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "jsr")){
                //0100 1 PCOffset11
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x4;
                inst = inst << 1;
                inst |= 1;
                inst = addLabel(inst,11,startAddress,lArg1);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "jsrr")){
                //0100 000 BaseR 000000
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x4;
                inst = inst << 3;
                inst = addRegister(inst,3,lArg1);
                inst = inst << 6;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "ldb")){
                //0010 DR BaseR boffset6
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x2;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = addImm(inst,6,toNum(lArg3));
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "ldw")){
                //0110 DR BaseR boffset6
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x6;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = addImm(inst,6,toNum(lArg3));
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "lea")){
                //1110 DR PCoffset9
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xE;
                inst = addRegister(inst,3,lArg1);
                inst = addLabel(inst,9,startAddress,lArg2);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "nop")){
                if(strlen(lArg1) != 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x0000;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "not")){
                //1001 DR SR 1 11111
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x9;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = inst << 6;
                inst |= 0x3F;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "ret")){
                if(strlen(lArg1) != 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xC1C0;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "lshf")){
                //1101 DR SR 0 0 amount4
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xD;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = inst << 2;
                int amt = toNum(lArg3);
                if(amt < SHF_MIN || amt > SHF_MAX){
                    printf("Shift amt less than 0 or greater than 15");
                    exit(3);
                }
                inst = addUIMM(inst,4,amt);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "rshfl")){
                //1101 DR SR 0 1 amount4
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xD;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = inst << 2;
                inst |= 1;
                int amt = toNum(lArg3);
                if(amt < SHF_MIN || amt > SHF_MAX){
                    printf("Shift amt less than 0 or greater than 15");
                    exit(3);
                }
                inst = addUIMM(inst,4,amt);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "rshfa")){
                //1101 DR SR 1 1 amount4
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xD;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = inst << 2;
                inst |= 3;
                int amt = toNum(lArg3);
                if(amt < SHF_MIN || amt > SHF_MAX){
                    printf("Shift amt less than 0 or greater than 15");
                    exit(3);
                }
                inst = addUIMM(inst,4,amt);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "rti")){
                if(strlen(lArg1) != 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x8000;
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "stb")){
                //0011 SR BaseR boffset6
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 3;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = addImm(inst,6,toNum(lArg3));
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "stw")){
                //0111 SR BaseR offset6
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 7;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                inst = addImm(inst,6,toNum(lArg3));
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "trap")){
                //1111 0000 trapvect8
                if(strlen(lArg1) == 0 || strlen(lArg2) != 0 || strlen(lArg3) != 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0xF;
                inst = inst << 4;
                //value must be hex
                if(lArg1[0] != 'x'){
                    printf("TRAP value must be in hex\n");
                    exit(4);
                }
                int amt = toNum(lArg1);
                if (amt < TRAP_MIN || amt > TRAP_MAX){
                    printf("TRAP amt less than 0 or greater than 255\n");
                    exit(3);
                }
                inst = addUIMM(inst,8,amt);
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            else if(!strcmp(lOpcode, "xor")){
                //1001 DR SR1 0 00 SR2
                //1001 DR SR1 1 imm5
                if(strlen(lArg1) == 0 || strlen(lArg2) == 0 || strlen(lArg3) == 0 || strlen(lArg4) != 0){
                    printf("Unexpected or missing operands at %d\n", startAddress);
                    exit(4);
                }
                uint16_t inst = 0x9;
                inst = addRegister(inst,3,lArg1);
                inst = addRegister(inst,3,lArg2);
                //Register
                if(lArg3[0] == 'r'){
                    inst = inst << 3;
                    inst = addRegister(inst,3,lArg3);
                }
                //Immediate
                else if(lArg3[0] == 'x' || lArg3[0] == '#'){
                    inst = inst << 1;
                    inst |= 1;
                    inst = addImm(inst,5,toNum(lArg3));
                }
                //None of the above
                else{
                    printf("XOR has an unexpected operand\n");
                    exit(4);
                }
                fprintf( lOutfile, "0x%.4X\n", inst);
                startAddress += 2;
            }
            //Found a label and nothing else (label on own line)
            else if (strlen(lLabel) != 0 && strlen(lOpcode) == 0){
                lLabel[0] = '\0';
                lOpcode[0] = '\0';
                lArg1[0] = '\0';
                lArg2[0] = '\0';
                lArg3[0] = '\0';
                lArg4[0] = '\0';
                continue;
            }
            //Invalid opcode
            else{
                printf("Invalid opcode\n");
                exit(2);
            }
            //Reset all values
            lLabel[0] = '\0';
            lOpcode[0] = '\0';
            lArg1[0] = '\0';
            lArg2[0] = '\0';
            lArg3[0] = '\0';
            lArg4[0] = '\0';
        }
    } while( lRet != DONE );

    fclose(lInfile);
    fclose(lOutfile);
}