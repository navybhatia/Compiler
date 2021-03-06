JMP begin
// Test function
prt3: ADD R900 1 R901
LDI R901 R050 // Return address now in R050
ADD R901 1 R901
LDF R901 F051 // param 1
ADD R901 1 R901
LDI R901 R052 // param 2
ADD R901 1 R901
LDF R901 F053 // param 3
ADD R901 1 R901
LDI R901 R054 // param 4
PRTS R054
PRTF F053
PRTS "*"
PRTI R052
PRTS "="
PRTF F051
PRTS "\n"
ADD R900 5 R900
JMPI R050
// 
// Here is where the main program starts
//
// Basic integer operations
begin: MOVI 1036 R000
MOVI 1172 R001
MOVI 1169 R002
SUB  R001 R000 R003
SUB  R001 R002 R004
ADD  R004 R003 R005
MUL  R003 R004 R006
DIV  R006 R004 R007
MOD  R001 R000 R008
NEG  R008 R008
// Basic FP operations
MOVF 1.036 F000
MOVF 1.172 F001
MOVF 1.169 F002
FSUB F001 F000 F003
FSUB F001 F002 F004
FADD F004 F003 F005
FMUL F003 F004 F006
FDIV F006 F004 F007
FNEG F007 F007
// Basic logical operations
MOVI 1036 R000
MOVI 1048 R001
MOVI 1052 R002
OR  R001 R000 R003
AND R001 R002 R004
XOR R001 R002 R004
// A simple loop
IN R010
ADD R010 1 R010
SUB R010 1 R020
IN R021
MOVIF R021 F022
FDIV F022 100.0 F022
IN R021
MOVIF R021 F021
FADD F021 F022 F021
MOVF 0.0 F011
L1: SUB  R010 1 R010
JMPC GE 0 R010 L2
FADD F021 F011 F011
JMP L1
// A simple function call
L2: PRTF F011
PRTS "\n"
MOVI 10000 R900 // init SP
MOVS "Computed using loop that " R022
STI R022 R900
SUB R900 1 R900 // pushed param 4
STF F021 R900
SUB R900 1 R900 // pushed param 3
STI R020 R900
SUB R900 1 R900 // pushed param 2
STF F011 R900
SUB R900 1 R900 // pushed param 1
MOVL L3 R025
STI R025 R900
SUB R900 1 R900 // pushed return addr
JMP prt3
L3: PRTI R900
PRTS "\n"
// Input two events: first arg of event is int, second is float
IN  R201
PRTI R201
PRTS "("
INI R200
PRTI R200
PRTS ", "
INF F200
PRTF F200
PRTS ")\n"
IN  R201
PRTI R201
PRTS "("
INI R200
PRTI R200
PRTS ", "
INF F200
PRTF F200
PRTS ")\n"
PRTS "\n***DONE***\n"
