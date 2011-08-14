%{

#include "ICode.tab.h"
#include <iostream>

extern void errMsg(const char *);
using namespace std;

#define pout cout
extern int verbosity;
extern int yylex();
extern void yyerror(const char *s);
int offSet;

extern char* newlbl();
%}

%union{
  char*              cVal;
};

/* Operators */

%token TOK_LEX_ERROR 
%token TOK_INTNUM TOK_DOUBLENUM TOK_STRCONST TOK_IDENT TOK_IREG TOK_FREG 
%token TOK_BINIOP TOK_BINFOP TOK_NEG TOK_FNEG TOK_CMPIOP TOK_CMPFOP 
%token TOK_MOVL TOK_MOVS TOK_MOVI TOK_MOVF TOK_MOVIF TOK_MOVFI
%token TOK_STI TOK_STF TOK_LDI TOK_LDF
%token TOK_JMP TOK_JMPI TOK_JMPC TOK_JMPCI
%token TOK_PRTI TOK_PRTF TOK_PRTS TOK_IN TOK_INI TOK_INF
%token TOK_NEWLINE TOK_COLON TOK_LPAREN TOK_RPAREN 

/******************************** Type Info ********************************/

%type <cVal> TOK_IDENT TOK_STRCONST TOK_BINIOP TOK_BINFOP TOK_CMPIOP TOK_CMPFOP 
%type <cVal> TOK_INTNUM TOK_DOUBLENUM TOK_IREG TOK_FREG 
%type <cVal> label
%type <cVal> ioperand foperand soperand

/**************************************************************************/

%start specification
%%

specification: instruction_star {
   pout << "\n\tE_RT_exit(); return 0;\n}\n";
};

instruction_star: /* empty */ {
   pout << "#include \"E--_RT.c\"\n";
   pout << "int main(int argc, char *argv[]) {\n";
   pout << "\tE_RT_init(argc, argv);\n";
}
| instruction_star instruction_optlbl TOK_NEWLINE {}
| instruction_star TOK_NEWLINE {}
| instruction_star error TOK_NEWLINE {}
;

instruction_optlbl: label instruction {}
| instruction {}
;

label: TOK_IDENT TOK_COLON {
   pout << $1 << ": ";
}
;

instruction: 
  TOK_BINIOP ioperand ioperand TOK_IREG {
   pout << $4 << '=' << $1 << '(' << $2 << ',' << $3 << ");\n";
}
| TOK_BINFOP foperand foperand TOK_FREG {
   pout << $4 << '=' << $1 << '(' << $2 << ',' << $3 << ");\n";
}
| TOK_FNEG foperand foperand {
   pout <<  $3 << "= FNEG(" << $2 << ");\n";
}
| TOK_NEG ioperand ioperand {
   pout <<  $3 << "= NEG(" << $2 << ");\n";
}
| TOK_PRTI ioperand {
   pout << "PRTI(" << $2 << ");\n";
}
| TOK_PRTF foperand {
   pout << "PRTF(" << $2 << ");\n";
}
| TOK_PRTS soperand {
   pout << "PRTS(" << $2 << ");\n";
}
| TOK_JMP TOK_IDENT {
   pout << "JMP(" << $2 << ");\n";
}
| TOK_JMPI TOK_IREG {
   pout << "JMPI(" << $2 << ");\n";
}
| TOK_JMPC TOK_CMPIOP ioperand ioperand TOK_IDENT {
   pout << "JMPC(" << $2 << "(" << $3 << ", " << $4 << "), " << $5 << ");\n";
}
| TOK_JMPC TOK_CMPFOP foperand foperand TOK_IDENT {
   pout << "JMPC(" << $2 << "(" << $3 << ", " << $4 << "), " << $5 << ");\n";
}
| TOK_JMPCI TOK_CMPIOP ioperand ioperand TOK_IREG {
   pout << "JMPCI(" << $2 << "(" << $3 << ", " << $4 << "), " << $5 << ");\n";
}
| TOK_JMPCI TOK_CMPFOP foperand foperand TOK_IREG {
   pout << "JMPCI(" << $2 << "(" << $3 << ", " << $4 << "), " << $5 << ");\n";
}
| TOK_MOVL TOK_IDENT TOK_IREG {
   pout << "MOVL(" << $2 << ", " << $3 << ");\n";
}
| TOK_MOVS TOK_STRCONST TOK_IREG {
   pout << "MOVS(" << $2 << ", " << $3 << ");\n";
}
| TOK_MOVI ioperand TOK_IREG {
   pout << "MOVI(" << $2 << ", " << $3 << ");\n";
}
| TOK_MOVF foperand TOK_FREG {
   pout << "MOVF(" << $2 << ", " << $3 << ");\n";
}
| TOK_MOVIF ioperand TOK_FREG {
   pout << "MOVIF(" << $2 << ", " << $3 << ");\n";
}
| TOK_MOVFI foperand TOK_IREG {
   pout << "MOVFI(" << $2 << ", " << $3 << ");\n";
}
| TOK_STI ioperand ioperand {
   pout << "STI(" << $2 << ", " << $3 << ");\n";
}
| TOK_STF foperand ioperand {
   pout << "STF(" << $2 << ", " << $3 << ");\n";
}
| TOK_LDI ioperand TOK_IREG {
   pout << "LDI(" << $2 << ", " << $3 << ");\n";
}
| TOK_LDF ioperand TOK_FREG {
   pout << "LDF(" << $2 << ", " << $3 << ");\n";
}
| TOK_IN TOK_IREG {
   pout << "IN(" << $2 << ");\n";
}
| TOK_INI TOK_IREG {
   pout << "INI(" << $2 << ");\n";
}
| TOK_INF TOK_FREG {
   pout << "INF(" << $2 << ");\n";
}
;

ioperand: TOK_IREG {
   $$ = $1;
}
| TOK_INTNUM {
   $$ = $1;
}
;
  
soperand: TOK_IREG {
   $$ = $1;
}
| TOK_STRCONST {
   $$ = $1;
}
;
  
foperand: TOK_FREG {
   $$ = $1;
}
| TOK_DOUBLENUM {
   $$ = $1;
}
;
  
%%

char *newlbl() {
   static int c;
   static char s[16];

   sprintf(s, "L%d", c++);
   return s;
}
