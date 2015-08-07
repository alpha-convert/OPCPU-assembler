#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "defs.h"
#include "regs.h"

int OpCode(char *op, HashItem* hashmap){
	for(int i = 0; i < NUMOPS; ++i){
		if(strncmp((const char *)hashmap[i].key,op,3) == 0)
			return hashmap[i].val;
	}
	return NOT_FOUND;
}

inline int GPRegLookup(char *reg){
	int op = gpregs[atoi((const char *)reg + 1) - 1];
	if(op){
		return op;
	} else {
		printf("Register %s not found in general purpose registers\n", reg);
		return NOT_FOUND;
	}
}

int isGP(char *reg){
	if(reg[0] == 'R' || reg[0] == 'r'){
		return 1;
	} else {
		return NOT_FOUND;
	}
}

int isSP(char *reg){
	if(reg[0] != 'R' && reg[0] != 'r'){
		return 1;
	} else {
		return NOT_FOUND;
	}
}

int SPRegLookup(char *reg){
	if(strcmp(reg,"PC") == 0){
		return PC;
	} else if (strcmp(reg,"SP") == 0){
		return SP;
	} else if (strcmp(reg,"FP") == 0){
		return FP;
	} else if (strcmp(reg,"ZR") == 0){
		return ZR;
	} else if (strcmp(reg,"FR") == 0){
		return FR;
	} else if (strcmp(reg,"WR") == 0){
		return WR;
	}

	printf("Register %s not found in special purpose registers\n",reg);
	return NOT_FOUND; //never will be reached;
}

int parse9BitSigned(char *c){
	c += 2; //skip the '0b'
	int sign = (c[SIGN_BIT] == '1')? 0 : 1;	//get the sign bit
	int final = (int) strtol(c,0,2); //get the val bits	
	final |= sign << 9; //add on the sign bit
	return sign & 0x1ff; //mask off the lower 9 bits

}

int parse16Bit(char *c){
	return (int)strtol(c, NULL, 0);
}

int starrlen(char **starr){
		int i = 0;
		while(*starr[i] != '\0'){
			++i;
		}
		return i;
}


int main(void)
{
static HashItem opCodeLookup[] = {
		{"add",ADDCTRL},
		{"sub",SUBCTRL},
		{"mul",MULCTRL},
		{"div",DIVCTRL},
		{"and",ANDCTRL},
		{"or", ORCTRL},
		{"not",NOTCTRL},
		{"loa",LOACTRL},
		{"sto",STOCTRL},
		{"shr",SHRCTRL},
		{"shl",SHLCTRL},
		{"beq",BEQCTRL},
		{"blt",BLTCTRL},
		{"ll" ,LLCTRL}
	};
	
	Instruction *instructions = (Instruction *) malloc(100*sizeof(Instruction));
	int instructionIndex = 0;

	char code[] = "add r506 r506 r506 not r12 r300 blt r506 r506 0b100000001 ll r506 0xFF sub r2 SP r15";
	char *pch;
	char *tokens[strlen(code)]; //should be enough
	int tokenIndex;

	pch = strtok(code," ");
	for(tokenIndex = 0; pch; ++tokenIndex){
			tokens[tokenIndex] = (char *) malloc(strlen(pch) + 1);
			strcpy(tokens[tokenIndex],pch);
//			printf("tokens[%d] is located at %p, has size %lu. Current pch is %s.\n", tokenIndex, tokens[tokenIndex],sizeof(tokens[tokenIndex]), pch);

			pch = strtok(0, " ");
	}
	tokens[tokenIndex + 1] = (char *)malloc(sizeof(char));
	strcpy(tokens[tokenIndex + 1],"\0");

	for(tokenIndex = 0; tokenIndex < starrlen(tokens) - 1;instructionIndex++){
		int opcode = OpCode(tokens[tokenIndex],opCodeLookup); //look up the opcode control form the table
		switch(opcode){ //uh oh
				
				case ADDCTRL:
				case SUBCTRL:
				case MULCTRL:		//all of these have the same signature, so it shouldn't matter.
				case DIVCTRL:
				case ANDCTRL:
				case ORCTRL:
					instructions[instructionIndex].op |= opcode << 27; //push it into the byterinoes
					
					for(int argumentNum = 0; argumentNum < THREE_ARGS; ++argumentNum){ //for each argument in add
						tokenIndex++; //Another token
						if(isSP(tokens[tokenIndex]) > NOT_FOUND){ //check sp
							instructions[instructionIndex].op |= SPRegLookup(tokens[tokenIndex]) << argumentNum * 9; //the shifts are all multiples of 9	


						} else if(isGP(tokens[tokenIndex]) > NOT_FOUND) { //check gp
							instructions[instructionIndex].op |= GPRegLookup(tokens[tokenIndex]) << argumentNum * 9;	

						} else {
							printf("Invalid argument %s for opcode\n", tokens[tokenIndex]); //not found
						}
					}
					printf("Finished opcode with instruction 0x%x\n",instructions[instructionIndex].op);
					tokenIndex++;

					
					break;

				case NOTCTRL:
				case LOACTRL: //same signature
				case STOCTRL:
				case SHRCTRL:
				case SHLCTRL:
					instructions[instructionIndex].op |= opcode << 27;
					
					for(int argumentNum = 1; argumentNum <= TWO_ARGS; ++argumentNum){
						tokenIndex++; //Another token
						if(isSP(tokens[tokenIndex]) > NOT_FOUND){ //check sp
							instructions[instructionIndex].op |= SPRegLookup(tokens[tokenIndex]) << argumentNum * 9;	


						} else if(isGP(tokens[tokenIndex]) > NOT_FOUND) { //check gp
							instructions[instructionIndex].op |= GPRegLookup(tokens[tokenIndex]) << argumentNum * 9;	

						} else {
							printf("Invalid argument %s for opcode\n", tokens[tokenIndex]); //not found
						}
					}
					printf("Finished opcode with instruction 0x%x\n",instructions[instructionIndex].op);
					tokenIndex++;
					break;

				case BLTCTRL:
				case BEQCTRL:
					instructions[instructionIndex].op |= opcode << 27;

					for(int argumentNum = 1; argumentNum <= TWO_ARGS; ++argumentNum){
						tokenIndex++; //Another token
						if(isSP(tokens[tokenIndex]) > NOT_FOUND){ //check sp
							instructions[instructionIndex].op |= SPRegLookup(tokens[tokenIndex]) << argumentNum * 9;	


						} else if(isGP(tokens[tokenIndex]) > NOT_FOUND) { //check gp
							instructions[instructionIndex].op |= GPRegLookup(tokens[tokenIndex]) << argumentNum * 9;	

						} else {
							printf("Invalid argument %s for opcode\n", tokens[tokenIndex]); //not found
						}
					}

					//SPECIAL CASE WITH 9BIT SIGNED CONSTANT
					tokenIndex++;
					instructions[instructionIndex].op |= parse9BitSigned(tokens[tokenIndex]);			//special case with B instructions. have to parse the constatnt	

					printf("Finished opcode with instruction 0x%x\n",instructions[instructionIndex].op);
					tokenIndex++;
					break;

				case LLCTRL:
					instructions[instructionIndex].op |= opcode << 27;

					tokenIndex++;

					if(isSP(tokens[tokenIndex]) > NOT_FOUND){
						instructions[instructionIndex].op |= SPRegLookup(tokens[tokenIndex]) << 18;
					} else if(isGP(tokens[tokenIndex]) > NOT_FOUND){
						instructions[instructionIndex].op |= GPRegLookup(tokens[tokenIndex]) << 18;
					
					}
					tokenIndex++;

					instructions[instructionIndex].op |= parse16Bit(tokens[tokenIndex]);

					tokenIndex++;
					printf("Finished opcode with instruction 0x%x\n",instructions[instructionIndex].op);

					
					break;
				default:
					break;
					

		}
				

	}
	//free all pointers
	for(tokenIndex = 0; tokenIndex < starrlen(tokens);tokenIndex++){
		free(tokens[tokenIndex]);
	}
	free(instructions);
	
}


