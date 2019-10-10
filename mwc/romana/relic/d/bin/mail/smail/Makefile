# Makefile for smail (not a installation makefile)

# @(#)Makefile	2.5 (smail) 9/15/87

DESTDIR=/bin386/bin/

CFLAGS	= -O -n 
#
# System V Release 2.0 sites can use -lmalloc for a faster malloc
#
# LIBS	=	-lmalloc
LIBS=-lmisc
CC = cc
OBJECTS =	main.o map.o resolve.o deliver.o misc.o alias.o pw.o \
		headers.o getpath.o str.o getopt.o gethostname.o copyright.o

all: smail svbinmail lcasep pathproc mkfnames nptx

smail:		$(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o $(DESTDIR)smail $(LIBS)
		su root chmog 6511 root root $(DESTDIR)smail
		su root ln -f $(DESTDIR)smail $(DESTDIR)rmail

$(OBJECTS):	defs.h
#		$(CC) $(CFLAGS) -c $<

svbinmail:	svbinmail.c defs.h
		$(CC) $(CFLAGS) svbinmail.c -o svbinmail

lcasep:		lcasep.c
		$(CC) $(CFLAGS) lcasep.c -o lcasep

pathproc:	pathproc.sh
		cp pathproc.sh pathproc
		chmod 755 pathproc

mkfnames:	mkfnames.sh
		cp mkfnames.sh mkfnames
		chmod 755 mkfnames

nptx:		nptx.o pw.o str.o
		$(CC) $(CFLAGS) nptx.o pw.o str.o -o nptx $(LIBS)

nptx.o:		nptx.c
		$(CC) $(CFLAGS) -c nptx.c

install:	all
		@echo read doc/Install

clean:
		rm -f *.o *.ln a.out core

clobber:	clean
		rm -f smail rmail lcasep pathproc mkfnames svbinmail nptx
