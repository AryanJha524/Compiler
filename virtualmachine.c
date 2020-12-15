// This program was written by Aryan Jha
// COP 3402, Fall 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

// defining the constants that will be used through the progam
# define MAX_CODE_LENGTH 500
# define MAX_STACK_HEIGHT 1000

// this is the structure the for the instruction register
// contains the OP code, R, L, and M values
struct instruction_register {
        int op, r, l, m;
};


int base(int l, int base, int* stack) // l stand for L in the instruction format
{
        int b1; //find base L levels up
        b1 = base;
        while (l > 0)
        {
                b1 = stack[b1];
                l--;
        }
        return b1;
}

// convert main func. below into this (handle a array of code instructions rather than a file of numbers
// easy fix,,, create an array on ints from the code by looping through code array and plucking off vals,
// then process as normal, so I wouldn't have to change anything....)

void virtualmachine(struct instruction * code, int flag)
{
        // define components of PM/0
        int sp = MAX_STACK_HEIGHT;
        int bp = sp - 1;
        int pc = 0;
        int halt = 1;
        int stack[MAX_STACK_HEIGHT];

        // initialize the stack values as 0
        for (int i = 0; i < MAX_STACK_HEIGHT; i++)
        {
                stack[i] = 0;
        }

        int instructions[MAX_CODE_LENGTH];
        int * num_ars = (int *)calloc(1, sizeof(int));
        int num_instructions = 0;
        int registers[7];

        // initializing the instruction register and its values
        struct instruction_register instruct_reg;
        instruct_reg.op = 0;
        instruct_reg.r = 0;
        instruct_reg.l = 0;
        instruct_reg.m = 0;
        // read in instructions from instruction code array
        for (int i = 0; i < size_instructions; i++) // loop through array of instruction code
        {
                instructions[num_instructions] = code[i].opcode;
                instructions[num_instructions + 1] = code[i].r;
                instructions[num_instructions + 2] = code[i].l;
                instructions[num_instructions + 3] = code[i].m;

                num_instructions+=4;
        }
        // printing the initial values of the components
        if (flag == 1)
        {
          printf("Initial Values\t\t\tpc\tbp\tsp\n");
          printf("Registers: ");
        }


        for(int i = 0; i < 7; i++)
        {
                registers[i] = 0;
                if (flag == 1)
                {
                  printf("%d ", registers[i]);
                }
        }
        if (flag == 1)
        {
          printf("\nStack: \n\n");
        }
        pc = 0;
        int cur_stack_size = 0;
        int cur_index = 0;
        int print_index = 0;
        int counter = 0;
        int extra_tab = 0;
        int old_sp = MAX_STACK_HEIGHT - 1;

        // loop through all instruction or until halt instruction is met and execute
        char word[4];
        while(halt == 1)
        {
                instruct_reg.op = instructions[pc];
                instruct_reg.r = instructions[pc + 1];
                instruct_reg.l = instructions[pc + 2];
                instruct_reg.m = instructions[pc + 3];
                if(instruct_reg.op == 1)
                {
                        strcpy(word, "LIT");
                }
                else if(instruct_reg.op == 2)
                {
                        strcpy(word, "RTN");
                }
                else if(instruct_reg.op == 3)
                {
                        strcpy(word, "LOD");
                }
                else if(instruct_reg.op == 4)
                {
                        strcpy(word, "STO");
                }
                else if(instruct_reg.op == 5)
                {
                        strcpy(word, "CAL");
                }
                else if(instruct_reg.op == 6)
                {
                        strcpy(word, "INC");
                }
                else if(instruct_reg.op == 7)
                {
                        strcpy(word, "JMP");
                }
                else if(instruct_reg.op == 8)
                {
                        strcpy(word, "JPC");
                }
                else if(instruct_reg.op == 9)
                {
                        strcpy(word, "SYS");
                }
                else if(instruct_reg.op == 10)
                {
                        strcpy(word, "NEG");
                }
                else if(instruct_reg.op == 11)
                {
                        strcpy(word, "ADD");
                }
                else if(instruct_reg.op == 12)
                {
                        strcpy(word, "SUB");
                }
                else if(instruct_reg.op == 13)
                {
                        strcpy(word, "MUL");
                }
                else if(instruct_reg.op == 14)
                {
                        strcpy(word, "DIV");
                }
                else if(instruct_reg.op == 15)
                {
                        strcpy(word, "ODD");
                }
                else if(instruct_reg.op == 16)
                {
                        strcpy(word, "MOD");
                }
                else if(instruct_reg.op == 17)
                {
                        strcpy(word, "EQL");
                }
                else if(instruct_reg.op == 18)
                {
                        strcpy(word, "NEQ");
                }
                else if(instruct_reg.op == 19)
                {
                        strcpy(word, "LSS");
                }
                else if(instruct_reg.op == 20)
                {
                        strcpy(word, "LEQ");
                }
                else if(instruct_reg.op == 21)
                {
                        strcpy(word, "GTR");
                }
                else if(instruct_reg.op == 22)
                {
                        strcpy(word, "GEQ");
                }
                if (flag == 1)
                {
                  printf("\n%d %s: %d %d %d", pc/4, word, instruct_reg.r, instruct_reg.l, instruct_reg.m);
                }
                switch (instruct_reg.op)
                {
                case 1:           // LIT R, 0, M: RF[R] <- M;
                        registers[instruct_reg.r] = instruct_reg.m;
                        pc += 4;
                        break;
                case 2:           // RTN  0, 0, 0: sp = bp + 1 --> bp = stack[sp - 2] --> pc = stack[sp - 3]
                        old_sp = sp; // since we change sp, save the previous state in case a new ar is created
                        sp = bp + 1;
                        bp = stack[sp - 2];
                        pc = stack[sp - 3];
                        pc = pc * 4;
                        break;
                case 3:           // LOD R,L,M: RF[R] = stack[base(L, bp) - M];
                        registers[instruct_reg.r] = stack[base(instruct_reg.l, bp, stack) - instruct_reg.m];
                        pc += 4;
                        break;
                case 4:           // STO R, L, M: stack[base(L, bp) - M] = RF[R];
                        stack[base(instruct_reg.l, bp, stack) - instruct_reg.m] = registers[instruct_reg.r];
                        pc += 4;
                        break;
                case 5:           // CAL 0, L, M
                        num_ars = (int *)realloc(num_ars, sizeof(int)  + 1); // increase number of ARs by 1
                        if(cur_index == 0) // if this is the first AR, we must + 1 to the size due to 0-indexing
                        {
                                num_ars[cur_index] = old_sp - sp + 1;
                        }
                        else
                        {
                                num_ars[cur_index] = old_sp - sp; // the size of the AR should be previous sp - current sp
                        }
                        cur_index += 1; // increase the current index of the AR array so next time an AR is created, we updated the current AR size
                        cur_stack_size = 0; // since a new AR was created, the current # of elements in the AR is 0
                        stack[sp - 1] = base(instruct_reg.l, bp, stack);
                        stack[sp - 2] = bp;
                        stack[sp - 3] = pc / 4 + 1;
                        old_sp = sp;
                        bp = sp - 1;
                        pc = instruct_reg.m * 4;
                        break;
                case 6:            // INC 0, 0, M: sp <- sp - M
                        sp = sp - instruct_reg.m;
                        cur_stack_size = old_sp - sp;
                        pc += 4;
                        break;
                case 7:           // – JMP   0, 0, M: pc = M;
                        pc = (4 * instruct_reg.m);
                        break;
                case 8:           // – JPC   R, 0, M    if RF[R] == 0 then { pc = M; }
                        if(registers[instruct_reg.r] == 0)
                        {
                                pc = 4 * instruct_reg.m;
                        }
                        else
                        {
                                pc += 4;
                        }
                        break;
                case 9:           // – SYS   R, 0, 1 or 2 or 3
                        if(instruct_reg.m == 1)
                        {
                                extra_tab = 1;
                                if (flag == 1)
                                {
                                  printf("\nRF[%d] = %d\n", instruct_reg.r, registers[instruct_reg.r]);
                                }
                                pc += 4;
                        }
                        else if (instruct_reg.m == 2)
                        {
                                extra_tab = 1;
                                int val;
                                printf("\nEnter a value for Register File %d\n", instruct_reg.r);
                                scanf("%d", &val);
                                registers[instruct_reg.r] = val;
                                pc += 4;
                        }
                        else if (instruct_reg.m == 3)
                        {
                                halt = 0;
                        }
                        break;
                case 10:           // NEG	R, L, M (RF[R] = - RF[R]
                        registers[instruct_reg.r] = -1 * registers[instruct_reg.r];
                        pc += 4;
                        break;
                case 11:           // ADD   R, L, M
                        registers[instruct_reg.r] = registers[instruct_reg.l] + registers[instruct_reg.m];
                        pc += 4;
                        break;
                case 12:           // SUB
                        registers[instruct_reg.r] = registers[instruct_reg.l] - registers[instruct_reg.m];
                        pc += 4;
                        break;
                case 13:           // MUL
                        registers[instruct_reg.r] = registers[instruct_reg.l] * registers[instruct_reg.m];
                        pc += 4;
                        break;
                case 14:           // DIV
                        registers[instruct_reg.r] = registers[instruct_reg.l] / registers[instruct_reg.m];
                        pc += 4;
                        break;
                case 15:           // ODD
                        registers[instruct_reg.r] = registers[instruct_reg.r] % 2;
                        pc += 4;
                        break;
                case 16:           // MOD
                        registers[instruct_reg.r] = registers[instruct_reg.l] % registers[instruct_reg.m];
                        pc += 4;
                        break;
                case 17:           // EQL
                        if (registers[instruct_reg.l] == registers[instruct_reg.m])
                        {
                                registers[instruct_reg.r] = 1;
                        }
                        else
                        {
                                registers[instruct_reg.r] = 0;
                        }
                        pc += 4;
                        break;
                case 18:           // NEQ
                        if (registers[instruct_reg.l] != registers[instruct_reg.m])
                        {
                                registers[instruct_reg.r] = 1;
                        }
                        else
                        {
                                registers[instruct_reg.r] = 0;
                        }
                        pc += 4;
                        break;
                case 19:           // LSS
                        if (registers[instruct_reg.l] < registers[instruct_reg.m])
                        {
                                registers[instruct_reg.r] = 1;
                        }
                        else
                        {
                                registers[instruct_reg.r] = 0;
                        }
                        pc += 4;
                        break;
                case 20:           // LEQ
                        if (registers[instruct_reg.l] <= registers[instruct_reg.m])
                        {
                                registers[instruct_reg.r] = 1;
                        }
                        else
                        {
                                registers[instruct_reg.r] = 0;
                        }
                        pc += 4;
                        break;
                case 21:           // GTR
                        if (registers[instruct_reg.l] > registers[instruct_reg.m])
                        {
                                registers[instruct_reg.r] = 1;
                        }
                        else
                        {
                                registers[instruct_reg.r] = 0;
                        }
                        pc += 4;
                        break;
                case 22:           // GEQ
                        if (registers[instruct_reg.l] >= registers[instruct_reg.m])
                        {
                                registers[instruct_reg.r] = 1;
                        }
                        else
                        {
                                registers[instruct_reg.r] = 0;
                        }
                        pc += 4;
                        break;
                }
                // printing out the values of different component of the CPU
                if (flag == 1)
                {
                  if(extra_tab == 1)
                  {
                        printf("\t\t\t\t%d\t%d\t%d\n", pc/4, bp, sp);
                  }
                  else
                  {
                        printf("\t\t\t%d\t%d\t%d\n", pc/4, bp, sp);
                  }
                }
                extra_tab = 0;
                if (flag == 1)
                {
                  printf("Registers: ");
                  for (int j = 0; j < 7; j++)
                  {
                          printf("%d ", registers[j]);
                  }
                  printf("\nStack: ");
                }
                for(int j = MAX_STACK_HEIGHT - 1; j >= sp; j--)
                {
                        if (print_index == num_ars[counter]) // if our counter = the size of current AR, print a dividing line
                        {
                                if (flag == 1)
                                {
                                  printf("| %d ", stack[j]);
                                }
                                if (counter + 1 <= sizeof(num_ars) / 4) // make sure we don't exceed the size of num AR array
                                {
                                        counter += 1; // increase the index of AR array to the next AR
                                }
                                print_index = 0;
                        }
                        else // we are currently within an AR, so print elements as usual
                        {
                                if (flag == 1)
                                {
                                    printf("%d ", stack[j]);
                                }
                        }
                        print_index += 1;
                }
                print_index = 0;
                counter = 0;
                if (flag == 1)
                {
                  printf("\n");
                }
        }
        free(num_ars);
        return;
}
