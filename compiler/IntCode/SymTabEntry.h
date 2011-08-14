#ifndef STE_H
#define STE_H

#include "SymTab.h"
#include "Value.h"
#include "ProgramElem.h"

class SymTabEntry;
class ExprNode;

class SymTabEntry: public ProgramElem {
 public:
  enum Kind {
    // If this enum is changed, kindTag() needs to be updated correspondingly
	UNKNOWN_KIND,
	GLOBAL_KIND, 
    CLASS_KIND, FUNCTION_KIND,
	VARIABLE_KIND, 
	EVENT_KIND,
	RULE_KIND, 
	BLOCK_KIND, RULE_BLOCK_KIND, EVENT_BLOCK_KIND, 
	ERROR_KIND
  };

  static string kindTag(Kind k);

 public:
  /*****************************************************************
	The implementation of this class, as well its descendents.
	makes definite assumtions about the use of kind, name, prev, next
	and the symbol table fields. As such, methods operating on these
	fields are non-virtual. (Overriding them in subclasses is strongly
	discouraged.) All other operations are virtual, whether or not
	we can see any potential use of overriding them.
  ****************************************************************/

  SymTabEntry(string name, Kind kind, int line, int column, string file,
	      Type* t=NULL);
  virtual ~SymTabEntry() {};

  string name() const { return name_; }
  void name(string str) { name_=str; };

  Kind kind() const { return kind_; } 
  void kind(Kind kind) { kind_ = kind; };

  const SymTab* symTab() const { return st_; };

  const SymTabEntry* prev() const { return prev_ ;};
  const SymTabEntry* next() const  { return next_ ;};

  void prev(SymTabEntry *se) { prev_ = se; }
  void next(SymTabEntry *se) { next_ = se; }

  SymTab* symTab() { return st_; };
  SymTabEntry* prev() { return prev_ ;};
  SymTabEntry* next()  { return next_ ;};

  ErrorST insert(SymTabEntry* ste) {
    if (st_ == NULL) st_ = new SymTab();
    return st_->insert(ste);
  }

  virtual bool operator==(const SymTabEntry&) const;
  virtual bool operator!=(const SymTabEntry& ste) const 
  { return !operator==(ste); };

  virtual void print(ostream& os, int indent=0) const {};
  virtual void typePrint(ostream& os, int indent=0) const {};
  virtual void printST(ostream& os,int ind=0,char ldelim='{',char rdelim='}',
					   bool linebreaks=true, int first=0, int last=0) const {
	  if (symTab() != NULL) 
		symTab()->printST(os, ind, ldelim, rdelim, linebreaks, first, last);
  }
  virtual void typePrintST(ostream& os,int ind=0,char ldelim='{',
						   char rdelim='}', bool linebreaks=true, int first=0, 
						   int last=0) const {
	  if (symTab() != NULL) 
		symTab()->typePrintST(os, ind, ldelim, rdelim, linebreaks, first,last);
  }
  virtual const Type* typeCheck() { 
	if (symTab() != NULL) return symTab()->typeCheck();
    else return &(Type::voidType);};

  virtual void generateCode() { 
	//if (symTab() != NULL) return symTab()->generateCode();
    };

 private:
  string name_;
  Kind kind_;
  SymTab* st_;

 private: 
  // These two fields are used to link the STEs so that their order
  // of declaration can be maintained.
  SymTabEntry* next_;
  SymTabEntry* prev_;

 private:
  const SymTabEntry& operator=(const SymTabEntry&); // Disable assignment
  SymTabEntry(const SymTabEntry&);                  // and copy constructor
};
	
inline ostream& operator<<(ostream& out, const SymTabEntry* ste){
  ste->print(out, 0);
  return out;
}
#endif
