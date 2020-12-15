#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"
#include "virtualmachine.h"

int main(int argc, char** argv)
{

        int lexflag = 0;
        int codegenflag = 0;
        int vmflag = 0;
        // save the args to a string array
        char ** flagcheck = argv;
        if (argc < 2)
        {
                printf("error: please include file name");
                return 0;
        }
        FILE * ifp = fopen(argv[1], "r");
        if (ifp == NULL)
        {
                printf("Error, could not open the file..\n");
                exit(1);
        }
        // allocate memory for the input file as a string
        char * inputfile = malloc(500 * sizeof(char));
        char c = fgetc(ifp);
        int i = 0;
        // store the code in the input file as a char array / string until EOF reached
        while (1)
        {
                inputfile[i++] = c;
                c = fgetc(ifp);
                if (c == EOF)
                {
                        break;
                }

        }
        // set the last character as a the null terminator for the string
        inputfile[i] = '\0';
        printf("Source code: \n");
        for (int x = 0; x < i; x++)
        {
                printf("%c", inputfile[x]);
        }
        // since we processed the inputfile, the number of args is reduced by one
        argc--;
        // increase the arg we are looking at, since we processed the inputfile
        flagcheck++;
        // check for flags, argc represents the number of arguments found
        // loops through all the args and compares the flags provided at each



        while (argc > 1)
        {
                if (strcmp(flagcheck[1], "-l") == 0)
                {
                        lexflag = 1;
                }
                if (strcmp(flagcheck[1], "-a") == 0)
                {
                        codegenflag = 1;
                }
                if (strcmp(flagcheck[1], "-v") == 0)
                {
                        vmflag = 1;
                }
                argc--;
                flagcheck++;
        }

        // compilation process
        struct lexeme * list = lex_analyze(argv[1]);
        struct symbol * table = parse(list);
        struct instruction * code = generate_code(table, list);
        printf("\n");
        // printing output (depending on flags)
        int character_reader;
        if (lexflag == 1)
        {
                // print lexoutput.txt
                FILE * lexoutptr = fopen("lexoutput.txt", "r");
                if (lexoutptr == NULL)
                {
                        printf("error couldn't open the file.. terminating\n");
                        return 0;
                }
                while ((character_reader = fgetc(lexoutptr)) != EOF)
                {
                        printf("%c", character_reader);
                }
                printf("\n");
                fclose(lexoutptr);
        }
        printf("\n");
        if (codegenflag == 1)
        {
                // print codegenoutput.txt of code generation

                FILE * codegenptr = fopen("codegenoutput.txt", "r");
                if (codegenptr == NULL)
                {
                        printf("error couldn't open the file.. terminating\n");
                        return 0;
                }
                while ((character_reader = fgetc(codegenptr)) != EOF)
                {
                        printf("%c", character_reader);
                }
                printf("\n");
                fclose(codegenptr);
        }
        printf("\n");
        virtualmachine(code, vmflag);

        fclose(ifp);
        free(inputfile);
        free(list);
        free(table);
        free(code);
        return 0;
}
