#include "E--_RT.c"
int main(int argc, char *argv[]) {
	E_RT_init(argc, argv);
JMP(main);
main: MOVI(0, R999);
JMP(ML1);
RL3: MOVI(2, R901);
MOVI(10000, R777);
MOVS("NAVDEEP", R777);
PRTS(R777);
PRTS("a:b\n");
JMP(L4);
RL5: MOVI(2, R904);
MOVI(10000, R777);
MOVS("NAVDEEP", R777);
PRTS(R777);
PRTS("a OR b:c\n");
JMP(L6);
ML1: R900=MUL(128,0);
R900=ADD(10999,R900);
R001=ADD(R900,97);
STI(1, R001);
R001=ADD(R900,98);
STI(-1, R001);
R001=ADD(R900,99);
STI(-1, R001);
R001=ADD(R900,100);
STI(-1, R001);
R900=MUL(128,1);
R900=ADD(10999,R900);
R001=ADD(R900,97);
STI(-1, R001);
R001=ADD(R900,98);
STI(2, R001);
R001=ADD(R900,99);
STI(-1, R001);
R001=ADD(R900,100);
STI(-1, R001);
R900=MUL(128,2);
R900=ADD(10999,R900);
R001=ADD(R900,97);
STI(-1, R001);
R001=ADD(R900,98);
STI(-1, R001);
R001=ADD(R900,99);
STI(-1, R001);
R001=ADD(R900,100);
STI(-1, R001);
MOVI(2, R902);
MOVI(0, R901);
R903=MUL(128,0);
R903=ADD(11383,R903);
R001=ADD(R903,97);
STI(1, R001);
R001=ADD(R903,98);
STI(1, R001);
R001=ADD(R903,99);
STI(-1, R001);
R001=ADD(R903,100);
STI(-1, R001);
R903=MUL(128,1);
R903=ADD(11383,R903);
R001=ADD(R903,97);
STI(-1, R001);
R001=ADD(R903,98);
STI(-1, R001);
R001=ADD(R903,99);
STI(2, R001);
R001=ADD(R903,100);
STI(-1, R001);
R903=MUL(128,2);
R903=ADD(11383,R903);
R001=ADD(R903,97);
STI(-1, R001);
R001=ADD(R903,98);
STI(-1, R001);
R001=ADD(R903,99);
STI(-1, R001);
R001=ADD(R903,100);
STI(-1, R001);
MOVI(2, R905);
MOVI(0, R904);
L2: IN(R000);
JMPC(GT(0, R000), L0);
JMPC(GT(0, R901), L4);
R001=MUL(128,R901);
R001=ADD(10999,R001);
R001=ADD(R000,R001);
LDI(R001, R002);
JMPC(EQ(R002, R902), RL3);
MOVI(R002, R901);
L4: JMPC(GT(0, R904), L6);
R001=MUL(128,R904);
R001=ADD(11383,R001);
R001=ADD(R000,R001);
LDI(R001, R002);
JMPC(EQ(R002, R905), RL5);
MOVI(R002, R904);
L6: JMP(L2);
L0: MOVI(31000, R000);
MOVS("The End", R000);
PRTS(R000);

	E_RT_exit(); return 0;
}