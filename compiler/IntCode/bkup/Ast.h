#ifndef AST_H
#define AST_H

#include "all.h"
#include "Value.h"
#include "ProgramElem.h"
#include "SymTabEntry.h"

class BlockEntry;
class EFSA;
class EventEntry;
class FunctionEntry;
class LabelEntry;
class OpNode;
class PatNode;
class PrimitivePatNode;
class RuleNode;
class SymTabEntry;
class VariableEntry;

/*****************************************************************************
   Here is the class hierarchy:
                                               ProgramElem
											       |
                                                AstNode
     +--------------------------------------------+----------------+
     |		         |                 |                           | 
 BasePatNode      ExprNode          RuleNode                    StmtNode
     |               |                                             |
     |               |                                             |
     |      +---------+----------+-----------+                     |
     |      |         |          |           |                     |
     |  RefExprNode  OpNode  ValueNode  InvocationNode             |
     |                                                             |
     |                                      +---------------+------+-----+
     |                                      |               |            |
     |                                 ExprStmtNode   CompoundStmtNode IfNode
     |
     |
   +-+------------------+
   |                    |
   |                    |
PrimitivePatNode    PatNode

******************************************************************************/
    
class AstNode: public ProgramElem {
 public:
  enum NodeType  {
    PAT_NODE,
    EXPR_NODE, 
    REACTION_NODE, 
    STMT_NODE, 
    SEQUENCE_NODE, 
    RULE_NODE
  };

 public: 
  AstNode(NodeType nt, int line=0, int column=0, string file="");
  AstNode(const AstNode&); // copy constructor 
  //virtual ~AstNode() {};

  NodeType nodeType() const { return nodeType_;}

  virtual const Type* typeCheck()=0;
  virtual void print(ostream& os, int indent=0) const=0;
  virtual void typePrint(ostream& os, int indent=0) const=0;
  virtual string generateCode(){return "";};

 private: 
  NodeType nodeType_;
  const AstNode* operator=(const AstNode& other); /* disable asg */
};

inline ostream& operator<<(ostream& os, const AstNode& an) {
  an.print(os);
  return os;
};

/****************************************************************/
class ExprNode: public AstNode {
 public:
  enum ExprNodeType   {
    REF_EXPR_NODE, 
    OP_NODE, 
    VALUE_NODE, 
    INV_NODE
   };

 public:
  ExprNode(ExprNodeType et, const Value* val=0, int line=0, int column=0,
		   string file=""); // val is saved, but not copied
  ExprNode(const ExprNode&);
  virtual ~ExprNode() {};
 
  virtual ExprNode* clone() const=0;

  ExprNodeType exprNodeType() const { return exprType_;};
  void exprNodeType(ExprNodeType t) { exprType_ = t; };

  const Value* value() const { return val_; }
  void value(Value *val) {val_=val;}

  const Type* coercedType() const { return coercedType_; }
  void coercedType(const Type* type) { coercedType_ = type; }

  const Type* typeCheck()=0;
  void print(ostream& os, int indent=0) const=0;
  void typePrint(ostream& os, int indent=0) const=0;
  string generateCode(){};

 private:
  ExprNodeType exprType_;
  const Value *val_; // reference semantics for val_ and coercedType_
  const Type* coercedType_; 
};

/****************************************************************/
class RefExprNode: public ExprNode {
 public:
  RefExprNode(string ext, const SymTabEntry* ste=NULL, 
	      int line=0, int column=0, string file="");
  RefExprNode(const RefExprNode&);
  ExprNode* clone() const { return new RefExprNode(*this); }

  ~RefExprNode() {};

  string ext() const { return ext_;};
  void ext(string str) { ext_ = str;}; 

  const SymTabEntry* symTabEntry() const { return sym_;};
  void symTabEntry(const SymTabEntry *ste)  { sym_ = ste;};

  const Type* typeCheck();
  void print(ostream& os, int indent=0) const;
  void typePrint(ostream& os, int indent=0) const;
  string generateCode();

 private:
  string ext_;
  const SymTabEntry* sym_;
};

/****************************************************************/

#define MAX_OP_ARITY 2

class OpNode: public ExprNode {
 public:
  enum OpCode {
    UMINUS, PLUS, MINUS, MULT, DIV, MOD, 
    EQ, NE, GT, LT, GE, LE,
    AND, OR, NOT, 
    BITNOT, BITAND, BITOR, BITXOR, SHL, SHR,
    ASSIGN, PRINT, INVALID
  };
    
  enum OpPrintType {PREFIX, INFIX, POSTFIX};
  struct OpInfo {
    OpCode code_;
    const char* name_;
    int arity_;
	int needParen_;
    OpPrintType prtType_;
    Type::TypeTag argType_[3]; 
    // operators with > 2 args can be supported
    // only if types of args k through N are identical, for 1 <= k <= 3, 
    // and are given by argType[k-1]
    Type::TypeTag outType_;
	const char *typeConstraints_;

	Type::TypeTag argType(int i) {
	  if (i >= arity_) return Type::ERROR;
	  else if (i > 2) return argType_[2];
	  else return argType_[i];
	}
  };

 public:
  static const int VARIABLE = 100;
 public:
  OpNode(OpCode op, ExprNode *l, ExprNode *r=NULL,
	 int line=0, int column=0, string file="");
  OpNode(const OpNode&);
  ExprNode* clone() const { return new OpNode(*this); }
  ~OpNode() {};

  void computeValue();
  OpCode opCode() const { return opCode_;};
  const ExprNode* arg(unsigned int i) const 
    { return (i < arg_.size())? arg_[i] : NULL; };
  unsigned int arity() const { return arity_; }

  void opCode(OpCode a) { opCode_ = a; };
  ExprNode* arg(unsigned int i) 
    { return (i < arg_.size())? arg_[i] : NULL; };
  vector<ExprNode*>* args() 
    { return &arg_; }

  const Type* typeCheck();
  void print(ostream& os, int indent=0) const;  
  void typePrint(ostream& os, int indent=0) const;
  string generateCode();
  
 private:
  unsigned int arity_;
  OpCode   opCode_;
  vector<ExprNode*> arg_;
};

/****************************************************************/

class ValueNode: public ExprNode {
 public:
  ValueNode(Value* val=0, int line=0, int column=0, string file="")
    : ExprNode(ExprNode::VALUE_NODE, val, line,column,file) {
    if (val != NULL) {type((Type*)(val->type())); coercedType((Type*)(val->type()));
    }
  }
  ValueNode(const ValueNode& val): ExprNode(val) {};
  ExprNode* clone() const { return new ValueNode(*this); }
  ~ValueNode() {};

  const Type* typeCheck();
  void print(ostream& os, int indent=0) const;
  void typePrint(ostream& os, int indent=0) const;
  string generateCode();

 private:
  /* val_ field is already included in ExprNode, so no new data members */
};

/****************************************************************/

class InvocationNode: public ExprNode {
  // Used to represent function invocation
 public:
  InvocationNode(const SymTabEntry *ste, vector<ExprNode*>* param=0, 
		 int line=0, int column=0, string file="");
  InvocationNode(const InvocationNode&);
  ExprNode* clone() const  { return new InvocationNode(*this); }
  ~InvocationNode() {};

  const SymTabEntry* symTabEntry() const { return ste_; };

  int  arity() const {
	if (params_ == NULL)
	  return 0;
	else return params_->size();
  }

  const vector<ExprNode*>* params() const { return params_;};
  vector<ExprNode*>* params() { return params_;}
  void params(vector<ExprNode*>* args){ params_ = args;};
  const ExprNode* param(unsigned int i) const 
    { return (params_ != NULL && i < params_->size())? (const ExprNode*)((*params_)[i]) : NULL; };
  ExprNode* param(unsigned int i)
    { return (ExprNode*)((const InvocationNode*)this->param(i));}
  void param(ExprNode* arg, unsigned int i) 
    { if (params_ != NULL && i < params_->size()) (*params_)[i] = arg;};

  const Type* typeCheck();
  void print(ostream& os, int indent=0) const;
  void typePrint(ostream& os, int indent=0) const;
  string generateCode();

 private:
  vector<ExprNode*>* params_;
  const SymTabEntry *ste_; // reference semantics
};

/****************************************************************/
// There are 3 kinds of PatNodes:
//   PrimitivePatNodes are of the form: event|cond
//   PatNodes consist of PrimitivePatNodes or PatNodes composed with
//      ":", "||" and "*" operators

class BasePatNode: public AstNode {
 public:
  enum PatNodeKind {PRIMITIVE, EMPTY, NEG, SEQ, OR, STAR, UNDEFINED};

 public:
  BasePatNode(PatNodeKind pk, int ln=0, int col=0, string f=""):
	AstNode(PAT_NODE, ln, col, f) {
	parent_ = NULL; root_ = NULL; patKind_ = pk;};
  BasePatNode(const BasePatNode& bpn): AstNode(bpn) {
	patKind_ = bpn.patKind_; parent_ = bpn.parent_; root_ = bpn.root_;}
  ~BasePatNode() {};
  //virtual BasepatNode* clone() const { return new BasePatNode(*this);}	

  PatNodeKind kind() const { return patKind_; };
  void kind(PatNodeKind p) {patKind_ = p;}

  const BasePatNode* parent() const { return parent_; } 
  BasePatNode* parent() { return parent_;} 

  virtual string generatePatCode(int index) const = 0;
  virtual bool hasSeqOps() const=0;
  virtual bool hasNeg() const=0;
  virtual bool hasAnyOrOther() const=0;
  virtual bool isNegatable() const {
	return (!hasSeqOps()); }

 private:
  PatNodeKind patKind_;
  BasePatNode* parent_;
  BasePatNode* root_;
};

/****************************************************************/

class PrimitivePatNode: public BasePatNode {
 public:
  PrimitivePatNode(EventEntry* ee, vector<VariableEntry*>* params, 
				   ExprNode* c=NULL,
				   int line=0, int column=0, string file="");
  //PrimitivePatNode(const PrimitivePatNode& ppn);
  ~PrimitivePatNode() {};
  //BasePatNode* clone() { return new PrimitivePatNode(*this); }

  const EventEntry* event() const { return ee_; }
  EventEntry* event() { return ee_; }

  const vector<const VariableEntry*>* params() const { 
	return (vector<const VariableEntry*>*)params_; }
  vector<VariableEntry*>* params() { return params_; }

  const ExprNode* cond() const { return cond_; }
  ExprNode* cond() { return cond_; }
  void cond(ExprNode* c) { cond_ = c;}

  ExprNode* condition() { return condition_; }
  const ExprNode* condition() const { return condition_; }

  const list<const OpNode*>& asgs() const { 
	return (list<const OpNode*>&)asgs_; }  
  list<OpNode*>& asgs() { return asgs_; }  

  bool hasSeqOps() const;
  bool hasNeg() const;
  bool hasAnyOrOther() const;

  const Type* typeCheck();
  void print(ostream& os, int indent=0) const; 
  void typePrint(ostream& os, int indent=0) const;
  string generatePatCode(int index) const;

 private:

  EventEntry* ee_;
  vector<VariableEntry*>* params_;
  /* cond_ may contain assignments as well as other expressions */
  /* condition_ contains all expresions in cond_ other than assignments */
  ExprNode* cond_;      
  ExprNode* condition_; 
  list<OpNode*> asgs_;
};

/****************************************************************/
class PatNode: public BasePatNode {
 public: 
  PatNode(int line=0, int column=0, string file="");
  PatNode(PatNodeKind pk, BasePatNode *p1, BasePatNode*p2=NULL, int line=0, int column=0, string file="");
  
  ~PatNode() {};
  //AstNode* clone() 
  //  { return new PatNode(*this); }

  const BasePatNode* pat1() const { return pat1_; }
  BasePatNode* pat1() { return pat1_; }
  const BasePatNode* pat2() const { return pat2_; }
  BasePatNode* pat2() { return pat2_; }

  bool hasNeg() const;
  bool hasSeqOps() const;
  bool hasAnyOrOther() const;

  const Type* typeCheck();
  void print(ostream& os, int indent=0) const; 
  void typePrint(ostream& os, int indent=0) const;
  string generatePatCode(int index) const;

 private: 
  PatNode(const PatNode&);

  BasePatNode *pat1_;
  BasePatNode *pat2_;
};


/****************************************************************/

class StmtNode: public AstNode {
 public:
  enum StmtNodeKind { ILLEGAL=-1, EXPR, IF, COMPOUND, RETURN, WHILE, BREAK};
 public: 
  StmtNode(StmtNodeKind skm, int line=0, int column=0, string file=""):
	AstNode(AstNode::STMT_NODE, line,column,file) { skind_ = skm; };
  ~StmtNode() {};
  //AstNode* clone() 
  //  { return new StmtNode(*this); }

  StmtNodeKind stmtNodeKind() const { return skind_;}

  void print(ostream& os, int indent) const = 0;
  void typePrint(ostream& os, int indent=0) const=0;
  const Type* typeCheck() = 0;
  string generateCode(){};

 private:
  StmtNodeKind skind_;
};
/****************************************************************/
class WhileNode: public StmtNode{
 public: 
  
  WhileNode(ExprNode* cond, StmtNode* thenStmt, 
		  int line=0, int column=0, string file="");
  ~WhileNode(){};
  //AstNode* clone() 
  //  { return new IfNode(*this); }

  const ExprNode* cond() const {return cond_;}
  const StmtNode* thenStmt() const  { return then_;};

  ExprNode* cond() {return cond_;}      
  StmtNode* thenStmt() { return then_;};

  const Type* typeCheck();
  void print(ostream& os, int indent) const;
  void  typePrint(ostream& os, int indent) const;
  string generateCode();

 private: 
  ExprNode *cond_;
  StmtNode *then_;

  WhileNode(const WhileNode&);
};
/****************************************************************/
class BreakNode: public StmtNode{

public:
  BreakNode(int n, 
		  int line=0, int column=0, string file="");
  ~BreakNode(){};
  int getN(){return n_;}
  
  const Type* typeCheck();
  void print(ostream& os, int indent) const;
  void  typePrint(ostream& os, int indent) const;
  string generateCode();

private:
  int n_;
};

/****************************************************************/

class ReturnStmtNode: public StmtNode {
 public:
  ReturnStmtNode(ExprNode *e, FunctionEntry* fe, 
				 int line=0, int column=0, string file=""):
    StmtNode(StmtNode::RETURN,line,column,file) { expr_ = e; fun_ = fe;};
  ~ReturnStmtNode() {};

  const Type* typeCheck();
  void print(ostream& os, int indent) const {
	os << "return "; 
	if(expr_ != NULL) expr_->print(os, indent); else os << "NULL";};
  void typePrint(ostream& os, int indent=0) const {
	os << "return "; 
	if(expr_ != NULL) expr_->typePrint(os, indent); else os << "NULL";};
  string generateCode();
  FunctionEntry* func() { return fun_;}

 private:
  ExprNode* expr_;
  FunctionEntry* fun_;
};

/****************************************************************/

class ExprStmtNode: public StmtNode {
 public:
  ExprStmtNode(ExprNode* e, int line=0, int column=0, string file=""):
    StmtNode(StmtNode::EXPR,line,column,file) { expr_ = e; };
  ~ExprStmtNode() {};
  //AstNode* clone() 
  //  { return new ExprStmtNode(*this); }

  const Type* typeCheck();
  void print(ostream& os, int indent) const { 
	if (expr_ != NULL) { expr_->print(os, indent);}};
  void typePrint(ostream& os, int indent) const { 
	if (expr_ != NULL) { expr_->typePrint(os, indent);}};
  string generateCode();

 private:
  ExprNode* expr_;
};

/****************************************************************/

class CompoundStmtNode: public StmtNode{
 public: 
  CompoundStmtNode(list<StmtNode*> *Slist, int ln=0, int col=0, string fl=""):
	StmtNode(StmtNode::COMPOUND, ln,col,fl) { stmts_ = Slist;};
  ~CompoundStmtNode() { };
  //AstNode* clone() 
  //  { return new CompoundStmtNode(*this); }

  const list<StmtNode*>* stmts() const {  return stmts_;}

  list<StmtNode*>* stmts() {  return stmts_;}
  void add(StmtNode *s) 
    { if(stmts_ != NULL) stmts_->push_back(s); };
  
  const Type* typeCheck();
  void  printWithoutBraces(ostream& os, int indent) const;
  void  typePrintWithoutBraces(ostream& os, int indent) const;
  void  print(ostream& os, int indent) const;
  void  typePrint(ostream& os, int indent) const;
  string generateCode();

 private:
  CompoundStmtNode(const CompoundStmtNode&);

  list<StmtNode*>   *stmts_;
};

/****************************************************************/

class IfNode: public StmtNode{
 public: 
  
  IfNode(ExprNode* cond, StmtNode* thenStmt, 
		 StmtNode* elseStmt=NULL, int line=0, int column=0, string file="");
  ~IfNode(){};
  //AstNode* clone() 
  //  { return new IfNode(*this); }

  const ExprNode* cond() const {return cond_;}
  const StmtNode* elseStmt() const { return else_;};
  const StmtNode* thenStmt() const  { return then_;};

  ExprNode* cond() {return cond_;}      
  StmtNode* elseStmt() { return else_;};
  StmtNode* thenStmt() { return then_;};

  const Type* typeCheck();
  void print(ostream& os, int indent) const;
  void  typePrint(ostream& os, int indent) const;
  string generateCode();

 private: 
  ExprNode *cond_;
  StmtNode *then_, *else_;

  IfNode(const IfNode&);
};

/****************************************************************/

class RuleNode: public AstNode {
 public:
  RuleNode(BlockEntry *re, BasePatNode* pat, StmtNode* reaction, 
	   int line=0, int column=0, string file="");
  ~RuleNode() {};
  //AstNode* clone() 
  //  { return new RuleNode(*this); }

  const BlockEntry* ruleEntry() const { return rste_; };
  BlockEntry* ruleEntry() { return rste_; };

  const BasePatNode* pat() const { return pat_; };
  BasePatNode* pat() { return pat_; };              

  const StmtNode* reaction() const { return reaction_; };   
  StmtNode* reaction() { return reaction_; };   

  const Type* typeCheck();
  void print(ostream& os, int indent=0) const;
  void  typePrint(ostream& os, int indent) const;
  string generateCode();

 private:
  BlockEntry    *rste_;
  BasePatNode *pat_;
  StmtNode *reaction_;
   
  RuleNode(const RuleNode&);
};

/****************************************************************/
#endif
