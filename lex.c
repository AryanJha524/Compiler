// This program was written by Aryan Jha
// COP 3402, Fall 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lex.h"

struct token {
        char name[12];
        int num;
        int is_num;
        token_identifier id;
};


struct lexeme * lex_analyze(char * inputfile)
{
        // creating an array of tokens to be stored in the lexeme list
        struct lexeme * lexemeList = malloc(5000 * sizeof(struct lexeme));
        struct token * lexemeTable = malloc(5000 * sizeof(struct token));
        int lexemeList_index = 0;
        for (int i = 0; i < 5000; i++)
        {
                // initialize the array
                lexemeList[i].id = 0;
        }

        // setting arrays that contain reserved words
        const char* reservedWords[]={"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};

        // declaring arrays that contain special symbols
        const char specialSymbols[]={'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>',  ';', ':'};

        // declaring the input file pointer
        FILE * input_file = fopen(inputfile, "r");
        if (input_file == NULL)
        {
                printf("Error, file could not be opened.. please check file name.\n");
                return 0;
        }
        // declaring the output file pointer
        FILE * output_file = fopen("lexoutput.txt", "w");

        // declaring variables to be used during scanning in of input file
        int character_reader;
        int lookAhead = 0;
        int error_count = 0;
        int comment_checker = 0;
        int non_alpha_num = 0;

        character_reader = fgetc(input_file);

        // this is the main loop which reads until the EOF is encountered
        while (character_reader != EOF)
        {
                // check for any whitespaces and ignore them
                if(character_reader == ' ' || character_reader == '\n' || character_reader == '\t' || character_reader == '\r')
                {
                        character_reader=fgetc(input_file);
                        lookAhead=0;
                        continue;
                }

                // case where the token starts with a character, which can either be
                // a reserved word or a variable.
                if (isalpha(character_reader))
                {
                        // create an array to store identifier (max length is 11, but account for null terminator)
                        char alpha_string[12];
                        // copies the first occurrence of character into the string we need
                        memset(alpha_string, 0, sizeof(alpha_string));

                        int index=0;
                        alpha_string[index]=character_reader;

                        index++;
                        lookAhead=1;
                        // check if identifier length is too long, loop runs until non character or non numeric symbol is encountered
                        while(isalpha(character_reader=fgetc(input_file)) || isdigit(character_reader))
                        {
                                if(index + 1 > 11)
                                {
                                        printf("3) Error: Name too long: %s\n", alpha_string);
                                        while (isalpha(character_reader = fgetc(input_file)) || isdigit(character_reader))
                                        {
                                                // character_reader keeps incrementing until we escape this identifier which is too long
                                        }
                                        error_count = 1;
                                        //lookAhead=0; // check if u can delete this
                                        break;
                                }
                                // continue building this identifier
                                alpha_string[index]=character_reader;
                                index++;
                        }

                        // if we have an error, continue onto the next token
                        if (error_count == 1)
                        {
                                error_count = 0;
                                continue;
                        }

                        // loop to check if the identifier we just read in is a reserved word or not
                        int reserved_word = -1;
                        for (int i = 0; i < 14; i++)
                        {
                                if (strcmp(alpha_string, reservedWords[i]) == 0)
                                {
                                        reserved_word = i;
                                }
                        }

                        switch (reserved_word)
                        {
                        case 0:
                                lexemeList[lexemeList_index].id = constsym;
                                strcpy(lexemeTable[lexemeList_index].name, "const");
                                lexemeTable[lexemeList_index].id = constsym;
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 1:
                                lexemeList[lexemeList_index].id = varsym;
                                lexemeTable[lexemeList_index].id = varsym;
                                strcpy(lexemeTable[lexemeList_index].name, "var");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 2:
                                lexemeList[lexemeList_index].id = procsym;
                                lexemeTable[lexemeList_index].id = procsym;
                                strcpy(lexemeTable[lexemeList_index].name, "procedure");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 3:
                                lexemeList[lexemeList_index].id = callsym;
                                lexemeTable[lexemeList_index].id = callsym;
                                strcpy(lexemeTable[lexemeList_index].name, "call");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 4:
                                lexemeList[lexemeList_index].id = beginsym;
                                lexemeTable[lexemeList_index].id = beginsym;
                                strcpy(lexemeTable[lexemeList_index].name, "begin");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 5:
                                lexemeList[lexemeList_index].id = endsym;
                                lexemeTable[lexemeList_index].id = endsym;
                                strcpy(lexemeTable[lexemeList_index].name, "end");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 6:
                                lexemeList[lexemeList_index].id = ifsym;
                                lexemeTable[lexemeList_index].id = ifsym;
                                strcpy(lexemeTable[lexemeList_index].name, "if");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 7:
                                lexemeList[lexemeList_index].id = thensym;
                                lexemeTable[lexemeList_index].id = thensym;
                                strcpy(lexemeTable[lexemeList_index].name, "then");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 8:
                                lexemeList[lexemeList_index].id = elsesym;
                                lexemeTable[lexemeList_index].id = elsesym;
                                strcpy(lexemeTable[lexemeList_index].name, "else");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 9:
                                lexemeList[lexemeList_index].id = whilesym;
                                lexemeTable[lexemeList_index].id = whilesym;
                                strcpy(lexemeTable[lexemeList_index].name, "while");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 10:
                                lexemeList[lexemeList_index].id = dosym;
                                lexemeTable[lexemeList_index].id = dosym;
                                strcpy(lexemeTable[lexemeList_index].name, "do");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 11:
                                lexemeList[lexemeList_index].id = readsym;
                                lexemeTable[lexemeList_index].id = readsym;
                                strcpy(lexemeTable[lexemeList_index].name, "read");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 12:
                                lexemeList[lexemeList_index].id = writesym;
                                lexemeTable[lexemeList_index].id = writesym;
                                strcpy(lexemeTable[lexemeList_index].name, "write");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        case 13:
                                lexemeList[lexemeList_index].id = oddsym;
                                lexemeTable[lexemeList_index].id = oddsym;
                                strcpy(lexemeTable[lexemeList_index].name, "odd");
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;

                        default:
                                // no matches, so default to store the string we read in as an identifier
                                lexemeList[lexemeList_index].id = identsym;
                                strcpy(lexemeList[lexemeList_index].name, alpha_string);
                                lexemeTable[lexemeList_index].id = identsym;
                                strcpy(lexemeTable[lexemeList_index].name, alpha_string);
                                lexemeTable[lexemeList_index].is_num = 0;
                                break;
                        }
                        lexemeList_index++;
                        // end of if lexical unit started with an character
                }
                // case for if the next character is a digit
                else if (isdigit(character_reader))
                {
                        // convert the character into the integer represenation
                        int number = character_reader - '0';
                        int length = 0;
                        int digit;


                        int lookAhead = 1;
                        while (isdigit(character_reader = fgetc(input_file)))
                        {
                                if(length>4)
                                {
                                        printf("2) Error: Number too long: %d\n", number);
                                        while (isdigit(character_reader = fgetc(input_file)))
                                        {
                                                // encountered a error, loop until character_reader breaks out of this number
                                        }
                                        error_count = 1;
                                        break;
                                }
                                digit = character_reader-'0';
                                // keep increasing the numeric value of the number we are reading in
                                number = 10 * number + digit;
                                length++;
                        }

                        // error, we have encountered an identifier which starts with a number
                        if (isalpha(character_reader))
                        {
                                printf("1) Error: Variable does not start with letter: %d%c\n", number, character_reader);
                                while (isdigit(character_reader) || isalpha(character_reader = fgetc(input_file)))
                                {
                                        // loop until character reader escapes this invalid indentifier
                                }
                                // continue to next lexical unit
                                continue;
                        }
                        // we've encountered a potential special symbol
                        else if (!isalpha(character_reader) && !isdigit(character_reader))
                        {
                                // case where there is no space between number and next symbol
                                lookAhead = 0;
                                non_alpha_num = 1;
                        }
                        // if we encountered a number too long, continue with out reading the token
                        if (error_count == 1)
                        {
                                error_count = 0;
                                continue;
                        }
                        // if no errors, store this token in the lexeme list
                        lexemeList[lexemeList_index].id = numbersym;
                        lexemeList[lexemeList_index].num = number;
                        lexemeTable[lexemeList_index].id = numbersym;
                        lexemeTable[lexemeList_index].is_num = 1;
                        lexemeTable[lexemeList_index].num = number;
                        lexemeList_index++;
                }
                // final case, handle any special symbols
                else
                {
                        if (non_alpha_num == 1)
                        {
                                non_alpha_num = 0;
                        }
                        lookAhead=0;
                        int special_symbol = -1;
                        for (int i = 0; i < 13; i++)
                        {
                                // if the character reader is equal to a specific special symbol
                                // then save that index
                                if (specialSymbols[i] == character_reader)
                                {
                                        special_symbol = i;
                                }
                        }

                        switch (special_symbol)
                        {
                        case 0:
                                lexemeList[lexemeList_index].id = plussym;
                                strcpy(lexemeTable[lexemeList_index].name, "+");
                                lexemeTable[lexemeList_index].id = plussym;
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 1:
                                lexemeList[lexemeList_index].id = minussym;
                                lexemeTable[lexemeList_index].id = minussym;
                                strcpy(lexemeTable[lexemeList_index].name, "-");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 2:
                                lexemeList[lexemeList_index].id = multsym;
                                lexemeTable[lexemeList_index].id = multsym;
                                strcpy(lexemeTable[lexemeList_index].name, "*");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 3:
                                // encountered the start of a comment, we don't want to tokenize
                                // anything within this comment
                                character_reader=fgetc(input_file);
                                lookAhead=1;
                                if(character_reader == '*')
                                {
                                        comment_checker = 1;
                                        lookAhead = 0;
                                        character_reader = fgetc(input_file);
                                        // keep reading values within this comment until end of comment is reached
                                        while(comment_checker == 1)
                                        {
                                                // if this symbol is encountered, the next character must be a
                                                // '/' to symbolize the end of the comment
                                                if(character_reader =='*')
                                                {
                                                        character_reader = fgetc(input_file);
                                                        // encountered the end of this comment, we can break
                                                        if(character_reader =='/')
                                                        {
                                                                comment_checker = 0;
                                                        }
                                                }
                                                else
                                                // keep reading in values for this comment
                                                {
                                                        character_reader = fgetc(input_file);
                                                }
                                        }
                                }
                                // else just a slash was encountered
                                else
                                {
                                        lexemeList[lexemeList_index].id = slashsym;
                                        lexemeList_index++;
                                }
                                break;
                        case 4:
                                lexemeList[lexemeList_index].id = lparentsym;
                                lexemeTable[lexemeList_index].id = lparentsym;
                                strcpy(lexemeTable[lexemeList_index].name, "(");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 5:
                                lexemeList[lexemeList_index].id = rparentsym;
                                lexemeTable[lexemeList_index].id = rparentsym;
                                strcpy(lexemeTable[lexemeList_index].name, ")");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 6:
                                lexemeList[lexemeList_index].id = eqsym;
                                lexemeTable[lexemeList_index].id = eqsym;
                                strcpy(lexemeTable[lexemeList_index].name, "=");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 7:
                                lexemeList[lexemeList_index].id = commasym;
                                lexemeTable[lexemeList_index].id = commasym;
                                strcpy(lexemeTable[lexemeList_index].name, ",");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 8:
                                lexemeList[lexemeList_index].id = periodsym;
                                lexemeTable[lexemeList_index].id = periodsym;
                                strcpy(lexemeTable[lexemeList_index].name, ".");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 9:
                                character_reader = fgetc(input_file);
                                lookAhead=1;
                                if(character_reader == '>')
                                {
                                        lexemeList[lexemeList_index].id = neqsym;
                                        lexemeTable[lexemeList_index].id = neqsym;
                                        strcpy(lexemeTable[lexemeList_index].name, "<>");
                                        lexemeTable[lexemeList_index].is_num = 0;
                                        lookAhead=0;
                                }
                                else if(character_reader == '=')
                                {
                                        lexemeList[lexemeList_index].id = leqsym;
                                        lexemeTable[lexemeList_index].id = leqsym;
                                        strcpy(lexemeTable[lexemeList_index].name, "<=");
                                        lexemeTable[lexemeList_index].is_num = 0;
                                        lookAhead=0;
                                }
                                else
                                {
                                        lexemeList[lexemeList_index].id = lessym;
                                        lexemeTable[lexemeList_index].id = lessym;
                                        strcpy(lexemeTable[lexemeList_index].name, "<");
                                        lexemeTable[lexemeList_index].is_num = 0;
                                }
                                lexemeList_index++;
                                break;
                        case 10:
                                character_reader = fgetc(input_file);
                                lookAhead=1;
                                if(character_reader == '=')
                                {
                                        lexemeList[lexemeList_index].id = geqsym;
                                        lexemeTable[lexemeList_index].id = geqsym;
                                        strcpy(lexemeTable[lexemeList_index].name, ">=");
                                        lexemeTable[lexemeList_index].is_num = 0;
                                        lookAhead=0;
                                }
                                else
                                {
                                        lexemeList[lexemeList_index].id = gtrsym;
                                        lexemeTable[lexemeList_index].id = gtrsym;
                                        strcpy(lexemeTable[lexemeList_index].name, ">");
                                        lexemeTable[lexemeList_index].is_num = 0;
                                }
                                lexemeList_index++;
                                break;
                        case 11:
                                lexemeList[lexemeList_index].id = semicolonsym;
                                lexemeTable[lexemeList_index].id = semicolonsym;
                                strcpy(lexemeTable[lexemeList_index].name, ";");
                                lexemeTable[lexemeList_index].is_num = 0;
                                lexemeList_index++;
                                break;
                        case 12:
                                character_reader = fgetc(input_file);
                                if(character_reader == '=')
                                {
                                        lexemeList[lexemeList_index].id = becomessym;
                                        lexemeTable[lexemeList_index].id = becomessym;
                                        strcpy(lexemeTable[lexemeList_index].name, ":=");
                                        lexemeTable[lexemeList_index].is_num = 0;
                                        lexemeList_index++;
                                }
                                else
                                {
                                        printf("4) Error: Invalid symbols. : not followed by =\n");
                                }
                                break;
                        // if none of the results matched, just print an error
                        default:
                                printf("4) Error: Invalid symbols.\n");
                                break;
                        }
                }
                // read in next character as new lexical unit
                if (non_alpha_num != 1)
                {
                        if(lookAhead == 0)
                        {
                                character_reader = fgetc(input_file);
                        }
                }
        }
        // file is done being processed, now we can print out the lexeme table and the list

        fprintf(output_file, "Lexeme Table: \n");
        fprintf(output_file, "lexeme\t\t\ttoken type\n");
        for (int i = 0; i < lexemeList_index; i++)
        {
                if (lexemeTable[i].is_num == 0)
                {
                        fprintf(output_file, "%s\t\t\t\t%d\n", lexemeTable[i].name, lexemeTable[i].id);
                }
                else
                {
                        fprintf(output_file, "%d\t\t\t\t%d\n", lexemeTable[i].num, lexemeTable[i].id);
                }
        }
        fprintf(output_file, "\n");

        // printing the list
        fprintf(output_file, "Lexeme List: \n");
        fprintf(output_file, "%d", lexemeList[0].id);
        if(lexemeList[0].id == 2)
        {
                fprintf(output_file," %s", lexemeList[0].name);
        }
        // dealing with numbers, so print num value
        else if(lexemeList[0].id == 3) {
                fprintf(output_file," %d", lexemeList[0].num);
        }

        for(int i = 1; i < lexemeList_index; i++)
        {
                fprintf(output_file," %d", lexemeList[i].id);
                if(lexemeList[i].id == 2)
                {
                        fprintf(output_file," %s", lexemeList[i].name);
                }
                else if(lexemeList[i].id == 3)
                {
                        fprintf(output_file," %d",lexemeList[i].num);
                }
        }

        // close file pointers to prevent memory leaks
        fclose(output_file);
        fclose(input_file);
        free(lexemeTable);
        return lexemeList;
}
