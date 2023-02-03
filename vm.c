/*
Daniel Thew
COP3402
Fall 2022
Prof. Montagne
*/

//basic libraries, string.h used for strcpy
#include <stdio.h>
#include <string.h>
#include "compiler.h"


//constants
#define ARRAY_SIZE 500


/**********************************************/
/*		Find base L levels down		 */
/*							 */
/**********************************************/
int base(int* pas, int BP, int L)
{
	int arb = BP;	// arb = activation record base
	while (L > 0)     //find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

//main function to handle everything BUT base function
void execute(int trace_flag, instruction* code)
{
	//halt flag
	int halt = 1;

	//initialize project address space (pas) to hold all zeroes
	//read through every instruction in the input, storing into pas
	int pas[ARRAY_SIZE] = { 0 };
	//int cur_field;
	//fscanf(fp, "%d", &cur_field);
	int code_count = 0;
	int pas_count = 0;
	while (code[code_count].op != -1) {
		pas[pas_count] = code[code_count].op;
		pas[pas_count + 1] = code[code_count].l;
		pas[pas_count + 2] = code[code_count].m;
		code_count++;
		pas_count += 3;
	}

	//BP = next index after last M
	//SP = BP-1
	//PC = 0
	int BP = pas_count;
	int SP = BP - 1;
	int PC = 0;

	//IR
	instruction IR;

	if(trace_flag==1){
		printf("VM Execution:\n");
		//print initial values
		printf("%18s %2s %2s %2s\n", "PC", "BP", "SP", "stack");
		printf("Initial Values:  %d %d %d\n", PC, BP, SP);
	}

	//string to hold op code abbreviation (3 chars for op_code, one for \0
	char op_code[4];
	//switch case to determine the op code
	while (halt == 1) {
		//fetch
		//set OP to the opcode at the PC
		//set level to level at the PC + 1
		//set M to value at the PC + 2
		//increment PC by 3 (next line in file)
		IR.op = pas[PC];
		IR.l = pas[PC + 1];
		IR.m = pas[PC + 2];
		PC += 3;

		//execute
		//switch based on the IR's current opcode
		//in EVERY switch, depending on the opcode, we will change op_code to store the instruction code
		switch (IR.op) {
			//lit - Pushes a constant value (literal) M onto the stack
			//increment SP and store IR's M val in that index of pas (in the stack)
		case 1:
			strcpy(op_code, "LIT");
			SP++;
			pas[SP] = IR.m;
			break;
			//opr - Operation to be performed on the data at the top of the stack
			//(or return from function)
		case 2:
			//cases 0->10
			switch (IR.m) {
				//rtn
			case 0:
				strcpy(op_code, "RTN");
				SP = BP - 1;
				BP = pas[SP + 2];
				PC = pas[SP + 3];
				break;
				//add
			case 1:
				strcpy(op_code, "ADD");
				pas[SP - 1] += pas[SP];
				SP--;
				break;
				//sub
			case 2:
				strcpy(op_code, "SUB");
				pas[SP - 1] -= pas[SP];
				SP--;
				break;
				//mul
			case 3:
				strcpy(op_code, "MUL");
				pas[SP - 1] *= pas[SP];
				SP--;
				break;
				//div
			case 4:
				strcpy(op_code, "DIV");
				pas[SP - 1] /= pas[SP];
				SP--;
				break;
				//eql
			case 5:
				strcpy(op_code, "EQL");
				pas[SP - 1] = (pas[SP - 1] == pas[SP]);
				SP--;
				break;
				//neq
			case 6:
				strcpy(op_code, "NEQ");
				pas[SP - 1] = (pas[SP - 1] != pas[SP]);
				SP--;
				break;
				//lss
			case 7:
				strcpy(op_code, "LSS");
				pas[SP - 1] = (pas[SP - 1] < pas[SP]);
				SP--;
				break;
				//leq
			case 8:
				strcpy(op_code, "LEQ");
				pas[SP - 1] = (pas[SP - 1] <= pas[SP]);
				SP--;
				break;
				//gtr
			case 9:
				strcpy(op_code, "GTR");
				pas[SP - 1] = (pas[SP - 1] > pas[SP]);
				SP--;
				break;
				//geq
			case 10:
				strcpy(op_code, "GEQ");
				pas[SP - 1] = (pas[SP - 1] >= pas[SP]);
				SP--;
				break;
				//default for switch
			default:
				break;
			}
			break;
			//lod - Load value to top of stack from the stack location at
			//offset M from L lexicographical levels down
		case 3:
			strcpy(op_code, "LOD");
			SP++;
			pas[SP] = pas[base(pas, BP, IR.l) + IR.m];
			break;
			//sto - Store value at top of stack in the stack location at offset M
			//from L lexicographical levels down
		case 4:
			strcpy(op_code, "STO");
			pas[base(pas, BP, IR.l) + IR.m] = pas[SP];
			SP--;
			break;
			//cal - Call procedure at code index M (generates new
			//Activation Record and PC <- M)
		case 5:
			strcpy(op_code, "CAL");
			pas[SP + 1] = base(pas, BP, IR.l);//static link
			pas[SP + 2] = BP;//dynamic link
			pas[SP + 3] = PC;//return address
			BP = SP + 1;
			PC = IR.m;
			break;
			//inc - Allocate M memory words (increment SP by M). First 
			//four are reserved to   Static Link (SL), Dynamic Link (DL),
			//and Return Address (RA)
		case 6:
			strcpy(op_code, "INC");
			SP += IR.m;
			break;
			//jmp - Jump to instruction M (PC <- M)
		case 7:
			strcpy(op_code, "JMP");
			PC = IR.m;
			break;
			//jpc - Jump to instruction M if top stack element is 0
		case 8:
			strcpy(op_code, "JPC");
			if (pas[SP] == 0)
				PC = IR.m;
			SP--;
			//wrt/red/hlt
		case 9:
			switch (IR.m) {
				//wrt - Write the top stack element to the screen
			case 1:
				strcpy(op_code, "WRT");
				printf("Output result is: %d\n", pas[SP]);
				SP--;
				break;
				//red - Read in input from the user and store it on top of the stack
			case 2:
				strcpy(op_code, "RED");
				SP++;
				//prompt, as shown in OUTPUT FILE section of rubric
				printf("Please Enter an Integer: ");
				scanf("%d", &pas[SP]);
				break;
				//hlt End of program (Set Halt flag to zero)
			case 3:
				strcpy(op_code, "HLT");
				halt = 0;
				break;
				//default for switch
			default:
				break;
			}
			//default for switch
		default:
			break;
		}

		if (trace_flag == 1) {
			//print current instruction, counters, and stack
			printf("%s %4d %4d %4d %2d %2d ", op_code, IR.l, IR.m, PC, BP, SP);
			//beginning at the very first BP and ending at the SP
			for (int ctr = pas_count; ctr <= SP; ctr++) {
				//at new AR's OTHER THAN THE START OF THE FIRST, print the '|' symbol
				if (ctr == BP && ctr != pas_count) {
					printf(" | ");
				}
				//print the stack number
				printf("%d ", pas[ctr]);
			}
			//newline for next instruction set
			printf("\n");
		}
	}
}