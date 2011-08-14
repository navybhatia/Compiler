#include "IntermediateCode.h"
#include <stdlib.h>

using namespace std;

vector<string> globalVariables;
vector<string> usedRegs;
vector<string> loopVars;

int Quadruple::getnArgs()
{
	return args.size();
}

Quadruple::Quadruple(string inst, vector <string> ar, string lb)
{
	instruction=inst;
	args=ar;
	label=lb;
}
string Quadruple::getCode()
{
	string code;
	if(label!="")
		code+=label+": ";
	code+=instruction+" ";
	for(int i=0;i<args.size();i++)
		code+=args[i]+" ";
	return code;
}
bool Quadruple::isLabel()
{
	if(label!="" && instruction=="" && args.size()==0)
		return true;
	return false;
}

IntermediateCode::IntermediateCode()
{
}
void IntermediateCode::addCode(Quadruple *code)
{
	codelist.push_back(code);
}
void IntermediateCode::printCode()
{
	intcodefile=fopen("IntermediateCode.i","w");
	cout<<"size = "<<codelist.size();
	for(int i=0;i<codelist.size();i++)
	{
		string code=codelist[i]->getCode();
		if(codelist[i]->isLabel())
			fprintf(intcodefile,"%s",code.c_str());
		else
			fprintf(intcodefile,"%s\n",code.c_str());
		//delete codelist[i];
	}
	fclose(intcodefile);
}
IntermediateCode::~IntermediateCode()
{
	//printCode();
	//fclose(intcodefile);
}
void IntermediateCode::buildinstr(string instruction, string arg1)
{
	vector <string> args;
	args.push_back(arg1);
	Quadruple *quad=new Quadruple(instruction,args,"");
	addCode(quad);
}

void IntermediateCode::buildinstr(string instruction, string arg1, string arg2)
{
	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	Quadruple *quad=new Quadruple(instruction,args,"");
	addCode(quad);
}
void IntermediateCode::buildinstr(string instruction, string arg1, string arg2, string arg3)
{
	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	Quadruple *quad=new Quadruple(instruction,args,"");
	addCode(quad);
}
void IntermediateCode::buildinstr(string instruction, string arg1, string arg2, string arg3, string arg4)
{
	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	args.push_back(arg4);
	Quadruple *quad=new Quadruple(instruction,args,"");
	addCode(quad);
}

/*
void IntermediateCode::buildinstrlb(string instruction, string arg1,string label)
{
	vector <string> args;
	args.push_back(arg1);
	Quadruple *quad=new Quadruple(instruction,args,label);
	Code.addCode(quad);
}

void IntermediateCode::buildinstrlb(string instruction, string arg1,string arg2, string label)
{
	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	Quadruple *quad=new Quadruple(instruction,args,label);
	Code.addCode(quad);
}

void IntermediateCode::buildinstrlb(string instruction, string arg1,string arg2, string arg3, string label)
{
	vector <string> args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	Quadruple *quad=new Quadruple(instruction,args,label);
	Code.addCode(quad);
}*/

void IntermediateCode::buildinstrlabel(string label)
{
	vector <string> args;
	Quadruple *quad=new Quadruple("",args,label);
	addCode(quad);
}
IntermediateCode IntermediateCode::operator+(IntermediateCode &code)
{
	vector <Quadruple *> newlist;
	int i;
	vector <Quadruple *> list2=code.getList();
	vector <Quadruple *> list1=this->getList();
	for(i=0;i<list1.size();i++)
		newlist.push_back(list1[i]);
	for(i=0;i<list2.size();i++)
		newlist.push_back(list2[i]);
	IntermediateCode ret(newlist);
	return ret;
}


void IntermediateCode::createBlocks()
{

       vector <Quadruple *> temp;
       for(int i=0;i<codelist.size();i++)
       {
	       if(!(codelist[i]->isLabel()))
	       {
		       temp.push_back(codelist[i]);       
	       }

	       if(codelist[i]->isLabel())
	       {
		       string templbl = codelist[i]->getLabel();
		       if(templbl[0] != 'F' && templbl[0] != 'R' && templbl != "main" && templbl[0] != 'M' )
			       temp.push_back(codelist[i]);       

	       }
	       if(codelist[i]->isLabel())
	       {
		       string templbl = codelist[i]->getLabel();
		       if(templbl[0] == 'F' || templbl[0] == 'R' || templbl == "main" || templbl[0] == 'M')
		       {
				blocks.push_back(temp);
        	        	temp.clear();
				temp.push_back(codelist[i]);
			}
		}
       }
       if(temp.size()!=0)
               blocks.push_back(temp);
}

bool Quadruple::isJump()
{
       if(args.size()!=0)
               if(instruction.size()==3 && instruction.substr(0,3)=="JMP")
                       return true;
       if(args.size()!=0)
               if(instruction.size()==4 && instruction.substr(0,4)=="JMPI")
                       return true;

       return false;
}

bool Quadruple::isJumpImd()
{
       if(args.size()!=0)
               if(instruction.size()==4 && instruction.substr(0,4)=="JMPI")
                       return true;

       return false;
}
bool Quadruple::isJumpCond()
{
       if(args.size()!=0)
               if(instruction.size()==4 && instruction.substr(0,4)=="JMPC")
                       return true;
       return false;
}


void IntermediateCode::removeLabels()
{
	vector<string> old_label;
	vector<string> new_label;
	string old_l,new_l;
	int j,k;
	for(int i=0;i<codelist.size();i++)
	{
		if((i+1) < codelist.size())
		{ 
			if(codelist[i]->isLabel() && codelist[i+1]->isJump() && !(codelist[i+1]->isJumpImd()))	
			{
				old_l = codelist[i]->getLabel();
				new_l = codelist[i+1]->getiArgs(0);
				old_label.push_back(old_l);
				new_label.push_back(new_l);
			}			
		}       	
	}
	for(int i=0;i<codelist.size();i++)
	{
		/*if(codelist[i]->isJump())
		{
			for(j=0;j<old_label.size();j++)
			{	
				if(codelist[i]->getiArgs(0)==old_label[j])
				{	
					codelist[i]->setiArgs(0,new_label[j]);			
				}
			}
		}
		if(codelist[i]->isJumpCond())
		{
			for(j=0;j<old_label.size();j++)
			{	
				if(codelist[i]->getiArgs(3)==old_label[j])
				{	
					codelist[i]->setiArgs(3,new_label[j]);			
				}
			}
		}*/
		int no_arguements = codelist[i]->getnArgs();
		for(j=0;j<no_arguements;j++)
		{
			for(k=0;k<old_label.size();k++)
			{	
				if(codelist[i]->getiArgs(j)==old_label[k])
				{	
					codelist[i]->setiArgs(j,new_label[k]);			
				}
			}
		}

		if(codelist[i]->isLabel())
		{
			for(j=0;j<new_label.size();j++)
			{
				if(codelist[i]->getLabel()==old_label[j])
				{
					codelist.erase(codelist.begin()+i,codelist.begin()+i+2);
				}				
			}		
		}					
	}
}

void IntermediateCode::printBlocks()
{
	int i,j=0;
	FILE *optfile = fopen("oIntermediateCode.i","w");
	string temp;
	for(i=0;i<blocks.size();i++)
	{
		for(j=0;j<blocks[i].size();j++)
		{
			temp = blocks[i][j]->getCode();
			fprintf(optfile,"%s",temp.c_str());
			if(!(blocks[i][j]->isLabel()))
				fprintf(optfile,"\n");
		}
		//fprintf(optfile,"\n");

	}
}

void IntermediateCode::organizeBlocks()
{
	int i,j;
	vector<vector <Quadruple *> > o_blocks;
	vector<vector <Quadruple *> > oldblocks = blocks;
	vector <string *> temp;

/*	o_blocks.push_back(blocks[0]);

	int end = oldblocks[0].size()-1;
	string next, orig_next;
	if(oldblocks[0][end]->getnArgs() != 0)
		next = oldblocks[0][end]->getiArgs(0);


	oldblocks.erase(oldblocks.begin());
*/
	int count=0;
	for(i=0;i<codelist.size();i++)
	{
		if(codelist[i]->isLabel())
		{
			count++;	//cout<<"Label has been found "<<codelist[i]->getLabel()<<endl;
			path.push_back(temp);
		}
	}

	string currentlbl;
	int current=-1;
	for(i=0;i<codelist.size();i++)
	{
		if(codelist[i]->isLabel())
		{
			currentlbl = codelist[i]->getLabel();	
			j=0;
			if(currentlbl!="main")
			{
				while(isalpha(currentlbl[j]))
				{
					j++;
				}
				currentlbl = currentlbl.substr(j,currentlbl.size());
				current = atoi(currentlbl.c_str());
			}
		}
		//else
		{
			//cout<<current<<"  "<<codelist[i]->getCode()<<endl;
			
		}
	}



}


bool Quadruple::isMov()
{
       if(args.size()!=0)
               if(instruction.size()==4 && instruction.substr(0,4)=="MOVI")
                       return true;
		else if(instruction.size()==4 && instruction.substr(0,4)=="MOVF")
			return true;
       return false;
}

bool IntermediateCode::isLocal(string s)
{
	for (int i=0; i<globalVariables.size(); i++)
	{
		if(globalVariables[i].substr(0,4)==s.substr(0,4))
			return false;
	}
	return true;
}

void IntermediateCode::emptylabels()
{
       vector <string> old_label;
       vector <string> new_label;
       for(int i=0;i<blocks.size();i++)
       {
               for(int j=0 ;j<=blocks[i].size()-1;j++)
               {
                       if(j < blocks[i].size()-1)
                       if(blocks[i][j]->isLabel() && blocks[i][j+1]->isLabel())
                       {
                               old_label.push_back(blocks[i][j]->getLabel());
                               new_label.push_back(blocks[i][j+1]->getLabel());
                               blocks[i].erase(blocks[i].begin()+j);
			       j--;
                       }
               }

       }

       for(int i=0;i<old_label.size();i++)
               cout<<"Old "<<old_label[i]<<"  New:: "<<new_label[i]<<endl;

       for(int i=0;i<blocks.size();i++)
       {
               for(int j=0 ;j<=blocks[i].size()-1;j++)
               {
                       int no_arguements = blocks[i][j]->getnArgs();
                       for(int k=0;k<no_arguements;k++)
                       {
                               for(int l=0;l<old_label.size();l++)
                               {
                                       if(blocks[i][j]->getiArgs(k)==old_label[l])
                                       {
                                               blocks[i][j]->setiArgs(k,new_label[l]);
                                       }
                               }
                       }

               }
       }
}


void IntermediateCode::findGlobalVariables()
{
	int mainIndex=-1;
	globalVariables.clear();
	for(int i=0;i<blocks.size();i++)
	{		
		if(blocks[i][0]->isLabel() && (blocks[i][0]->getCode().substr(0,4)=="main"))
		{
			mainIndex=i;						
		}	
	}
	for(int i=0;i<blocks[mainIndex].size();i++)
	{
		if(blocks[mainIndex][i]->isWrite())
		{			
			globalVariables.push_back(blocks[mainIndex][i]->getiArgs(1));
		}
	}
}

bool IntermediateCode::isUsed(string s)
{
/*	if(s=="R999" || s=="R998" || s=="R997" || s=="R996")
		return true; */
        if(s.substr(0,2) == "R9")
		return true; 
	for (int i=0; i<usedRegs.size(); i++)
	{
		if(usedRegs[i]==s)
			return true;
	}
	return false;
}

bool Quadruple::isWrite()
{
       if(args.size()!=0)
               if(instruction.size()==4 && instruction.substr(0,4)=="MOVI")
                       return true;
                else if(instruction.size()==4 && instruction.substr(0,4)=="MOVF")
                        return true;
                else if(instruction.size()==3 && instruction.substr(0,3)=="ADD")
                        return true;
                else if(instruction.size()==4 && instruction.substr(0,4)=="FADD")
                        return true;
                else if(instruction.size()==3 && instruction.substr(0,3)=="SUB")
                        return true;
                else if(instruction.size()==4 && instruction.substr(0,4)=="FSUB")
                        return true;
                else if(instruction.size()==3 && instruction.substr(0,3)=="DIV")
                        return true;
                else if(instruction.size()==4 && instruction.substr(0,4)=="FDIV")
                        return true;
                else if(instruction.size()==3 && instruction.substr(0,3)=="MUL")
                        return true;
                else if(instruction.size()==4 && instruction.substr(0,4)=="FMUL")
                        return true;
                else if(instruction.size()==3 && instruction.substr(0,3)=="MOD")
                        return true;
                else if(instruction.size()==3 && instruction.substr(0,3)=="STI")
                        return true;
                else if(instruction.size()==3 && instruction.substr(0,3)=="LDI")
                        return true;
       return false;
}

void IntermediateCode::eliminateDCode()
{
	bool loopVar=false;
	findGlobalVariables();
	for(int i=0;i<blocks.size();i++)
        {
		loopVariables(i);
		usedRegs.clear();
	        if(blocks[i][0]->isLabel() && !((blocks[i][0]->getCode().substr(0,1)=="F") || (blocks[i][0]->getCode().substr(0,1)=="R")))
			continue;
		
                for(int j=blocks[i].size()-1;j>=0;j--)
		{
			int n=blocks[i][j]->getnArgs();
			if(n>0)
			{
				for(int loop_cnt=0;loop_cnt<loopVars.size();loop_cnt++)
				{
	                               if(loopVars[loop_cnt].substr(0,4)==blocks[i][j]->getiArgs(n-1).substr(0,4))
					{
						//cout<<"Loop var  : "<<loopVars[loop_cnt]<<"\tJ: "<<blocks[i][j]->getiArgs(n-1)<<endl;
						loopVar=true;
					}
				}
				
				if((blocks[i][j]->getiArgs(n-1).substr(0,1)=="F" || blocks[i][j]->getiArgs(n-1).substr(0,1)=="R") && (blocks[i][j]->isWrite()) 
					&& !isUsed(blocks[i][j]->getiArgs(n-1)) && isLocal(blocks[i][j]->getiArgs(n-1)) && loopVar==false)
				{
					blocks[i].erase(blocks[i].begin()+j);
					continue;
				}
				loopVar=false;
				

				for(int k=0;k<blocks[i][j]->getnArgs();k++)
					if(blocks[i][j]->getiArgs(k).substr(0,1)=="F" || blocks[i][j]->getiArgs(k).substr(0,1)=="R")
						usedRegs.push_back(blocks[i][j]->getiArgs(k));
	
			}				
		}	
		
         }
 }

void IntermediateCode::loopVariables(int k)
{
	vector <string> locals;
	vector <vector<int> >	locals_i;
	vector <int>	locals_def_i;
	vector <string> locals_def;
	vector <string> local_labels;
	vector <int>    local_labels_i;
	loopVars.clear();

	int i = k;
                for(int j=0;j<blocks[i].size();j++)
		{
			if(blocks[i][j]->isLabel())
			{
				local_labels.push_back(blocks[i][j]->getLabel());		
				local_labels_i.push_back(j);
			}
			Quadruple* temp = blocks[i][j];
			for(int k=0;k<temp->getnArgs();k++)
			{
				string temparg = temp->getiArgs(k);
				if((temparg.substr(0,1)=="F") || (temparg.substr(0,1)=="R"))
				{
					if(k!= temp->getnArgs()-1)
					{
						int flag = 0;
						vector <int> temp;
						for(int ltemp=0;ltemp<locals.size();ltemp++)
						{
							if(locals[ltemp] == temparg)		
							{	
								flag=1;
								locals_i[ltemp].push_back(j);
								break;
							}
						}
						if(flag==0)
						{	locals.push_back(temparg);
							temp.push_back(j);
							locals_i.push_back(temp);
						}
					}
					else
					{
						int flag = 0;
						for(int ltemp=0;ltemp<locals_def.size();ltemp++)
						{
							if(locals_def[ltemp] == temparg)		
							{	
								flag=1;
								locals_def_i[ltemp] = j;
								break;
							}
						}
						if(flag==0)
						{	locals_def.push_back(temparg);
							locals_def_i.push_back(j);
						}
					}
				}
				for(int ltemp=0;ltemp<local_labels.size();ltemp++)
				{
					if(temparg == local_labels[ltemp])
					{	//cerr<<temparg<<endl;
						//cout<<temp->getCode()<<"  "<<local_labels_i[ltemp]<<endl;

						for(int lltemp=0;lltemp<locals.size();lltemp++)
						{

/**
							if(locals_i[lltemp] < local_labels_i[ltemp])
							{	continue;
							}**/

							for(int llltemp=0;llltemp<locals_def.size();llltemp++)
							{
			
							if(locals[lltemp] == locals_def[llltemp])
								{
								for(int d=0;d<locals_i[lltemp].size();d++)
								{
								if(locals_i[lltemp][d] < locals_def_i[llltemp] && locals_i[lltemp][d] > local_labels_i[ltemp])	
								{	loopVars.push_back(locals[lltemp]);
									break;
								}
								}
								}
							}

						}
						cerr<<endl;
						//for(int llltemp=0;llltemp<locals_def.size();llltemp++)
						//	cerr<<locals_def[llltemp]<<" "<<locals_def_i[llltemp]<<"\t";
						cerr<<endl;

					}
				}
			}
		}

}

void IntermediateCode::constPropagation()
{ string args1,args2,targs1,targs2;

	for(int i=0;i<blocks.size();i++)
	{    
		usedRegs.clear();
		if(blocks[i][0]->isLabel() )
		{
			loopVariables(i);
			
			//cout<<"Loop Variables are "<<endl;
			//for(int loop_cnt=0;loop_cnt<loopVars.size();loop_cnt++)
			//	cout<<loopVars[loop_cnt]<<"\t";
			//cout<<endl;	
		     	for(int j=0 ;j<=blocks[i].size()-1;j++)
			{  
 
				int n=blocks[i][j]->getnArgs();

				if(n>=2)
				{
				args2 =blocks[i][j]->getiArgs(n-2);
				args1 = blocks[i][j]->getiArgs(n-1);

				if((args2.substr(0,1)!="F" && args2.substr(0,1)!="R")&&(args1.substr(0,1)=="F" || args1.substr(0,1)=="R") && (blocks[i][j]->isMov()))
				{

					if(!(isUsed(args1)))
					{
						//cout<<"\nPropagating "<<blocks[i][j]->getCode()<<"\n";
						for(int k=0;k<blocks[i][j]->getnArgs();k++)
						{

							usedRegs.push_back(blocks[i][j]->getiArgs(k));
							for(int m=j+1;m<blocks[i].size();m++)
							{ 
								int r = blocks[i][m]->getnArgs();
								if(r<2)
									continue;        
								targs2=blocks[i][m]->getiArgs(r-2);
								targs1=blocks[i][m]->getiArgs(r-1);

								if((targs2.substr(0,1)!="F" && targs2.substr(0,1)!="R")&&(targs1==args1) && (blocks[i][m]->isMov()))
								{
									args2=targs2;
									cout<<"reached here"<<blocks[i][j]->getiArgs(r-2)<<endl;
								}

								for(int l =0; l<blocks[i][m]->getnArgs()-1;l++)
								{
									int loop_flag = 0;
									for(int var_cnt=0;var_cnt<loopVars.size();var_cnt++)
									{
										if(blocks[i][m]->getiArgs(l)==loopVars[var_cnt])
										{	loop_flag =1;
											break;
										}
									}
									if(blocks[i][j]->getiArgs(k)==blocks[i][m]->getiArgs(l) && !loop_flag)                            
									{            
										cout<<blocks[i][m]->getiArgs(l)<<"replaced by "<<blocks[i][j]->getiArgs(n-2)<<endl;
										blocks[i][m]->setiArgs(l,blocks[i][j]->getiArgs(n-2));
									}
								}
							}



						}    

					}    

				}                

				} 
			}
		}
	}
}
bool Quadruple::isAdd()
{
       if(args.size()!=0)
               if(instruction.size()==3 && instruction.substr(0,3)=="ADD")
                       return true;
        else if(instruction.size()==4 && instruction.substr(0,4)=="FADD")
            return true;
       return false;
}

bool Quadruple::isMul()
{
       if(args.size()!=0)
               if(instruction.size()==3 && instruction.substr(0,3)=="MUL" )
                       return true;
        else if(instruction.size()==4 && instruction.substr(0,4)=="FMUL")
            return true;
       return false;
}

vector<string>& Quadruple::getargsbyref()
{
   return args;
}

void IntermediateCode::strengthreduction(){
 string args,args1,args2;
 int flag = 0;

//Quadruple sr;

   for(int i=0;i<blocks.size();i++)
   {
       //if(blocks[i][0]->isLabel() )//&& (blocks[i][0]->getCode().substr(0,1)=="L"))
       {
           for(int j=0 ;j<=blocks[i].size()-1;j++)
           {
                               flag = 0;
               int n=blocks[i][j]->getnArgs();
               if(n==3)
               {
                   args = blocks[i][j]->getiArgs(n-2);
                   args1 = blocks[i][j]->getiArgs(n-3);
                   args2 = blocks[i][j]->getiArgs(n-1);
                   //`cout<<args1<<":"<<args<<":"<<args2;
                   if((args=="0"||args1=="0") && (blocks[i][j]->isAdd()))
                   {
                   blocks[i][j]->setinstruction("MOVI");
                                       flag = 1;
                   if(args1=="0")
                                         {
                                             if(args == args2) {
                                                 flag = 0;

blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                                             }
                                             else
                             blocks[i][j]->setiArgs(n-3,args);
                     }
                                       else if(args1 == args2) {
                                                 flag = 0;

blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                                               }
                   }
                   // instruction.size()==4 && instruction.substr(0,4)=="MOVI");
                   else if ((args=="0.0"||args1=="0.0") && (blocks[i][j]->isAdd()))
                   {
                                       flag = 1;
                    blocks[i][j]->setinstruction("MOVF");
                    if(args1=="0.0"){
                       if(args == args2){
                       flag =0 ;
                       blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                       }
                       else
                   blocks[i][j]->setiArgs(n-3,args);
                   }
                                  else if (args1 == args2){
                        flag =0;

blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                                               }
                   }
                   //instruction.size()==4 && instruction.substr(0,4)=="MOVF");
                   else if((args=="0"||args1=="0") && (blocks[i][j]->isMul()))
                   {
                                       flag = 1;
                    blocks[i][j]->setinstruction("MOVI");
                   if(args=="0")
                       blocks[i][j]->setiArgs(n-3,args);
                   }
                   // instruction.size()==4 && instruction.substr(0,4)=="MOVI");
                   else if ((args=="0.0"||args1=="0.0") && (blocks[i][j]->isMul()))
                   { blocks[i][j]->setinstruction("MOVF");
                                       flag = 1;
                   if(args=="0.0"){
                       blocks[i][j]->setiArgs(n-3,args);
                   }
                   }
                   else if ((args=="1"||args1=="1") && (blocks[i][j]->isMul()))
                   { blocks[i][j]->setinstruction("MOVI");
                                       flag = 1;
                   if(args1=="1"){
                       if(args == args2){
                       flag =0 ;
                       blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                       }
                       else
                   blocks[i][j]->setiArgs(n-3,args);
                   }
                                  else if (args1 == args2){
                        flag =0;

blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                                               }
                   }
                   else if ((args=="1.0"||args1=="1.0") &&
(blocks[i][j]->isMul()))
                   { blocks[i][j]->setinstruction("MOVF");
                                       flag = 1;
                   if(args1=="1.0"){
                       if(args == args2){
                       flag =0 ;
                       blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                       }
                       else
                   blocks[i][j]->setiArgs(n-3,args);
                   }
                                  else if (args1 == args2){
                        flag =0;

blocks[i].erase(blocks[i].begin()+j);
                                                 j--;
                                               }
                   }
                   ///instruction.size()==4 && instruction.substr(0,4)=="MOVF");
                               if(flag) {
               blocks[i][j]->setiArgs(n-2,args2);
               //cout<<"Size: "<<blocks[i][j]->getargs().size()<<"last is "<<endl;
               if(blocks[i][j]->getargsbyref().size()!=0)

blocks[i][j]->getargsbyref().erase((blocks[i][j]->getargsbyref()).begin()+n-1);
               }
               }



           }

              }
      }

}

