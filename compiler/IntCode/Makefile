CC = g++
CFLAGS = -c -g -Wno-deprecated
FLEX = flex
BISON = bison

demo:	driver.o E--_lexer.o E--_parser.o E--.tab.h Ast.o STEClasses.o SymTabMgr.o Value.o Type.o SymTabEntry.o Error.o ParserUtil.o SymTab.o nSTEClasses.o nAst.o RegTable.o genIntermediateCode.o MemManage.o IntermediateCode.o Misc.o CommonSubExpr.o
	$(CC) -o demo E--_lexer.o E--_parser.o driver.o Ast.o STEClasses.o SymTabMgr.o Value.o Type.o SymTabEntry.o Error.o ParserUtil.o SymTab.o nSTEClasses.o nAst.o RegTable.o genIntermediateCode.o MemManage.o IntermediateCode.o Misc.o CommonSubExpr.o -lfl -ggdb

E--_lexer.o:	E--_lexer.C E--.tab.h
	$(CC) $(CFLAGS) E--_lexer.C

E--_lexer.C:	E--_lexer.l++
	$(FLEX) -p -8 -Ce -oE--_lexer.C E--_lexer.l++

E--_parser.o: E--_parser.C E--.tab.h
	$(CC) -c $(CFLAGS) E--_parser.C

E--_parser.C: E--_parser.y++
	$(BISON) -t -d -v -o E--_parser.C E--_parser.y++; \
    mv E--_parser.H E--.tab.h

SymTabMgr.o: SymTabMgr.h SymTabMgr.C
	$(CC) -c $(CFLAGS) SymTabMgr.C

SymTabEntry.o: SymTabEntry.h SymTabEntry.C
	$(CC) -c $(CFLAGS) SymTabEntry.C

STEClasses.o: STEClasses.h STEClasses.C
	$(CC) -c $(CFLAGS) STEClasses.C

Ast.o: Ast.h Ast.C
	$(CC) -c $(CFLAGS) Ast.C

nSTEClasses.o: STEClasses.h nSTEClasses.C
	$(CC) -c $(CFLAGS) nSTEClasses.C

nAst.o: Ast.h nAst.C
	$(CC) -c $(CFLAGS) nAst.C

Value.o: Value.h Value.C
	$(CC) -c $(CFLAGS) Value.C

Type.o: Type.h Type.C
	$(CC) -c $(CFLAGS) Type.C

Error.o: Error.h Error.C
	$(CC) -c $(CFLAGS) Error.C

ParserUtil.o: ParserUtil.h ParserUtil.C
	$(CC) -c $(CFLAGS) ParserUtil.C

SymTab.o: SymTab.h SymTab.C
	$(CC) -c $(CFLAGS) SymTab.C

RegTable.o: RegTable.h RegTable.cpp
	$(CC) -c $(CFLAGS) RegTable.cpp

genIntermediateCode.o: genIntermediateCode.h genIntermediateCode.cpp
	$(CC) -c $(CFLAGS) genIntermediateCode.cpp

MemManage.o: MemManage.h MemManage.cpp
	$(CC) -c $(CFLAGS) MemManage.cpp

CommonSubExpr.o: CommonSubExpr.h CommonSubExpr.cpp
	$(CC) -c $(CFLAGS) CommonSubExpr.cpp

Misc.o: Misc.h Misc.cpp
	$(CC) -c $(CFLAGS) Misc.cpp

IntermediateCode.o: IntermediateCode.h IntermediateCode.cpp
	$(CC) -c $(CFLAGS) IntermediateCode.cpp

driver.o: driver.C
	$(CC) -c $(CFLAGS) -DTEST_TYPE driver.C

clean:
	-echo "Removing all object files!"
	-rm -f demo *.o
	-echo "Removing intermediate files!"
	-rm -f E--_parser.C
	-rm -f E--_parser.output
