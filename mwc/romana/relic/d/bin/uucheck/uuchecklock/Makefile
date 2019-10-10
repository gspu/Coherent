# Makefile for uuchecklock, which checks for uucp lock files
# part of uucheck, the uucp sanity checker.
#
# Set these next two according to where you want these installed.
BIN=../install/usr/lib/uucheck/
# BTW if LIB= anything other than /usr/lib/uucheck, you will have to edit
# uucheck by hand.
LIB=../install/usr/lib/uucheck/
DESTDIR=/interim/usr/lib/uucheck/
OBJS=main.o uuchecklock.o parse_args.o copyright.o
LIBS=
YACC=
LEX=
YACCFLAGS=
LEXFLAGS=
CFLAGS=-O -I..
#
uuchecklock:	$(OBJS)
	$(CC) -o $(DESTDIR)$@ $(CFLAGS) $(OBJS) $(LIBS)
	su root chmog 511 bin bin $(DESTDIR)$@

parse_args.o:	../parse_args.c

copyright.o:	../copyright.c

install:
	cp -d uuchecklock $(BIN)
	chown bin $(BIN)/uuchecklock
	chgrp bin $(BIN)/uuchecklock

clean:
	rm -f $(OBJS) core

