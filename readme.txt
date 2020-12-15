This program was written by Aryan Jha in Fall 2020 for Systems Software.
Please read all the sections before running this program!

HOW TO RUN THIS COMPILER
-----------------------
1. Download all the .c and .h files into the same directory and navigate to that directory.
2. Compile all the .c files with the command ==> "gcc driver.c lex.c parser.c codegen.c virtualmachine.c".
3. After compilation, to execute run the command ==> "./a.out <input_file_name.txt> <flags>". (BUT WAIT, read the steps below before you do this!)
  3a. <input_file_name.txt> is a REQUIRED field! Please provide the name to the input file here. (Note: the input file must be a .txt extension).
      If the file is in the directory with all the .c and .h files, you don't have to provide the complete path, just the name. If the file sits in
      another directory, please provide the full path to that file
  3b. <flags> are an optional field. The flags are "-l", "-a", "-v". These can be entered in any order, and you can pick and choose to enter all, some,
      or none of them. "-l" will print the lexeme table and list. "-a" will display the code generation. "-v" will print the virtual machine stack trace
      to the terminal.
4. That's it! (Side note: the program will always print the source code from the input file and whether or not the program is syntactically correct,
   regardless of the flags).


ERROR NOTES
------------
1. If the PL/0 code provided is syntactically incorrect, the compilation process will terminate, and the error will be printed to the screen.
2. Errors can also occur the lexical analyzer portion, if an unknown symbol is encountered.
3. Inside the errorFiles folder, all files with the title ErrorXInput.txt correspond to various errors that will be produced if the source code from those files
   are run through this compiler. Their outputs will be printed to the terminal, but also can be found in ErrorXoutput.txt

OUTPUT NOTES
------------
1. The input1.txt is the example source code provided. It's code generation output and vm stack trace output are in
   codegenoutput.txt and vmoutput.txt respectively.
2. The different .txt files created by these programs represent the output from the different parts of the compiler.
  2a. lexoutput.txt contains the lexeme list and lexeme table
  2b. codegenoutput.txt contains whether or not the program is syntactically correct (if no, then what error caused it to fail),
      and contains the code generated from codegen.c
  2c. vmoutput.txt contains the virtual machine stack trace.
