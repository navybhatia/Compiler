#include "Error.h"
#include "ParserUtil.h"

using namespace std;

static int errorCount = 0;
static int warningCount = 0;

static void msg(const string& type, const string& errorMsg, int line, int col, const char* file) {
  if (line == 0) {
	line = yylinenum;
	file = yyfilename;
  }
  cerr << file << ':' << line;
  if (col != 0)
	cerr << '.' << col;
  cerr << ':';
  cerr << type << errorMsg << endl;
}

void 
internalError(const string& s, int line, const char* file) {
  msg("Internal error: ", s, line, 0, file);
}

void errMsg(const string& errorMsg, int line, int col, const char* file) {
  msg("Error: ", errorMsg, line, col, file);
  errorCount++;
  if (errorCount > MAX_ERROR_BEFORE_QUIT) {
    cerr << "Too many errors.  Exiting ... " << endl;
    exit(-1);
  }
}

void errMsg(const string& msg, const ProgramElem* p) {
  if (p == NULL)
    errMsg(msg);
  else errMsg(msg, p->line(), p->column(), p->file().c_str());
}

void warnMsg(const string& warningMsg, int line, int col, const char* file) {
  msg("Warning: ", warningMsg, line, col, file);
  warningCount++;
}

void warnMsg(const string& msg , const ProgramElem* p) {
  if (p == NULL)
    warnMsg(msg);
  else warnMsg(msg, p->line(), p->column(), p->file().c_str());
}

void resetErrs() {
  warningCount = errorCount = 0;
}

void resetWarns() {
  warningCount = 0;
}

int warnCount() {
  return warningCount;
}

int errCount() {
  return errorCount;
}
