#include "Misc.h"
string Misc::toString(int N)
{
	stringstream ss;
	ss<<N;
	return ss.str();
}
string Misc::getLabel()
{
	return "L"+toString(label++);
}
string Misc::lookupFunc(string label)
{
	if(LabelToFunc.find(label)!=LabelToFunc.end())
		return LabelToFunc[label];
	else
		return "";
}
string Misc::lookupLabel(string func)
{
	if(FuncToLabel.find(func)!=FuncToLabel.end())
		return FuncToLabel[func];
	else
		return "";
}
void Misc::mapFuncToLabel(string func, string label)
{
	FuncToLabel[func]=label;
}
void Misc::mapLabelToFunc(string label, string func)
{
	LabelToFunc[label]=func;
}
Misc::Misc()
{
	label=0;
}
