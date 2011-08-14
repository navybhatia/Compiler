#include "STEClasses.h"
#include "Value.h"
#include "ParserUtil.h"

/****************************************************************/
const Type* GlobalEntry::typeCheck() {
    vector<RuleNode*> rls;
    rls = rules();

    if (symTab() != NULL) symTab()->typeCheck();

   for (unsigned int i=0; i< rls.size(); i++) {
	  if ((rls)[i] != NULL)
		(rls)[i]->typeCheck();

     }

    return NULL;
}
void GlobalEntry::typePrint(ostream&, int indent) const {
}
/****************************************************************/
const Type* BlockEntry::typeCheck() {
}
void BlockEntry::typePrint(ostream&, int indent) const {
}
/****************************************************************/
const Type* VariableEntry::typeCheck() {
    if(initVal_) {
         if(!(initVal_->coercedType()->isSubType(*(type()))))
         cout<<file()<<":"<<line()<<": "<<"Error:Assignment between incompatible types\n";
    }
    
}
void VariableEntry::typePrint(ostream&, int indent) const {
}
/****************************************************************/
const Type* ClassEntry::typeCheck() {
}
void ClassEntry::typePrint(ostream&, int indent) const {
}
/****************************************************************/
const Type* FunctionEntry::typeCheck() {
    if(body_)
        body_->typeCheck();

    return type()->retType();
}
void FunctionEntry::typePrint(ostream&, int indent) const {
}
/****************************************************************/
const Type* EventEntry::typeCheck() {
}
void EventEntry::typePrint(ostream&, int indent) const {
}
