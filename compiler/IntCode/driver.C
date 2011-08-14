#include <libgen.h>
#include <unistd.h>

#include <iostream>
#include "all.h"
#include "Ast.h"
#include <stdio.h>
#include "SymTabMgr.h"
#include "STEClasses.h"
#include "SymTab.h"
#include "Value.h"

using namespace std;

#include "E--.tab.h"

#define CPP_PROG_NAME "CPP" // Environment variable to specify location of cpp
#define DEFAULT_CPP_PROG_NAME "cpp"
#define CC_PROG_NAME "CC" // Environment variable to specify C compiler
#define DEFAULT_CC_PROG_NAME "gcc -g"
#define CC_PROG_OPTS "CFLAGS"

#define MAX_DEBUG_LEVEL 2

#ifdef DEBUG_LEXER
#ifdef LEXER_OUT
ofstream lexOutStr(LEXER_OUT);
ostream& lexOut = lexOutStr;
#else
ostream& lexOut = cout;
#endif
#endif

extern EventEntry* any;
extern int yyparse();
extern int yylinenum;
extern const char* yyfilename;
extern YYSTYPE yylval;

void yyerror(const char *s)
{
  errMsg(s);
}

SymTabMgr stm;
string outputFile;
const char* inputFile = "";
string cppShellCmd, ccShellCmd;

int debugLevel;
bool genCCode;
bool genBinCode;
bool genSharedLib;
bool genDottyCode;
bool genPrologCode;
string outFileSuffix;  
extern FILE* yyin;

void
printUsage(const char* cmd) {
  cerr << "Usage: " << cmd << //" [-v <num>] "
	"[-I include_dir] [-L<library directory>]\n   [-l<library>]"
    "[-Dmacro[=defn]] [-Uname] " //[-t <target language>]\n   "
    "[-o <output file>] <input file>\n" 
    /*"\t-v: debugging\n"
    "\t\t0: none\n"
    "\t\t1: print AST 'out' file\n"
    "\t\t2: print lots of internal debugging info\n"
    "\t-o output file name\n"
	"\t-t: Specify type of output\n"
	"\t\tc: Output C code\n"
	"\t\tb: Output a binary (.o) file\n"
	"\t\ts: Output a shared library (default)\n"
	"\t\td: Output a dotty file\n"
	"\t\tp: Output Prolog code\n"*/
	"Environment variable CPP can be used to override the default command\n"
    "for running cpp program. This variable can include the program name\n"
    "as well as command-line arguments to the command. Similarly, the\n"
    "environment variable CC can specify the command to be run for compiling\n"
    "C code produced by this compiler, but the options to the compiler can\n"
    "specified independently using the environment variable CFLAGS.\n";
}

int 
parseOptions(int argc, char* argv[]) {
  int c;
  opterr = 0; // Suppress printing of errors by getopt.
  int num_t_options=0;

  if (getenv(CPP_PROG_NAME) == NULL)
	cppShellCmd = DEFAULT_CPP_PROG_NAME;  else cppShellCmd = getenv(CPP_PROG_NAME);
  cppShellCmd += " ";

  if (getenv(CC_PROG_NAME) == NULL)
	ccShellCmd = DEFAULT_CC_PROG_NAME;
  else ccShellCmd = getenv(CC_PROG_NAME);
  ccShellCmd += " ";
  if (getenv(CC_PROG_OPTS) != NULL) {
	ccShellCmd += getenv(CC_PROG_OPTS);
	ccShellCmd += " ";
  }

  while (1) {
	if ((argc > 2) || (argc < 2)) {
		cerr << "Please specify only a single input file\n";
		return -1;
	}
	else {
	  inputFile = argv[1];
	  return 0;
	}
  }

  genSharedLib = !(genCCode || genBinCode || genDottyCode || genPrologCode);
  if (genSharedLib)
	outFileSuffix = ".so";

  if (*inputFile == '\0') 
    return -1;

  if (outputFile == "") {
	size_t pos;
	if (genSharedLib) {
	  string sinputFile(inputFile);
	  if ((pos = sinputFile.rfind('/')) == string::npos)
		outputFile = "lib" + sinputFile;
	  else outputFile = 
			 sinputFile.substr(0, pos) + "/lib" + sinputFile.substr(pos+1);
	}
	else outputFile = inputFile;

	if ((pos = outputFile.rfind('.')) != outputFile.npos)
	  outputFile.replace(pos, outputFile.size(), outFileSuffix);
	else outputFile += outFileSuffix;
  }

  return 0;
}

int 
main(int argc, char *argv[], char *envp[]) {

  string ccCmd;
  int optionsOK = parseOptions(argc, argv);
  if (optionsOK < 0)
	return -1;

  cppShellCmd += inputFile;
  cppShellCmd += " ";

  /* ccCmd = "mv ";
  ccCmd += outputFile;
  ccCmd += " ";
  ccCmd += outputFile;
  ccCmd += ".c; ";
  ccCmd += ccShellCmd;
  if (genSharedLib)
	ccCmd += "-shared ";
  ccCmd += "-o ";
  ccCmd += outputFile;*/

  if ((yyin = popen(cppShellCmd.c_str(), "r")) == NULL) {
    cerr << "Unexpected error in reading input file\n";
    return 1;
  }

#ifdef TEST_LEXER
  int token;

  while (token = yylex()) {
	if (token == TOK_SYNTAX_ERROR) {
	  cout << yyfilename << ":" << yylinenum 
		   << ": Error: Unrecognized token `" << yytext << "'\n";
	}
	else {
      cout << "Token: " << token << " ";
      switch(token)
        {
        case TOK_UINTNUM:
          cout << "Attribute: (int) " << yylval.uVal;
          break;

        case TOK_DOUBLENUM:
          cout << "Attribute: (float) " << yylval.dVal;
          break;

        case TOK_STRCONST:
          cout << "Attribute: (string) " << yylval.cVal;
          break;

        case TOK_ID:
          cout << "Attribute: (id) " << yylval.cVal;
          break;

        default:
          break;
        }
      cout << endl;
	}
  }

  return 0;
#else
  //   yydebug = 1;
  any = new EventEntry("any");
  if (stm.insert(any) == OK) {
    Type *te = new Type((vector<Type*>*)NULL, Type::EVENT);
    any->type(te);
  }
  yyparse();
  stm.leaveToScope(SymTabEntry::GLOBAL_KIND);
  GlobalEntry *ge = (GlobalEntry*)(stm.currentScope());
  if (ge != NULL) {
#ifdef TEST_AST
	cout << "Finished parsing, here is the AST\n";
	ge->print(cout, 0);
#endif
#ifdef TEST_TYPE
	ge->typeCheck();
	ge->typePrint(cout, 0);
	ge->generateCode();
#endif
  }
#endif
}
