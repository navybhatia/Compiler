#ifndef  SymTabMgr_H
#define  SymTabMgr_H

#include "SymTabEntry.h"

const static int MAX_SCOPE_STACK_SIZE = 10000;

class SymTabMgr {
 public:
  SymTabMgr();
  virtual ~SymTabMgr();

  const SymTabEntry* lookUp(string name) const;
  const SymTabEntry* lookUpInScope(string name, SymTabEntry::Kind k) const;
  const SymTabEntry* currentScope() const;
  const SymTabEntry* currentScope(SymTabEntry::Kind) const;

  SymTabEntry* lookUp(string name)
    {return (SymTabEntry*)(((const SymTabMgr*)this)->lookUp(name));}
  SymTabEntry* lookUpInScope(string name, SymTabEntry::Kind k)
    {return (SymTabEntry*)(((const SymTabMgr*)this)->lookUpInScope(name,k));}
  SymTabEntry* currentScope()
    {return (SymTabEntry*)(((const SymTabMgr*)this)->currentScope());}
  SymTabEntry* currentScope(SymTabEntry::Kind k)
    {return (SymTabEntry*)(((const SymTabMgr*)this)->currentScope(k));}

  ErrorST enterScope(SymTabEntry *ste);
  ErrorST leaveScope();
  ErrorST leaveScope(SymTabEntry::Kind k);
  ErrorST leaveToScope(SymTabEntry::Kind k);
  ErrorST insert(SymTabEntry *ste);

 private:
  const SymTabMgr& operator=(const SymTabMgr&); // Disable assignment and
  SymTabMgr(const SymTabMgr&);                  // copy constructor

 private: 
  int top_; 
  SymTabEntry* scopeStack_[MAX_SCOPE_STACK_SIZE];
  bool isNameSpace_[MAX_SCOPE_STACK_SIZE];
};

extern SymTabMgr stm;
#endif
