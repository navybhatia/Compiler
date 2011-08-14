#include "Ast.h"					
#include "ParserUtil.h"					

extern const OpNode::OpInfo opInfo[];
/****************************************************************/
const Type* ExprNode::typeCheck()
{
}
void ExprNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* RefExprNode::typeCheck()
{
    return coercedType();
}
void RefExprNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* ValueNode::typeCheck()
{
    return coercedType();
}
void ValueNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* InvocationNode::typeCheck()
{
    const vector<ExprNode*>* args = NULL;
    const vector<Type*>* formalArgs = NULL;
     //os<<"FunctionInvocation::print"<<endl;
     //os<<type()->fullName();
     //os<<type()->fullName();
     if(!ste_)
     {
         cout<<"Invalid call. No such function";
         return NULL;
     }
     args = params();
     formalArgs = ((FunctionEntry*)(ste_))->type()->argTypes();

     if(!args && !formalArgs)
     {
         return ste_->type()->retType();
     }
     else if((!args && formalArgs) || (args && !formalArgs) || (args->size() != formalArgs->size())) {
        if(!formalArgs)
             cout<<file()<<":"<<line()<<": "<<"Error: arguments not expected for "<<ste_->name()<<endl; 
         else
             cout<<file()<<":"<<line()<<": "<<"Error:"<<formalArgs->size()<<" arguments expected for "<<ste_->name()<<endl; 
         return ste_->type()->retType();
     }

     for (unsigned int i=0; i< args->size(); i++) {
		  if ((*args)[i] != NULL)
			if(!((*args)[i]->coercedType()->isSubType((*formalArgs)[i]->tag())))
                            cout<<file()<<":"<<line()<<": "<<"Error: Type mismatch for argument "<<(i+1)<<" to "<<ste_->name()<<endl; 
		}

    return ste_->type()->retType();

}
void InvocationNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* PrimitivePatNode::typeCheck()
{
    const vector<Type*>* params = NULL;

    params = ee_->type()->argTypes();

     if((!params && !params_) || (!params->size() && !params_))
     {
         return NULL;
     }
     else if((!params && params_) || (params && !params_) || (params->size() != params_->size()))
     {
         cout<<file()<<":"<<line()<<": "<<"Error: Mismatch in number of arguments\n"; 
         return NULL;
     }

     for (unsigned int i=0; i< params_->size(); i++) {
		  if (((*params_)[i] != NULL) && ((*params)[i] != NULL))
			if(!((*params_)[i]->type()->isSubType((*params)[i]->tag())))
                            cout<<file()<<":"<<line()<<": "<<"Error: Type mismatch for argument "<<(i+1)<<" to "<<ee_->name()<<endl; 
		}

    return NULL;
    
}
void PrimitivePatNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* PatNode::typeCheck()
{
    if(pat1_)
        pat1_->typeCheck();

    if(pat2_)
        pat2_->typeCheck();

    if(pat1_ && pat2_ && hasSeqOps() && (pat1_->hasNeg() || pat2_->hasNeg()))
    {
         cout<<file()<<":"<<line()<<": "<<"Error: Invalid use of pattern negation"<<endl; 
    }
    return NULL;
}
void PatNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* StmtNode::typeCheck()
{
}
void StmtNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* ReturnStmtNode::typeCheck()
{
    if(expr_ && !(expr_->coercedType()->isSubType(fun_->type()->retType()->tag())))
    {
         cout<<file()<<":"<<line()<<": "<<"Error: Return value incompatible with current function's type"<<endl; 
    }
    else if(!expr_ && (fun_->type()->retType()->tag() != Type::VOID))
         cout<<file()<<":"<<line()<<": "<<"Error: Return value incompatible with current function's type"<<endl; 

    return fun_->type()->retType();
}

const Type* ExprStmtNode::typeCheck()
{
    return expr_->typeCheck();
}

const Type* CompoundStmtNode::typeCheck()
{
     list<StmtNode*>::iterator i;
    unsigned j = 0;
    if (stmts_ != NULL) {
    int size = stmts_->size();
		for (i=stmts_->begin(); j < stmts_->size(); i++, j++) {
		  if ((*i) != NULL) {
			(*i)->typeCheck() ;
		  }
		}
	  }
    return NULL;
}
void CompoundStmtNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* IfNode::typeCheck()
{
    if((cond_->typeCheck()->tag() != Type::ERROR) && !(cond_->typeCheck()->isSubType(Type::BOOL)))
         cout<<file()<<":"<<line()<<": "<<"Error: If conditional Expr is not Boolean "<<endl;
    if(then_)
        then_->typeCheck();
    if(else_)
        else_->typeCheck();
    
    return NULL;
}
void IfNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* RuleNode::typeCheck()
{
    pat_->typeCheck();
    if(reaction_)
        reaction_->typeCheck();
}

void RuleNode::typePrint(ostream&, int indent) const {/* Fill in yours */}

const Type* WhileNode::typeCheck()
{
    if(!(cond_->coercedType()->isSubType(Type::BOOL)))
         cout<<file()<<":"<<line()<<": "<<"Error: While conditional Expr is not Boolean "<<endl;
    if(then_)
        then_->typeCheck();
    
    return NULL;
}

void WhileNode::typePrint(ostream&, int indent) const {/* Fill in yours */}


const Type* BreakNode::typeCheck()
{
     return NULL;
}

void BreakNode::typePrint(ostream&, int indent) const {/* Fill in yours */}
