#ifndef __LEX_H
#define __LEX_H

typedef enum {
        nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
        gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym,
        whilesym, dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_identifier;

struct lexeme {
        token_identifier id;
        char name[12];
        int num;
};

struct lexeme * lex_analyze(char * inputfile);

#endif
