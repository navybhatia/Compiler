CC = g++
CFLAGS = -c -g -Wno-deprecated
FLEX = flex
BISON = bison

demo:	driver.o ICode_lexer.o ICode_parser.o ICode.tab.h 
	$(CC) -o demo ICode_lexer.o ICode_parser.o driver.o -lfl

ICode_parser.o:	ICode_parser.C ICode.tab.h
	$(CC) $(CFLAGS) ICode_parser.C

ICode_lexer.o: ICode_lexer.C ICode.tab.h
	$(CC) -c $(CFLAGS) ICode_lexer.C

ICode_lexer.C:	ICode.l
	$(FLEX) -p -8 -Ce -oICode_lexer.C ICode.l

ICode_parser.C: ICode.y
	$(BISON) -t -d -v -o ICode_parser.C ICode.y; \
    mv ICode_parser.H ICode.tab.h

driver.o: driver.C ICode_parser.C
	$(CC) -c $(CFLAGS) driver.C

clean:
	-echo "Removing all object files and compiled lexer and bison files!"
	-rm -f demo *.o ICode_parser.C ICode_lexer.C ICode.tab.h
