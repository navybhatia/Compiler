#include "CommonSubExpr.h"
string CSE::searchExpr(string expr)
{
	for(int i=0;i<exprlist.size();i++)
		if(exprlist[i].expression==expr)
			return exprlist[i].regstr;
	return "";
}

void CSE::removeRefExpr(string ref)
{
	for(int i=0;i<exprlist.size();i++)
		if(exprlist[i].arg1==ref || exprlist[i].arg2==ref)
		{
			exprlist.erase(exprlist.begin()+i);
			i--;
		}
}
void CSE::insert(ExprEntry entry)
{
	exprlist.push_back(entry);
}

void CSE::clear()
{
	exprlist.clear();
}
