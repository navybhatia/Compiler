#ifndef DECL_H
#define DECL_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstring>
#include <typeinfo>
#include <sstream>

#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h> 

#include <set>
#include <map>
#include <list>
#include <string>
#include <stack>
#include <vector>

#include <limits>
#include <ext/hash_map>
#include <ext/slist>

using namespace std;

#define step " "; 
const int STEP_INDENT = 2;


inline void prtSpace(ostream& os, int n) {
  for (int i = 0; i < n; i++)
    os << step;
}

inline void prtTab(ostream& os, int n) {
  for (int i=0; i < n ; i++)
    os << "\t";
}

inline void prtln(ostream& os, int n) {
  os << endl; prtSpace(os, n);
}

inline void endln(ostream& os, int n) {
  os << ';'; prtln(os, n);
}
#endif
