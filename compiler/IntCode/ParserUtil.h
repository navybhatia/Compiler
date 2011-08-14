#ifndef __PARSER_UTIL_H__
#define __PARSER_UTIL_H__

#include <stdio.h>
extern int yylinenum;
extern int yycolumnno;
extern const char* yyfilename;
extern FILE *yyin;

extern void yyerror(char *);
extern int yyparse(void);
extern int yylex(void);

#include "SymTabMgr.h"

extern SymTabMgr stm;

#include "STEClasses.h"
extern EventEntry *any, *other;

#include "SymTab.h"
ErrorST insertSymbolErrCheck(SymTabEntry* se);
void enterScopeErrCheck(SymTabEntry*);

#define errMsgLn(s) errMsg(s)

string newName(const string& type);
string itoa(int i);

#include "all.h"
#include "Ast.h"
#include "Value.h"
#include "Error.h"
#endif
