#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"


// defining global variables that will be used throughout
int list_index = 0;
int table_index = 0;
int token;
char id [12];
int num;
int numErrors = 0;

// initializing the functions that will be used throughout this program
struct symbol * parse(struct lexeme * list);
void program(struct lexeme * list, struct symbol * table);
void getToken(struct lexeme * list);
void pblock(int level, struct lexeme * list, struct symbol * table);
int const_dec(int level, struct lexeme * list, struct symbol * table);
int var_dec(int level, struct lexeme * list, struct symbol * table);
int procedure_dec(int level, struct lexeme * list, struct symbol * table);
void pstatement(int level, struct lexeme * list, struct symbol * table);
void pcondition(int level, struct lexeme * list, struct symbol * table);
void pexpression(int level, struct lexeme * list, struct symbol * table);
void pterm(int level, struct lexeme * list, struct symbol * table);
void pfactor(int level, struct lexeme * list, struct symbol * table);
void insertTable(struct symbol * table, int tx, int kind, char * name, int val, int level, int addr, int mark);
int checkIfNamePresent(int level, struct symbol * table, char * name);
void error(int error_number);
int checkIfExists(int level, int kind, struct symbol * table, char * name);


struct symbol * parse(struct lexeme * list)
{
        // allocating our symbol table
        struct symbol * table = malloc(500 * sizeof(struct symbol));
        for (int i = 0; i < 500; i++)
        {
          table[i].mark = -1; // represents uninitialized
        }

        program(list, table);
        return table;
}


void program(struct lexeme * list, struct symbol * table)
{
        getToken(list);
        // adding the main procedure to the symbol table
        insertTable(table, table_index, 3, "main", 0, 0, 0, 0);
        // block function
        pblock(0, list, table);
        if (token != periodsym)
        {
                error(9);
        }
        return;
}


void pblock(int level, struct lexeme * list, struct symbol * table)
{
        int numSymbols = 0;
        numSymbols += const_dec(level, list, table);
        numSymbols += var_dec(level, list, table);
        numSymbols += procedure_dec(level, list, table);
        pstatement(level, list, table);
        // go through the symbol table numSymbols of times and mark the last
        // numSymbols num. of symbols

        for (int i = table_index - 1; numSymbols >= 0; i--)
        {
                if (table[i].mark != 1)
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


int const_dec(int level, struct lexeme * list, struct symbol * table)
{
        int numConst = 0;
        if(token == constsym)
        {
                do
                {
                        getToken(list);
                        if (token != identsym)
                        {
                                error(4); // constant must be followed by indentifer
                        }
                        if(checkIfNamePresent(level, table, id) == 1)
                        {
                                error(26); // cannot assign another value to const
                        }
                        // grab the next token
                        getToken(list);
                        if (token != eqsym)
                        {
                                error(3); // identifier must be followed by an equals sign
                        }
                        getToken(list);
                        if (token != numbersym)
                        {
                                error(2); // = must be followed by a number
                        }
                        // no errors encountered, save to symbol table
                        insertTable(table, table_index, 1, id, num, level, 0, 0);
                        numConst++;
                        getToken(list);
                } while (token == commasym);

                if (token != semicolonsym)
                {
                        error(17);
                }
                getToken(list);
                return numConst;
        }
        return 0;
}


int var_dec(int level, struct lexeme * list, struct symbol *table)
{
        int numVars = 0;
        if (token == varsym)
        {
                do
                {
                        numVars++;
                        getToken(list);
                        if (token != identsym)
                        {
                                error(11);
                        }
                        if (checkIfNamePresent(level, table, id) == 1)
                        {
                                error(26); // name present in table
                        }
                        insertTable(table, table_index, 2, id, 0, level, numVars + 2, 0); // num vars + size of ar (index 0)
                        getToken(list);

                } while (token == commasym);
                // if the next token after this loop isn't a semicolon, error
                if (token != semicolonsym)
                {
                        error(17); // missing semicolon
                }
                getToken(list);
                return numVars;
        }
        return 0;
}

int procedure_dec(int level, struct lexeme * list, struct symbol * table)
{
        int numProcedures = 0;
        if (token == procsym)
        {
                do
                {
                        getToken(list);
                        if (token != identsym)
                        {
                                error(11);
                        }
                        if (checkIfNamePresent(level, table, id) == 1)
                        {
                                error(26); // name present in table
                        }
                        insertTable(table, table_index, 3, id, 0, level, 0, 0);
                        numProcedures++;
                        getToken(list);
                        if (token != semicolonsym)
                        {
                                error(5);
                        }
                        getToken(list);
                        pblock(level + 1, list, table); // go one level deeper
                        if (token != semicolonsym)
                        {
                                error(5);
                        }
                        getToken(list);
                } while (token == procsym);
                return numProcedures;
        }
        return numProcedures;
}

void pstatement(int level, struct lexeme * list, struct symbol * table)
{
        if (token == identsym)
        {
                // search symbol table working backwards to find latest unmarked var OR const with desired name
                // const = 1, var = 2, proc = 3
                if ((checkIfExists(level, 2, table, id) == 0) && (checkIfExists(level, 1, table, id) == 0))
                {
                        error(19); /// --- FIX
                }
                getToken(list);
                if (token != becomessym) // token != ':=' <-- find symbol for this
                {
                        error(1);
                }
                getToken(list);
                pexpression(level, list, table);
                return;
        }
        if (token == callsym)
        {
                getToken(list);
                // search symbol table for latest unmarked procedure with this name
                // if you cant find one, error
                if (checkIfExists(level, 3, table, id) == 0)
                {
                        error(19); /// --- FIX
                }
                getToken(list);
                return;
        }
        if (token == beginsym)
        {
                getToken(list);
                pstatement(level, list, table);

                while (token == semicolonsym)
                {

                        getToken(list);
                        pstatement(level, list, table);

                }
                // SHOULD BE AT endsym
                if (token != endsym)
                {
                        error(24); // end symbol missing
                }
                getToken(list);
                return;
        }
        if (token == ifsym)
        {
                getToken(list);

                pcondition(level, list, table);
                if (token != thensym)
                {
                        error(16); // then expected
                }
                getToken(list);
                pstatement(level, list, table);
                return;
        }
        if (token == elsesym)
        {
                getToken(list);
                pstatement(level, list, table);
                return;
        }
        if (token == whilesym)
        {
                getToken(list);
                pcondition(level, list, table);

                if (token != dosym)
                {
                        error(18); // do symbol expected
                }
                getToken(list);
                pstatement(level, list, table);
                return;
        }
        if (token == readsym)
        {
                getToken(list);

                // search symbol table working backwards to find the
                // latest unmarked var with the desired name, if you cant find,
                // then error
                if (checkIfExists(level, 2, table, id) == 0)
                {
                        error(19); /// --- FIX
                }
                getToken(list);
                return;
        }
        if (token == writesym)
        {
                getToken(list);
                pexpression(level, list, table);
                return;
        }
        return;
}


void pcondition(int level, struct lexeme * list, struct symbol * table)
{
        if (token == oddsym)
        {
                getToken(list);
                pexpression(level, list, table);
        }
        else
        {
                pexpression(level, list, table);
                if (token != eqsym && token != neqsym && token != lessym && token != leqsym && token != gtrsym && token != geqsym)
                {
                        error(20); // relational operation expected
                }
                // grab next token
                getToken(list);
                pexpression(level, list, table);
        }
        return;
}


void pexpression(int level, struct lexeme * list, struct symbol * table)
{
        if (token == plussym || token == minussym)
        {
                getToken(list);
        }
        pterm(level, list, table);
        while (token == plussym || token == minussym)
        {
                getToken(list);
                pterm(level, list, table);
        }
        return;
}


void pterm(int level, struct lexeme * list, struct symbol * table)
{
        pfactor(level, list, table);
        while (token == multsym || token == slashsym)
        {
                getToken(list);
                pfactor(level, list, table);
        }
        return;
}


void pfactor(int level, struct lexeme * list, struct symbol * table)
{
        if (token == identsym)
        {
                // search through symbol table working backwards to find the latest
                // unmarked var OR const with the desired name, if you can't find either, then error
                if (checkIfExists(level, 2, table, id) == 0 && checkIfExists(level, 1, table, id) == 0)
                {
                        error(19);
                }
                getToken(list);

        }
        else if (token == numbersym)
        {
                getToken(list);
        }
        else if (token == lparentsym)
        {
                // grab next token
                getToken(list);
                pexpression(level, list, table);
                if (token != rparentsym)
                {
                        error(22); // right parenthesis expected
                }
                // grab next token
                getToken(list);

        }
        else
        {
                error(23); // preceding factor cannot be followed by this symbol
        }
        return;

}

void insertTable(struct symbol * table, int tx, int kind, char * name, int val, int level, int addr, int mark)
{
        table[tx].kind = kind;
        strcpy(table[tx].name, name);
        table[tx].val = val;
        table[tx].level = level;
        table[tx].addr = addr;
        table[tx].mark = mark; // unmarked

        // increase the table index for next data entry
        table_index++;
        return;
}


int checkIfNamePresent(int level, struct symbol * table, char * name)
{
        for (int i = table_index; i >= 0; i--)
        {
                if (strcmp(table[i].name, name) == 0 && table[i].mark == 0 && table[i].level == level)
                {
                        return 1;
                }
        }
        return 0;
}

int checkIfExists(int level, int kind, struct symbol * table, char * name)
{
        for (int i = 500; i >= 0; i--)
        {
                if (strcmp(table[i].name, id) == 0 && table[i].mark == 0 && table[i].kind == kind)
                {
                        return 1;
                }

        }
        return 0;
}

void getToken(struct lexeme * list)
{
        token = list[list_index].id;
        if (token == 2)
        {
                strcpy(id, list[list_index].name);
        }
        else if (token == 3)
        {
                num = list[list_index].num;
        }
        list_index++;
        return;
}


void error(int error_number)
{
        FILE * output_file = fopen("codegenoutput.txt", "w");
        numErrors++;
        switch (error_number)
        {
        case 1:
                fprintf(output_file, "\n1. Use = instead of :=.\n");
                printf("\n1. Use = instead of :=.\n");
                break;
        case 2:
                fprintf(output_file, "\n2. = must be followed by a number.\n");
                printf("\n2. = must be followed by a number.\n");
                break;
        case 3:
                fprintf(output_file, "\n3. Identifier must be followed by =.\n");
                printf("\n3. Identifier must be followed by =.\n");
                break;
        case 4:
                fprintf(output_file, "\n4. const, var, procedure must be followed by identifier.\n");
                printf("\n4. const, var, procedure must be followed by identifier.\n");
                break;
        case 5:
                fprintf(output_file, "\n5. Semicolon or comma missing.\n");
                printf("\n5. Semicolon or comma missing.\n");
                break;
        case 6:
                fprintf(output_file, "\n6. Incorrect symbol after procedure declaration.\n");
                printf("\n6. Incorrect symbol after procedure declaration.\n");
                break;
        case 7:
                fprintf(output_file, "\n7. Statement expected.\n");
                printf("\n7. Statement expected.\n");
                break;
        case 8:
                fprintf(output_file, "\n8. Incorrect symbol after statement part in block.\n");
                printf("\n8. Incorrect symbol after statement part in block.\n");
                break;
        case 9:
                fprintf(output_file, "\n9. Period expected.\n");
                printf("\n9. Period expected.\n");
                break;
        case 10:
                fprintf(output_file, "\n10. Semicolon between statements missing.\n");
                printf("\n10. Semicolon between statements missing.\n");
                break;
        case 11:
                fprintf(output_file, "\n11. Undeclared identifier.\n");
                printf("\n11. Undeclared identifier.\n");
                break;
        case 12:
                fprintf(output_file, "\n12. Assignment to constant or procedure is not allowed.\n");
                printf("\n12. Assignment to constant or procedure is not allowed.\n");
                break;
        case 13:
                fprintf(output_file, "\n13. Assignment operator expected.\n");
                printf("\n13. Assignment operator expected.\n");
                break;
        case 14:
                fprintf(output_file, "\n14. call must be followed by an identifier.\n");
                printf("\n14. call must be followed by an identifier.\n");
                break;
        case 15:
                fprintf(output_file, "\n15. Call of a constant or variable is meaningless.\n");
                printf("\n15. Call of a constant or variable is meaningless.\n");
                break;
        case 16:
                fprintf(output_file, "\n16. then expected.\n");
                printf("\n16. then expected.\n");
                break;
        case 17:
                fprintf(output_file, "\n17. Semicolon or } expected.\n");
                printf("\n17. Semicolon or } expected.\n");
                break;
        case 18:
                fprintf(output_file, "\n18. do expected.\n");
                printf("\n18. do expected.\n");
                break;
        case 19:
                fprintf(output_file, "\n19. Incorrect symbol following statement.\n");
                printf("\n19. Incorrect symbol following statement.\n");
                break;
        case 20:
                fprintf(output_file, "\n20. Relational operator expected.\n");
                printf("\n20. Relational operator expected.\n");
                break;
        case 21:
                fprintf(output_file, "\n21. Expression must not contain a procedure identifier.\n");
                printf("\n21. Expression must not contain a procedure identifier.\n");
                break;
        case 22:
                fprintf(output_file, "\n22. Right parenthesis missing.\n");
                printf("\n22. Right parenthesis missing.\n");
                break;
        case 23:
                fprintf(output_file, "\n23. The preceding factor cannot begin with this symbol.\n");
                printf("\n23. The preceding factor cannot begin with this symbol.\n");
                break;
        case 24:
                fprintf(output_file, "\n24. End symbol Expected.\n");
                printf("\n24. End symbol Expected.\n");
                break;
        case 25:
                fprintf(output_file, "\n25. This number is too large.\n");
                printf("\n25. This number is too large.\n");
                break;
        case 26:
                fprintf(output_file, "\n26. Cannot reassign constant.\n");
                printf("\n26. Cannot reassign constant.\n");
                break;
        }
        fprintf(output_file, "Compilation process terminating...\n");
        fclose(output_file);
        exit(1);
}
