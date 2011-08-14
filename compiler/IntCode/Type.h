#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>
#include "Error.h"

class Type;
class Value;
class ProgramElem;
class SymTabEntry; 

class Type {
 public:
  // Dont change the order below --- it is necessary for isSubType() logic to 
  // work correctly. Also, typeName[], typeSize[], type[] need to be in sync.
  enum TypeTag  {
    ERROR, UNKNOWN, VOID, 
    BOOL, 
    STRING,
    BYTE, 
    UINT, INT,
    DOUBLE,
    CLASS, 
    EVENT, FUNCTION,
    NUMERIC, /* BYTE to DOUBLE */
    INTEGRAL, /* BYTE to INT*/
    SIGNED, /* INT, DOUBLE */
    UNSIGNED, /* BYTE, UINT */
    SCALAR, /* BOOL to DOUBLE*/
    PRIMITIVE, /* BOOL to DOUBLE */
    NATIVE, /* BOOL to DOUBLE */
    DATA, /* VOID to CLASS*/
    ANY   /* VOID to FUNCTION */
  };

 /***************** Define useful (static) functions on TypeTag */
 public:
  static const int SIZE_UNDEF;

  static const string name(TypeTag t);
  static int size(TypeTag t);

  static const Type errorType, voidType, unkType;
  static const Type boolType;
  static const Type stringType; 
  static const Type byteType, 
    uintType, intType, 
    doubleType;
  
  static const Type* type[];
  static bool isString(TypeTag t) { return (t==STRING);}
  static bool isNumeric(TypeTag t) 
	{ return (((t >= BYTE) && (t <= DOUBLE)) || (t==INTEGRAL)||(t==NUMERIC));}
  static bool isBool(TypeTag t) { return (t == BOOL); }
  static bool isInt(TypeTag t) {return ((t == INT) || (t == UINT)); }
  static bool isIntegral(TypeTag t) 
	{ return (((t >= BYTE) && (t <= INT)) || (t == INTEGRAL));}
  static bool isSigned(TypeTag t) {
    return ((t==INT) || (t==DOUBLE) || (t==SIGNED)); }
  static bool isUnsigned(TypeTag t) 
	{ return ((isIntegral(t) && !isSigned(t)) || (t==UNSIGNED));}
  static bool isFloat(TypeTag t) {return (t==DOUBLE);}
  static bool isPrimitive(TypeTag t) 
	{ return (((t >= BOOL) && (t <= DOUBLE)) || 
			  ((t >= NUMERIC) && (t<=PRIMITIVE)));}
  static bool isNative(TypeTag t) { 
    return isPrimitive(t); }
  static bool isData(TypeTag t) { return ((t >= VOID) && (t <= CLASS));}
  static bool isValid(TypeTag t) { return ((t >= VOID) && (t <= FUNCTION)); }
  static bool isScalar(TypeTag t) { return ((t >= BOOL) && (t <= DOUBLE)); }

 /**************** Define data members of Type and then its methods */

 private:
  TypeTag tag_;
  union {
    SymTabEntry* typeDesc_; // class
    struct {                // For event, function
      Type* retType_;       // Applicable only for functions
      vector<Type*>* argTypes_;  // Applicable for module, event, tuples
    };
  };

 public:
  Type(TypeTag tag=VOID);            // All elementary types
  Type(SymTabEntry* td, TypeTag t);  // Must be class 
  Type(vector<Type*>* tupleType, TypeTag t); // For events
  Type(vector<Type*>* argt, Type* rt);       // For functions
  Type(const Type& t) {operator=(t);};
  ~Type() {};
  
  const Type& operator=(const Type&);
  // Assignment does a deep copy for all fields except typeDesc_, which is
  // is a SymTabEntry* and has a reference semantics. (Note SymTabEntry
  // doesn't even have a copy constructor or assignment.)

  string name() const {return name(tag_); }
  string fullName() const;

  TypeTag tag() const { return tag_; }
  const SymTabEntry* typeDesc() const {
    if ((tag_ != CLASS)) return NULL;
    else return typeDesc_;
  };
  SymTabEntry* typeDesc() {
    if ((tag_ != CLASS)) return NULL;
    else return typeDesc_;
  };
  int arity() const {
    if ((tag_ != EVENT) && (tag_ != FUNCTION) || (argTypes_ == NULL)) return 0;
    else return argTypes_->size(); 
  };  
  const vector<const Type*>* argTypes() const { 
    if ((tag_ != EVENT) && (tag_ != FUNCTION)) return NULL;
    else return (vector<const Type*>*) argTypes_; 
  };  
  vector<Type*>* argTypes() { 
    if ((tag_ != EVENT) && (tag_ != FUNCTION)) return NULL;
    else return argTypes_; 
  };
  const Type* retType() const {
    if (tag_ != FUNCTION) return NULL;
    else return retType_;
  };

  void tag(TypeTag t) { tag_ = t; }
  void typeDesc(SymTabEntry* ste) {
    if ((tag_ == CLASS))
	  typeDesc_ = ste; 
    else errMsg("Type::typeDesc(SymTabEntry*) called when type = " + name()); 
  };
  void argTypes(vector<Type*>* t) { 
    if ((tag_ == EVENT) || (tag_ == FUNCTION)) 
	  argTypes_ = t;
    else errMsg("Type::argTypes(vector<Type*> *) called when type = " + name()); 
  };
  void retType(Type* t) {
    if (tag_ == FUNCTION) 
	  retType_ = t;
    else errMsg("Type::retType(Type *) called when type = " + name());
  };

  void print(ostream &os, int indent=0) const;

  bool isAssignable(const Type& t) const;// initialization with literal
  bool isSubType(const Type& t) const;   // Is this a subtype of t?
  bool isSubType(TypeTag t) const;       // Is this a subtype of t?
  bool isEquiv(const Type& t) const      // Type equivalence
    { return (isSubType(t) && t.isSubType(*this)); };

  bool operator==(const Type& t) const;  // strict equality
  bool operator!=(const Type& t) const { return !operator==(t);};

  void printArgs(ostream& os, int indent=0)const;
 private:

};

inline ostream& operator<< (ostream& os, const Type& tt) {
  tt.print(os);
  return os;
};
#endif
