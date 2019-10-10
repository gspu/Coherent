# Makefile for checkperms, permissions checking interpreter
# part of uucheck the uucp sanity checker.
#
# Destination directory for installing executable files
BIN=../install/usr/lib/uucheck/
# Destination directory for installing data files
# BTW if LIB= anything other than /usr/lib/uucheck/, you will have to edit
# uucheck by hand.
LIB=../install/usr/lib/uucheck/
DESTDIR=/interim/usr/lib/uucheck/
OBJS=y.tab.o lex.yy.o main.o monitor.o hosts.o do_check.o messages.o \
	chmod_chk.o util.o parse_args.o copyright.o
LIBS=-ly -ll
YACC=yacc
LEX=lex
YACCFLAGS=
LEXFLAGS=
CFLAGS=-O -I..
PROGS=uucp.perms checkperms
#
all:	$(PROGS)
	>done!

checkperms:	$(OBJS) uucp.perms
	$(CC) -o $(DESTDIR)checkperms $(CFLAGS) $(OBJS) $(LIBS)
	su root chmog 511 bin bin $(DESTDIR)$@

uucp.perms:	uucp.perms
		co $@ ; mv $@ $(DESTDIR)
		su root chmog 444 bin bin $(DESTDIR)$@

y.tab.o:	checkperms.y
	$(YACC) $(YACCFLAGS) checkperms.y
	$(CC) -c $(CFLAGS) y.tab.c

lex.yy.o:	checkperms.lex
	$(LEX) $(LEXFLAGS) checkperms.lex
	$(CC) -c $(CFLAGS) lex.yy.c

main.o:	checkperms.h monitor.h ../uucheck.h

parse_args.o: ../parse_args.c

copyright.o: ../copyright.c

install:
	cp -d checkperms $(BIN)/uucheckperms
	chown bin $(BIN)/uucheckperms $(LIB)/uucp.perms
	chgrp bin $(BIN)/uucheckperms $(LIB)/uucp.perms

clean:
	rm -f $(OBJS) y.tab.c y.tab.h lex.yy.c core

