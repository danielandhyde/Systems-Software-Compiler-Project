//Daniel Thew
//COP3402 Fall 22
//HW2 - Scanner

//libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "compiler.h"

//defined for universal lengths, such as error message (arbitrary length), characters, and digits
#define MAX_ERR 100
#define MAX_CHAR 12
#define MAX_DIG 6

lexeme* lex_analyze(int list_flag, char* input)
{

    int err_flag = 0;
    //open the file in read-only
    //FILE* fp;
    //fp = fopen(argv[1], "r");

    //lexeme array for each possible Reserved Word lexeme
    lexeme special_words[] = { {keyword_const, "const",  0}, { keyword_var,"var", 0}, {keyword_procedure, "procedure", 0}, {keyword_call, "call", 0},
    {keyword_begin, "begin", 0}, {keyword_end, "end", 0}, {keyword_if, "if", 0}, {keyword_then, "then", 0}, {keyword_else, "else", 0}, {keyword_while, "while", 0}, {keyword_do, "do", 0},
    {keyword_read, "read", 0}, {keyword_write, "write", 0}, {keyword_def, "def", 0} };

    //lexeme array for each possible Special Character lexeme
    lexeme special_char[] = { {period, ".", 0}, {assignment_symbol, ":=", 0}, {minus, "-", 0}, {semicolon, ";", 0}, {left_curly_brace, "{", 0}, {right_curly_brace, "}", 0},
    {equal_to, "==", 0}, {not_equal_to, "<>", 0}, {less_than, "<", 0}, {less_than_or_equal_to, "<=", 0}, {greater_than, ">", 0}, {greater_than_or_equal_to, ">=", 0},
    {plus, "+", 0}, {times, "*", 0}, {division, "/", 0}, {left_parenthesis, "(", 0}, {right_parenthesis, ")", 0} };

    //print for the start of the lexeme array
    if(list_flag == 1) printf("Lexeme List:\nlexeme\t\ttoken type\n");

    //array and a holder for its current index/length for future use
    //NOTE: THE ARRAY IS NOT DYNAMICALLY ALLOCATED. This was done to simplify the handling of memory. I don't realistically expect to see >50,000 lexemes, which explains
    //the seemingly arbitrary size of the array. Fundamentally, all operations are the same, save for the hard limit on lexemes.
    lexeme token_arr[50000];
    //lexeme* token_arr;
    int arr_ind = 0;

    //scan through file char by char to determine current lexeme
    //char cur_char = fgetc(fp);
    char cur_char = input[0];
    int c_index = 0;

    //while not at the end of file
    while (cur_char != '\0') {

        //reset the current lexeme and its index
        char cur_lex[MAX_CHAR] = "";
        int index = 0;

        //flag in case we eventually have to read another character after checking for a two-character Special Character lexeme
        int flag_for_special = 0;

        //scan through invisible spaces without tokenizing
        while (iscntrl(cur_char) || isspace(cur_char)) {
            //cur_char = fgetc(fp);
            c_index++;
            cur_char = input[c_index];
        }

        //scan through comments without tokenizing, then go to the next loop
        if (cur_char == '#') {
            while (cur_char != '\n' && cur_char != '\0') {
                //cur_char = fgetc(fp);
                c_index++;
                cur_char = input[c_index];
            }
            continue;
        }

        //immediately break if we end up at EOF after going through invisible spaces
        if (cur_char == '\0') break;

        //if the lexeme starts with a letter
        if (isalpha(cur_char)) {
            //for the duration of the lexeme
            while (isalpha(cur_char) || isdigit(cur_char)) {
                //add the char to the lexeme and get the next char; iterate the lexeme index
                cur_lex[index] = cur_char;
                //cur_char = fgetc(fp);
                c_index++;
                cur_char = input[c_index];
                index++;
            }
            
            //if the lexeme is too big
            if (index >= MAX_CHAR) {
                //create a lexeme to hold the error and add it to the token array
                lexeme temp = { 34,  "", 0 };
                err_flag = 1;
                token_arr[arr_ind] = temp;
                arr_ind++;
            }

            //if the lexeme is a reserved word "main" or "null
            else if (strcmp(cur_lex, "main") == 0 || strcmp(cur_lex, "null") == 0) {
                //create a lexeme to hold the error and add it to the token array
                lexeme temp = { 35, "", 0 };
                err_flag = 1;
                token_arr[arr_ind] = temp;
                arr_ind++;
            }

            //if there arent any errors
            else {
                //chuck the cur_lex into a new string (just adds \0 to the end. idk, works for me)
                char lex[MAX_CHAR];
                strcpy(lex, cur_lex);
                //flag for if the lexeme is a Reserved Word
                int flag = 0;
                //for each special word, check if it matches the lexeme
                //if so, add the special word lexeme to the token array and exit the for loop
                int size_spec_arr = sizeof(special_words) / sizeof(special_words[0]);
                for (int i = 0; i < size_spec_arr; i++) {
                    if (strcmp(special_words[i].identifier_name, lex) == 0) {
                        lexeme temp = { special_words[i].type,"", 0 };
                        strcpy(temp.identifier_name, lex);
                        token_arr[arr_ind] = temp;
                        arr_ind++;
                        flag = 1;
                        break;
                    }
                }
                //if the lexeme isnt a special word, add the identifier with its name to the token array
                if (!flag) {
                    lexeme temp = { identifier, "",  0 };
                    strcpy(temp.identifier_name, lex);
                    token_arr[arr_ind] = temp;
                    arr_ind++;
                }

            }
        }

        //if the lexeme starts with a digit
        else if (isdigit(cur_char)) {
            //flag in case the lexeme holds an alphabetical char
            int alpha_flag = 0;
            //for the length of the lexeme, concatenate the string with the next char
            while (isdigit(cur_char) || isalpha(cur_char)) {
                cur_lex[index] = cur_char;
                //cur_char = fgetc(fp);
                c_index++;
                cur_char = input[c_index];
                index++;
                //if there's an alpha char, flag it
                if (isalpha(cur_char)) {
                    alpha_flag = 1;;
                }
            }
            //if the number is too long
            if (index >= MAX_DIG) {
                //create a lexeme to hold the error and add it to the token array
                lexeme temp = { 36, "", 0 };
                err_flag = 1;
                token_arr[arr_ind] = temp;
                arr_ind++;
            }
            //if there's a char in the number
            else if (alpha_flag) {
                //create a lexeme to hold the error and add it to the token array
                lexeme temp = { 37, "", 0 };
                err_flag = 1;
                token_arr[arr_ind] = temp;
                arr_ind++;
            }
            //if the number has no errors
            else {
                //chuck the cur_lex into a new string (just adds \0 to the end. idk, works for me)
                //then create a new number lexeme and add it to the token array
                char lex[MAX_DIG];
                strcpy(lex, cur_lex);
                int val = atoi(lex);
                lexeme temp = { number, "", val };
                strcpy(temp.identifier_name, lex);
                token_arr[arr_ind] = temp;
                arr_ind++;
            }
        }

        //if the current char is a symbol character
        else {
            //set up a lexeme to potentially hold 3 chars (including \0); it begins with the current char
            char lex[3];
            lex[0] = cur_char;
            //get the next char
            //cur_char = fgetc(fp);
            c_index++;
            cur_char = input[c_index];
            //if lex[0] holds a char that may be followed with another char to form one special lexeme, check to see if it does
            //if it does, add cur_char to the lexeme and flag it as special
            if (lex[0] == '<') {
                if (cur_char == '>') {
                    lex[1] = cur_char;
                    lex[2] = '\0';
                    flag_for_special = 1;
                }
                else if (cur_char == '=') {
                    lex[1] = cur_char;
                    lex[2] = '\0';
                    flag_for_special = 1;
                }
            }
            else if (lex[0] == '>') {
                if (cur_char == '=') {
                    lex[1] = cur_char;
                    lex[2] = '\0';
                    flag_for_special = 1;
                }
            }
            else if (lex[0] == '=') {
                if (cur_char == '=') {
                    lex[1] = cur_char;
                    lex[2] = '\0';
                    flag_for_special = 1;
                }
            }
            else if (lex[0] == ':') {
                if (cur_char == '=') {
                    lex[1] = cur_char;
                    lex[2] = '\0';
                    flag_for_special = 1;
                }
            }
            //otherwise, cut the lexeme short
            else {
                lex[1] = '\0';
            }
            //flag to make sure the char is actually a special char and not an invalid one
            int true_char_flag = 0;
            //for each special char, check if the current lexeme matches
            //if so, add the current lexeme to the token array and flag it; break from the for loop
            for (int i = 0; i < sizeof(special_char)/sizeof(special_char[0]); i++) {
                if (strcmp(special_char[i].identifier_name, lex) == 0) {
                    lexeme temp = { special_char[i].type, "", 0 };
                    strcpy(temp.identifier_name, lex);
                    token_arr[arr_ind] = temp;
                    arr_ind++;
                    true_char_flag = 1;
                    break;
                }
            }

            //if the current lexeme is NOT a special char, and is thus invalid
            if (!true_char_flag) {

                //create a lexeme to hold the error and add it to the token array
                lexeme temp = { 38, "", 0 };
                err_flag = 1;
                token_arr[arr_ind] = temp;
                arr_ind++;
            }
        }

        //if we used the last cur_char in a two-char special lexeme, get the next char
        if (flag_for_special) {
            //cur_char = fgetc(fp);
            c_index++;
            cur_char = input[c_index];
        }
    }

    lexeme* fin_arr;
    fin_arr = NULL;
    int fin_count = 0;

    if (list_flag == 1) {
        fin_arr = malloc(sizeof(lexeme) * arr_ind);
        //for each lexeme in the token array up to the highest index
        for (int x = 0; x < arr_ind; x++) {
            //if not an error, print the name and type
            if (token_arr[x].type < 34) {
                printf("%s\t\t%d\n", token_arr[x].identifier_name, token_arr[x].type);
                //printf("%s\t\t%d\n", fin_arr[x].identifier_name, fin_arr[x].type);
                fin_arr[fin_count] = token_arr[x];
                fin_count++;
            }
            //if an error, just print the message
            else if(token_arr[x].type == 34) {
                printf("%s\n", "Lexical Analyzer Error: maximum identifier length is 11");
            }
            else if (token_arr[x].type == 35) {
                printf("%s\n", "Lexical Analyzer Error: identifiers cannot be named 'null' or 'main'");
            }
            else if (token_arr[x].type == 36) {
                printf("%s\n", "Lexical Analyzer Error: maximum number length is 5");
            }
            else if (token_arr[x].type == 37) {
                printf("%s\n", "Lexical Analyzer Error: identifiers cannot begin with digits");
            }
            else if (token_arr[x].type == 38) {
                printf("%s\n", "Lexical Analyzer Error: invalid symbol");
            }
        }
    }
    if (err_flag == 1) return NULL;
    return fin_arr;
}

