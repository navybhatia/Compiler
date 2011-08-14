#include "all.h"
#include "SymTab.h"
#include "SymTabEntry.h"

SymTab::iterator&
SymTab::iterator::operator++ () {
 if (current != NULL)  
   current = current->next();
 return *this; 
}

SymTab::const_iterator&
SymTab::const_iterator::operator++ () {
  if (current != NULL) 
    current = current->next();
  return *this; 
}

SymTab::SymTab(int n):map_(n) {
  first_ = last_ = NULL;
}

const SymTabEntry* 
SymTab::lookUp(string name) const {
  if (map_.count(name) > 0)
    return map_.find(name)->second; 
  else return NULL;
}

ErrorST 
SymTab::insert(SymTabEntry* se) {
  if (map_.count(se->name()) > 0) {
    //errMsg("Duplicate Symbol " + se->name());
    return DUPLICATE_SYM;
  } 
  else {
    map_[se->name()] = se;

    se->next(NULL);
    se->prev(last_);
    if (map_.size() == 1)
      first_ = se;
	else last_->next(se);
	last_ = se;
  }
//  if (map_.bucket_count() <= map_.size())
//    map_.resize(map_.bucket_count()*2);
  return OK;
}

const Type*
SymTab::typeCheck() {
  for (SymTab::iterator it = begin(); (it != end()); ++it) {
	SymTabEntry *ste = (SymTabEntry *)(*it);
	ste->typeCheck();
  }
  return &(Type::voidType);
}

void 
SymTab::printST(ostream& os, int indent, char leftdelim, char rightdelim, 
				bool linebreak, int first, int last) const {
  int i; SymTab::const_iterator it = begin(); int n_printed=0;

  if ((first == 0) && (last == 0))
	last = 1000000;

  for (i=0; (it != end()) && (i < last); i++, ++it)  {
	SymTabEntry *ste = (SymTabEntry *)(*it);
	if (i >= first) {
	  if ((ste->kind() != SymTabEntry::RULE_BLOCK_KIND) && 
		  (ste->kind() != SymTabEntry::EVENT_BLOCK_KIND) && 
		  (ste->name() != "any")) {
		n_printed++;
	  }
	}
  }

  if (leftdelim != '\0') {
	os << leftdelim;
	if ((n_printed > 0) && (linebreak))
	  prtln(os, indent+STEP_INDENT);
  }

  for (i=0, it=begin();
	   (it != end()) && (i < last); i++, ++it)  {
	SymTabEntry *ste = (SymTabEntry *)(*it);
	if (i >= first) {
	  if ((ste->kind() != SymTabEntry::RULE_BLOCK_KIND) && 
		  (ste->kind() != SymTabEntry::EVENT_BLOCK_KIND) && 
		  (ste->name() != "any")) {
		ste->print(os,indent+STEP_INDENT);
		if ((leftdelim == '\0') && (rightdelim != '\0'))
		  os << rightdelim;
		if (--n_printed > 0) {
		  if (linebreak)
			prtln(os,indent+STEP_INDENT);
		  else os << ", ";
		}
		else if (linebreak)
		  prtln(os,indent);
	  }
	}
  }

  if (leftdelim != '\0') // This is not a typo -- we shd check leftdelim
	os << rightdelim; 
  //if (linebreak)
  //prtln(os, indent);
}

void 
SymTab::typePrintST(ostream& os, int indent, char leftdelim, char rightdelim, 
				bool linebreak, int first, int last) const {
  int i; SymTab::const_iterator it = begin(); int n_printed=0;

  //os << "printST(" << indent << ")";
  if ((first == 0) && (last == 0))
	last = 1000000;

  for (i=0; (it != end()) && (i < last); i++, ++it)  {
	SymTabEntry *ste = (SymTabEntry *)(*it);
	if (i >= first) {
	  if ((ste->kind() != SymTabEntry::RULE_BLOCK_KIND) && 
		  (ste->kind() != SymTabEntry::EVENT_BLOCK_KIND) && 
		  (ste->name() != "state") && (ste->name() != "other") &&
		  (ste->name() != "any")) {
		n_printed++;
	  }
	}
  }

  if (leftdelim != '\0') {
	os << leftdelim;
	if ((n_printed > 0) && (linebreak))
	  prtln(os, indent+STEP_INDENT);
  }

  for (i=0, it=begin();
	   (it != end()) && (i < last); i++, ++it)  {
	SymTabEntry *ste = (SymTabEntry *)(*it);
	if (i >= first) {
	  if ((ste->kind() != SymTabEntry::RULE_BLOCK_KIND) && 
		  (ste->kind() != SymTabEntry::EVENT_BLOCK_KIND) && 
		  (ste->name() != "state") && (ste->name() != "other") &&
		  (ste->name() != "any")) {
		ste->typePrint(os,indent+STEP_INDENT);
		if ((leftdelim == '\0') && (rightdelim != '\0'))
		  os << rightdelim;
		if (--n_printed > 0) {
		  if (linebreak)
			prtln(os,indent+STEP_INDENT);
		  else os << ", ";
		}
		else if (linebreak)
		  prtln(os,indent);
	  }
	}
  }

  if (leftdelim != '\0') // This is not a typo -- we shd check leftdelim
	os << rightdelim; 
  //if (linebreak)
  //prtln(os, indent);
}
