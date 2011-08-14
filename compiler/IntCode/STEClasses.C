#include "STEClasses.h"
#include "Value.h"
#include "ParserUtil.h"


void GlobalEntry::print(ostream& os, int indent) const
{
    vector<RuleNode*> rls;
        os<<endl;
        prtSpace(os, STEP_INDENT);
        //printST(os, indent, '\0', ';');
        printST(os, indent, '\0', '\0');
        rls = rules();
        os<<endl;
	for (unsigned int i=0; i< rls.size(); i++) {
	  if ((rls)[i] != NULL)
		(rls)[i]->print(os,indent);
          os<<endl;
	}
          prtSpace(os, indent+STEP_INDENT);
}

void EventEntry::print(ostream& os, int indent) const
{
     const vector<const Type*>* params = NULL;
     const SymTab* st = symTab();
     string info;
     //os<<"EventEntry::print"<<endl;
     //os<<type()->fullName();
     info += "event ";
     info += name();
     params = type()->argTypes();
      if ((params != NULL) && (params->size())) {
      info += "(";
                SymTab::const_iterator it = st->begin();
		for (unsigned int i=0; ((it != st->end()) && (i< params->size())); i++, ++it) {
		  if ((*params)[i] != NULL) {
	                SymTabEntry *ste = (SymTabEntry *)(*it);
			info += (*params)[i]->fullName() ;
                        info += " ";
                        info += ste->name();
                    }
		  if (i < params->size() - 1)
			info += ", ";
		}
     info += ")";
	  }

     info += ";";
     //prtSpace(stream, STEP_INDENT);
     os<<info;

}

void ClassEntry::print(ostream& os, int indent) const
{
     //prtSpace(os, indent);
     //os<<"class "<<name();
 //    os<<"ClassEntry::print"<<endl;
     //prtSpace(os, indent+STEP_INDENT);
     os<<type()->fullName();
     os<<";";
}
void VariableEntry::print(ostream& os, int indent) const
{
//     os<<"VariableEntry::print"<<endl;
     //os<<type()->fullName();
     os<<type()->fullName();
     os<<" "<<name();
     if(initVal_) {
          os<<" = ";
         initVal_->print(os, indent);
     }
     os<<";";
}
void FunctionEntry::print(ostream& os, int indent) const
{
     const vector<const Type*>* params = NULL;
     int a = 10;
     const SymTab* st = symTab();
     string info;
     //os<<"FunctionEntry::print"<<endl;
     //os<<type()->fullName();
     //os<<type()->fullName();
     info += type()->name(type()->retType()->tag());
     info += " ";
     info += name();
     params = type()->argTypes();
     info += "(";
      if ((params != NULL) && (params->size())) {
                SymTab::const_iterator it = st->begin();
		for (unsigned int i=0; ((it != st->end()) && (i< params->size())); i++, ++it) {
		  if ((*params)[i] != NULL){
	                SymTabEntry *ste = (SymTabEntry *)(*it);
			info += (*params)[i]->fullName() ;
                        info += " ";
                        info += ste->name();
                    }
		  if (i < params->size() - 1)
			info += ", ";
		}
	  }
     info += ")";

     //prtSpace(stream, STEP_INDENT);
     os<<info;
     if(body_)
         body_->printWithoutBraces(os,indent);
     else
         os<<";";
     //os<<name();
}
void BlockEntry::print(ostream& out, int indent) const
{
    cout<<"BlockEntry::print";
}
