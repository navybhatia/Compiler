#include <sstream>
#include <string>
#include <map>

using namespace std;

class Misc
{
	int label;
	map<string,string> FuncToLabel;
	map<string,string> LabelToFunc;
	public :
	Misc();
	string getLabel();
	string toString(int);
	string lookupFunc(string);
	string lookupLabel(string);
	void mapLabelToFunc(string,string);
	void mapFuncToLabel(string,string);
};
