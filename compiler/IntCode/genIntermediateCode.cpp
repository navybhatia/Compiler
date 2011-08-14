#include "genIntermediateCode.h"
//Stack Pointer is R999
//Return Address Reg is R998
//Base Pointer is R998
CSE exprObject;
CSE tempexprObject;
RegTable Table;
RegTable *functionTable;
MemManage *functionMMU;
MemManage MMU;
MemManage ruleMMU;
IntermediateCode Code;
IntermediateCode functionCode;
IntermediateCode finalCode;
Misc misc;
vector <string> activeRegisters;
list<string> whileLabels;
int inFunction;
int activeRegFill;
int DefinitionEnd=0;

list<RulePattern*> pattern;
RulePattern *global_pat = NULL;

vector<string> events;
int next_location[MAX_STATES][MAX_EVENTS];
int any_event = 0;
int curr_state = 0;
int pat_star = 0;
int pat_not = 0;
int final_state = 0;
int update_state = 0;
//#define Navdeep 1
string toString(int N);
int toAscii(string S);


bool isDouble(string str)
{
	for(int i=0;i<str.size();i++)
		if(str[i]=='.')
			return true;
	return false;
}
void printRegEx()
{
//#ifdef NAVDEEP

       cout<<endl;
       cout<<"=================================================";
       cout<<endl;
    for(int i = 0; i< MAX_STATES; i++)
        cout<<"\t"<<i;
    for(int j = 0; j< MAX_EVENTS; j++){
       cout<<endl;
       cout<<j;
       for(int i = 0; i< MAX_STATES; i++)
         cout<<"\t"<<next_location[i][j];
    }
       cout<<"================================================="<<endl;
       cout<<"Final_State ="<<final_state;
//#endif
}
void updateDataForStar(int curr_state, int temp_state)
{
    for(int j = 0; j< MAX_EVENTS; j++){
       if(next_location[curr_state-1][j] == curr_state) 
            next_location[curr_state -1][j] = temp_state;
    }
    
}
void setNextStateforAll(int cur_state, int val)
{
    for(int j = 0; j< MAX_EVENTS; j++)
     {
        if((next_location[cur_state][j] == -1) || (val == -1))
            next_location[cur_state][j] = val;
        else
            cout<<endl<<"need to debug this"<<endl;
     }
}
bool constructRegEx(RulePattern *rule_pat, int temp_index)
{
    vector<string> pat = rule_pat->getPattern();
    int index = temp_index -1;
    bool ret = true;

    if(pat[index] == ":") {
         //if(pat[2*temp_index-1] == "*")
          //   update_state = 1;
         pat_star = 0;
         ret = constructRegEx(rule_pat, 2*temp_index);

         //if(pat[2*temp_index-1] != "*")
         if(ret)
             curr_state++;
         else
             update_state = 0;

         ret = constructRegEx(rule_pat, 2*temp_index+1);
         if(ret)
             curr_state++;
         final_state = curr_state;
    }
    else if(pat[index] == "|") {
         pat_star = 0;
         constructRegEx(rule_pat, 2*temp_index);
         constructRegEx(rule_pat, 2*temp_index+1);
         update_state = 0;
         final_state = curr_state+1;
    }
    else if(pat[index] == "*") {
         pat_star = 1;
         //if((index > 0) && pat[temp_index/2 - 1] == ":")
           //  update_state = 1;
         int temp_state = curr_state;
         constructRegEx(rule_pat, 2*temp_index);
         pat_star = 0;
         update_state = 0;
         ret = false;
         //final_state = curr_state;
         if(temp_state != curr_state)
             updateDataForStar(curr_state, temp_state);
         else if(temp_state != final_state)
             updateDataForStar(final_state, temp_state);

         final_state = temp_state;
    }
    else if(pat[index] == "!") {
         pat_not = 1;
         setNextStateforAll(curr_state,curr_state+1);
         //memset(next_location[curr_state], curr_state+1, sizeof(int)*MAX_EVENTS);
         constructRegEx(rule_pat, 2*temp_index);
         pat_not = 0;
         final_state = curr_state+1;
    }
    else {
         if(pat_star) {
             if(pat[index] == "@") {
                  setNextStateforAll(curr_state,curr_state);
                  any_event = 1;
                  //memset(next_location[curr_state], curr_state, sizeof(int)*MAX_EVENTS);
               }
             else {
                  next_location[curr_state][toAscii(pat[index])] = curr_state + update_state;
                  any_event = 0;
             }
                  //Next_location[curr_state][toAscii(pat[index])-65] = curr_state + update_state;
             cout<<"state = "<<curr_state<<" "<<pat[index]<<" next state="<<curr_state<<endl;
         }
         else if(pat_not) {
             if(pat[index] == "@") {
                  setNextStateforAll(curr_state,-1);
                  any_event = 1;
                  //memset(next_location[curr_state], -1, sizeof(int)*MAX_EVENTS);
               }
             else {
                  next_location[curr_state][toAscii(pat[index])] = -1;
                  any_event = 0;
             }
                  //next_location[curr_state][toAscii(pat[index])-65] = -1;
             cout<<"state = "<<curr_state<<" "<<pat[index]<<" next state= -1"<<endl;
         }
         else {
             if(pat[index] == "@") {
                  setNextStateforAll(curr_state,curr_state+1);
                  any_event = 1;
                 // memset(next_location[curr_state], curr_state+1, sizeof(int)*MAX_EVENTS);
               }
             else if(next_location[curr_state][toAscii(pat[index])] == -1){
                  any_event = 0;
                  next_location[curr_state][toAscii(pat[index])] = curr_state+1;
                  //next_location[curr_state][toAscii(pat[index])-65] = curr_state+1;
             }
             else if(any_event) {
                  next_location[curr_state][toAscii(pat[index])] = curr_state+1;
                  if(pat[index-1] == "*") {
                      if(index/2 == 1) {
                          setNextStateforAll(curr_state+1,curr_state);
                          next_location[curr_state+1][toAscii(pat[index])] = curr_state+1;
                      }
                      else
                          next_location[curr_state][toAscii(pat[index])] = curr_state+2;
                          next_location[curr_state+2][toAscii(pat[index])] = curr_state+2;
                  }
                  any_event = 0;
                  //next_location[curr_state][toAscii(pat[index])-65] = curr_state+1;
             }
             else
                  ret = false;
         final_state = curr_state+1;
             cout<<"state = "<<curr_state<<" "<<pat[index]<<" next state="<<curr_state+1<<endl;
         }
    }
  
    return ret;
}

//Overloaded Functions to build instructions
void buildinstr(string instruction, string arg1)
{
	if(inFunction)
		functionCode.buildinstr(instruction,arg1);
	else
		Code.buildinstr(instruction,arg1);
	/*vector <string> args;
	args.push_back(arg1);
	Quadruple *quad=new Quadruple(instruction,args,"");
	Code.addCode(quad,type);*/
}

void buildinstr(string instruction, string arg1, string arg2)
{
	if(inFunction)
		functionCode.buildinstr(instruction,arg1,arg2);
	else
		Code.buildinstr(instruction,arg1,arg2);
/*	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	Quadruple *quad=new Quadruple(instruction,args,"");
	Code.addCode(quad);*/
}
void buildinstr(string instruction, string arg1, string arg2, string arg3)
{
	if(inFunction)
		functionCode.buildinstr(instruction,arg1,arg2,arg3);
	else
		Code.buildinstr(instruction,arg1,arg2,arg3);
	/*vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	Quadruple *quad=new Quadruple(instruction,args,"");
	Code.addCode(quad);*/
}

void buildinstr(string instruction, string arg1, string arg2, string arg3,string arg4)
{
	if(inFunction)
		functionCode.buildinstr(instruction,arg1,arg2,arg3,arg4);
	else
		Code.buildinstr(instruction,arg1,arg2,arg3,arg4);
}
/*
void buildinstrlb(string instruction, string arg1,string label)
{
	vector <string> args;
	args.push_back(arg1);
	Quadruple *quad=new Quadruple(instruction,args,label);
	Code.addCode(quad);
}

void buildinstrlb(string instruction, string arg1,string arg2, string label)
{
	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	Quadruple *quad=new Quadruple(instruction,args,label);
	Code.addCode(quad);
}

void buildinstrlb(string instruction, string arg1,string arg2, string arg3, string label)
{
	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	Quadruple *quad=new Quadruple(instruction,args,label);
	Code.addCode(quad);
}
*/
void buildinstrlabel(string label)
{
	if(inFunction)
		functionCode.buildinstrlabel(label);
	else
		Code.buildinstrlabel(label);
	/*vector <string> args;
	Quadruple *quad=new Quadruple("",args,label);
	Code.addCode(quad);*/
}


string toString(int N)
{
	stringstream ss;	
	string ret;
	ss<<N;
	ret = ss.str();
	return ret;
}
string toDouble(string ret)
{
	bool dot=false;
	for(int i=0;i<ret.size();i++)
		if(ret[i]=='.')
			dot=true;
	if(!dot)
		ret+=".0";
	return ret;
}
string toString(double N)
{
	stringstream ss;	
	string ret;
	ss<<N;
	ret = ss.str();
	return toDouble(ret);
}

int toAscii(string S)
{
     char c;
     int i ;
     sscanf(S.c_str(),"%c",&c);
     i = (int)c;
	//i= atoi(S.c_str());

    return i;
}

int toInt(string S)
{
	int N;
	stringstream ss(S);
	ss>>N;
	return N;
}

string IfNode::generateCode()
{
             string iflabel,value,lbl;
       iflabel=misc.getLabel();
       ExprNode *node=cond();       
      
       lbl=misc.getLabel();
       value = node->generateCode();

    if(node->exprNodeType()==ExprNode::REF_EXPR_NODE || node->exprNodeType()==ExprNode::INV_NODE)
        {
// 	   buildinstr("JMPC","EQ",value,"1","IF"+lbl);
  	   buildinstr("JMPC","EQ",value,"1",lbl);
	value=lbl;
        //   buildinstr("JMP",value);
        }
            
             //buildinstr("JMPC","EQ",value,"0",iflabel);
     
               if (else_ != NULL)
               {  
               //string elselabel=misc.getLabel();
               //buildinstrlabel(elselabel);
               //CompoundStmtNode *elsestmt=body();
               //if(node!=NULL)
               else_->generateCode();
               buildinstr("JMP", iflabel);
//             buildinstr("JMP", "ENDIF"+iflabel);
	       }
	       else
	       {
		       buildinstr("JMP", iflabel);
		       // buildinstr("JMP", "ENDIF"+iflabel);
	       }
	       buildinstrlabel(value);
	       //buildinstrlabel("IF"+value);
	       if(then_!=NULL)
	       {
		       then_->generateCode();
               }
               //buildinstrlabel("ENDIF"+iflabel);
               buildinstrlabel(iflabel);
    return "";


}

/*
void VariableEntry::generateCode()
{
	Type *var_type = type();	
	if(varKind()==VariableEntry::GLOBAL_VAR)
	{
		string Name=name();
		ExprNode *node=initVal();
		if(node!=NULL)
		{
			//const Value *valnode=node->value();
			string val=node->generateCode();
			int address=MMU.getAddress();
			MapEntry *entry=new MapEntry(address);
			Table.insert(Name,entry);
			MMU.incrementAddress();

					
			if(Type::isInt(var_type->tag()))
			{
				string regstr=Table.getFreeIntReg();
				buildinstr("MOVI",val,regstr);
				buildinstr("STI",regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isFloat(var_type->tag()))		
			{
				string regstr=Table.getFreeFloatReg();	
				if(Type::isFloat(node->coercedType()->tag()))
					buildinstr("MOVF",(val),regstr);
				else
					buildinstr("MOVIF",(val),regstr);
				buildinstr("STF", regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isBool(var_type->tag()))		
			{
				string regstr=Table.getFreeIntReg();
				buildinstr("MOVI",val,regstr);
				buildinstr("STI", regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isString(var_type->tag()))		
			{
				string val_temp = "\""+val+"\"";
				string regstr=Table.getFreeIntReg();
				buildinstr("MOVS",val_temp,regstr);
				buildinstr("STI", regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));	
			}
		}
		else
		{
			int address=MMU.getAddress();
			MapEntry *entry=new MapEntry(address);
			Table.insert(Name,entry);
			MMU.incrementAddress();
		}
	}
	else if(varKind()==VariableEntry::LOCAL_VAR)
	{
		string Name=name();
		ExprNode *node=initVal();
		if(node!=NULL)
		{
			//const Value *valnode=node->value();
			string val=node->generateCode();
			int address=functionMMU->getAddress();
			//cout<<address<<endl;
			MapEntry *entry=new MapEntry(address);
			functionTable->insert(Name,entry);
			functionMMU->incrementAddress();
			
			if(Type::isInt(var_type->tag()))
			{
				string regstr=Table.getFreeIntReg();
				string regstr1=Table.getFreeIntReg();
				buildinstr("MOVI",val,regstr);
				buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STI", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isFloat(var_type->tag()))		
			{
				string regstr=Table.getFreeFloatReg();
				string regstr1=Table.getFreeIntReg();
				if(Type::isFloat(node->coercedType()->tag()))
					buildinstr("MOVF",(val),regstr);
				else
					buildinstr("MOVIF",(val),regstr);
				buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STF", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");				
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isBool(var_type->tag()))		
			{
				string regstr=Table.getFreeIntReg();
				string regstr1=Table.getFreeIntReg();
				buildinstr("MOVI",val,regstr);
				buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STI", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");				
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isString(var_type->tag()))		
			{
				string val_temp = "\""+val+"\"";
				string regstr=Table.getFreeIntReg();
				string regstr1=Table.getFreeIntReg();
				buildinstr("MOVS",val,regstr);
				buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STI", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
		}
		else
		{
			int address=functionMMU->getAddress();
			MapEntry *entry=new MapEntry(address);
			functionTable->insert(Name,entry);
			functionMMU->incrementAddress();
		}
	}
	else if(varKind()==VariableEntry::PARAM_VAR)
	{
		string Name=name();
		int address=functionMMU->getAddress();
		MapEntry *entry=new MapEntry(address);
		functionTable->insert(Name,entry);
		functionMMU->incrementAddress();

		if(Type::isInt(var_type->tag()))
		{
			string regstr=Table.getFreeIntReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDI","R999",regstr);
			buildinstr("STI",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
		if(Type::isFloat(var_type->tag()))		
		{
			string regstr=Table.getFreeFloatReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDF","R999",regstr);
			buildinstr("STF",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
		if(Type::isBool(var_type->tag()))		
		{
			string regstr=Table.getFreeIntReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDI","R999",regstr);
			buildinstr("STI",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
		if(Type::isString(var_type->tag()))		
		{
			string regstr=Table.getFreeIntReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDI","R999",regstr);
			buildinstr("STI",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
	}
}*/


void VariableEntry::generateCode()
{
	Type *var_type = type();	
	//cout<<"Name : "<<name()<<endl;
	if(varKind()==VariableEntry::GLOBAL_VAR)
	{
		string Name=name();
		//cout<<"Global "<<Name<<endl;
		ExprNode *node=initVal();
		if(node!=NULL)
		{
			//const Value *valnode=node->value();
			string val=node->generateCode();
			int address=MMU.getAddress();
			MapEntry *entry=new MapEntry(address);
			Table.insert(Name,entry);
			MMU.incrementAddress();

					
			if(Type::isInt(var_type->tag()))
			{
				string regstr=Table.getFreeIntReg();
				buildinstr("MOVI",val,regstr);
				buildinstr("STI",regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isFloat(var_type->tag()))		
			{
				string regstr=Table.getFreeFloatReg();	
				if(Type::isFloat(node->coercedType()->tag()))
					buildinstr("MOVF",(val),regstr);
				else
					buildinstr("MOVIF",(val),regstr);
				buildinstr("STF", regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isBool(var_type->tag()))		
			{
				string regstr=Table.getFreeIntReg();
				buildinstr("MOVI",val,regstr);
				buildinstr("STI", regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isString(var_type->tag()))		
			{
				string val_temp = "\""+val+"\"";
				string regstr=Table.getFreeIntReg();
				buildinstr("MOVS",val_temp,regstr);
				buildinstr("STI", regstr,toString(entry->getEntry()));
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));	
			}
		}
		else
		{

			int address=MMU.getAddress();
			MapEntry *entry=new MapEntry(address);
			Table.insert(Name,entry);
			MMU.incrementAddress();
			string regstr;
			if(Type::isFloat(var_type->tag()))
			{
				regstr=Table.getFreeFloatReg();
				buildinstr("MOVF","0.0",regstr);
			}
			else
			{
				regstr=Table.getFreeIntReg();
				buildinstr("MOVI","0",regstr);
			}
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
	}
	else if(varKind()==VariableEntry::LOCAL_VAR)
	{
		string Name=name();
		ExprNode *node=initVal();
		if(node!=NULL)
		{
			//cout<<"LOCAL "<<Name<<endl;
			//const Value *valnode=node->value();
			string val=node->generateCode();
			int address=functionMMU->getAddress();
			//cout<<address<<endl;
			MapEntry *entry=new MapEntry(address);
			functionTable->insert(Name,entry);
			functionMMU->incrementAddress();
			string regstr=Table.getFreeIntReg();
			string regstr1=Table.getFreeIntReg();
			
			if(Type::isInt(var_type->tag()))
			{
				buildinstr("MOVI",val,regstr);
				/*buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STI", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");*/
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isFloat(var_type->tag()))		
			{
				if(Type::isFloat(node->coercedType()->tag()))
					buildinstr("MOVF",(val),regstr);
				else
					buildinstr("MOVIF",(val),regstr);
				/*buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STF", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");				*/
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isBool(var_type->tag()))		
			{
				buildinstr("MOVI",val,regstr);
				/*buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STI", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");				*/
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(Type::isString(var_type->tag()))		
			{
				string val_temp = "\""+val+"\"";
				buildinstr("MOVS",val,regstr);
				/*buildinstr("ADD",toString(entry->getEntry()),"R997",regstr1);
				buildinstr("STI", regstr,regstr1);
				buildinstr("ADD","R999","1","R999");*/
				entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			}
			if(activeRegFill)
				activeRegisters.push_back(regstr);
		}
		else
		{
			int address=functionMMU->getAddress();
			MapEntry *entry=new MapEntry(address);
			functionTable->insert(Name,entry);
			functionMMU->incrementAddress();
			string regstr=Table.getFreeIntReg();
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
	}
	else if(varKind()==VariableEntry::PARAM_VAR)
	{
		string Name=name();
		int address=functionMMU->getAddress();
		MapEntry *entry=new MapEntry(address);
		functionTable->insert(Name,entry);
		functionMMU->incrementAddress();

		if(Type::isInt(var_type->tag()))
		{
			string regstr=Table.getFreeIntReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDI","R999",regstr);
			buildinstr("STI",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
		if(Type::isFloat(var_type->tag()))		
		{
			string regstr=Table.getFreeFloatReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDF","R999",regstr);
			buildinstr("STF",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
		if(Type::isBool(var_type->tag()))		
		{
			string regstr=Table.getFreeIntReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDI","R999",regstr);
			buildinstr("STI",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
		if(Type::isString(var_type->tag()))		
		{
			string regstr=Table.getFreeIntReg();
			buildinstr("SUB","R999","1","R999");
			buildinstr("LDI","R999",regstr);
			buildinstr("STI",regstr,toString(entry->getEntry()));
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
		}
	}
}

void insert_common_sub(string arg1, string arg2, string regstr,string op)
{
	struct ExprEntry entry;
	entry.expression=arg1+op+arg2;
	entry.arg1=arg1;
	entry.arg2=arg2;
	entry.regstr=regstr;
	exprObject.insert(entry);
}

string PrintStmtNode::generateCode()
{
	vector <ExprNode*> *arg_list=args();
	for(int i=0;i<(arg_list->size());i++)
	{
		const Type *ctype=(*arg_list)[i]->coercedType();
		if(Type::isFloat(ctype->tag()))
		{
			string reg=(*arg_list)[i]->generateCode();
			buildinstr("PRTF",reg);
		}
		else if(Type::isString(ctype->tag()))
		{
			string reg=(*arg_list)[i]->generateCode();
			buildinstr("PRTS",reg);
		}
		else
		{
			string reg=(*arg_list)[i]->generateCode();
			buildinstr("PRTI",reg);
		}
	}
	return "";
}

string OpNode::generateCode()
{
	int ar=arity();
	const Type* ctype = coercedType();
	string regstr;

	if(ar==1)
	{
		ExprNode *node1=arg(0);


		if(Type::isInt(ctype->tag()))
		{
		        string val1=node1->generateCode();
			if(opCode()==OpNode::UMINUS)
			{	regstr=Table.getFreeIntReg();
				buildinstr("NEG",(val1),regstr);
			}
		}

		else if(Type::isFloat(ctype->tag()))
		{
		       string val1=node1->generateCode();
			
			if(opCode()==OpNode::UMINUS)
			{	regstr=Table.getFreeFloatReg();
				buildinstr("FNEG",(val1),regstr);
			}
		}
		else if(Type::isBool(ctype->tag()))
		{
				if(opCode()==OpNode::NOT)
				{
					string lbl1=node1->generateCode();
					string regstr_expr;
					cout<<"hi";
                                        if(node1->exprNodeType()==ExprNode::REF_EXPR_NODE || node1->exprNodeType()==ExprNode::INV_NODE){
                                               regstr_expr = lbl1;
						cout<<"Regst_expr:"<<regstr_expr<<endl;
                                               lbl1 = misc.getLabel();
                                               buildinstr("JMPC", "EQ", "0", regstr_expr,lbl1);
                                        }       
				return lbl1;
				}
		}
	}

	if(ar==2)
	{
		ExprNode *node1=arg(0);
		ExprNode *node2=arg(1);
		const Type* node1_type = node1->coercedType();
		const Type* node2_type = node2->coercedType();	

		if(Type::isFloat(node1_type->tag()))
		{
			if(opCode()==OpNode::ASSIGN)
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();
				if(Type::isFloat(node2->coercedType()->tag()))
					buildinstr("MOVF",val2,val1);
				else
					buildinstr("MOVIF",val2,val1);
				regstr = val1;
				RefExprNode *noderef=(RefExprNode *)node1;
				exprObject.removeRefExpr(noderef->ext());
			}
		}
		else
		{
			if(opCode()==OpNode::ASSIGN)
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();
				buildinstr("MOVI",val2,val1);
				regstr = val1;
				RefExprNode *noderef=(RefExprNode *)node1;
				exprObject.removeRefExpr(noderef->ext());
				cout<<noderef->ext()<<endl;
			}
		}
	
		if(Type::isBool(ctype->tag()))
		{  	string label=misc.getLabel();

			if(Type::isUnsigned(node1_type->tag()))
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();

				if(opCode()==OpNode::GT)
					buildinstr("JMPC","UGT",(val1),(val2),label);
				if(opCode()==OpNode::GE)
					buildinstr("JMPC","UGE",(val1),(val2),label);
			}
			else if(Type::isInt(node1_type->tag()) && Type::isInt(node2_type->tag()) )
			{	
				string val1=node1->generateCode();
				string val2=node2->generateCode();

				if(opCode()==OpNode::EQ)
				{	buildinstr("JMPC","EQ",(val1),(val2),label);
				}
				if(opCode()==OpNode::GT)
				{	buildinstr("JMPC","GT",(val1),(val2),label);
				}
				if(opCode()==OpNode::LT)
				{	buildinstr("JMPC","GT",(val2),(val1),label);
				}
				if(opCode()==OpNode::GE)
				{	buildinstr("JMPC","GE",(val1),(val2),label);
				}
				if(opCode()==OpNode::LE)
				{	buildinstr("JMPC","GE",(val2),(val1),label);
				}
				if(opCode()==OpNode::NE)
				{	buildinstr("JMPC","NE",(val1),(val2),label);
				}
			

			}
			else if(Type::isFloat(node1_type->tag()) || Type::isFloat(node2_type->tag()) )
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();
	
				if(opCode()==OpNode::GT)
					buildinstr("JMPC","FGT",(val1),(val2),label);
				if(opCode()==OpNode::LT)
					buildinstr("JMPC","FGT",(val2),(val1),label);
				if(opCode()==OpNode::GE)
					buildinstr("JMPC","FGE",(val1),(val2),label);
				if(opCode()==OpNode::LE)
					buildinstr("JMPC","FGE",(val2),(val1),label);
				if(opCode()==OpNode::EQ)
					buildinstr("JMPC","FEQ",(val1),(val2),label);
				if(opCode()==OpNode::NE)
					buildinstr("JMPC","FNE",(val1),(val2),label);
			}
			else
			{
				if(opCode()==OpNode::AND)
				{
					string lbl1 = node1->generateCode();
					string regstr_expr;
                                        if(node1->exprNodeType()==ExprNode::REF_EXPR_NODE || node1->exprNodeType()==ExprNode::INV_NODE){
                                               regstr_expr = lbl1;
                                               lbl1 = misc.getLabel();
					       buildinstr("JMPC", "EQ", "1", regstr_expr,lbl1);
                                        } 
					buildinstr("JMP", label);
					buildinstrlabel(lbl1);
					string lbl2 = node2->generateCode();
                                        if(node2->exprNodeType()==ExprNode::REF_EXPR_NODE || node2->exprNodeType()==ExprNode::INV_NODE ){
                                               regstr_expr = lbl2;
                                               lbl2 = misc.getLabel();
					       buildinstr("JMPC", "EQ", "1", regstr_expr,lbl2);
                                         } 
					buildinstrlabel(label);
                                        label = lbl2;
				}
				if(opCode()==OpNode::OR)
				{
                                        string regstr_expr;
					string lbl1 = node1->generateCode();
                                        if(node1->exprNodeType()==ExprNode::REF_EXPR_NODE || node1->exprNodeType()==ExprNode::INV_NODE ){
                                               regstr_expr = lbl1;
                                               lbl1 = misc.getLabel();
                                               buildinstr("JMPC", "EQ", "1", regstr_expr,lbl1);
                                        }
					string lbl2 = node2->generateCode();
                                        if(node2->exprNodeType()==ExprNode::REF_EXPR_NODE || node2->exprNodeType()==ExprNode::INV_NODE ){
                                               regstr_expr = lbl2;
                                               lbl2 = misc.getLabel();
                                               buildinstr("JMPC", "EQ", "1", regstr_expr,lbl2);
                                        }
                                  	buildinstr("JMP", label);
                                	buildinstrlabel(lbl1);
                                  	buildinstr("JMP", lbl2);
                                	buildinstrlabel(label);
                                        label = lbl2;

				}
			}
				regstr = label;
				return regstr;

		}
                else if(Type::isInt(ctype->tag()))
		{
			int cseflag=0;
			string arg1;
			string arg2;
			ExprNodeType exprType1=node1->exprNodeType();
			ExprNodeType exprType2=node2->exprNodeType();
			if((exprType1==ExprNode::VALUE_NODE || exprType1==ExprNode::REF_EXPR_NODE) && ((exprType2==ExprNode::VALUE_NODE || exprType2==ExprNode::REF_EXPR_NODE)))
			{
				if(exprType1==ExprNode::VALUE_NODE)
					arg1=node1->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node1;
					arg1=noderef->ext();
				}
				if(exprType2==ExprNode::VALUE_NODE)
					arg2=node2->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node2;
					arg2=noderef->ext();
				}
				cseflag=1;
			}
			string val1=node1->generateCode();
			string val2=node2->generateCode();

			regstr=Table.getFreeIntReg();
			if(opCode()==OpNode::PLUS)
			{
				if(cseflag)
				{
					string expression=arg1+"+"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"+");
				}
				buildinstr("ADD",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::MULT)
			{
				if(cseflag)
				{
					string expression=arg1+"*"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"*");
				}
				buildinstr("MUL",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::DIV)
			{
				if(cseflag)
				{
					string expression=arg1+"/"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"/");
				}
				buildinstr("DIV",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::MINUS)
			{
				if(cseflag)
				{
					string expression=arg1+"-"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"-");
				}
				buildinstr("SUB",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::MOD)
			{
				if(cseflag)
				{
					string expression=arg1+"%"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"%");
				}
				buildinstr("MOD",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::BITAND)
			{
				if(cseflag)
				{
					string expression=arg1+"&"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"&");
				}
				buildinstr("AND",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::BITOR)
			{
				if(cseflag)
				{
					string expression=arg1+"|"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"|");
				}
				buildinstr("OR",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::BITXOR)
			{
				if(cseflag)
				{
					string expression=arg1+"^"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"^");
				}
				buildinstr("XOR",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::SHL)
			{	
				if(cseflag)
				{
					string expression=arg1+"<<"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"<<");
				}
				cout<<"Not Valid"<<endl;			
				buildinstr("SHL",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::SHR)
			{
				if(cseflag)
				{
					string expression=arg1+">>"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,">>");
				}
				cout<<"Not Valid"<<endl;
				buildinstr("SHR",(val1),(val2),regstr);
			}	
		}

		else if(Type::isFloat(ctype->tag()))
		{
			int cseflag=0;
			string arg1;
			string arg2;
			ExprNodeType exprType1=node1->exprNodeType();
			ExprNodeType exprType2=node2->exprNodeType();
			if((exprType1==ExprNode::VALUE_NODE || exprType1==ExprNode::REF_EXPR_NODE) && ((exprType2==ExprNode::VALUE_NODE || exprType2==ExprNode::REF_EXPR_NODE)))
			{
				if(exprType1==ExprNode::VALUE_NODE)
					arg1=node1->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node1;
					arg1=noderef->ext();
				}
				if(exprType2==ExprNode::VALUE_NODE)
					arg2=node2->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node2;
					arg2=noderef->ext();
				}
				cseflag=1;
			}
			string val1=node1->generateCode();
			string val2=node2->generateCode();

			regstr=Table.getFreeFloatReg();
			if(opCode()==OpNode::PLUS)
			{
				if(cseflag)
				{
					string expression=arg1+"+"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"+");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FADD",(reg1),(reg2),regstr);
			}
			if(opCode()==OpNode::MULT)
			{
				if(cseflag)
				{
					string expression=arg1+"*"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"*");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FMUL",(reg1),(reg2),regstr);
			}
			if(opCode()==OpNode::DIV)
			{
				if(cseflag)
				{
					string expression=arg1+"/"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"/");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FDIV",(reg1),(reg2),regstr);
			}
			if(opCode()==OpNode::MINUS)
			{
				if(cseflag)
				{
					string expression=arg1+"-"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"-");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FSUB",(reg1),(reg2),regstr);
			}
		
		}
		if(activeRegFill)
			activeRegisters.push_back(regstr);
		return (regstr);
	}
}

/*
string OpNode::generateCode()
{
	int ar=arity();
	const Type* ctype = coercedType();
	string regstr;

	if(ar==1)
	{
		ExprNode *node1=arg(0);


		if(Type::isInt(ctype->tag()))
		{
		        string val1=node1->generateCode();
			if(opCode()==OpNode::UMINUS)
			{	regstr=Table.getFreeIntReg();
				buildinstr("NEG",(val1),regstr);
			}
		}

		else if(Type::isFloat(ctype->tag()))
		{
		       string val1=node1->generateCode();
			
			if(opCode()==OpNode::UMINUS)
			{	regstr=Table.getFreeFloatReg();
				buildinstr("FNEG",(val1),regstr);
			}
		}
		else if(Type::isBool(ctype->tag()))
		{
				if(opCode()==OpNode::NOT)
				{
					string lbl1=node1->generateCode();
					string regstr_expr;
					cout<<"hi";
                                        if(node1->exprNodeType()==ExprNode::REF_EXPR_NODE || node1->exprNodeType()==ExprNode::INV_NODE){
                                               regstr_expr = lbl1;
						cout<<"Regst_expr:"<<regstr_expr<<endl;
                                               lbl1 = misc.getLabel();
                                               buildinstr("JMPC", "EQ", "0", regstr_expr,lbl1);
                                        }       
				return lbl1;
				}
		}
	}

	if(ar==2)
	{
		ExprNode *node1=arg(0);
		ExprNode *node2=arg(1);
		const Type* node1_type = node1->coercedType();
		const Type* node2_type = node2->coercedType();	

		if(Type::isFloat(node1_type->tag()))
		{
			if(opCode()==OpNode::ASSIGN)
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();
				if(Type::isFloat(node2->coercedType()->tag()))
					buildinstr("MOVF",val2,val1);
				else
					buildinstr("MOVIF",val2,val1);
				regstr = val1;
				RefExprNode *noderef=(RefExprNode *)node1;
				exprObject.removeRefExpr(noderef->ext());
			}
		}
		else
		{
			if(opCode()==OpNode::ASSIGN)
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();
				buildinstr("MOVI",val2,val1);
				regstr = val1;
				RefExprNode *noderef=(RefExprNode *)node1;
				exprObject.removeRefExpr(noderef->ext());
			}
		}
	
		if(Type::isBool(ctype->tag()))
		{  	string label=misc.getLabel();

			if(Type::isUnsigned(node1_type->tag()))
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();

				if(opCode()==OpNode::GT)
					buildinstr("JMPC","UGT",(val1),(val2),label);
				if(opCode()==OpNode::GE)
					buildinstr("JMPC","UGE",(val1),(val2),label);
			}
			else if(Type::isInt(node1_type->tag()) && Type::isInt(node2_type->tag()) )
			{	
				string val1=node1->generateCode();
				string val2=node2->generateCode();

				if(opCode()==OpNode::EQ)
				{	buildinstr("JMPC","EQ",(val1),(val2),label);
				}
				if(opCode()==OpNode::GT)
				{	buildinstr("JMPC","GT",(val1),(val2),label);
				}
				if(opCode()==OpNode::LT)
				{	buildinstr("JMPC","GT",(val2),(val1),label);
				}
				if(opCode()==OpNode::GE)
				{	buildinstr("JMPC","GE",(val1),(val2),label);
				}
				if(opCode()==OpNode::LE)
				{	buildinstr("JMPC","GE",(val2),(val1),label);
				}
				if(opCode()==OpNode::NE)
				{	buildinstr("JMPC","NE",(val1),(val2),label);
				}
			

			}
			else if(Type::isFloat(node1_type->tag()) || Type::isFloat(node2_type->tag()) )
			{
				string val1=node1->generateCode();
				string val2=node2->generateCode();
	
				if(opCode()==OpNode::GT)
					buildinstr("JMPC","FGT",(val1),(val2),label);
				if(opCode()==OpNode::LT)
					buildinstr("JMPC","FGT",(val2),(val1),label);
				if(opCode()==OpNode::GE)
					buildinstr("JMPC","FGE",(val1),(val2),label);
				if(opCode()==OpNode::LE)
					buildinstr("JMPC","FGE",(val2),(val1),label);
				if(opCode()==OpNode::EQ)
					buildinstr("JMPC","FEQ",(val1),(val2),label);
				if(opCode()==OpNode::NE)
					buildinstr("JMPC","FNE",(val1),(val2),label);
			}
			else
			{
				if(opCode()==OpNode::AND)
				{
					string lbl1 = node1->generateCode();
					string regstr_expr;
                                        if(node1->exprNodeType()==ExprNode::REF_EXPR_NODE || node1->exprNodeType()==ExprNode::INV_NODE){
                                               regstr_expr = lbl1;
                                               lbl1 = misc.getLabel();
					       buildinstr("JMPC", "EQ", "1", regstr_expr,lbl1);
                                        } 
					buildinstr("JMP", label);
					buildinstrlabel(lbl1);
					string lbl2 = node2->generateCode();
                                        if(node2->exprNodeType()==ExprNode::REF_EXPR_NODE || node2->exprNodeType()==ExprNode::INV_NODE ){
                                               regstr_expr = lbl2;
                                               lbl2 = misc.getLabel();
					       buildinstr("JMPC", "EQ", "1", regstr_expr,lbl2);
                                         } 
					buildinstrlabel(label);
                                        label = lbl2;
				}
				if(opCode()==OpNode::OR)
				{
                                        string regstr_expr;
					string lbl1 = node1->generateCode();
                                        if(node1->exprNodeType()==ExprNode::REF_EXPR_NODE || node1->exprNodeType()==ExprNode::INV_NODE ){
                                               regstr_expr = lbl1;
                                               lbl1 = misc.getLabel();
                                               buildinstr("JMPC", "EQ", "1", regstr_expr,lbl1);
                                        }
					string lbl2 = node2->generateCode();
                                        if(node2->exprNodeType()==ExprNode::REF_EXPR_NODE || node2->exprNodeType()==ExprNode::INV_NODE ){
                                               regstr_expr = lbl2;
                                               lbl2 = misc.getLabel();
                                               buildinstr("JMPC", "EQ", "1", regstr_expr,lbl2);
                                        }
                                  	buildinstr("JMP", label);
                                	buildinstrlabel(lbl1);
                                  	buildinstr("JMP", lbl2);
                                	buildinstrlabel(label);
                                        label = lbl2;

				}
			}
				regstr = label;
				return regstr;

		}
                else if(Type::isInt(ctype->tag()))
		{
			int cseflag=0;
			string arg1;
			string arg2;
			ExprNodeType exprType1=node1->exprNodeType();
			ExprNodeType exprType2=node2->exprNodeType();
			if((exprType1==ExprNode::VALUE_NODE || exprType1==ExprNode::REF_EXPR_NODE) && ((exprType2==ExprNode::VALUE_NODE || exprType2==ExprNode::REF_EXPR_NODE)))
			{
				if(exprType1==ExprNode::VALUE_NODE)
					arg1=node1->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node1;
					arg1=noderef->ext();
				}
				if(exprType2==ExprNode::VALUE_NODE)
					arg2=node2->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node2;
					arg2=noderef->ext();
				}
				cseflag=1;
			}
			string val1=node1->generateCode();
			string val2=node2->generateCode();

			regstr=Table.getFreeIntReg();
			if(opCode()==OpNode::PLUS)
			{
				if(cseflag)
				{
					string expression=arg1+"+"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"+");
				}
				buildinstr("ADD",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::MULT)
			{
				if(cseflag)
				{
					string expression=arg1+"*"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"*");
				}
				buildinstr("MUL",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::DIV)
			{
				if(cseflag)
				{
					string expression=arg1+"/"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"/");
				}
				buildinstr("DIV",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::MINUS)
			{
				if(cseflag)
				{
					string expression=arg1+"-"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"-");
				}
				buildinstr("SUB",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::MOD)
			{
				if(cseflag)
				{
					string expression=arg1+"%"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"%");
				}
				buildinstr("MOD",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::BITAND)
			{
				if(cseflag)
				{
					string expression=arg1+"&"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"&");
				}
				buildinstr("AND",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::BITOR)
			{
				if(cseflag)
				{
					string expression=arg1+"|"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"|");
				}
				buildinstr("OR",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::BITXOR)
			{
				if(cseflag)
				{
					string expression=arg1+"^"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"^");
				}
				buildinstr("XOR",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::SHL)
			{	
				if(cseflag)
				{
					string expression=arg1+"<<"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"<<");
				}
				cout<<"Not Valid"<<endl;			
				buildinstr("SHL",(val1),(val2),regstr);
			}
			if(opCode()==OpNode::SHR)
			{
				if(cseflag)
				{
					string expression=arg1+">>"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,">>");
				}
				cout<<"Not Valid"<<endl;
				buildinstr("SHR",(val1),(val2),regstr);
			}	
		}

		else if(Type::isFloat(ctype->tag()))
		{
			int cseflag=0;
			string arg1;
			string arg2;
			ExprNodeType exprType1=node1->exprNodeType();
			ExprNodeType exprType2=node2->exprNodeType();
			if((exprType1==ExprNode::VALUE_NODE || exprType1==ExprNode::REF_EXPR_NODE) && ((exprType2==ExprNode::VALUE_NODE || exprType2==ExprNode::REF_EXPR_NODE)))
			{
				if(exprType1==ExprNode::VALUE_NODE)
					arg1=node1->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node1;
					arg1=noderef->ext();
				}
				if(exprType2==ExprNode::VALUE_NODE)
					arg2=node2->value()->ival();
				else
				{
					RefExprNode *noderef=(RefExprNode *)node2;
					arg2=noderef->ext();
				}
				cseflag=1;
			}
			string val1=node1->generateCode();
			string val2=node2->generateCode();

			regstr=Table.getFreeFloatReg();
			if(opCode()==OpNode::PLUS)
			{
				if(cseflag)
				{
					string expression=arg1+"+"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"+");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FADD",(reg1),(reg2),regstr);
			}
			if(opCode()==OpNode::MULT)
			{
				if(cseflag)
				{
					string expression=arg1+"*"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"*");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FMUL",(reg1),(reg2),regstr);
			}
			if(opCode()==OpNode::DIV)
			{
				if(cseflag)
				{
					string expression=arg1+"/"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"/");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FDIV",(reg1),(reg2),regstr);
			}
			if(opCode()==OpNode::MINUS)
			{
				if(cseflag)
				{
					string expression=arg1+"-"+arg2;
					string cseregstr=exprObject.searchExpr(expression);
					if(cseregstr!="")
						return cseregstr;
					insert_common_sub(arg1, arg2, regstr,"-");
				}
				string reg1=val1;
				string reg2=val2;
				if(Type::isInt(node1->coercedType()->tag()))
				{
					reg1=Table.getFreeFloatReg();
					buildinstr("MOVIF",val1,reg1);
				}
				if(Type::isInt(node2->coercedType()->tag()))
				{
					reg2=Table.getFreeFloatReg();
					buildinstr("MOVIF",val2,reg2);
				}
				buildinstr("FSUB",(reg1),(reg2),regstr);
			}
		
		}
		if(activeRegFill)
			activeRegisters.push_back(regstr);
		return (regstr);
	}
}
*/
string RefExprNode::generateCode()
{
	//const SymTabEntry *ste=symTabEntry();
	const VariableEntry *ste=(VariableEntry *)symTabEntry();
	string name=ext();
	if(ste->varKind()==VariableEntry::GLOBAL_VAR)
	{
		const Type* ctype = coercedType();
		MapEntry *entry=Table.search(name);
		int address=entry->getEntry();
		if(entry->isReg())
		{
			string id=toString(address);
			string regstr;
			int l=id.size();
			for(int i=0;i<max(0,3-l);i++)
				id="0"+id;
			if(Type::isFloat(ctype->tag()))
				regstr="F"+id;
			else
				regstr="R"+id;
			return regstr;
		}
		else
		{
			string reg;
			if(Type::isFloat(ctype->tag()))
			{
				reg=Table.getFreeFloatReg();
				entry->setReg(toInt(reg.substr(1,reg.size()-1)));
				buildinstr("LDF",toString(address),reg);
			}
			else
			{
				reg=Table.getFreeIntReg();
				entry->setReg(toInt(reg.substr(1,reg.size()-1)));
				buildinstr("LDI",toString(address),reg);
			}
			return reg;
		}
	}
	else if(ste->varKind()==VariableEntry::PARAM_VAR)
	{
		const Type *ctype = ste->type();
		MapEntry *entry=functionTable->search(name);
		int address=entry->getEntry();
		if(entry->isReg())
		{
			string id=toString(address);
			string regstr;
			int l=id.size();
			for(int i=0;i<max(0,3-l);i++)
				id="0"+id;
			if(Type::isFloat(ctype->tag()))
				regstr="F"+id;
			else
				regstr="R"+id;
			return regstr;
		}
		else
		{
			string reg1=Table.getFreeIntReg();
			buildinstr("ADD","R997",toString(address),reg1);
			string regstr;
			if(Type::isFloat(ctype->tag()))
			{
				regstr=Table.getFreeFloatReg();
				buildinstr("LDF",reg1,regstr);
			}
			else
			{
				regstr=Table.getFreeIntReg();
				buildinstr("LDI",reg1,regstr);
			}
			if(activeRegFill)
				activeRegisters.push_back(regstr);
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			return regstr;
		}
	}
	else if(ste->varKind()==VariableEntry::LOCAL_VAR)
	{
		const Type *ctype = ste->type();
		MapEntry *entry=functionTable->search(name);
		//cout<<"LOCAL "<<name<<endl;
		int address=entry->getEntry();
		if(entry->isReg())
		{
			string id=toString(address);
			string regstr;
			int l=id.size();
			for(int i=0;i<max(0,3-l);i++)
				id="0"+id;
			if(Type::isFloat(ctype->tag()))
				regstr="F"+id;
			else
				regstr="R"+id;
			return regstr;
		}
		else
		{
			string reg1=Table.getFreeIntReg();
			buildinstr("ADD","R997",toString(address),reg1);
			string regstr;
			if(Type::isFloat(ctype->tag()))
			{
				regstr=Table.getFreeFloatReg();
				buildinstr("LDF",reg1,regstr);
			}
			else
			{
				regstr=Table.getFreeIntReg();
				buildinstr("LDI",reg1,regstr);
			}
			if(activeRegFill)
				activeRegisters.push_back(regstr);
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			return regstr;
		}
	}
	return "";
}

/*
string RefExprNode::generateCode()
{
	//const SymTabEntry *ste=symTabEntry();
	const VariableEntry *ste=(VariableEntry *)symTabEntry();
	string name=ext();
	if(ste->varKind()==VariableEntry::GLOBAL_VAR)
	{
		const Type* ctype = coercedType();
		MapEntry *entry=Table.search(name);
		int address=entry->getEntry();
		if(entry->isReg())
		{
			string id=toString(address);
			string regstr;
			int l=id.size();
			for(int i=0;i<max(0,3-l);i++)
				id="0"+id;
			if(Type::isFloat(ctype->tag()))
				regstr="F"+id;
			else
				regstr="R"+id;
			return regstr;
		}
		else
		{
			string reg;
			if(Type::isFloat(ctype->tag()))
			{
				reg=Table.getFreeFloatReg();
				entry->setReg(toInt(reg.substr(1,reg.size()-1)));
				buildinstr("LDF",toString(address),reg);
			}
			else
			{
				reg=Table.getFreeIntReg();
				entry->setReg(toInt(reg.substr(1,reg.size()-1)));
				buildinstr("LDI",toString(address),reg);
			}
			return reg;
		}
	}
	else if(ste->varKind()==VariableEntry::PARAM_VAR)
	{
		const Type *ctype = ste->type();
		MapEntry *entry=functionTable->search(name);
		int address=entry->getEntry();
		if(entry->isReg())
		{
			string id=toString(address);
			string regstr;
			int l=id.size();
			for(int i=0;i<max(0,3-l);i++)
				id="0"+id;
			if(Type::isFloat(ctype->tag()))
				regstr="F"+id;
			else
				regstr="R"+id;
			return regstr;
		}
		else
		{
			string reg1=Table.getFreeIntReg();
			buildinstr("ADD","R997",toString(address),reg1);
			string regstr;
			if(Type::isFloat(ctype->tag()))
			{
				regstr=Table.getFreeFloatReg();
				buildinstr("LDF",reg1,regstr);
			}
			else
			{
				regstr=Table.getFreeIntReg();
				buildinstr("LDI",reg1,regstr);
			}
			if(activeRegFill)
				activeRegisters.push_back(regstr);
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			return regstr;
		}
	}
	else if(ste->varKind()==VariableEntry::LOCAL_VAR)
	{
		const Type *ctype = ste->type();
		MapEntry *entry=functionTable->search(name);
		int address=entry->getEntry();
		if(entry->isReg())
		{
			string id=toString(address);
			string regstr;
			int l=id.size();
			for(int i=0;i<max(0,3-l);i++)
				id="0"+id;
			if(Type::isFloat(ctype->tag()))
				regstr="F"+id;
			else
				regstr="R"+id;
			return regstr;
		}
		else
		{
			string reg1=Table.getFreeIntReg();
			buildinstr("ADD","R997",toString(address),reg1);
			string regstr;
			if(Type::isFloat(ctype->tag()))
			{
				regstr=Table.getFreeFloatReg();
				buildinstr("LDF",reg1,regstr);
			}
			else
			{
				regstr=Table.getFreeIntReg();
				buildinstr("LDI",reg1,regstr);
			}
			if(activeRegFill)
				activeRegisters.push_back(regstr);
			entry->setReg(toInt(regstr.substr(1,regstr.size()-1)));
			return regstr;
		}
	}
	return "";
}
*/
string ValueNode::generateCode()
{
	Type *ctype = type();	
	if(Type::isInt(ctype->tag()))
		return toString(value()->ival());
	if(Type::isFloat(ctype->tag()))
		return toString(value()->dval());
	if(Type::isBool(ctype->tag()))
		return toString(value()->bval());
	if(Type::isString(ctype->tag()))
		return ("\""+value()->sval()+"\"");
}
/*
string ValueNode::generateCode()
{
	Type *ctype = type();	
	if(Type::isInt(ctype->tag()))
		return toString(value()->ival());
	if(Type::isFloat(ctype->tag()))
		return toString(value()->dval());
	if(Type::isBool(ctype->tag()))
		return toString(value()->bval());
	if(Type::isString(ctype->tag()))
		return (value()->sval());
}*/

string CompoundStmtNode::generateCode()
{
	list <StmtNode*> *statements=stmts();
	list <StmtNode*> :: iterator it;
	string temp;
	if(statements!=NULL)
		for(it=statements->begin();it!=statements->end();it++)
			if((*it)!=NULL)
				temp=(*it)->generateCode();
	return temp;
}

void FunctionEntry::generateCode()
{
	//Save to temporary CSE object
	tempexprObject=exprObject;
	exprObject.clear();

	//Function entry initialization
	inFunction=1;
	string fname=name();
	activeRegisters.clear();
	activeRegFill=1;
	//activeRegisters.push_back("R999");
	activeRegisters.push_back("R998");
	activeRegisters.push_back("R997");
	//activeRegisters.push_back("R996");
	
	//Building Tables and corresponding labels
	functionTable=new RegTable(); 
	functionMMU=new MemManage();
	string label=misc.getLabel();
	label = "F" + label;	
	misc.mapFuncToLabel(fname,label);
	misc.mapLabelToFunc(label,fname);
	buildinstrlabel(label);

	Type *ty=type();
	int psize=ty->arity();
	functionMMU->setAddress(psize);
	int i=0;
	buildinstr("MOVI","R999","R997");
	buildinstr("SUB","R997","1","R997");
	buildinstr("LDI","R997","R998");
	SymTab *st = symTab();
	if(st!=NULL)
	{
		SymTab::iterator it=st->begin();
		if(it!=NULL)
			for (; (it != st->end()) && i<psize; ++it,i++) 
			{
				if((*it)==NULL)
					continue;
				VariableEntry *ste=(VariableEntry* )(*it);
				string Name=ste->name();
				MapEntry *entry=new MapEntry(psize-i-1);
				functionTable->insert(Name,entry);
				buildinstr("SUB","R997","1","R997");
			}
		st->generateCode(psize,st->size());
	}

	CompoundStmtNode *node=body();
	string temp;
	if(node!=NULL)
		temp=node->generateCode();
	buildinstr("JMPI","R998");

	//Processing required after function ends
	delete functionTable;
	delete functionMMU;
	inFunction=0;
	activeRegFill=0;
	activeRegisters.clear();

	//Restore expr Object
	exprObject=tempexprObject;
}
#if 0
void FunctionEntry::generateCode()
{
	inFunction=1;
	string fname=name();
	activeRegisters.clear();
	functionTable=new RegTable(); 
	functionMMU=new MemManage();
	string label=misc.getLabel();
	misc.mapFuncToLabel(fname,label);
	misc.mapLabelToFunc(label,fname);
	buildinstrlabel(label);

	SymTab *st = symTab();
	Type *ty=type();
	int psize=ty->arity();
	functionMMU->setAddress(psize);
	int i=0;
	buildinstr("MOVI","R999","R997");
	buildinstr("SUB","R997","1","R997");
	buildinstr("LDI","R997","R998");
	for (SymTab::iterator it = st->begin(); (it != st->end()) && i<psize; ++it,i++) 
	{
		VariableEntry *ste=(VariableEntry* )(*it);
		string Name=ste->name();
		MapEntry *entry=new MapEntry(psize-i-1);
		functionTable->insert(Name,entry);
		buildinstr("SUB","R997","1","R997");
	}

	st->generateCode(psize,st->size());
	CompoundStmtNode *node=body();
	string temp;
	if(node!=NULL)
		temp=node->generateCode();
	buildinstr("JMPI","R998");
	delete functionTable;
	delete functionMMU;
	inFunction=0;
	activeRegisters.clear();
}
#endif

string ExprStmtNode::generateCode()
{
	return expr_->generateCode();
}
string ReturnStmtNode::generateCode()
{
	FunctionEntry *fe=func();
	Type *tty=fe->type();
	const Type *ty=tty->retType();
	if(ty->isFloat(ty->tag()))
	{
		string val=expr_->generateCode();
		buildinstr("MOVF",val,"F996");
	}
	else
	{
		string val=expr_->generateCode();
		buildinstr("MOVI",val,"R996");
	}
	buildinstr("JMPI","R998");

	return "";
}

string InvocationNode::generateCode()
{
	const SymTabEntry *ste=symTabEntry();
	string fname=ste->name();
	string label=misc.lookupLabel(fname);
	string rlabel=misc.getLabel();
	vector <ExprNode*> *args=params();

	int size;
	if(args!=NULL)
		size=args->size();
	else
		size=0;
	string reglabel=Table.getFreeIntReg();
	if(inFunction)
	{
		//activeRegisters.push_back(reglabel);
		for(int i=0;i<activeRegisters.size();i++)
		{
			if(activeRegisters[i][0]=='R')
			{
				buildinstr("STI",activeRegisters[i],"R999");
				buildinstr("ADD","R999","1","R999");
			}
			else if(activeRegisters[i][0]=='F')
			{
				buildinstr("STF",activeRegisters[i],"R999");
				buildinstr("ADD","R999","1","R999");
			}
			else if(isDouble(activeRegisters[i]))
			{
				buildinstr("STF",activeRegisters[i],"R999");
				buildinstr("ADD","R999","1","R999");
			}
			else
			{
				buildinstr("STI",activeRegisters[i],"R999");
				buildinstr("ADD","R999","1","R999");
			}
		}
	}
	activeRegFill=0;
	for(int i=size-1;i>=0;i--)
	{
		string reg=(*args)[i]->generateCode();
		if(reg[0]=='R')
		{
			buildinstr("STI",reg,"R999");
			buildinstr("ADD","R999","1","R999");
		}
		else if(reg[0]=='F')
		{
			buildinstr("STF",reg,"R999");
			buildinstr("ADD","R999","1","R999");
		}
		else if(isDouble(reg))
		{
			buildinstr("STF",reg,"R999");
			buildinstr("ADD","R999","1","R999");
		}
		else
		{
			buildinstr("STI",reg,"R999");
			buildinstr("ADD","R999","1","R999");
		}

	}
	activeRegFill=1;
	buildinstr("MOVL",rlabel,reglabel);
	buildinstr("STI",reglabel,"R999");
	buildinstr("ADD","R999","1","R999");
	buildinstr("JMP",label);
	buildinstrlabel(rlabel);
	buildinstr("MOVI","R997","R999");
	buildinstr("SUB","R999","1","R999");
	if(inFunction)
	{
		for(int i=activeRegisters.size()-1;i>=0;i--)
		{
			if(activeRegisters[i][0]=='R')
			{
				buildinstr("LDI","R999",activeRegisters[i]);
				buildinstr("SUB","R999","1","R999");
			}
			else if(activeRegisters[i][0]=='F')
			{
				buildinstr("LDF","R999",activeRegisters[i]);
				buildinstr("SUB","R999","1","R999");
			}
			else if(isDouble(activeRegisters[i]))
			{
				buildinstr("LDF","R999",activeRegisters[i]);
				buildinstr("ADD","R999","1","R999");
			}
			else
			{
				buildinstr("LDI","R999",activeRegisters[i]);
				buildinstr("ADD","R999","1","R999");
			}
		}
	}
	const Type *tty=ste->type();
	const Type *ty=tty->retType();
	string regstr;
	if(ty->isFloat(ty->tag()))
	{
		regstr=Table.getFreeFloatReg();
		buildinstr("MOVF","F996",regstr);
	}
	else
	{
		regstr=Table.getFreeIntReg();
		buildinstr("MOVI","R996",regstr);
	}
	activeRegisters.push_back(regstr);
	return regstr; 
}
#if 0
string InvocationNode::generateCode()
{
	const SymTabEntry *ste=symTabEntry();
	string fname=ste->name();
	string label=misc.lookupLabel(fname);
	string rlabel=misc.getLabel();
	vector <ExprNode*> *args=params();

	int size=args->size();
	for(int i=size-1;i>=0;i--)
	{
		string reg=(*args)[i]->generateCode();
		buildinstr("STI",reg,"R999");
		buildinstr("ADD","R999","1","R999");
	}
	string reglabel=Table.getFreeIntReg();
	buildinstr("MOVL",rlabel,reglabel);
	buildinstr("STI",reglabel,"R999");
	buildinstr("ADD","R999","1","R999");
	buildinstr("JMP",label);
	buildinstrlabel(rlabel);
	return "";
}
#endif

string PatNode::generatePatCode(int index) const
{
    //global_pat->insertPat("\"" + ee_->name() + "\"");
    if(kind() == BasePatNode::SEQ) {
        global_pat->insertPat(index, ":");
        ruleMMU.incrementAddress();
        pat1_->generatePatCode(2*index);
        pat2_->generatePatCode(2*index + 1);
    }
    else if(kind() == BasePatNode::NEG) {
        global_pat->insertPat(index, "!");
        ruleMMU.incrementAddress();
        pat1_->generatePatCode(2*index);
        global_pat->insertPat(2*index + 1, "$");
        ruleMMU.incrementAddress();
    }
    else if(kind() == BasePatNode::OR) {
        global_pat->insertPat(index, "|");
        ruleMMU.incrementAddress();
        pat1_->generatePatCode(2*index);
        pat2_->generatePatCode(2*index + 1);
    }
    else if(kind() == BasePatNode::STAR) {
        global_pat->insertPat(index, "*");
        ruleMMU.incrementAddress();
        pat1_->generatePatCode(2*index);
        global_pat->insertPat(2*index+1, "$");
        ruleMMU.incrementAddress();
    }
 
    return "";
}

void EventEntry::generateCode()
{
    if(string("any") != name()) {
        events.push_back(name());
    }
}

string PrimitivePatNode::generatePatCode(int index) const
{
    //global_pat->insertPat("\"" + ee_->name() + "\"");
    if(string("any") != ee_->name()) {
        global_pat->insertPat(index, ee_->name());
       // events.push_back(ee_->name());
    }
    else {
        global_pat->insertPat(index, "@");
    }
        
    ruleMMU.incrementAddress();

    return "";
}

string RuleNode::generateCode()
{
	//Save Common SubExpressions
	tempexprObject=exprObject;
	exprObject.clear();

	string ruleLabel=misc.getLabel();
	ruleLabel = "R" + ruleLabel;
	string jmplabel=misc.getLabel();
	string reg =Table.getFreeIntRegForRule();
	string curStateReg =Table.getFreeIntRegForRule();
	string finalStateReg =Table.getFreeIntRegForRule();
        int mem = ruleMMU.getAddress();
        int len = 0;
	//cout<<"FLAG2"<<endl;
	//buildinstr("MOVI",toString(mem),reg);
        global_pat = new RulePattern;

        pat_->generatePatCode(1);

        //ruleMMU.incrementAddress();
        //len = ruleMMU.getAddress() - mem;
        len = global_pat->getPatSize();
        cout<<"len = "<<len<<endl;
	//buildinstr("STI","0",reg);
        //allocate DS to be used by GEntry
        global_pat->setRegister(reg);
        global_pat->setCurStateRegister(curStateReg);
        global_pat->setFinalStateRegister(finalStateReg);
        global_pat->setRuleLabel(ruleLabel);
        global_pat->setOutLabel(jmplabel);
        global_pat->setPatLength(len);
        global_pat->setBaseAddress(mem);
        global_pat->setRelBaseAddress(mem-1);

        for(int i = 0; i<MAX_STATES; i++)
             setNextStateforAll(i,-1);
        curr_state = 0;
        final_state = 0; 
        any_event = 0;
        constructRegEx(global_pat, 1);
        global_pat->setFinalState(final_state);
        ruleMMU.setAddress(mem + (final_state+1)*MAX_EVENTS);
        global_pat->setMatrix(next_location[0], final_state);
        printRegEx();

        pattern.push_back(global_pat);

	buildinstrlabel(ruleLabel);
	buildinstr("MOVI",toString(final_state), curStateReg);
	buildinstr("MOVI","10000", "R777");
	buildinstr("MOVS","\"NAVDEEP\"", "R777");
	buildinstr("PRTS","R777");
         
	//Initialize function Tables
	functionTable=new RegTable();
	functionMMU=new MemManage();


	if(rste_)
		rste_->generateCode();
	else
		cout<<"BlockEntry is NULL"<<endl;

	if(reaction_)
	    reaction_->generateCode();
        buildinstr("JMP",jmplabel);

       	//Restore Common SubExpression
	exprObject=tempexprObject;
	delete functionTable;
	delete functionMMU;
	functionTable=NULL;
	functionMMU=NULL;

	return "";
}

void BlockEntry::generateCode()
{
	if(!DefinitionEnd)
		return;
	SymTab *st=symTab();
	if(st)
		st->generateCode(0,st->size());
	else
		cout<<"SymTab is NULL"<<endl;
}

void GlobalEntry::generateCode()
{
	inFunction=0;
	vector<RuleNode*> rls;
	rls = rules();

	functionCode.buildinstr("JMP","main");
	//buildinstr("JMP","main");
	buildinstrlabel("main");
        ruleMMU.setAddress(11000);
	if(symTab()!=NULL)symTab()->generateCode(0,symTab()->size());

	DefinitionEnd=1;

	buildinstr("MOVI",toString(MMU.getAddress()),"R999");
               if(rls.size())
        {
	    string endlabel=misc.getLabel();
	    string dataInitLabel=misc.getLabel();
	    dataInitLabel = "M" + dataInitLabel;
	    string inputlabel=misc.getLabel();
	    string reg=Table.getFreeIntReg();
	    
            buildinstr("JMP",dataInitLabel);

	    for (unsigned int i=0; i< rls.size(); i++)
	    {
	        	if ((rls)[i] != NULL)
	        	{
		        	//cout<<"FLAG1"<<endl;
	        		(rls)[i]->generateCode();
	        	}
	    }
            buildinstrlabel(dataInitLabel);
            list<RulePattern *>::iterator i;
#ifdef Navdeep 
            buildinstrlabel("INIT_START");
#endif
                string tempReg = Table.getFreeIntReg();
                string nextStateReg = Table.getFreeIntReg();
            for(i = pattern.begin(); i!=pattern.end(); i++)
            {
                RulePattern *rule_pat = *i;
                vector<array*> pat = rule_pat->getMatrix();
                int j;
                //string tempReg = Table.getFreeIntReg();

                for(j = 0; j<pat.size(); j++) {
	            buildinstr("MUL",toString(MAX_EVENTS), toString(j), rule_pat->getRegister());
	            buildinstr("ADD",toString(rule_pat->getRelBaseAddress()),rule_pat->getRegister(), rule_pat->getRegister());
                    //for(int k = 0; k<MAX_EVENTS; k++)
                    for(int k = 0; k<events.size(); k++)
                    {
	                buildinstr("ADD",rule_pat->getRegister(),toString(toAscii(events[k])),tempReg);
	                //buildinstr("MOVI",toString(toAscii(*j)),tempReg);
	                buildinstr("STI",toString((pat[j]->arr[toAscii(events[k])])),tempReg);
                    }
                }
	            buildinstr("MOVI",toString(rule_pat->getFinalState()),rule_pat->getFinalStateRegister());
	            buildinstr("MOVI","0",rule_pat->getCurStateRegister());
             }
                
#ifdef Navdeep 
            buildinstrlabel("INIT_END");
#endif
            buildinstrlabel(inputlabel);
	    buildinstr("IN",reg);
	    buildinstr("JMPC","GT", "0", reg, endlabel);
#ifdef Navdeep 
            buildinstrlabel("PAT_MATCH_START");
#endif
            for(i = pattern.begin(); i!=pattern.end(); i++)
            {
                RulePattern *rule_pat = *i;
	        
                buildinstr("JMPC", "GT", "0", rule_pat->getCurStateRegister(),rule_pat->getOutLabel());
                buildinstr("MUL", toString(MAX_EVENTS),rule_pat->getCurStateRegister() ,tempReg);
                buildinstr("ADD", toString(rule_pat->getRelBaseAddress()), tempReg, tempReg);
                //buildinstr("SUB", reg, "65", reg);
                buildinstr("ADD", reg, tempReg, tempReg);
                buildinstr("LDI", tempReg, nextStateReg);
                buildinstr("JMPC", "EQ", nextStateReg, rule_pat->getFinalStateRegister(),rule_pat->getRuleLabel());
                buildinstr("MOVI", nextStateReg, rule_pat->getCurStateRegister());
                
                buildinstrlabel(rule_pat->getOutLabel());

                free(rule_pat);
	    }
#ifdef Navdeep 
            buildinstrlabel("PAT_MATCH_END");
#endif
	    buildinstr("JMP", inputlabel);
	    buildinstrlabel(endlabel);
	    buildinstr("MOVI","31000",reg);
	    buildinstr("MOVS","\"The End\"",reg);
	    buildinstr("PRTS",reg);
	 }

	finalCode=functionCode+Code;
	//Optimizations
	//finalCode.createBlocks();
	finalCode.printCode();

	//finalCode.removeLabels();
	finalCode.createBlocks();

	finalCode.eliminateDCode();
	finalCode.constPropagation();
	finalCode.strengthreduction();
	finalCode.eliminateDCode();

	//finalCode.organizeBlocks();
	finalCode.emptylabels();
	finalCode.printBlocks();
	//finalCode.optimize();
}

void SymTab::generateCode(int first, int last)
{
	int i=0;
	for (SymTab::iterator it = begin(); (it != end()) && i<last; ++it,i++) 
	{
		if(i>=first)
		{
			SymTabEntry *ste = (SymTabEntry *)(*it);
			ste->generateCode();
		}
	}
}
string WhileNode::generateCode()
{
	//((CompoundStmtNode*)thenStmt())->stmts().get(0).print();
	
	string startlabel=misc.getLabel();
        buildinstrlabel(startlabel);
	//buildinstrlabel("WS"+startlabel);
	string endlabel=misc.getLabel();
	whileLabels.push_front(endlabel);
	ExprNode *node = cond();	
	
	if(node !=NULL)
	{
		//	buildinstr("WHILE","START");
	        	string val = node->generateCode();	
	        	string regstr ;	
		if(node->exprNodeType()==ExprNode::REF_EXPR_NODE || node->exprNodeType()==ExprNode::INV_NODE)
		{
                        regstr = val;
                        val = misc.getLabel();
			buildinstr("JMPC","EQ",regstr,"1",val);		
			//buildinstr("JMP","WE"+endlabel);
			buildinstr("JMP",endlabel);
			//buildinstrlabel(endlabel);
		}
		else if(node->exprNodeType()==ExprNode::VALUE_NODE)
		{
			regstr=val;
                        val=misc.getLabel();
                        buildinstr("JMPC","EQ",regstr,"1",val);
			buildinstr("JMP",endlabel);
		}
                else
	        {
			//regstr=val;
			//val=misc.getLabel();
			//buildinstr("JMPC","EQ",regstr,"1",val);
			buildinstr("JMP",endlabel);
		}
		buildinstrlabel(val);
		thenStmt()->generateCode();
		
		//buildinstr("JMP","WS"+startlabel);
		//buildinstrlabel("WE"+endlabel);

		buildinstr("JMP",startlabel);
		buildinstrlabel(endlabel);
	//	buildinstr("WHILE","END");
	}	
	whileLabels.remove(endlabel);

// TO BE CHANGED 
	return "a"; 
}
string BreakNode::generateCode()
{
     
    list<string>::iterator current = whileLabels.begin();
    if(whileLabels.size()<getN())
    {
    cout<<file()<<":"<<line()<<": "<<"Error: Number of nested whiles not consistent with break <constant> "<<endl;
    }
    else if(whileLabels.size()>0)
    {
        for(int i=1;i<getN();i++)
        {
            current++;
        }
        buildinstr("JMP",(*current));
        //buildinstr("JMP","WE"+(*current));
    }
// TO BE CHANGED
    return "a";
}
