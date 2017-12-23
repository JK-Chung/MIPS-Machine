/**************************************************************************
 * MIPS MACHINE
 * This is a simulator of a MIPS machine done for university. It fully
 * implements handling of the MIPS ISA, all registers and some memory.
 *
 **************************************************************************/

/**************************************************************************
 * PREPROCESSOR HANDLING
 **************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEFAULT_MEMORY_PATH "./defaultMemory.txt"
#define ADDRESS_LENGTH 26
#define OPCODE_LENGTH 6
#define WORD_LENGTH ADDRESS_LENGTH + OPCODE_LENGTH	//defines length of printed binStrin

#define MEMORY_CELLS 500 				//defines no of memory addresses
							//addresses were cut down to allow for
							//a more user-friendly display of memory

#define NO_REGISTER_LENGTH 5				//no of bits required to show a register
#define NO_REGISTERS 32					//no of registers

//The following define the lengths of fields of the different formats
//R-Format
#define FUNCT_LENGTH 6
#define SHAMT_LENGTH 5

//I-Format
#define IMMD_LENGTH 16

/**************************************************************************
 * STRUCT DEFINITIONS
 **************************************************************************/
/** Associates the registers and memory into a single data type **/
typedef struct {
    uint32_t memory[MEMORY_CELLS];   	/*Main Memory*/
    int32_t AC;				/* To remove from this here existence next */
    uint32_t IR;                   	/*Instruction Register*/
    int32_t MBR;                    	/*Memory Buffer Register*/
    unsigned int PC : ADDRESS_LENGTH;   /*Program Counter*/
    unsigned int MAR : ADDRESS_LENGTH;  /*Memory Address Register*/
    int32_t inReg;
    int32_t outReg;

    int32_t registers[NO_REGISTERS];
} computer;

/** To be filled by a parsing function. Holds the value of the fields of an R-Format **/
typedef struct {
	unsigned int rs:NO_REGISTER_LENGTH;
	unsigned int rt:NO_REGISTER_LENGTH;
	unsigned int rd:NO_REGISTER_LENGTH;
	unsigned int shamt:SHAMT_LENGTH;
	unsigned int funct:FUNCT_LENGTH;
} R_Format;

/** To be filled by a parsing function. Holds the value of the fields of an I-Format **/
typedef struct {
	unsigned int rs:NO_REGISTER_LENGTH;
	unsigned int rt:NO_REGISTER_LENGTH;
	//All immd fields (in assembly as well) will be interpreted as Two's Complement
	int immd:IMMD_LENGTH;
} I_Format;

/**************************************************************************
 * FUNCTION DECLARATIONS
 **************************************************************************/
computer startComputer(void);
void addi(computer *c);
void and(computer *c);
void or(computer *c);
void loadUpperImmediate(computer *c);
void printProcessor(computer c);
void jump(computer*);
void jumpRegister(computer *c);
void jumpAndLink(computer *c);
void branchIfNotEqual(computer *C);
void halt(computer *c);
void store(computer*);
void printStartHelp(void);
void load(computer*);
void add(computer*);
void subt(computer*);
void input(computer*);
void output(computer*);
void printMemory(computer*);
void translateMemory(computer*);
void loadFile(computer*);
void loadDefault(computer*);
void loadUser(computer*);
void readFromFile(computer *comp, char *path);
void branchIfEqual(computer*);
void orImmediate(computer*);
int parseAddress(uint32_t machineInstruction);
int parseOpcode(uint32_t machineInstruction);
int parseFunction(uint32_t machineInstruction);
R_Format parse4RFormat(uint32_t machineInstruction);
I_Format parse4IFormat(uint32_t machineInstruction);
int bin2Dec(char *binString);
int uBin2Dec(char *inputBinString);
char *intDec2Bin(unsigned int,int);
void *uBin2TwoComp (char *binString);
void *invertBinString(char *binString);
void addOneToBinString(char *binString);
void disassembleMemory(computer c);
char *disassembleLine(uint32_t machineInstruction);
const char *getOpcodeLabel(uint32_t machineInstruction);
void printInformation(computer c);

/**************************************************************************
 * HANDLING OF REGISTER LABELS
 **************************************************************************/
/** This String array allows for easier dissassembling in the disassembleLine function **/
char *registerLabels[] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};

/** Allows for easy referencing into register array of a computer struct **/
typedef enum {zero,at,v0,v1,a0,a1,a2,a3,t0,t1,t2,t3,t4,t5,t6,t7,s0,s1,s2,s3,s4,s5,s6,s7,t8,t9,k0,k1,gp,sp,fp,ra} registers;


/**************************************************************************
 * MAIN FUNCTION
 **************************************************************************/
/** Handles fetch-decode-execute cycle and the initial load into memory **/
int main(int argc, char *argv[]) {
    computer aComputer = startComputer();
    
    //Handle command line arguments
    if(argc == 1) {
	printf("Argument missing. If reading from file, please provide the path as the third argument.\n");
	printf("Quitting application...\n");
	return 0;
    }
    if(!strcmp(argv[1], "-d"))
	loadDefault(&aComputer);
    else if(!strcmp(argv[1], "-c"))
	loadUser(&aComputer);
    else if(!strcmp(argv[1], "-f"))
	readFromFile(&aComputer,argv[2]);
    else {
	printf("Invalid argument. Quiting application...\n");
	return 0;
    }

    // Translate memory contents into assembly
    disassembleMemory(aComputer);

    // Ask user to start the fetch-decode-execute cycle
    printStartHelp();
    
    //Fetch-decode-execute cycle
    while(aComputer.PC < MEMORY_CELLS) {
        // Copy the PC to the MAR
        aComputer.MAR = aComputer.PC;
        // Copy contents of memory at address MAR to IR
        aComputer.IR = aComputer.memory[aComputer.MAR];
        //Increment PC
	printf("%d:\n",aComputer.PC);
        aComputer.PC++;
        
        // Store machine instruction and parse opcode and function
        uint32_t machineInstruction = aComputer.IR;
        int opcode = parseOpcode(machineInstruction);
	int function = parseFunction(machineInstruction);

        // Parse address and place into MAR (Note this assumes that instruction is J-Format)
	// If in other format, then functions should not use MAR and parse intruction differently
        int address = parseAddress(machineInstruction);
        aComputer.MAR = address;
	
        printInformation(aComputer);
        switch(opcode) {
            case 0:
		switch(function) {
			case 0x00:	halt(&aComputer);		return 0;
		    	case 0x08:	jumpRegister(&aComputer);	break;
		    	case 0x20:	add(&aComputer);		break;
			case 0x22:	subt(&aComputer);		break;
			case 0x24:	and(&aComputer);		break;
			case 0x25:	or(&aComputer);			break;
		} break;
	    case 0x02:			jump(&aComputer);		break;
            case 0x04:			jumpAndLink(&aComputer);	break;
            case 0x05:			input(&aComputer);		break;
            case 0x06:			output(&aComputer);		break;
	    case 0x08:			addi(&aComputer);		break;
            case 0x0d:			orImmediate(&aComputer);	break;
	    case 0x0f:			loadUpperImmediate(&aComputer);	break;
	    case 0x23:			load(&aComputer);		break;
	    case 0x2b:			store(&aComputer);		break;
            case 0x3b:			branchIfEqual(&aComputer);	break;
	    case 0x3c:			branchIfNotEqual(&aComputer);	break;
        }
    }
}

/**************************************************************************
 * FUNCTIONS TO MANAGE INSTANCE OF COMPUTER STRUCT
 **************************************************************************/
/** Returns an initialised computer instance **/
computer startComputer(void) {
    computer toReturn = {{0},0,0,0,0,0,0,0,{0}};
    toReturn.registers[sp] = MEMORY_CELLS;
    toReturn.registers[fp] = MEMORY_CELLS;
    return toReturn;
}

/** Prints help on using computer. Prompts the user if he wishes to start it **/
void printStartHelp(void) {
	printf("Memory initialised. Disassembled translation of the memory was printed up to the first halt instruction\n");
    printf("Every instruction that is run will be printed on screen along with $pc's value\n");
    printf("When program halts, memory and register contents will be printed\n");
    printf("READY TO RUN? (press enter to continue)\n");
    char enter = '\0';
    while(enter != '\r' && enter != '\n')
	enter = getchar();
}

void printMemory(computer *C){
    printf("CURRENT MEMORY CONTENTS\n");
    for(int i = 0; i < MEMORY_CELLS; i++)
	printf("[%d]\t%s (dec = %d)\n", i, intDec2Bin(C->memory[i],WORD_LENGTH), C->memory[i]);
    printf("\n");
}

void printProcessor(computer c) {
	printf("PRINTING PROCESSOR CONTENTS\n");
	for(int i = 0; i < NO_REGISTERS; i++)
		printf("[%s]\t%s (dec = %d)\n",registerLabels[i],intDec2Bin(c.registers[i],WORD_LENGTH),c.registers[i]);
	printf("\n");
}
/** Prints out the instruction that is running**/
void printInformation(computer c) {
	printf("Stack pointer is at:\t%d\n",c.registers[sp]);
	printf("Machine Instruction:\t%s\n",intDec2Bin(c.IR,WORD_LENGTH));
	printf("Assembly Instruction:\t%s\n\n",disassembleLine(c.IR));
}

/**************************************************************************
 * PARSING FUNCTIONS
 **************************************************************************/
/** Takes in a machine instruction and returns address field. Treats it like a J-Format **/
int parseAddress(uint32_t machineInstruction) { return machineInstruction & ((int) pow(2,ADDRESS_LENGTH) - 1); }
/** Parses and returns the integer value of the opcode of the inputted machine instruction **/
int parseOpcode(uint32_t machineInstruction) { return machineInstruction >> ADDRESS_LENGTH; }
/** Parses and returns the function field. Assumes machineInstruction is of the R-Format **/
int parseFunction(uint32_t machineInstruction) { return machineInstruction & ((int) pow(2,FUNCT_LENGTH) - 1); }

/** 
 * Takes in a machine instruction and parses the value of its fields into an instance of the
 * R_Format struct. This is then returned and allows the callee to read the value of the fields.
 * Treats instruction like an R-Format instruction
**/
R_Format parse4RFormat(uint32_t machineInstruction) {
	int mask4RegisterFields = pow(2,NO_REGISTER_LENGTH) - 1;
	int mask4ShamtField = pow(2,SHAMT_LENGTH) - 1;

	//Only a mask is necessary to parse FUNCT field
	unsigned int funct = machineInstruction & ((int) pow(2,NO_REGISTER_LENGTH) - 1);
	machineInstruction >>= FUNCT_LENGTH;
	
	//Other fields require both bit-shifting and masks
	unsigned int shamt = machineInstruction & mask4ShamtField;
	machineInstruction >>= SHAMT_LENGTH;

	unsigned int rd = machineInstruction & mask4RegisterFields;
	machineInstruction >>= NO_REGISTER_LENGTH;

	unsigned int rt = machineInstruction & mask4RegisterFields;
	machineInstruction >>= NO_REGISTER_LENGTH;

	unsigned int rs = machineInstruction & mask4RegisterFields;

	//Create and return a R_Format struct instance
	R_Format toReturn = {rs,rt,rd,shamt,funct};
	return toReturn;
}

/** 
 * Takes in a machine instruction and parses the value of its fields into an instance of the
 * I_Format struct. This is then returned and allows the callee to read the value of the fields
 * Instruction treated like an I-Format
**/
I_Format parse4IFormat(uint32_t machineInstruction) {
	int mask4RegisterFields = pow(2,NO_REGISTER_LENGTH) - 1;

	//Use mix of masking and shifting machineInstruction to obtain field values
	//All immd fields (in assembly as well) will be interpreted as Two's Complement
	//IMPORTANT: immd field will consequently be sign-extended in all operations, inc. ori!
	int32_t immd = (int16_t) (machineInstruction & ((int) pow(2,IMMD_LENGTH) - 1));
	machineInstruction >>= IMMD_LENGTH;

	unsigned int rt = machineInstruction & mask4RegisterFields;
	machineInstruction >>= NO_REGISTER_LENGTH;

	unsigned int rs = machineInstruction & mask4RegisterFields;

	//Create and return an instance of the I-Format struct
	I_Format toReturn = {rs,rt,immd};
	return toReturn;
}

/**************************************************************************
 * COMPUTER FUNCTIONS
 **************************************************************************/
void halt(computer *c) {
	printMemory(c);
	printProcessor(*c);
	printf("Halt instruction was detected. Memory and processor contents were printed.\n");
	printf("If default memory was used, fib calculation will be stored in $v0 and address 101\n");
	printf("Now halting...\n");
}

/** 
 * Takes the base address in the rs register with the offset given in the immediate field. Due to
 * word-addressable system, implemenation acts as a load-word instruction
**/
void load(computer *computer) {
	I_Format parse4I = parse4IFormat(computer->IR);
	computer->registers[parse4I.rt] 
		= computer->memory[computer->registers[parse4I.rs] + parse4I.immd];
}

/** 
 * Takes the base address in the rs register with the offset given in the immediate field. Due to
 * word-addressable system, implementation acts as a store-word instruction.
**/
void store(computer *computer) {
	I_Format parse4I = parse4IFormat(computer->IR);
	computer->memory[computer->registers[parse4I.rs] + parse4I.immd] = computer->registers[parse4I.rt];
}

/**Performs Jump command, sets PC to MAR**/
void jump(computer *computer) { computer->PC = computer->MAR; }

/**Performs MIPS Instruction "jr" to the inputted computer**/
void jumpRegister(computer *c) { c->PC = c->registers[parse4RFormat(c->IR).rs]; }

/**Performs MIPS Instruction "subtract" to the inputted computer**/
void subt(computer *Comp){
    R_Format parsed = parse4RFormat(Comp->IR);
    Comp->registers[parsed.rd] = Comp->registers[parsed.rs] - Comp->registers[parsed.rt];
}

/**Performs MIPS Instruction "add" to the inputted computer**/
void add(computer *Comp){
    R_Format parsed = parse4RFormat(Comp->IR);
    Comp->registers[parsed.rd] = Comp->registers[parsed.rs] + Comp->registers[parsed.rt];
}

/**Performs input instruction and saves input to memory**/
void input(computer *C){
    /**creating string**/
    char string[128];
    /**scanning user input and placing converted int in inReg**/
    printf("Now loading user-input into memory address: %d (0x%x)\n",C->MAR,C->MAR);
    printf("NOTE THE FOLLOWING:\n");
    printf(" - Enter input in Two's Complement binary\n");
    printf(" - Dashes allowed for readability (do not use at beginning of string)\n");
    printf(" - USE APPROPRIATE SIGN BIT\n\n");
    printf("If using default memory, input will be used to calculate a Fibonacci number\n");
    printf("Fibonacci number will be stored in register $v0 and address 101\n");
    printf("CAUTION: Large numbers should not be used for Fib. Recursion is used in the "
	   "calculation; large numbers may cause the stack to grow into the code block\n");
    printf("MEMORY HAS %d ADDRESSES\n",MEMORY_CELLS);
    printf("For a test, an input of \"011010\" (26 in binary) has been shown to work\n");
    printf("ENTER INPUT: ");
    scanf("%s",string);
    C->inReg = bin2Dec(string);
    /**Storing user input in memory location pointed to by MAR**/
    C->memory[C->MAR]=C->inReg;
}

/**Performs output instruction. outputs binary representation of memory location pointed to by MAR**/
void output(computer *C){
    printf("[%d]\t%s\n\n", C->MAR, intDec2Bin(C->memory[C->MAR], WORD_LENGTH));
}

/** Performs the MIPS "addi" instruction to the inputted computer **/
void addi(computer *c) { 
     I_Format parsed = parse4IFormat(c->IR);
     c->registers[parsed.rt] = c->registers[parsed.rs] + parsed.immd;
}

/** Performs the MIPS "and" instruction to the inputted computer **/
void and(computer *c) {
     R_Format parsed = parse4RFormat(c->IR);
     c->registers[parsed.rd] = c->registers[parsed.rs] & c->registers[parsed.rt];
}

/** Performs the MIPS "or" instruction to the inputted computer **/
void or(computer *c) {
     R_Format parsed = parse4RFormat(c->IR);
     c->registers[parsed.rd] = c->registers[parsed.rs] | c->registers[parsed.rt];
}

/** Performs the MIPS "jal" instruction to the inputted computer **/
void jumpAndLink(computer *c) {
     c->registers[ra] = c->PC;
     c->PC = c->MAR;
}

/** 
 * Performs the MIPS "lui" instruction to the inputted computer 
 * Loads the immediate field bits into the upper half of the register
**/
void loadUpperImmediate(computer *c) {
     I_Format parsed = parse4IFormat(c->IR);
     c->registers[parsed.rt] = parsed.immd << IMMD_LENGTH;
}

/**
 * Performs the MIPS "beq" instruction to the inputted computer
 * I_Format: If contents of register rs equals that of register rt,
 * skip to the instruction stored in memory location immd
**/
void branchIfEqual(computer *C) {
    I_Format parsed = parse4IFormat(C->IR);
    if(C->registers[parsed.rs] == C->registers[parsed.rt])
	C->PC = parsed.immd;
}

/**
 * Performs the MIPS "bne" instruction to the inputted computer
 * I_Format: If contents of register rs does not equals that of register rt,
 * skip to the instruction stored in memory location immd
**/
void branchIfNotEqual(computer *C) {
    I_Format parsed = parse4IFormat(C->IR);
    if(C->registers[parsed.rs] != C->registers[parsed.rt])
	C->PC = parsed.immd;
}

/** Performs 'orimmediate' instriuction**/
void orImmediate(computer *c) {
    I_Format parsed = parse4IFormat(c->IR);
    c->registers[parsed.rt] = c->registers[parsed.rs] | parsed.immd;
}


/**************************************************************************
 * INITIALISATION OF COMPUTER'S MEMORY
 **************************************************************************/
/** Loads instructions directly into memory starting at 0 **/
void loadDefault(computer *computer){ readFromFile(computer, DEFAULT_MEMORY_PATH); }

/**
 * Reads data in to memory from txt file pointed to by "path" parameter
**/
void readFromFile(computer *comp, char *path){
	FILE *fp = fopen(path,"r");
	int limit = 360;
	char line[limit];
	char binString[limit];
	int address = 0;
	
	while(fgets(line,limit,fp) != NULL) {
		//Ignore lines starting with comments or blank lines
		if(line[0] == '\n' || line[0] == '#')
			continue;
		//Only the first string of a line will be considered
		sscanf(line,"%s",binString);
		comp->memory[address++] = uBin2Dec(binString);
	}
	fclose(fp);
}

/**
 * Loads user input's as 32 bit machine code and places them into the memory
 * until EXIT is found
**/
void loadUser(computer *Comp){
    printf("Please enter in the desired memory contents (in binary). Each line entered will be stored in sequential addresses starting at address 0\n");
    printf("Enter \"EXIT\" when completed\n");
    int address = 0;
    int limit = 360;
    while(1) {
	char userInput[limit];
        scanf("%s", userInput);
	if(strcmp(userInput, "EXIT"))
		Comp->memory[address++] = uBin2Dec(userInput);
	else
		return;
    }
}

/**************************************************************************
 * BINARY STRING TRANSLATION AND HANDLING
 **************************************************************************/
/**
 * Takes in a signed integer and returns a string representing the Two's-Complement binary representation
 * of the integer. Length of binary string equal to length of WORD_LENGTH. binary nunmber padded with sign bit
**/
char *intDec2Bin(unsigned int decimal, int length) {
	// Create a space on the heap on which to store the returned string
	char *binString = malloc(length+1);
	*(binString+length) = '\0';
	// *binAdjust used to set the characters of the String
	char *binAdjust = binString;

	//Record if input is negative
	int isNegative = 0;
	if(decimal < 0)
		isNegative = 1;

	//At end of loop, binAdjust points to null-character
	for(int i = 0; i < length; i++)
		*(binAdjust++) = '0';

	// Make binAdjust point to the last non-null character
	binAdjust--;

	// Run the algorithm for finding out each bit of the binary representation
	while(decimal != 0) {
		*(binAdjust--) = (decimal % 2) + '0';	//Adding ASCII zero converts decimal digit to character
		decimal /= 2;
	}

	// If negative, now that magnitude is known, it must be converted
	if(isNegative)
		uBin2TwoComp(binString);

	return binString;
}

/**
 * Takes in a pointer to a String representing a Two-Complement binary number. Returns a signed integer
 * equal to the binary number. Won't work if binString starts with a '-'
**/
int bin2Dec(char *inputBinString) {
	// Create a new String that the function can modify
	char binString[strlen(inputBinString)+1];
	strcpy(binString,inputBinString);

	// If negative, treat number like normal except convert to positive equivalent first
	int isNegative = 0;
	if(*binString == '1') {
		uBin2TwoComp(binString);
		isNegative = 1;
	}

	int decimal = uBin2Dec(binString);
	
	if(isNegative)
		decimal *= -1;

	return decimal;
}

/** Takes in a binString, treats it as unsigned and returns an int of its value */
int uBin2Dec(char *inputBinString) {
	// Create a new String that the function can modify
	char binString[strlen(inputBinString)+1];
	strcpy(binString,inputBinString);

	// Makes binAdjust point to the last non-null character
	char *binAdjust = strchr(binString,'\0') - 1;

	// Reads each character and adds the value of a bit if it is a '1' char
	int decimal = 0, powerOfTwo = 1;
	for(; binAdjust >= binString; binAdjust--) {
		if(*binAdjust == '1')
			decimal += powerOfTwo;
		else if(*binAdjust == '-')
			continue; //do nothing; do not shift powerOfTwo
		powerOfTwo <<= 1;
	}

	return decimal;
}

/**
* Takes in a String representing an unsigned binary number. And converts it to a string representing
* the negative of the number (in Two's Complement form). Requires the MSB to be 0
**/
void *uBin2TwoComp (char *binString) {
    invertBinString(binString);
    addOneToBinString(binString);
}

/**
* Takes in a binary string and modifies it so that it represents 1 plus its initial value
**/
void addOneToBinString(char *binString) {
    //Point binAdjust to the last non-null character
    char *binAdjust = strchr(binString,'\0') - 1;
    //Find the least significant 0 to overflow the one into
    for(;binAdjust >= binString; binAdjust--) {
        if(*binAdjust == '0') {
            *binAdjust = '1';
            break;
        } else if(*binAdjust == '1')
            *binAdjust = '0';
    }
}

/**
* Takes in a binary string and inverts each bit of the binary string
**/
void *invertBinString(char *binString) {
    for(;*(binString) != '\0'; binString++) {
        if(*binString == '0')
            *binString = '1';
        else if(*binString == '1')
            *binString = '0';
    }
}

/**************************************************************************
 * DISASSEMBLER FUNCTIONS
 **************************************************************************/
/** Disassembles the memory contents from 0 up to and including the first halt instruction **/
void disassembleMemory(computer c) {
	for(int i = 0; i < MEMORY_CELLS; i++) {
		printf("[%d]\t%s\n", i, disassembleLine(c.memory[i]));
		// If printed instruction was halt, then stop printing memory
		if(!parseOpcode(c.memory[i]) && !parseFunction(c.memory[i]))
			break;
	}    
}

/** Takes in a single instruction in machine code and returns a String of the assembly translation **/
char *disassembleLine(uint32_t machineInstruction) {
	// Final String will concatenate all arguments
	const char *label = getOpcodeLabel(machineInstruction);
	const int maxCharArrayLength = 13;
	char first[maxCharArrayLength];
	char second[maxCharArrayLength];
	char third[maxCharArrayLength];
		
	// Initially set all arg strings to null in case they are not there come concatenation
	memset(first,0,maxCharArrayLength);
	memset(second,0,maxCharArrayLength);
	memset(third,0,maxCharArrayLength);
	
	// Prepare parsing for the switch statement and handling of arguments
	int opcode = parseOpcode(machineInstruction);
	R_Format r = parse4RFormat(machineInstruction);
	I_Format i = parse4IFormat(machineInstruction);

	// MIPS assembly shows different instructions in different ways. Some will have only two 
	// register arguments, some will have one address argument, some will have brackets.
	// This switch statement seeks to group instructions that are displayed in the same way
	switch(opcode) {
		case 0: 
			// Halt has no arguments
			if(!parseFunction(machineInstruction)) { } // No arguments; no need to change their Strings
			else if(parseFunction(machineInstruction) == 0x08) // jr only has one register argument
				strcpy(first, registerLabels[r.rs]);	
			// The other functions all have three register arguments
			else {
				strcpy(first, registerLabels[r.rd]);
				strcpy(second, registerLabels[r.rs]);
				strcpy(third, registerLabels[r.rt]);
			}
			break;
		// The following cases are all J-Format Instructions (one address arg only)
		case 0x02: case 0x04: case 0x05: case 0x06:
			snprintf(first, maxCharArrayLength, "%d",parseAddress(machineInstruction));
			break;
		// Takes in two register arguments
		case 0x07:
			strcpy(first, registerLabels[r.rs]);
			strcpy(second, registerLabels[r.rt]);
			break;
		// Takes in two register arguments and one immd argument
		case 0x08: case 0x0d: 
			strcpy(first, registerLabels[i.rt]);
			strcpy(second, registerLabels[i.rs]);
			snprintf(third, maxCharArrayLength, "%d", i.immd);
			break;
		// lui takes in one register argument and an immd
		case 0x0f:
			strcpy(first, registerLabels[i.rt]);
			snprintf(second, maxCharArrayLength, "%d", i.immd);
			break;
		// sw and lw take in two register and an immd. Instructions also require brackets
		case 0x23: case 0x2b:
			strcpy(first, registerLabels[i.rt]);
			snprintf(second, maxCharArrayLength, "%d(%s)", i.immd, registerLabels[i.rs]);
			break;
		// beq and bne take in two register arguments and an immd
		case 0x3b: case 0x3c:
			strcpy(first, registerLabels[r.rs]);
			strcpy(second, registerLabels[r.rt]);
			snprintf(third, maxCharArrayLength, "%d", i.immd);
			break;			
	}
	//Prepare a String that fits all arguments plus the null-character and max possible spaces
	int lineLength = strlen(label) + strlen(first) + strlen(second) + strlen(third) + 3 + 1;
	char *assemblyLine = malloc(lineLength);;
	snprintf(assemblyLine, lineLength, "%s %s %s %s", label,first,second,third);
	return assemblyLine;
}

/** Returns a const string of the opcode label only **/
const char *getOpcodeLabel(uint32_t machineInstruction) {
	char *label;
	int opcode = parseOpcode(machineInstruction);
	switch(parseOpcode(machineInstruction)) {
		case 0:
			switch(parseFunction(machineInstruction)) {
				case 0x00: label = "halt";			break;
				case 0x08: label = "jr";			break;
				case 0x20: label = "add";			break;
				case 0x22: label = "sub";			break;
				case 0x24: label = "and";			break;
				case 0x25: label = "or";			break;
				default:   label = "UNRECOGNISED OPCODE";
			} break;
		case 0x02: label = "j"; 					break;
		case 0x04: label = "jal"; 					break;
		case 0x05: label = "input"; 					break;
		case 0x06: label = "output"; 					break;
		case 0x07: label = "skipCond"; 					break;
		case 0x08: label = "addi"; 					break;
		case 0x0d: label = "ori";					break;
		case 0x0f: label = "lui";					break;
		case 0x23: label = "lw";					break;
		case 0x2b: label = "sw";					break;
		case 0x3b: label = "beq";					break;
		case 0x3c: label = "bne";					break;
		default:   label = "UNRECOGNISED OPCODE";			break;
	}
	return label;
}
