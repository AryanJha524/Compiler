#ifndef __PARSER_H
#define __PARSER_H
#include "lex.h"

struct symbol
{
        int kind;
        char name[12];
        int val;
        int level;
        int addr;
        int mark;
};

struct symbol * parse(struct lexeme * list);
extern int numErrors;
#endif
