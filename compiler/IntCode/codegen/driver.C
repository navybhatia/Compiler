#include <libgen.h>
#include <unistd.h>
#include <iostream>
 #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <error.h>
#include <stdio.h>
using namespace std;

#include "ICode.tab.h"

#define DEFAULT_CPP_PROG_NAME "cpp"
#define DEFAULT_CC_PROG_NAME "gcc -Wall -g"

#define MAX_DEBUG_LEVEL 2


#ifdef DEBUG_LEXER
#ifdef LEXER_OUT
ofstream lexOutStr(LEXER_OUT);
ostream& lexOut = lexOutStr;
#else
ostream& lexOut = cout;
#endif
#endif

extern int yyparse();
extern int yylinenum;
extern const char* yyfilename;
extern YYSTYPE yylval;

void errMsg(const char* s) {
  cerr << yyfilename << ":" << yylinenum << ":" << s << endl;
};

int errs;
void yyerror(const char *s)
{
  errMsg(s);
  errs++;
}

//SymTabMgr stm;
string of, outputFile;

const char* inputFile = "";
string cppShellCmd, ccShellCmd;

int debugLevel;
extern FILE* yyin;

void
printUsage(const char* cmd) {
   cerr << "Usage: " << cmd << "<input file>\n" <<
	"Environment variable CPP can be used to override the default command\n"
    "for running cpp program. This variable can include the program name\n"
    "as well as command-line arguments to the command. Similarly, the\n"
    "environment variable CC can specify the command to be run for compiling\n"
    "C code produced by this compiler, but the options to the compiler can\n"
    "specified independently using the environment variable CFLAGS.\n";
}

int 
parseOptions(int argc, char* argv[]) {

  size_t pos;
  cppShellCmd = DEFAULT_CPP_PROG_NAME;
  cppShellCmd += " ";

  ccShellCmd = DEFAULT_CC_PROG_NAME;
  ccShellCmd += " ";

  if ((argc > 2) || (argc < 2)) {
     cerr << "Please specify only a single input file\n";
     return -1;
  }
  else {
     inputFile = argv[1];
  }

  if (*inputFile == '\0') 
    return -1;

  of = inputFile;
  if ((pos = of.rfind('.')) == of.npos) {
     cerr << "Input files must have a `.i' suffix\n";
     exit(1);
  }
  of.erase(pos, of.size());
  outputFile = of + ".c";

  return 0;
}

int 
main(int argc, char *argv[], char *envp[]) {

  string ccCmd; int ofd;
  int optionsOK = parseOptions(argc, argv);
  if (optionsOK < 0)
	return -1;

  cppShellCmd += inputFile;
  cppShellCmd += " ";

  if (((ofd = open(outputFile.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) < 0) ||
      (dup2(ofd, 1) < 0)) {
     cerr << "Unable to open file " << outputFile << " for writing; exiting\n";
     perror("");
     exit(1);
  }
  
  if ((yyin = popen(cppShellCmd.c_str(), "r")) == NULL) {
    cerr << "Unexpected error in reading input file\n";
    return 1;
  }

  yyparse();
  if (errs > 0)
     return errs;

  ccCmd = ccShellCmd + " -o " + of + " " + outputFile;

  cerr << ccCmd << endl;
  return system(ccCmd.c_str());
}
