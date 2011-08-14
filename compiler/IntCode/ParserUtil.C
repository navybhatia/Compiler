#include "ParserUtil.h"

static int variableCount = 333;     
EventEntry *any, *other;
extern int yylineno;
int yycolumnno;

static void 
errPrint(ErrorST code, const string& sym) {
  switch (code) {
  case DUPLICATE_SYM:
	errMsgLn("Redefinition of name " + sym);
	break;
  case SYM_NOT_PRESENT:
	errMsgLn("Undeclared symbol `" + sym + "'");
	break;
  case INVALID_SCOPE:
	errMsgLn("Definition of `" + sym +"' is invalid in " + 
		   stm.currentScope()->name());
	break;
  default:
	break;
  }
}

ErrorST insertSymbolErrCheck(SymTabEntry* se) {
  ErrorST err = stm.insert(se);
  errPrint(err, se->name());
  return err;
}

void enterScopeErrCheck(SymTabEntry* se) {
  errPrint(stm.enterScope(se), se->name());
}

string newName(const string& type) {
  string s("__");
  s +=type+"__"+ itoa(variableCount++);
  return s;
}

string itoa(int i) {
  char a[20];
  sprintf(a, "%d", i);
  return string(a);
}
