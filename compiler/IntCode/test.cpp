#include <iostream>
#include <map>
#include <utility> // make_pair
#include "RegTable.h"
using namespace std;
 
int main()
{
	RegTable t;
	cout<<t.getFreeIntReg()<<endl;
	cout<<t.getFreeFloatReg()<<endl;	
	MapEntry *r=new MapEntry(1);
	MapEntry *r1=new MapEntry(2);
	t.insert("r000",r);
	t.insert("f000",r1);
	t.printUsedIntList();
	t.del("r000");
	t.printUsedIntList();
	t.print();

}
