JMP main 
main:  MOVI 0 R999 
JMP ML1 
RL3:  MOVI 1 R002 
MOVI 10000 R777 
MOVS "NAVDEEP" R777 
PRTS R777 
PRTS "c\n" 
JMP L4 
ML1:  MUL 128 0 R001 
ADD 10999 R001 R001 
ADD R001 97 R004 
STI -1 R004 
ADD R001 98 R004 
STI -1 R004 
ADD R001 99 R004 
STI 1 R004 
ADD R001 100 R004 
STI -1 R004 
MUL 128 1 R001 
ADD 10999 R001 R001 
ADD R001 97 R004 
STI -1 R004 
ADD R001 98 R004 
STI -1 R004 
ADD R001 99 R004 
STI -1 R004 
ADD R001 100 R004 
STI -1 R004 
MOVI 1 R003 
MOVI 0 R002 
L2:  IN R000 
JMPC GT 0 R000 L0 
JMPC GT 0 R002 L4 
MUL 128 R002 R004 
ADD 10999 R004 R004 
ADD R000 R004 R004 
LDI R004 R005 
JMPC EQ R005 R003 RL3 
MOVI R005 R002 
L4:  JMP L2 
L0:  MOVI 31000 R000 
MOVS "The End" R000 
PRTS R000 