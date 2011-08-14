#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>

#define FUNC 1
#define CODE 2

using namespace std;

class Quadruple
{
	string instruction;
	vector <string> args;
	string label;
	public :
	Quadruple(string, vector <string> , string label="");
	string getCode();
	string getLabel(){return label;};
	string getInstruction(){return instruction;}
	string getiArgs(int i){return args[i];}
	void setiArgs(int i,string s){ args[i] = s;}
	int getnArgs();
	bool isLabel();
	bool isJump();
	bool isJumpImd();
	bool isJumpCond();
	bool isMov();
	bool isWrite();
        bool isAdd();
        bool isMul();
        void setinstruction(string s) { instruction = s;}
        vector<string>& getargsbyref();

};

class IntermediateCode
{
	FILE *intcodefile;
	vector <Quadruple *> codelist;
	vector<vector <Quadruple *> > blocks;
	vector<vector <string *> > path;
	public:
	IntermediateCode();
	IntermediateCode(vector <Quadruple *> list)
	{
		codelist=list;
	}
	void addCode(Quadruple *);
	~IntermediateCode();
	vector <Quadruple *> getList(){return codelist;}
	void printCode();
	void buildinstrlabel(string);
	void buildinstr(string);
	void buildinstr(string,string);
	void buildinstr(string,string,string);
	void buildinstr(string,string,string,string);
	void buildinstr(string,string,string,string,string);
	IntermediateCode operator+(IntermediateCode &);
	void emptylabels();
	void createBlocks();
	void printBlocks();
	void organizeBlocks();
	void removeLabels();
	void constPropagation();
	void strengthreduction();

    void eliminateDCode();
    void findGlobalVariables();
    bool isUsed(string);
    bool isLocal(string);
    void loopVariables(int );

};
