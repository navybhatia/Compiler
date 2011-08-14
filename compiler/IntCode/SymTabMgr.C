#include "SymTabMgr.h"
#include "STEClasses.h"

SymTabMgr::SymTabMgr() { 
  top_ = 0;
  scopeStack_[top_] = new GlobalEntry(string("GLOBAL")) ;
  isNameSpace_[top_] = false;
}

SymTabMgr::~SymTabMgr() { 
  // To be filled in later.
}

const SymTabEntry* 
SymTabMgr::lookUp(string name) const {
  const SymTabEntry *ste = 0;
  for (int i = top_; i >= 0; i--) {
    const SymTab* st = scopeStack_[i]->symTab();
    if ((st != NULL) && ((ste = st->lookUp(name)) != NULL))
      break;
  }
  return ste;
}

const SymTabEntry* 
SymTabMgr::lookUpInScope(string name, SymTabEntry::Kind k) const {
  const SymTabEntry *ste = currentScope(k);
  if ((ste != NULL) && (ste->symTab() != NULL)) {
    ste = ste->symTab()->lookUp(name);
    return ste;
  }
  else return NULL;
}


const SymTabEntry* 
SymTabMgr::currentScope() const {
  for (int i = top_; i >= 0; i--) {
    if (!isNameSpace_[i]) 
      return scopeStack_[i];
  }
  return NULL;
}	

const SymTabEntry* 
SymTabMgr::currentScope(SymTabEntry::Kind k) const {
  for (int i = top_; i >= 0; i--) {
    if (scopeStack_[i]->kind() == k) 
      return scopeStack_[i];
  }
  return NULL;
}	

ErrorST 
SymTabMgr::enterScope(SymTabEntry* ste) {
  if (ste == NULL)
    return INVALID_SCOPE;
  if (++top_ >= MAX_SCOPE_STACK_SIZE) {
    errMsg("Too many nested levels of scope, exiting");
    exit(-1);
  }
  scopeStack_[top_] = ste;
  isNameSpace_[top_] = false;
  return OK;
}

ErrorST 
SymTabMgr::leaveScope() {
  //cerr << "leaveScope called, top_=" <<  top_ << endl;
  if (top_ > 0) {
    top_--;
    return OK;
  }
  return INVALID_SCOPE;
}

ErrorST 
SymTabMgr::leaveScope(SymTabEntry::Kind kind) {
  ErrorST rv = leaveToScope(kind);
  if (rv == OK)  
	return leaveScope();
  else return rv;
}

ErrorST 
SymTabMgr::leaveToScope(SymTabEntry::Kind kind) {
  ErrorST rv=OK;
  for (int i = top_; (i > 0) && (scopeStack_[i]->kind() != kind); i--)
	rv = leaveScope();
  return rv;
}

ErrorST 
SymTabMgr::insert(SymTabEntry* se) { 
  for (int i=top_; i >= 0; i--) 
    if (scopeStack_[i] != NULL && !isNameSpace_[i])
      return (scopeStack_[i]->insert(se));
  return INVALID_SCOPE;
}
