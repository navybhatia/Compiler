JMP main 
main:  MOVI 10000 R000 
STI R000 0 
MOVI 10000 R001 
STI R001 1 
MOVI 2 R999 
JMP ML1 
RL3:  MOVI 1 R901 
MOVI 10000 R777 
MOVS "NAVDEEP" R777 
PRTS R777 
MOVI 1 R003 
MOVI 1 R005 
MOVI 1 R007 
MOVI 1 R009 
MOVI 1 R011 
MOVI 1 R013 
MOVI 1 R015 
MOVI 1 R017 
MOVI 1 R019 
MOVI 0 R021 
L5:  JMPC GT R000 0 L7 
JMP L6 
L7:  MOVI 100000 R001 
L8:  JMPC GT R001 0 L10 
JMP L9 
L10:  ADD R005 R007 R023 
MUL R023 R023 R025 
MUL R025 R023 R027 
MUL R027 R023 R029 
MOVI R029 R003 
ADD R011 R013 R030 
MUL R030 R030 R032 
MUL R032 R030 R034 
MUL R034 R030 R036 
MOVI R036 R009 
ADD R017 R019 R037 
MUL R037 R037 R039 
MUL R039 R037 R041 
MUL R041 R037 R043 
MOVI R043 R015 
SUB R001 1 R044 
MOVI R044 R001 
JMP L8 
L9:  SUB R000 1 R045 
MOVI R045 R000 
JMP L5 
L6:  PRTS "\n" 
PRTS "Done" 
PRTS "\n" 
JMP L4 
ML1:  MUL 128 0 R900 
ADD 10999 R900 R900 
ADD R900 97 R046 
STI 1 R046 
MUL 128 1 R900 
ADD 10999 R900 R900 
ADD R900 97 R046 
STI -1 R046 
MOVI 1 R902 
MOVI 0 R901 
L2:  IN R002 
JMPC GT 0 R002 L0 
JMPC GT 0 R901 L4 
MUL 128 R901 R046 
ADD 10999 R046 R046 
ADD R002 R046 R046 
LDI R046 R047 
JMPC EQ R047 R902 RL3 
MOVI R047 R901 
L4:  JMP L2 
L0:  MOVI 31000 R002 
MOVS "The End" R002 
PRTS R002 
