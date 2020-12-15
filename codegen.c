#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"
#include <limits.h>

// TODO --> print out to terminal after generate code function is done executing
// defining constants
# define CODE_SIZE 500
# define MAX_SYMBOL_TABLE_SIZE 500
# define MAX_PROC_LENGTH 500
// declaring global variables
int cx = 0; // code index
int lex_index = 0; // lex table index
char id[12];
char temp_proc[12];
int num;
int size_instructions = 0; // represents size of struct instruction array
int token;
char word[4];
int symbol_table_size = 0;

// declaring the functions that will be used throughout the program
struct instruction * generate_code(struct symbol * table, struct lexeme * list);
void getCodeGenToken(struct lexeme * list);
void block(int cur_proc_id, int level, struct lexeme * list, struct symbol * table, struct instruction * code);
void statement(int level, struct lexeme * list, struct symbol * table, struct instruction * code);
void condition(int level, struct lexeme * list, struct symbol * table, struct instruction * code);
void expression(int level, int r, struct lexeme * list, struct symbol * table, struct instruction * code);
void term(int level, int r, struct lexeme * list, struct symbol * table, struct instruction * code);
void factor(int level, int r, struct lexeme * list, struct symbol * table, struct instruction * code);
int findSymbolTableIndex(int id, struct symbol * table, int kind);
int findSymbolTableIndex2(int level, int kind, char * id, struct symbol * table);
int findSymbolTableIndexWithVal(int val, int kind, struct symbol * table);
void unmarkVal(char * id, struct symbol * table, int kind, int level);
void emit(int op, int r, int l, int m, struct instruction * code);
void printCodeArray(struct instruction * code);


struct instruction * generate_code(struct symbol * table, struct lexeme * list)
{
        // if we are generating code, that means the program must be syntactically correct (else this function wouldn't be called)

        struct instruction * code = malloc(500 * sizeof(struct instruction));

        // this array stores the index of the procedures in the symbol table
        int procedures[MAX_PROC_LENGTH];
        int proc_id = 0;
        int num_procs = 0;
        // emit a JMP for every procedure in symbol table
        for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
        {
                if (table[i].kind == 3)
                {
                        num_procs += 1;
                        table[i].val = proc_id + 1;
                        // storing our procedures in an array so we can update their addresses later
                        procedures[proc_id++] = i;
                        emit(7, 0, 0, 0, code); // emit a JMP for every procedure
                }
        }

        getCodeGenToken(list);
        block(1, 0, list, table, code);

        // for each convential jmp we initialized above, replace the
        // the m values with the actual addresses of the procedures since we now
        // know them
        // here is where u keep track of all procedures in an array
        // for every opcode == 7, find MOST RECENT OR FIRST proc in array
        // find the index of that proc in symbol table using function
        // update code[i].m val
        // DECREASE OR INCREASE  index of proc array to process next array to fix
        int proc_counter = 0;
        for (int i = 0; i < CODE_SIZE; i++)
        {
                if (code[i].opcode == 7)
                {
                        // assign the addresses to each of the jumps since after block is
                        // called, we presumably have found the correct addresses for each procedure
                        // code[i].m = table[i].addr;
                        if (table[i].kind == 3)
                        {

                          code[i].m = table[procedures[proc_counter]].addr;
                          proc_counter++;
                          if (proc_counter > num_procs)
                          {
                                  i = CODE_SIZE;
                                  break;
                          }
                        }
                }
        }

        // emit halt for end of program
        emit(9, 0, 0, 3, code);

        // calling a function to print out the generated code to a file after all fixes are made (stored in code array)
        printCodeArray(code);
        return code;
}

// instead of passing proc name and what not, pass the proc id and search for kind == 3 and the id we are looking for.
// this way you don't have to worry about pointers and bs
void block(int cur_proc_id, int level, struct lexeme * list, struct symbol * table, struct instruction * code)
{
        int numVars = 0;
        int numSymbols = 0;
        if (token == constsym)
        {
                do
                {
                        getCodeGenToken(list);
                        numSymbols++;
                        // unmark the const that is in the symbol table
                        // const = 1, var = 2, proc = 3
                        unmarkVal(id, table, 1, level); // may have to change to num instead of id CHECK
                        // token += 3
                        for (int i = 0; i < 3; i++)
                        {
                                getCodeGenToken(list);
                        }
                } while (token == commasym);

                // grab next token
                getCodeGenToken(list);
        }
        if (token == varsym)
        {
                do
                {
                        getCodeGenToken(list);
                        numVars++;
                        numSymbols++;
                        // unmark the var that is in the symbol table
                        unmarkVal(id, table, 2, level);
                        getCodeGenToken(list);

                } while (token == commasym);

                // grab next token
                getCodeGenToken(list);
        }
        if (token == procsym)
        {
                do
                {
                        getCodeGenToken(list);
                        strcpy(temp_proc, id);
                        numSymbols++;
                        // unmark the procedure that is in the symbol table
                        unmarkVal(id, table, 3, level);
                        for (int i = 0; i < 2; i++)
                        {
                                getCodeGenToken(list);
                        }
                        block(cur_proc_id + 1, level + 1, list, table, code);
                        // all functions end with a return
                        emit(2, 0, 0, 0, code);
                        getCodeGenToken(list);

                } while (token == procsym);

        }
        // update the symbol table entry for THIS procedure, 'm' for this procedure = current code index
        table[findSymbolTableIndex(cur_proc_id, table, 3)].addr = cx;
        // now after we return, cur_proc should be what we originally entered the function with
        emit(6, 0, 0, 3 + numVars, code); // emit INC
        statement(level, list, table, code);
        // mark the last numSymbols number of unmarked symbols
        for (int i = MAX_SYMBOL_TABLE_SIZE; i >= 0; i--)
        {
                if (table[i].mark == 0 && level == table[i].level)
                {
                        table[i].mark = 1;
                        numSymbols--;
                }
                if (numSymbols < 1)
                {
                        break;
                }
        }
        return;
}

void statement(int level, struct lexeme * list, struct symbol * table, struct instruction * code)
{
        int t1, c1, c2; // declaring table index and code index
        if (token == identsym)
        {
                // save symbol table index of the var entry with this id that is unmarked and closest in lex level
                t1 = findSymbolTableIndex2(level, 2, id, table);
                if (t1 < 0)
                {
                        // if we couldn't find a var, look for a const
                        t1 = findSymbolTableIndex2(level, 1, id, table);
                }
                // token +=2
                getCodeGenToken(list);
                getCodeGenToken(list);
                expression(level, 0, list, table, code);
                emit(4, 0, level - table[t1].level, table[t1].addr, code);

        }
        if (token == callsym)
        {
                getCodeGenToken(list);
                // save symbol table index of the procedure entry unmarked and closest in lex level
                t1 = findSymbolTableIndex2(level, 3, id, table);
                // finding correct address but emitting it is wrong
                emit(5, 0, level - table[t1].level, table[t1].addr, code);
                getCodeGenToken(list);
        }
        if (token == beginsym)
        {
                // grab next token
                getCodeGenToken(list);
                statement(level, list, table, code);
                while (token == semicolonsym)
                {
                        getCodeGenToken(list);
                        statement(level, list, table, code);
                }
                // grab next token
                getCodeGenToken(list);
        }
        if (token == ifsym)
        {
                getCodeGenToken(list);
                // calculate condition for if statement
                condition(level, list, table, code);
                // save the code index
                c1 = cx;

                emit(8, 0, 0, 0, code);

                // grab next token to move past the 'then'
                getCodeGenToken(list);

                statement(level, list, table, code);
                if (token == elsesym)
                {
                        getCodeGenToken(list);
                        c2 = cx;
                        emit(7, 0, 0, 0, code);
                        // fix jpc from earlier
                        code[c1].m = cx;
                        statement(level, list, table, code);
                        // fix jmp from earlier
                        code[c2].m = cx;
                }
                else
                {
                        // fix jpc from earlier if there was no 'else' statement
                        code[c1].m = cx;
                }
        }
        if (token == whilesym)
        {
                getCodeGenToken(list);

                // save code index for condition
                c1 = cx;
                condition(level, list, table, code);
                getCodeGenToken(list);

                // save code index for jump
                c2 = cx;
                emit(8, 0, 0, 0, code); // emit a jpc
                statement(level, list, table, code);
                emit(7, 0, 0, c1, code); // emit the saved index for the condition
                // fix the jpc from earlier
                code[c2].m = cx;
        }
        if (token == readsym)
        {
                getCodeGenToken(list);
                // save symbol table index of var entry unmarked and closest to current lex level
                t1 = findSymbolTableIndex2(level, 2, id, table); // MIGHT have to fix to include constants ?

                getCodeGenToken(list);

                emit(9, 0, 0, 2, code); // emit read
                emit(4, 0, level - table[t1].level, table[t1].addr, code); // MAYBE HERE STO 2
        }
        if (token == writesym)
        {
                getCodeGenToken(list);
                expression(level, 0, list, table, code);
                emit(9, 0, 0, 1, code);
        }

        return;
}

void condition(int level, struct lexeme * list, struct symbol * table, struct instruction * code)
{
        if (token == oddsym)
        {
                getCodeGenToken(list);

                expression(level, 0, list, table, code);
                emit(15, 0, 0, 0, code); // emit odd
        }
        else
        {
                expression(level, 0, list, table, code);
                if (token == eqsym)
                {
                        getCodeGenToken(list);

                        expression(level, 1, list, table, code);
                        emit(17, 0, 0, 1, code); // emit an = sym
                }
                if (token == neqsym)
                {
                        getCodeGenToken(list);
                        expression(level, 1, list, table, code);
                        emit(18, 0, 0, 1, code); // emit an <> sym
                }
                if (token == lessym)
                {
                        getCodeGenToken(list);
                        expression(level, 1, list, table, code);
                        emit(19, 0, 0, 1, code); // emit an < sym
                }
                if (token == leqsym)
                {
                        getCodeGenToken(list);
                        expression(level, 1, list, table, code);
                        emit(20, 0, 0, 1, code); // emit an <= sym
                }
                if (token == gtrsym)
                {
                        getCodeGenToken(list);
                        expression(level, 1, list, table, code);
                        emit(21, 0, 0, 1, code); // emit an > sym
                }
                if (token == geqsym)
                {
                        getCodeGenToken(list);
                        expression(level, 1, list, table, code);
                        emit(22, 0, 0, 1, code); // emit an >= sym
                }
        }
        return;
}

void expression(int level, int r, struct lexeme * list, struct symbol * table, struct instruction * code)
{
        if (token == plussym)
        {
                getCodeGenToken(list);
        }
        if (token == minussym)
        {
                getCodeGenToken(list);
                term(level, r, list, table, code);
                emit(10, r, 0, 0, code); // emit NEG
                while (token == plussym || token == minussym)
                {
                        if (token == plussym)
                        {
                                getCodeGenToken(list);
                                term(level, r + 1, list, table, code);
                                emit(11, r, r, r + 1, code); // add
                        }
                        if (token == minussym)
                        {
                                getCodeGenToken(list);
                                term(level, r + 1, list, table, code);
                                emit(12, r, r, r + 1, code); // subtract
                        }
                }
                return;
        }
        term(level, r, list, table, code);
        while (token == plussym || token == minussym)
        {
                if (token == plussym)
                {
                        getCodeGenToken(list);
                        term(level, r + 1, list, table, code);
                        emit(11, r, r, r + 1, code); // emit add
                }
                if (token == minussym)
                {
                        getCodeGenToken(list);

                        term(level, r + 1, list, table, code);
                        emit(12, r, r, r + 1, code); // emit sub
                }
        }
        return;
}


void term(int level, int r, struct lexeme * list, struct symbol * table, struct instruction * code)
{
        factor(level, r, list, table, code);
        while (token == multsym || token == slashsym)
        {
                if (token == multsym)
                {
                        getCodeGenToken(list);

                        factor(level, r + 1, list, table, code);
                        emit(13, r, r, r + 1, code); // emit mul
                }
                if (token == slashsym)
                {
                        getCodeGenToken(list);

                        factor(level, r + 1, list, table, code);
                        emit(14, r, r, r + 1, code); // emit div
                }
        }
        return;
}

void factor(int level, int r, struct lexeme * list, struct symbol * table, struct instruction * code)
{
        int t1, c1; // declaring symbol table index and code index vars
        if (token == identsym)
        {
                // save the symbol table index of var OR const entry unmarked with closest lex level
                t1 = findSymbolTableIndex2(level, 2, id, table);
                if (t1 < 0)
                {
                        t1 = findSymbolTableIndex2(level, 1, id, table);
                }
                if (table[t1].kind == 1) // const == 1
                {
                        emit(1, r, 0, table[t1].val, code); // emit LIT
                }
                if (table[t1].kind == 2) // var = 2
                {
                        emit(3, r, level - table[t1].level, table[t1].addr, code); // emit LOD
                }
                getCodeGenToken(list);
        }
        else if (token == numbersym)
        {
                // since getCodeToken advances our lex index automatically,
                // our number was stored in the previous index, thus we use
                // -1 to access it
                int num_val = list[lex_index - 1].num;
                emit(1, r, 0, num_val, code); // emit LIT
                getCodeGenToken(list);
        }
        else
        {
                getCodeGenToken(list);
                expression(level, r, list, table, code);
                getCodeGenToken(list);
        }
        return;
}

int findSymbolTableIndex(int id, struct symbol * table, int kind)
{
        int index = -1;
        for (int i = MAX_SYMBOL_TABLE_SIZE; i >= 0; i--)
        {
                if (id == table[i].val && table[i].kind == kind)
                {
                        index = i;
                        break;
                }
        }
        return index;
}

// finds index with CLOSEST lex level and unmarked
int findSymbolTableIndex2(int level, int kind, char * id, struct symbol * table)
{
        int index = -1;
        int closeness = INT_MAX;
        int difference = -1;
        for (int i = MAX_SYMBOL_TABLE_SIZE; i >= 0; i--)
        {
                if (strcmp(id, table[i].name) == 0 && table[i].kind == kind && table[i].mark == 0)
                {
                        difference = table[i].level - level;
                        if (difference < 0)
                        {
                                difference = difference * -1;
                        }
                        if (difference < closeness)
                        {
                                index = i;
                                closeness = difference;
                        }
                }
        }
        return index;

}

void unmarkVal(char * id, struct symbol * table, int kind, int level)
{
        for (int i = MAX_SYMBOL_TABLE_SIZE; i >= 0; i--)
        {
                if ((strcmp(id, table[i].name) == 0) && kind == table[i].kind && table[i].level == level)
                {
                        table[i].mark = 0;
                        return;
                }
        }
        return;
}
int findSymbolTableIndexWithVal(int val, int kind, struct symbol * table)
{
        int index = -1;
        for (int i = MAX_SYMBOL_TABLE_SIZE; i >= 0; i--)
        {
                if (val == table[i].val && kind == table[i].kind)
                {
                        index = i;
                        break;
                }
        }
        return index;
}

void getCodeGenToken(struct lexeme * list)
{
        token = list[lex_index].id;
        if (token == 2)
        {
                strcpy(id, list[lex_index].name);
        }
        else if (token == 3)
        {
                num = list[lex_index].num;
        }
        lex_index++;
        return;
}

void emit(int op, int r, int l, int m, struct instruction * code)
{
        if (cx > CODE_SIZE)
        {
                // error too much code;
                exit(1);
        }
        else
        {
                code[cx].opcode = op;
                code[cx].r = r;
                code[cx].l = l;
                code[cx].m = m;
                cx++;
        }
        // increment the number of instructions
        size_instructions++;
        return;
}

void printCodeArray(struct instruction * code)
{
        FILE * output_file = fopen("codegenoutput.txt", "w");
        fprintf(output_file, "No errors detected! This program is syntactically correct.\n");
        for (int i = 0; i < size_instructions; i++)
        {
                if(code[i].opcode == 1)
                {
                        strcpy(word, "LIT");
                }
                else if(code[i].opcode == 2)
                {
                        strcpy(word, "RTN");
                }
                else if(code[i].opcode == 3)
                {
                        strcpy(word, "LOD");
                }
                else if(code[i].opcode == 4)
                {
                        strcpy(word, "STO");
                }
                else if(code[i].opcode == 5)
                {
                        strcpy(word, "CAL");
                }
                else if(code[i].opcode == 6)
                {
                        strcpy(word, "INC");
                }
                else if(code[i].opcode == 7)
                {
                        strcpy(word, "JMP");
                }
                else if(code[i].opcode == 8)
                {
                        strcpy(word, "JPC");
                }
                else if(code[i].opcode == 9)
                {
                        strcpy(word, "SYS");
                }
                else if(code[i].opcode == 10)
                {
                        strcpy(word, "NEG");
                }
                else if(code[i].opcode == 11)
                {
                        strcpy(word, "ADD");
                }
                else if(code[i].opcode == 12)
                {
                        strcpy(word, "SUB");
                }
                else if(code[i].opcode == 13)
                {
                        strcpy(word, "MUL");
                }
                else if(code[i].opcode == 14)
                {
                        strcpy(word, "DIV");
                }
                else if(code[i].opcode == 15)
                {
                        strcpy(word, "ODD");
                }
                else if(code[i].opcode == 16)
                {
                        strcpy(word, "MOD");
                }
                else if(code[i].opcode == 17)
                {
                        strcpy(word, "EQL");
                }
                else if(code[i].opcode == 18)
                {
                        strcpy(word, "NEQ");
                }
                else if(code[i].opcode == 19)
                {
                        strcpy(word, "LSS");
                }
                else if(code[i].opcode == 20)
                {
                        strcpy(word, "LEQ");
                }
                else if(code[i].opcode == 21)
                {
                        strcpy(word, "GTR");
                }
                else if(code[i].opcode == 22)
                {
                        strcpy(word, "GEQ");
                }

                fprintf(output_file, "%d\t%s\t%d\t%d\t%d\n", i, word, code[i].r, code[i].l, code[i].m);
        }
        fclose(output_file);
        return;
}
