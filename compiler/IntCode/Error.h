#ifndef __ERROR__H__
#define __ERROR__H__

#include <string>
#include <iostream>
using namespace std;
extern int yylinenum;
extern const char* yyfilename;


#define MAX_ERROR_BEFORE_QUIT 100

#define internalErr(s) internalError(s, __LINE__, __FILE__)

using namespace std;

class ProgramElem;

void errMsg(const string&, int=0, int=0, const char* file="");
void inline errMsg(const char* s, int ln=0, int col=0, const char* file="") {
  errMsg(string(s), ln, col, file);}
void errMsg(const string&, const ProgramElem*);
void warnMsg(const string&, int=0,int=0, const char* file="");
void warnMsg(const string&, const ProgramElem*);

void resetErrs() ;
void resetWarns();
int warnCount() ;
int errCount();

void internalError(const string& s, int line, const char* file);

#define debug
#ifdef debug
#define internalWarn(s) errMsg(s); internalErr(s);
#else
#define internalWarn(s)
#endif

#endif /*  __ERROR__H__ */
