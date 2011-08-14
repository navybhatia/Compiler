#ifndef REGTABLE_H
#define REGTABLE_H
#include <iostream>
#include <map>
#include <list>
#include <utility> // make_pair
#include <stdlib.h>
#include <stdio.h>
//#define ERROR -1
//#define SUCCESS 0
using namespace std;
class MapEntry;
class RegTable;

class MapEntry
{
	int reg;
	int memaddr;
    public: 
	MapEntry(int address)
	{
		memaddr=address;
		//reg=-1 denotes not in register
		reg=-1;
	}
	bool isReg()
	{
		return (reg!=-1);
	}
	int setReg(int r)
	{
		reg=r;
	}	
	int getEntry()
	{
		if(reg!=-1)
			return reg;
		else
			return memaddr;
	}
	
};
typedef map<string, MapEntry*> mapType;
class RegTable
{
	private:
		mapType Map;
		list<string> freeIntRuleList;
		list<string> freeIntList;
		list<string> freeFloatList;
		list<string> usedIntList;
		list<string> usedFloatList;
	public:
		RegTable()
		{
			
			for(int i=0;i<900;i++)
			{
				char *r,*f;
				r=new char[4];
				f=new char[4];								
				sprintf(r,"R%03d",i);
				sprintf(f,"F%03d",i);
				freeIntList.push_back(r);
				freeFloatList.push_back(f);
			}		
			for(int i=900;i<996;i++)
			{
				char *r;
				r=new char[4];
				sprintf(r,"R%03d",i);
				freeIntRuleList.push_back(r);
			}		
			
		}
	        int insert(string key,MapEntry* r);
	        MapEntry* search(string key);
                int del(string key);
		void print();
		void printUsedIntList();
		void printUsedFloatList();
		int update(string key,MapEntry *value);
		string getFreeIntReg();
		string getFreeIntRegForRule();
		string getFreeFloatReg();
		int delReg(string key);
};
#endif
