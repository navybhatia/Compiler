%option noyywrap
%option never-interactive

%{
#include "ICode.tab.h"
#include <iostream>

using namespace std;

int last_ret;

#ifdef DEBUG_LEXER
extern ostream& lexOut;
#define yylex_ret(x) {       lexOut << #x << endl; return x; }
#define yylex_retval(x, y) { lexOut << #x << "\t``"<< y << "''\n"; return x; }
#else
#define yylex_ret(x) \
	last_ret = x; return x
#define yylex_retval(x, y) yylex_ret(x)
#endif

const char* yyfilename="";
char string_buf[10000];
char *string_buf_ptr;
int tabWidth=8;

int yylinenum=0;

static char*
copyStr(int begSkipLen=0, int endSkipLen=0) {
  int slen = yyleng+1-begSkipLen-endSkipLen;
  char *rv = new char[slen];
  strncpy(rv, yytext+begSkipLen, slen-1);
  rv[slen-1] = '\0';
  return rv;
}

static bool
strbuf_space_avl(int s) {
  bool rv = (string_buf_ptr + s) < (string_buf + sizeof(string_buf) - 2);
  if (!rv)
    printf("String constant too long\n");
  return rv;
}

int tokBegCol, tokEndCol, tokBegLn, tokEndLn;
int errTokBegLn, errTokEndLn, errTokBegCol, errTokEndCol;
extern int yyrecovering();
int lparen, lbrace, lbrack;

static void
check_and_strbuf_spl_char(unsigned long c) {
  if (c < 256) {
	if (strbuf_space_avl(yyleng))
     *string_buf_ptr++ = c;
  }	
  else printf("Invalid escape sequence\n");
}

%}
/* Definitions */

HEX_DIGIT      [0-9a-fA-F]

HEXNUM         0x{HEX_DIGIT}+

DECIMAL_DIGIT  [0-9]
INTNUM		   [-]?{DECIMAL_DIGIT}+

MANTISSA_EINT  {DECIMAL_DIGIT}*[.]{DECIMAL_DIGIT}+
MANTISSA_EFRAC {DECIMAL_DIGIT}+(([.]{DECIMAL_DIGIT}*)?)
FLOAT_MANTISSA ({MANTISSA_EINT}|{MANTISSA_EFRAC})
FLOAT_EXP	   [Ee]([+-]?){DECIMAL_DIGIT}+
FLOATNUM	   [-]?{FLOAT_MANTISSA}({FLOAT_EXP}?)

ALPHA       	[_A-Za-z] 
IDENT        	{ALPHA}({ALPHA}|{DECIMAL_DIGIT})*

IREG R[0-9][0-9][0-9]
FREG F[0-9][0-9][0-9]

NEWLINE		"\n"

%x STRING INCLUDE FSTART SKIPTOEOL
%%

^"#"   {
  BEGIN INCLUDE;
}

 /*********** Remember file/line number info in cpp output ********/

<INCLUDE>[ ][0-9]+[ ]+	{
  yylinenum = atoi(yytext+1) - 1;
  BEGIN FSTART;
}

<FSTART>\"([^"]+)\" {
  yyfilename = copyStr(1, 1);
  BEGIN SKIPTOEOL;
}

<SKIPTOEOL>[^\n]*\n {
    yylinenum++ ;
    //cout << "# " << yylinenum << " \"" << yyfilename << "\"\n";
	BEGIN 0;
}

"\"" { 
  string_buf_ptr = yytext;
  BEGIN(STRING); 
}

<STRING>\\. { 
} 

<STRING>"\"" { 
  yylval.cVal = new char[yytext - string_buf_ptr + 2];
  strncpy(yylval.cVal, string_buf_ptr, yytext - string_buf_ptr+1);
  yylval.cVal[yytext - string_buf_ptr+1] = '\0';

  BEGIN 0;
  yylex_retval(TOK_STRCONST, yylval.cVal);
}

<STRING>. { 
}

"(" {
  yylex_ret(TOK_LPAREN);
}


")" {
  yylex_ret(TOK_RPAREN);
}


"ADD"|"SUB"|"DIV"|"MUL"|"MOD"|"AND"|"OR"|"XOR" {
  yylval.cVal=copyStr();
  yylex_retval(TOK_BINIOP, yylval.cVal);
}

"FADD"|"FSUB"|"FDIV"|"FMUL" {
  yylval.cVal=copyStr();
  yylex_retval(TOK_BINFOP, yylval.cVal);
}

"NEG" {
  yylex_retval(TOK_NEG, yylval.cVal);
}

"FNEG" {
  yylex_retval(TOK_FNEG, yylval.cVal);
}

"UGT"|"UGE"|"GT"|"GE"|"NE"|"EQ" {
  yylval.cVal=copyStr();
  yylex_retval(TOK_CMPIOP, yylval.cVal);
}

"FGT"|"FGE"|"FNEQ"|"FEQ" {
  yylval.cVal=copyStr();
  yylex_retval(TOK_CMPFOP, yylval.cVal);
}

"PRTS" {
  yylex_ret(TOK_PRTS);
}

"PRTI" {
  yylex_ret(TOK_PRTI);
}

"PRTF" {
  yylex_ret(TOK_PRTF);
}

"JMP" {
  yylex_ret(TOK_JMP);
}

"JMPI" {
  yylex_ret(TOK_JMPI);
}

"JMPC" {
  yylex_ret(TOK_JMPC);
}

"JMPCI" {
  yylex_ret(TOK_JMPCI);
}

"MOVL" {
  yylex_ret(TOK_MOVL);
}

"MOVS" {
  yylex_ret(TOK_MOVS);
}

"MOVI" {
  yylex_ret(TOK_MOVI);
}

"MOVF" {
  yylex_ret(TOK_MOVF);
}

"MOVIF" {
  yylex_ret(TOK_MOVIF);
}

"MOVFI" {
  yylex_ret(TOK_MOVFI);
}

"STI" {
  yylex_ret(TOK_STI);
}

"STF" {
  yylex_ret(TOK_STF);
}

"LDI" {
  yylex_ret(TOK_LDI);
}

"LDF" {
  yylex_ret(TOK_LDF);
}

"IN" {
  yylex_ret(TOK_IN);
}

"INI" {
  yylex_ret(TOK_INI);
}

"INF" {
  yylex_ret(TOK_INF);
}

{IREG} {
  yylval.cVal=copyStr();
  yylex_retval(TOK_IREG, yylval.cVal);
}

{FREG} {
  yylval.cVal=copyStr();
  yylex_retval(TOK_FREG, yylval.cVal);
}

{IDENT} {
  yylval.cVal=copyStr();
  yylex_retval(TOK_IDENT, yylval.cVal);
}

 /**************** Recognize numeric values ****************/

{INTNUM} { 
  yylval.cVal=copyStr();
  yylex_retval(TOK_INTNUM, yylval.cVal);
}

{FLOATNUM} { 
  yylval.cVal=copyStr();
  yylex_retval(TOK_DOUBLENUM, yylval.cVal);
}

 /**************** Recognize whitespace characters ****************/

{NEWLINE}	{ yylinenum++;
              tokBegLn = tokEndLn = yylinenum;
              yylex_ret(TOK_NEWLINE);
            }

":"       {yylex_ret(TOK_COLON);};
[ ]       { /*advanceCol();*/}
[\t]      { /*tabAdvanceCol();*/}
[\r]      { /*advanceCol();*/}

 /**************** Unrecognized character ==> error ****************/

. {
	return TOK_LEX_ERROR;
}

%%
