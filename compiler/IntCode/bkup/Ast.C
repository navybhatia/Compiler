#include "Ast.h"					
#include "ParserUtil.h"					

/****************************************************************/
extern const OpNode::OpInfo opInfo[] = {
  // print name, arity, paren_flag, fixity, arg types, out type, constraints
  //
  // Paren_flag -- opnode->print() outputs is surrounded by parenthesis if 
  // this flag is set. As set below, the expression may not print correctly
  // in some rare cases, e.g., ~(b * c) will get printed as ~b * c,
  // which actually corresponds to (~b)*c. To ensure that things get printed
  // correctly all the time, more paren_flags should be set to 1, but this
  // will lead to more clutter in printed output. Basically, what we have done
  // here is to look are expressions by type -- arithmetic, relational, 
  // boolean, bit operations, etc. Within each type, the highest priority 
  // operator is printed without paren. This will work correctly, as long
  // as the language doesn't permit mixing of different types of expressions.
  // But this assumption doesn't always hold, as in the example above. Also,
  // there is an exception to this general approach in the case of unary minus
  // and * -- since (-a)*b and -(a*b) have the same meaning, we can exclude
  // paren for * without an error.
  //
  // Codes for constraints:
  // first character:
  //    N: No additional constraint over what is given by argTypes
  //    I: all arguments must have identical type
  //    S: one of the arguments must have a type that is a supertype of
  //        of all other arguments. All other arguments require a coercion 
  //        operation to be introduced so as to convert their type to S.
  //    s: one of the arguments must have a type that is a subtype of
  //        of all other arguments. 
  //    L: all list arguments (and list output) must have same type. In 
  //        addition, all non-list arguments (and output) must have same 
  //        type as that of elements in these lists
  //    T: all tuple arguments to the function must have same type.
  //    A: (assignment). Type of second argument must be a subtype of
  //       the first argument
  //
  // second character:
  //    O: output type is the same as out type. (In the following cases,
  //        the output type need not be equal to out type, but a subtype
  //        of it.) Since a TypeTag provides complete type information only
  //        for primitive types, `O' is applicable only in this case.
  //    digit: output type is the same as that of the digit'th argument
  //       In this case, a third character may be used, the code for
  //       which is as follows:
  //         'e' denotes that the output is of type alpha, where
  //             the type of digit'th argument is list(alpha)
  //         'l' denotes that the output is of type list(alpha), where
  //             alpha is the type of the digit'th argument.
  //    S: The output type is the same as that of the argument with the
  //        most general type. (Typically used with first character 'S')
  //    s: The output type is the same as that of the argument with the
  //        least general type. (Typically used with first character 'S')
  //    P: The output type is the product of the types of all arguments
  //    p: The output type is a component of the input tuple type. The
  //        following character specifies the component. A digit k specifies
  //        that the component number as k. The character 'a' indicates that
  //        the component number is given by an integer argument to the
  //        operator. The argument number is given by the following digit.
  //        'p' can be used only in conjunction with first character 'P'.
  //    L: Output type is the same as type of list arguments. Can be used
  //        only in conjunction with first character L.
  //    e: Output type is the same as type of element of list arguments. 
  //        Can be used only in conjunction with first character L.
  //
  {OpNode::UMINUS, "-",  1, 0, OpNode::PREFIX, {Type::SIGNED}, Type::SIGNED, "N1"},
  {OpNode::PLUS, "+",  2, 1, OpNode::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
  {OpNode::MINUS, "-",  2, 1, OpNode::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
  {OpNode::MULT, "*",  2, 0, OpNode::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
  {OpNode::DIV, "/",  2, 1, OpNode::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
  {OpNode::MOD, "%",  2, 1, OpNode::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "S2"},
  {OpNode::EQ, "==", 2, 0, OpNode::INFIX, {Type::PRIMITIVE, Type::PRIMITIVE}, Type::BOOL, "SO"},
  {OpNode::NE, "!=", 2, 0, OpNode::INFIX, {Type::PRIMITIVE, Type::PRIMITIVE}, Type::BOOL, "SO"},
  {OpNode::GT, ">",  2, 0, OpNode::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
  {OpNode::LT, "<",  2, 0, OpNode::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
  {OpNode::GE, ">=", 2, 0, OpNode::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
  {OpNode::LE, "<=", 2, 0, OpNode::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
  {OpNode::AND, "&&",  2, 1, OpNode::INFIX, {Type::BOOL, Type::BOOL}, Type::BOOL, "NO"},
  {OpNode::OR, "||",  2, 1, OpNode::INFIX, {Type::BOOL, Type::BOOL}, Type::BOOL, "NO"},
  {OpNode::NOT, "!",  1, 0, OpNode::PREFIX, {Type::BOOL}, Type::BOOL, "NO"}, 
  {OpNode::BITNOT, "~",  1, 0, OpNode::PREFIX, {Type::INTEGRAL}, Type::INTEGRAL, "N1"},
  {OpNode::BITAND, "&",  2, 1, OpNode::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "Ss"},
  {OpNode::BITOR, "|",  2, 1, OpNode::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "SS"},
  {OpNode::BITXOR, "^",  2, 0, OpNode::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "SS"},
  {OpNode::SHL, "<<", 2, 1, OpNode::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "N1"},
  {OpNode::SHR, ">>", 2, 1, OpNode::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "N1"},
  {OpNode::ASSIGN, "=",  2, 0, OpNode::INFIX, {Type::NATIVE, Type::NATIVE}, Type::VOID, "AO"},
  {OpNode::PRINT, "print", OpNode::VARIABLE, 1, OpNode::PREFIX, {Type::NATIVE}, Type::VOID, "NO"},
  {OpNode::INVALID, "invalid",            0, 0, OpNode::PREFIX, {}, Type::ERROR, "NO"}
};


void OpNode::computeValue()
{
	if(arity_==2)
	{
		if(opCode_==OpNode::PLUS)
		{
			Value *val=new Value(arg_[0]->value()->ival()+arg_[1]->value()->ival(),Type::INT);
			value(val);
		}
	}
}

OpNode::OpNode(OpCode op, ExprNode* a1, ExprNode* a2, 
			   int ln, int col, string file):
  ExprNode(ExprNode::OP_NODE, NULL, ln,col,file) {
  opCode_ = op;
  if (a1 != NULL) {
	arity_ = 1;
	arg_.push_back(a1);
	if (a2 != NULL) {
	  arity_++;
	  arg_.push_back(a2);
	}
  }

    Type *t = new Type((SymTabEntry*)NULL, Type::UNKNOWN);
    Type::TypeTag tag = Type::UNKNOWN;

    if((opCode_ == OpNode::UMINUS)) {
        if(a1->coercedType()->tag() == Type::UINT)
            tag = Type::INT;
        else
            tag = a1->coercedType()->tag();
    }
    else if(((opCode_ == OpNode::SHL) || (opCode_ == OpNode::SHR)) &&
        (((a1->coercedType()->tag() == Type::UINT) ||
           (a1->coercedType()->tag() == Type::INT)) &&
           (a2->coercedType()->tag() == Type::INT) ||
           (a2->coercedType()->tag() == Type::UINT))) {
            tag = Type::INT;
    }
    else if((opCode_ == OpNode::EQ) || (opCode_ == OpNode::NE) || 
        (opCode_ == OpNode::GT) || (opCode_ == OpNode::LT) ||
        (opCode_ == OpNode::GE) || (opCode_ == OpNode::LE)) {
            tag = Type::BOOL;
    }
    else if((opCode_ == OpNode::BITAND) || (opCode_ == OpNode::BITOR) || 
        (opCode_ == OpNode::BITXOR) || (opCode_ == OpNode::BITNOT)
        && (a1 && a2) && 
        (a1->coercedType()->tag() != Type::STRING) && 
        (a2->coercedType()->tag() != Type::STRING) && 
        (a1->coercedType()->tag() != Type::UNKNOWN) && 
        (a2->coercedType()->tag() != Type::UNKNOWN)){
                if((a1->coercedType()->tag() == Type::UINT) && (a2->coercedType()->tag() == Type::UINT))
                       tag = Type::UINT;
                else if((a1->coercedType()->tag() == Type::INT) && (a2->coercedType()->tag() == Type::INT))
                       tag = Type::INT;
                else if((a1->coercedType()->tag() == Type::BYTE) && (a2->coercedType()->tag() == Type::BYTE))
                       tag = Type::BYTE;
                else
                       tag = Type::DOUBLE;
    }
    else if (((opCode_ == OpNode::AND) || (opCode_ == OpNode::OR) ||
        (opCode_ == OpNode::NOT)) &&
         ((a1 && a1->coercedType()->tag() == Type::BOOL) ||
            (a2 && a2->coercedType()->tag() == Type::BOOL))) {
            tag = Type::BOOL;
    }
    else if(((opCode_ == OpNode::PLUS) || (opCode_ == OpNode::MINUS) || 
        (opCode_ == OpNode::DIV) || (opCode_ == OpNode::MULT) ||
        (opCode_ == OpNode::MOD)) && (a1 && a2) && 
        (a1->coercedType()->tag() != Type::STRING) && 
        (a2->coercedType()->tag() != Type::STRING) && 
        (a1->coercedType()->tag() != Type::UNKNOWN) && 
        (a2->coercedType()->tag() != Type::UNKNOWN)){

                if((a1->coercedType()->tag() == Type::UINT) && (a2->coercedType()->tag() == Type::UINT))
                       tag = Type::UINT;
                else if((a1->coercedType()->tag() == Type::INT) && (a2->coercedType()->tag() == Type::INT))
                       tag = Type::INT;
                else if((a1->coercedType()->tag() == Type::BYTE) && (a2->coercedType()->tag() == Type::BYTE))
                       tag = Type::BYTE;
                else if((a1->coercedType()->tag() == Type::DOUBLE) && (a2->coercedType()->tag() == Type::DOUBLE))
                       tag = Type::DOUBLE;
                else if((a1->coercedType()->tag() != a2->coercedType()->tag()) &&
                         ((a1->coercedType()->tag() == Type::DOUBLE) || (a2->coercedType()->tag() == Type::DOUBLE)))
                       tag = Type::DOUBLE;
                else if((a1->coercedType()->tag() != a2->coercedType()->tag()) &&
                         ((a1->coercedType()->tag() == Type::INT) || (a2->coercedType()->tag() == Type::INT)))
                       tag = Type::INT;
                else if((a1->coercedType()->tag() != a2->coercedType()->tag()) &&
                         ((a1->coercedType()->tag() == Type::UINT) || (a2->coercedType()->tag() == Type::UINT)))
                       tag = Type::UINT;
    }
    
    t->tag(tag);
    coercedType(t);
    //computeValue();
}

OpNode::OpNode(const OpNode &other):
  ExprNode(other) {
  arity_ = other.arity();
  opCode_ = other.opCode();
  for (unsigned int i=0; (i < other.arity()); i++) {
    if (other.arg_[i]) {
      arg_.push_back((other.arg_[i])->clone());
    } 
	else {
      arg_.push_back(NULL);
    }
  }
}

void 
OpNode::print(ostream& os, int indent) const {
  if (opInfo[opCode_].prtType_ == OpNode::PREFIX) {
	os << opInfo[opCode_].name_;
	if (arity_ > 0) {
	  if (opInfo[opCode_].needParen_) 
		os << '(';
	  for (unsigned i=0; i < arity_-1; i++) {
		if (arg_[i])
		  arg_[i]->print(os, indent);
	    else os << "NULL";
		os << ", ";
	  }
      if (arg_[arity_-1])
		arg_[arity_-1]->print(os, indent);
	  else os << "NULL";
	  if (opInfo[opCode_].needParen_) 
		os << ") ";
	}
  }
  else if ((opInfo[opCode_].prtType_ == OpNode::INFIX) && (arity_ == 2)) {
	if (opInfo[opCode_].needParen_) 
	  os << "(";
	if(arg_[0])
	  arg_[0]->print(os, indent);
	else os << "NULL";
	os << opInfo[opCode_].name_; 
	if(arg_[1])
	  arg_[1]->print(os, indent);
	else os << "NULL";
	if (opInfo[opCode_].needParen_) 
	  os << ")";
  }
  else internalErr("Unhandled case in OpNnode::print");
}

/****************************************************************/

/*
 * Implement them
 */

const Type* OpNode::typeCheck() {
    if((opCode_ == OpNode::ASSIGN) && !(arg_[1]->typeCheck()->isSubType(*(arg_[0]->coercedType()))))
    {
        cout<<file()<<":"<<line()<<": "<<"Error:Assigned expression must be a subtype of target"<<endl;
    }
    else if(((opCode_ == OpNode::EQ) || (opCode_ == OpNode::NE) || 
        (opCode_ == OpNode::GT) || (opCode_ == OpNode::LT) ||
        (opCode_ == OpNode::GE) || (opCode_ == OpNode::LE)) &&
         !(((arg_[0]->coercedType()->tag() == Type::INT) || 
            (arg_[0]->coercedType()->tag() == Type::UINT) ||
            (arg_[0]->coercedType()->tag() == Type::DOUBLE)) &&
           ((arg_[1]->coercedType()->tag() == Type::INT) || 
            (arg_[1]->coercedType()->tag() == Type::UINT) ||
            (arg_[1]->coercedType()->tag() == Type::DOUBLE))))  {
           cout<<file()<<":"<<line()<<": "<<"Error:Incompatible type for arguments for operator "<<opInfo[opCode_].name_<<endl;
    }
    else if (((opCode_ == OpNode::AND) || (opCode_ == OpNode::OR) ||
        (opCode_ == OpNode::NOT)) &&
         ((arg_[0]->coercedType()->tag() != Type::BOOL) ||
            (arg_[1]->coercedType()->tag() != Type::BOOL))) {
           cout<<file()<<":"<<line()<<": "<<"Error:Incompatible type for arguments for operator "<<opInfo[opCode_].name_<<endl;
    }

    return coercedType();
};
void OpNode::typePrint(ostream& os, int indent) const {};
AstNode::AstNode(NodeType nt, int line, int column, string file) :
	ProgramElem(NULL, line, column, file) {}
AstNode::AstNode(const AstNode& a) : ProgramElem(a) {}
ExprNode::ExprNode(ExprNode const& e) : AstNode::AstNode(e) {}
ExprNode::ExprNode(ExprNodeType et, const Value* val, int line, int column, string file) :
	AstNode((NodeType)et, line, column, file) {val_ = val;
						   exprType_ = et;};

void  CompoundStmtNode::printWithoutBraces(ostream& os, int indent) const
{
    list<StmtNode*>::iterator i;
    unsigned j = 0;
    //prtSpace(os, indent+STEP_INDENT);
    prtSpace(os, STEP_INDENT);
    os<<"{";
    if (stmts_ != NULL) {
    int size = stmts_->size();
		for (i=stmts_->begin(); j < stmts_->size(); i++, j++) {
		  if ((*i) != NULL) {
                  prtln(os, indent);
                  prtSpace(os, indent+STEP_INDENT*2);
			(*i)->print(os,indent) ;
                  if(((*i)->stmtNodeKind() != StmtNode::COMPOUND)
                       && ((*i)->stmtNodeKind() != StmtNode::IF))
                      os<<";";
                  if(j == (stmts_->size()) -1)
                  	prtln(os, indent);
                  prtSpace(os, indent+STEP_INDENT);
		  }
		}
	  }
    
    os<<"}";
    os<<";";
}
void  CompoundStmtNode::print(ostream& os, int indent) const
{
    list<StmtNode*>::iterator i;
    unsigned j = 0, prev_node_if = 0;
    //prtSpace(os, indent+STEP_INDENT);
    prtSpace(os, STEP_INDENT);
    os<<"{";
    if (stmts_ != NULL) {
    int size = stmts_->size();
		for (i=stmts_->begin(); j < stmts_->size(); i++, j++) {
		  if ((*i) != NULL) {
                  if(!prev_node_if) {
                      prtln(os, indent);
                      prtSpace(os, indent+STEP_INDENT*2);
                  }
                      prev_node_if = 0;
			(*i)->print(os,indent) ;
                  if(((*i)->stmtNodeKind() != StmtNode::COMPOUND)
                       && ((*i)->stmtNodeKind() != StmtNode::IF))
                      os<<";";
                  if (((*i)->stmtNodeKind() == StmtNode::IF))
                        prev_node_if = 1;
                  if((j == (stmts_->size()) -1) && (!prev_node_if))
                  	prtln(os, indent);
                  prtSpace(os, indent+STEP_INDENT);
		  }
		}
	  }
    
    os<<"}";
    os<<";";
    //prtln(os, indent);
    //prtSpace(os, indent+STEP_INDENT);
}
RefExprNode::RefExprNode(string ext, const SymTabEntry* ste, 
          int line, int column, string file)
//    : ExprNode(*this) {
    : ExprNode(ExprNode::REF_EXPR_NODE, NULL, line, column, file) {
      ext_ = ext;
      sym_ = ste;
      coercedType((Type*)(sym_->type()));
}
RefExprNode::RefExprNode(const RefExprNode& ren)
    : ExprNode(ren) {
}
void RefExprNode::print(ostream& os, int indent) const
{
    //VariableEntry *ve = NULL;
    if(sym_) {
     //ve = static_cast<VariableEntry*>(sym_);
     os<<sym_->type()->name(sym_->type()->tag());
     os<<" ";
     os<<ext_;
     /*if(ve->initVal()) {
          os<<" = ";
         ve->initVal()->print(os, indent);
      }*/
    }
    else
        os<<ext_;
}
void ValueNode::print(ostream& os, int indent) const
{
    value()->print(os,indent);
}
InvocationNode::InvocationNode(const SymTabEntry *ste, vector<ExprNode*>* param,
         int line, int column, string file)
    //: ExprNode(*this) {
    : ExprNode(ExprNode::INV_NODE, NULL, line, column, file) {
      ste_ = ste;
      params_ = param;
      coercedType(ste->type()->retType());
}
InvocationNode::InvocationNode(const InvocationNode& in)
    : ExprNode(in) {
}
void InvocationNode::print(ostream& os, int indent) const
{
     const vector<ExprNode*>* args = NULL;
     //os<<"FunctionInvocation::print"<<endl;
     //os<<type()->fullName();
     //os<<type()->fullName();
     if(!ste_)
     {
         os<<"Invalid call. No such function";
         return;
     }
     os<<ste_->name();
     args = params();
     os<<"(";
      if (args != NULL) {
		for (unsigned int i=0; i< args->size(); i++) {
		  if ((*args)[i] != NULL)
			(*args)[i]->print(os,indent) ;  
		  if (i < args->size() - 1)
			os<<", ";
		}
	  }
     os<<")";

     //prtSpace(stream, STEP_INDENT);
     //os<<info;
}
void IfNode::print(ostream& os, int indent) const
{
    os<<"if (";
    cond_->print(os,indent);
    os<<")";
    //prtln(os, indent);
    if(then_) {
    then_->print(os,indent);
    if(then_->stmtNodeKind() != StmtNode::COMPOUND)
        os<<";";
    }
    if (else_) {
        prtln(os, indent);
        os<<"else ";
        //prtln(os, indent);
        else_->print(os,indent);
        if(else_->stmtNodeKind() != StmtNode::COMPOUND)
            os<<";";
     /*   else {
            prtln(os, indent);
            prtSpace(os, indent+STEP_INDENT);
        }*/
    }
   // else if((then_) && (then_->stmtNodeKind() == StmtNode::COMPOUND)) {
    if((then_) || (else_)) {
        prtln(os, indent);
        prtSpace(os, indent+STEP_INDENT);
    }
}
void RuleNode::print(ostream& os, int indent) const
{
    //os<<"RuleNode::Print";
    pat_->print(os, indent);
    os<<"-->";
    if(reaction_)
        reaction_->print(os, indent);
    else
        os<<";";
    if((reaction_) && (reaction_->stmtNodeKind() == StmtNode::COMPOUND)) {
        prtln(os, indent);
        prtSpace(os, indent+STEP_INDENT);
    }
    os<<";;";
}
void PrimitivePatNode::print(ostream& os, int indent) const
{
    vector<Type*>* param_type = NULL;
    const vector<const VariableEntry*>* param_names = params();
    //os<<"PPNode::Print";
    prtSpace(os, indent+STEP_INDENT);
    os<<"(";
    os<<ee_->name();
    if(string("any") != ee_->name())
         os<<"(";
    param_type = ee_->type()->argTypes();
    if (param_type != NULL) {
	for (unsigned int i=0; i< param_type->size(); i++) {
	  if ((*param_type)[i] != NULL)
		os<<(*param_type)[i]->fullName()<<" "<<(*param_names)[i]->name(); 
	  if (i < param_type->size() - 1)
		os<<", ";
	}
	  }
    if(string("any") != ee_->name())
        os<<")";
    if(cond_) {
         os<<"|";
         cond_->print(os, indent);
    }
    os<<")";
    
    
}
void PatNode::print(ostream& os, int indent) const
{
    //os<<"PatNode::Print"<<endl;
    prtSpace(os, indent+STEP_INDENT);
    os<<"(";
    if(kind() == NEG)
         os<<"!";
    if(pat1_)
        pat1_->print(os, indent);
    if(kind() == SEQ)
         os<<":";
    else if(kind() == OR)
         os<<"\\/";
    if(pat2_)
        pat2_->print(os, indent);
    if(kind() == STAR)
         os<<"**";
    os<<")";
}
PrimitivePatNode::PrimitivePatNode(EventEntry* ee, vector<VariableEntry*>* params, 
                   ExprNode* c, int line, int column, string file):
  BasePatNode(PRIMITIVE, line, column, file){
      ee_ = ee;
      params_ = params;
      cond_ = c;
}
bool PrimitivePatNode::hasSeqOps() const 
{
    if((kind() == SEQ) || (kind() == STAR))
        return true;

    return false;
}
bool PrimitivePatNode::hasNeg() const
{
    if((kind() == NEG))
        return true;

    return false;
}
bool PrimitivePatNode::hasAnyOrOther() const
{
    if((kind() == OR))
        return true;

    return false;
}
PatNode::PatNode(int line, int column, string file):
  BasePatNode(EMPTY, line, column, file){

}
PatNode::PatNode(PatNodeKind pk, BasePatNode *p1, BasePatNode*p2, int line, int column, string file):
  BasePatNode(pk, line, column, file){
      pat1_ = p1;
      pat2_ = p2;
}
bool PatNode::hasSeqOps() const 
{
    if(((kind() == SEQ) || (kind() == STAR)) )
      /*|| (pat1_ && pat1_->hasSeqOps()) ||
         (pat2_ && pat2_->hasSeqOps()))*/
        return true;

    return false;
}
bool PatNode::hasNeg() const
{
    if((kind() == NEG))
        return true;

    return false;
}
bool PatNode::hasAnyOrOther() const
{
    if((kind() == OR))
        return true;

    return false;
}
IfNode::IfNode(ExprNode* cond, StmtNode* thenStmt, 
       StmtNode* elseStmt, int line, int column, string file):
  StmtNode(StmtNode::IF, line,column,file) { 
	      cond_ = cond;
	      then_ = thenStmt;
	      else_ = elseStmt;
}
RuleNode::RuleNode(BlockEntry *re, BasePatNode* pat, StmtNode* reaction, 
       int line, int column, string file):
    AstNode(RULE_NODE, line, column, file) {
      rste_ = re;
      pat_ = pat;
      reaction_ = reaction;
   }

void WhileNode::print(ostream& os, int indent) const
{
    os<<"while (";
    cond_->print(os,indent);
    os<<")";
    //prtln(os, indent);
    if(then_) {
    then_->print(os,indent);
    if(then_->stmtNodeKind() != StmtNode::COMPOUND)
        os<<";";
    }
   // else if((then_) && (then_->stmtNodeKind() == StmtNode::COMPOUND)) {
    if((then_)) {
        prtln(os, indent);
        prtSpace(os, indent+STEP_INDENT);
    }
}
void BreakNode::print(ostream& os, int indent) const
{
    os<<"break "<<n_;
}

WhileNode::WhileNode(ExprNode* cond, StmtNode* thenStmt, 
        int line, int column, string file):
  StmtNode(StmtNode::WHILE, line,column,file) { 
	      cond_ = cond;
	      then_ = thenStmt;
}

BreakNode::BreakNode(int n,int line, int column, string file):
  StmtNode(StmtNode::BREAK, line,column,file) { 
	      n_=n;
}


