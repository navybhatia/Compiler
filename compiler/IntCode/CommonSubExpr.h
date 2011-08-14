#include<vector>
#include<string>

using namespace std;

struct ExprEntry
{
	string expression;
	string arg1,arg2;
	string regstr;
};

class CSE
{
	vector <ExprEntry> exprlist;
	public:
	string searchExpr(string );
	void removeRefExpr(string );
	void insert(ExprEntry);
	void clear();
};
