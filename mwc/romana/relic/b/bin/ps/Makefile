# Makefile for Vlad's new ps command

DESTDIR=/bin386/bin/

SRC = ps.c

ps:	ps.o
	cc ps.c -o $(DESTDIR)ps
	chmog 6511 sys sys $(DESTDIR)ps
