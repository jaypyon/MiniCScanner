/***************************************************************
 *      Scanner routine for Mini C language                    *
 *                                   2020. 5. 11               *
 ***************************************************************/



#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include<math.h>
#include "Scanner.h"


enum tsymbol tnum[NO_KEYWORD] = {
    tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
    tchar, tdouble, tfor, tdo, tgoto, tswitch, tcase, tbreak, tdefault
};




char* tokenName[] = {
    "!",        "!=",      "%",       "%=",     "%ident",   "%number",
    /* 0          1           2         3          4          5        */
    "&&",       "(",       ")",       "*",      "*=",       "+",
    /* 6          7           8         9         10         11        */
    "++",       "+=",      ",",       "-",      "--",	    "-=",
    /* 12         13         14        15         16         17        */
    "/",        "/=",      ";",       "<",      "<=",       "=",
    /* 18         19         20        21         22         23        */
    "==",       ">",       ">=",      "[",      "]",        "eof",
    /* 24         25         26        27         28         29        */
    //   ...........    word symbols ................................. //
    /* 30         31         32        33         34         35        */
    "const",    "else",     "if",      "int",     "return",  "void",
    /* 36         37         38        39                              */
    "while",    "{",        "||",       "}",        "char",     "double",
    /* 42         43          44        45           46         47     */
    "for",        "do",      "goto",    "switch",    "case",    "break",
    /* 48		  49		  50		51			 52  			53   */
    "default",    "%string",  "%char",  ":",         "comment", "%real"
};//tsymbol로 저장된 int number에 따라서 출력될 때 사용된다.

char* keyword[NO_KEYWORD] = {
    "const",  "else",    "if",    "int",    "return",  "void",    "while",
    "char", "double", "for", "do", "goto", "switch", "case", "break", "default"
};

int line = 1;
int column = 1;
int dot_flag = 0;


char strtoken[MAX_LENGTH];


struct tokenType scanner(FILE *sourceFile)
{
    struct tokenType token;
    int i, index;
    char ch, id[ID_LENGTH];

    token.token_number = tnull;
    token.token_number = tnull;
    memset(token.comment_content, '\0', sizeof(token.comment_content));
    token.type_of_comment = 0;

    do {
        
        
        while (isspace(ch = fgetc(sourceFile))) {
            if (ch == '\n') {
                column = 1;//줄바뀜, 컬럼 초기화
                ++line;
            }
            else ++column;
        }	// state 1: skip blanks
        if (superLetter(ch)) { // identifier or keyword
            token.column_number = column;
            token.line_number = line;

            i = 0;
            do {
                if (i < ID_LENGTH) id[i++] = ch;
                ch = fgetc(sourceFile);
                column++;
            } while (superLetterOrDigit(ch));
            if (i >= ID_LENGTH) lexicalError(1);
            id[i] = '\0';
            ungetc(ch, sourceFile);  //  retract
            // find the identifier in the keyword table
            for (index = 0; index < NO_KEYWORD; index++)
                if (!strcmp(id, keyword[index])) break;
            if (index < NO_KEYWORD)    // found, keyword exit
            {
                token.token_number = tnum[index];
                strcpy_s(token.token_value, ID_LENGTH, "");
            }
            else {                     // not found, identifier exit
                token.token_number = tident;
                strcpy_s(token.token_value, ID_LENGTH, id);
            }
        }  // end of identifier or keyword
        else if (isdigitordot(ch)) {  // number
            token.column_number = column;
            token.line_number = line;
            //char real_str[MAX_LENGTH];
            double real_val = getNumber(sourceFile,ch);
            //sprintf(real_str,"%lf", real_val);
            if (dot_flag > 0) {
                token.token_number = treal;
                snprintf(strtoken, MAX_LENGTH, "%lf", real_val);
            }
            else {
                token.token_number = tnumber;
                snprintf(strtoken, MAX_LENGTH, "%d", (int)real_val);
            }
            
            strncpy_s(token.token_value,MAX_LENGTH, strtoken,MAX_LENGTH);
            dot_flag = 0;
            memset(strtoken, '\0', sizeof(strtoken));
        }
        else switch (ch) {  // special character
        case '/':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '*') {// text comment
                char comment_content[CMT_LENGTH];
                ch = fgetc(sourceFile);
                column++;
                int i = 0;
                if (ch == '*') { //documented comment
                    
                    token.type_of_comment = 1;
                    do {
                        while (ch != '*'){
                            if (ch == '\n') {
                                column = 1;//줄바뀜, 컬럼 초기화
                                ++line;
                            }
                            comment_content[i++] = ch;
                            ch = fgetc(sourceFile);
                            column++;
                        }
                        ch = fgetc(sourceFile);
                        column++;
                    } while (ch != '/');
                    comment_content[i] = '\0';
                    strcpy_s(token.comment_content,CMT_LENGTH, comment_content);
                    //printf("멀티라인:%s\n", token.comment_content);
                    
                }
                else {//일반적인 주석
                    token.type_of_comment = 2;
                    do {
                        while (ch != '*') {
                            if (ch == '\n') {
                                column = 1;//줄바뀜, 컬럼 초기화
                                ++line;
                            }
                            ch = fgetc(sourceFile);
                            column++;
                        };
                        ch = fgetc(sourceFile);
                        column++;
                    } while (ch != '/');
                }
                token.token_number = tcomment;
                break;
            }
            else if (ch == '/') {		
                char comment_content[CMT_LENGTH];
                ch = fgetc(sourceFile);
                column++;
                int i = 0;
                if (ch == '/') {//single line documented comment
                    token.type_of_comment = 3;
                    ch = fgetc(sourceFile);
                    column++;
                    while (ch != '\n') {
                       comment_content[i++] = ch;
                       ch = fgetc(sourceFile); column++;
                    }
                    comment_content[i] = '\0';
                    strcpy_s(token.comment_content, CMT_LENGTH, comment_content);
                    //printf("싱글라인:%s\n", token.comment_content);
                    column = 1;//줄바뀜, 컬럼 초기화
                    ++line;

                }
                else {// single line comment
                    token.type_of_comment = 4;
                    while (fgetc(sourceFile) != '\n');
                    column = 1;//줄바뀜, 컬럼 초기화
                    ++line;
                }
                token.token_number = tcomment;
                break;
            }
            else if (ch == '=') { 
                token.column_number = column;
                token.line_number = line;
                token.token_number = tdivAssign; 
            }
            else {
                token.column_number = column;
                token.line_number = line;
                token.token_number = tdiv;
                ungetc(ch, sourceFile); // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '!':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '=')  token.token_number = tnotequ;
            else {
                token.token_number = tnot;
                ungetc(ch, sourceFile); // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '%':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '=') {
                token.token_number = tremAssign;
            }
            else {
                token.token_number = tremainder;
                ungetc(ch, sourceFile);
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '&':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '&')  token.token_number = tand;
            else {
                lexicalError(2);
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '*':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '=')  token.token_number = tmulAssign;
            else {
                token.token_number = tmul;
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '+':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '+')  token.token_number = tinc;
            else if (ch == '=') token.token_number = taddAssign;
            else {
                token.token_number = tplus;
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '"':
            token.column_number = column;
            token.line_number = line;
            char string_content[MAX_LENGTH];
            i = 0;
            ch = fgetc(sourceFile);
            column++;
            while (ch != '"')
            {
                if (ch == '\n')
                    ch = ' ';
                string_content[i++] = ch;
                ch = fgetc(sourceFile);
                column++;
            }
            string_content[i] = '\0';
            column++;
            token.token_number = tstringltr;
            strcpy_s(token.token_value, MAX_LENGTH, string_content);
            break;
        case '\'':
            token.column_number = column;
            token.line_number = line;
            char character_str[MAX_LENGTH];
            ch = fgetc(sourceFile);
            column++;
            character_str[0] = ch;
            character_str[1] = '\0';
            token.token_number = tcharltr;
            strcpy_s(token.token_value, ID_LENGTH, character_str);
            ch = fgetc(sourceFile);
            break;
        case '-':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '-')  token.token_number = tdec;
            else if (ch == '=') token.token_number = tsubAssign;
            else {
                token.token_number = tminus;
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '<':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '=') token.token_number = tlesse;
            else {
                token.token_number = tless;
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '=':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '=')  token.token_number = tequal;
            else {
                token.token_number = tassign;
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '>':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '=') token.token_number = tgreate;
            else {
                token.token_number = tgreat;
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '|':
            token.column_number = column;
            token.line_number = line;
            ch = fgetc(sourceFile);
            column++;
            if (ch == '|')  token.token_number = tor;
            else {
                lexicalError(3);
                ungetc(ch, sourceFile);  // retract
            }
            strcpy_s(token.token_value, ID_LENGTH, "");
            break;
        case '(': token.column_number = column; token.line_number = line; token.token_number = tlparen; strcpy_s(token.token_value, ID_LENGTH, ""); column++;         break;
        case ')': token.column_number = column; token.line_number = line; token.token_number = trparen; strcpy_s(token.token_value, ID_LENGTH, ""); column++;       break;
        case ',': token.column_number = column; token.line_number = line; token.token_number = tcomma; strcpy_s(token.token_value, ID_LENGTH, ""); column++;        break;
        case ';': token.column_number = column; token.line_number = line; token.token_number = tsemicolon; strcpy_s(token.token_value, ID_LENGTH, ""); column++;     break;
        case '[': token.column_number = column; token.line_number = line; token.token_number = tlbracket; strcpy_s(token.token_value, ID_LENGTH, ""); column++;      break;
        case ']': token.column_number = column; token.line_number = line; token.token_number = trbracket; strcpy_s(token.token_value, ID_LENGTH, ""); column++;      break;
        case '{': token.column_number = column; token.line_number = line; token.token_number = tlbrace; strcpy_s(token.token_value, ID_LENGTH, ""); column++;        break;
        case '}': token.column_number = column; token.line_number = line; token.token_number = trbrace; strcpy_s(token.token_value, ID_LENGTH, ""); column++;        break;
        case ':': token.column_number = column; token.line_number = line; token.token_number = tcolon; strcpy_s(token.token_value, ID_LENGTH, ""); column++;        break;
        case EOF: token.column_number = column; token.line_number = line; token.token_number = teof; strcpy_s(token.token_value, ID_LENGTH, ""); column++;          break;
        default: {
            printf("Current character : %c", ch);
            lexicalError(4);
            break;
        }

        } // switch end
    } while (token.token_number == tnull);
    return token;
} // end of scanner

void lexicalError(int n)
{
    printf(" *** Lexical Error : ");
    switch (n) {
    case 1: printf("an identifier length must be less than 12.\n");
        break;
    case 2: printf("next character must be &\n");
        break;
    case 3: printf("next character must be |\n");
        break;
    case 4: printf("invalid character\n");
        break;
    }
}
int isdigitordot(char ch) {
    if (isdigit(ch))return 1;
    else if (ch == '.')return 1;
    else return 0;
}
int superLetter(char ch)
{
    if (isalpha(ch) || ch == '_') return 1;
    else return 0;
}

int superLetterOrDigit(char ch)
{
    if (isalnum(ch) || ch == '_') return 1;
    else return 0;
}

double getNumber(FILE* sourceFile,char firstCharacter){
    char ch = firstCharacter;
    char ret_str[MAX_LENGTH];
    memset(ret_str, '\0', sizeof(ret_str));
    int hex_flag = 0;
    int oct_flag = 0;
    double retvalue = 0.0;
    int i = 0;
    if (ch == '0') {
        ret_str[i++] = ch;
        ch = fgetc(sourceFile);
        column++;
        
        if (ch == '.' || ch == 'e' || ch == 'E') {
            dot_flag = 1;
            ret_str[i++] = ch;
            ch = fgetc(sourceFile);
            column++;
            while (isdigit(ch)|| ch == 'e' || ch == 'E'|| ch == '-' || ch == '+') {
                ret_str[i++] = ch;
                ch = fgetc(sourceFile);
                column++;
            }
        }
        else if (ch == 'x' || ch == 'X') {
            hex_flag = 1;
            ret_str[i++] = ch;
            ch = fgetc(sourceFile);
            column++;
            while (hexValue(ch)) {
                ret_str[i++] = ch;
                ch = fgetc(sourceFile);
                column++;
            }
        }
        else if ((ch >= '0') && (ch <= '7')) {
            oct_flag = 1;

            while ((ch >= '0') && (ch <= '7')) {
                ret_str[i++] = ch;
                ch = fgetc(sourceFile);
                column++;
            }
        }
        else {
            //printf("\nerror");
            //printf("%c\n", ch);
        }
        
    }
    else if (isdigit(ch)) {
        
        ret_str[i++] = ch;
        ch = fgetc(sourceFile);
        column++;
        while (isdigitordot(ch) || ch == 'e' || ch == 'E'|| ch == '-' || ch == '+') {
            if (ch == '.' || ch == 'e' || ch == 'E') {
                dot_flag = 1;
            }
            ret_str[i++] = ch;
            ch = fgetc(sourceFile);
            column++;
        }
    }
    else if (ch=='.'|| ch == 'e' || ch == 'E') {
        dot_flag = 1;
        ret_str[i++] = ch;
        ch = fgetc(sourceFile);
        column++;
        while (isdigitordot(ch) || ch == 'e' || ch == 'E' || ch == '-' || ch == '+') {
            ret_str[i++] = ch;
            ch = fgetc(sourceFile);
            column++;
        }
    }
    else {
        //printf("error");
    }
    ch = ungetc(ch, sourceFile);
    if (hex_flag) {
        retvalue = (double)strtol(ret_str, NULL, 16);
    }
    else if (oct_flag) {
        retvalue = (double)strtol(ret_str, NULL, 8);
    }
    else if (dot_flag) {
        retvalue = strtod(ret_str, NULL);
    }
    else {
        retvalue = (double)strtol(ret_str, NULL, 10);
        // int
    }
    //printf("%f\n", retvalue);
    return retvalue;

}

int hexValue(char ch)
{
    switch (ch) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return (ch - '0');
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return (ch - 'A' + 10);
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return (ch - 'a' + 10);
    default: return -1;
    }
}

void writeToken(struct tokenType token, FILE *outputFile,char *filename)
{

    if (token.type_of_comment == 1 || token.type_of_comment == 3) {
        fprintf(outputFile, "...\n Documented Comments ------> %s \n...\n",  token.comment_content);

    }
    else if(token.type_of_comment == 0){
        fprintf(outputFile, "Token  -----> %10s (  %10d ,  %10s ,%s, %10d , %10d )\n", 
            tokenName[token.token_number], token.token_number, token.token_value, filename, token.line_number, token.column_number);
    }
    
}
