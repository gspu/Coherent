# Makefile for uucheckname, permissions checking interpreter
# part of uucheck the uucp sanity checker.
#
# Set this according to where you want this installed.
BIN=../install/usr/lib/uucheck/
DESTDIR=/interim/usr/lib/uucheck/

LIBS=
OBJS=main.o uucheckname.o gethostname.o parse_args.o lookup.o interact.o copyright.o
CFLAGS=-O -I..
#
uucheckname:	$(OBJS)
	$(CC) -o $(DESTDIR)$@ $(CFLAGS) $(OBJS) $(LIBS)
	su root chmog 511 bin bin $(DESTDIR)$@
parse_args.o: ../parse_args.c

copyright.o: ../copyright.c

install:
	cp -d uucheckname $(BIN)
	chown bin $(BIN)/uucheckname
	chgrp bin $(BIN)/uucheckname

clean:
	rm -f $(OBJS) core
