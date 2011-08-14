#include <iostream>
#include <map>
#include <utility> // make_pair
#include <string.h>
#include "RegTable.h"
#define ERROR -1
#define SUCCESS 0
using namespace std;


int RegTable::insert(string key,MapEntry* r)
{
	if(search(key) == NULL)
	{
		Map.insert(pair<string, MapEntry*>(key, r));
		return SUCCESS;
	}
	else
		return ERROR;
}
/* Returns NULL if record not found */
MapEntry* RegTable::search(string key)
{
	mapType::iterator iter = Map.find(key);
	if( iter != Map.end() ) 
		return iter->second;
	else
		return NULL;
}
/*Returns 0 on success and -1 on error*/
int RegTable::del(string key)
{
	
	mapType::iterator iter = Map.find(key);
	delReg(key);	
	if( iter == Map.end() )
	{
		return ERROR;
	}
	delete(iter->second);
	Map.erase(iter);
	return SUCCESS;
}
void RegTable::print()
{
	cout<<"Key \t Value\n";
	for(mapType::const_iterator it = Map.begin(); it != Map.end(); ++it)
	{
	        cout <<it->first;
	        cout <<" \t "<< it->second->getEntry() << '\n';
	}
}
/*Returns 0 on success and -1 in error*/
int RegTable::update(string key,MapEntry *value)
{
	mapType::iterator iter = Map.find(key);
        if( iter == Map.end() )
        {
              return ERROR;
        }
	del(key);
	insert(key,value);
        return SUCCESS;
}
/*Returns name of the free integer register*/
string RegTable::getFreeIntRegForRule()
{
	string s=freeIntRuleList.front();
	freeIntRuleList.pop_front();
	usedIntList.push_front(s);
	return s;
	
}

/*Returns name of the free integer register*/
string RegTable::getFreeIntReg()
{
	string s=freeIntList.front();
	freeIntList.pop_front();
	usedIntList.push_front(s);
	return s;
	
}
void RegTable::printUsedIntList()
{

	cout<<"Used Int List\n";
	list<string>::iterator current = usedIntList.begin();
	for(int i=0;i<usedIntList.size();i++)
	{
		cout<<*current<<endl;
		current++;
	}
}

/*Returns name of the free float register*/
string RegTable::getFreeFloatReg()
{
	string s=freeFloatList.front();
	freeFloatList.pop_front();
	usedFloatList.push_front(s);
	return s;
	
}
void RegTable::printUsedFloatList()
{
	cout<<"Used Float List\n";
	list<string>::iterator current = usedFloatList.begin();
	for(int i=0;i<usedFloatList.size();i++)
	{
		cout<<*current<<endl;
		current++;
	}
}

/*Returns 0 on success and -1 on failiure*/
int RegTable::delReg(string key)
{
	if(key[0]=='R')
	{
		list<string>::iterator current = freeIntList.begin();
		for(int i=1;i<freeIntList.size();i++)
		{
			if((*current)==key)
			{
				cout<<"Register already in free list\n";
				return -1;
			}
			current++;
		}
		freeIntList.push_front(key);
		usedIntList.remove(key);
		return 0;
	}
	else if(key[0]=='F')
	{
		list<string>::iterator current = freeFloatList.begin();
		for(int i=1;i<freeFloatList.size();i++)
		{
			if((*current)==key)
			{
				cout<<"Register already in free list\n";
				return -1;
			}
			current++;
		}
		freeFloatList.push_front(key);
		usedFloatList.remove(key);
		return 0;
 	}
	else
		return -1;
}



