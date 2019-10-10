#
#	Makefile for kpatch command
#

kpatch:	y.tab.o lex.yy.o kpatch.o
	$(CC) -o $@ $< -ly -ll
	strip $@

y.tab.c: kpatch.yy patchl.h
	yacc kpatch.yy

y.tab.o: y.tab.c
	cc -c y.tab.c

lex.yy.c: kpatch.ll patchl.h
	lex kpatch.ll

lex.yy.o: lex.yy.c
	cc -c lex.yy.c
