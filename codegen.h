#ifndef __CODEGEN_H
#define __CODEGEN_H
#include "lex.h"
#include "parser.h"
struct instruction
{
        int opcode;
        int r;
        int l;
        int m;
};

extern int size_instructions;

struct instruction * generate_code(struct symbol *table, struct lexeme *list);

#endif
