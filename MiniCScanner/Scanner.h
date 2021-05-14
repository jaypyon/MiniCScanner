
/***************************************************************
 *      Header file of scanner routine for Mini C language     *
 *                                   2020. 5. 11               *
 ***************************************************************/

#ifndef SCANNER_H

#define SCANNER_H 1


#include <stdio.h>

#define NO_KEYWORD 16
#define ID_LENGTH 12
#define CMT_LENGTH 1000
#define MAX_LENGTH 80


struct tokenType {
    int token_number;
    char token_value[MAX_LENGTH];
    int column_number;
    int line_number;
    int type_of_comment;
    //1: documented comments 2: multi line comment 
    //3:single line documented comments 4: single line comment
    char comment_content[CMT_LENGTH];
};

enum tsymbol {
    tnull = -1,
    tnot,        tnotequ,      tremainder,tremAssign, tident,    tnumber,
    /* 0          1            2         3            4          5     */
    tand,         tlparen,     trparen,  tmul,       tmulAssign, tplus,
    /* 6          7            8         9           10         11     */
    tinc,         taddAssign,  tcomma,  tminus,      tdec,      tsubAssign,
    /* 12         13          14        15           16         17     */
    tdiv,         tdivAssign, tsemicolon,tless,      tlesse,    tassign,
    /* 18         19          20        21           22         23     */
    tequal,       tgreat,     tgreate,  tlbracket,   trbracket, teof,
    /* 24         25          26        27           28         29     */
    
    //   ...........    word symbols ................................. //
    
    /* 30         31          32        33           34         35     */
    tconst,       telse,      tif,      tint,        treturn,   tvoid,
    /* 36         37          38        39           40         41     */
    twhile,     tlbrace,      tor,      trbrace,     tchar,     tdouble,
    /* 42         43          44        45           46         47     */
    tfor,         tdo,        tgoto,    tswitch,     tcase,     tbreak,
    /* 48		  49		  50		51			 52  		53	   */
    tdefault,     tstringltr, tcharltr ,tcolon,      tcomment, treal
   
};


int superLetter(char ch);
int superLetterOrDigit(char ch);
double getNumber(FILE *sourceFile, char firstCharacter);
int hexValue(char ch);
void lexicalError(int n);
struct tokenType scanner(FILE *sourceFile);
void writeToken(struct tokenType token, FILE* outputFile);

#endif // !SCANNER_H

