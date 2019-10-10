# /usr/src/cmd0/sh/Makefile 4/29/93

DESTDIR=/bin386/bin/

# Version number for var.c.
VERSION=V4.0.7
# Parsing stack depth for y.tab.c.
YYMAXDEPTH=1024

#CFLAGS= -O2
CC=cc

# Undefine _copyright_ to pull in libc.a/copyright.o.
LDFLAGS= -n -i -u _copyright
# Don't bind shared on the PDP-11.
# LDFLAGS= -i

OBJ=\
	alloc.o\
	atoi.o\
	eval.o\
	exec1.o\
	exec2.o\
	exec3.o\
	extern.o\
	glob.o\
	lex.o\
	main.o\
	tab.o\
	trap.o\
	var.o\
	y.tab.o

SRCS=\
	sh.h\
	sh.y\
	alloc.c\
	atoi.c\
	eval.c\
	exec1.c\
	exec2.c\
	exec3.c\
	extern.c\
	glob.c\
	lex.c \
	main.c\
	tab.c\
	trap.c\
	var.c

# Primary target.
sh: $(OBJ)
	rm -f sh
	$(CC) $(LDFLAGS) -o $(DESTDIR)sh $(OBJ)
	strip $(DESTDIR)sh
	su root chmog 511 bin bin $(DESTDIR)sh

psh: $(OBJ)
	$(CC) -n -o psh -O -DPARANOID $(OBJ)

# Secondary targets.
alloc.o:	sh.h alloc.c
atoi.o:		atoi.c
eval.o:		sh.h eval.c
exec1.o:	sh.h exec1.c
exec2.o:	sh.h exec2.c
exec3.o:	sh.h exec3.c
extern.o:	sh.h extern.c
glob.o:		sh.h glob.c
lex.o:		sh.h lex.c y.tab.o
main.o:		sh.h main.c
tab.o:		sh.h tab.c
trap.o:		sh.h trap.c
var.o:		var.c sh.h
	$(CC) -c $(CFLAGS) -DVERSION=\"$(VERSION)\" $*.c
y.tab.c:	sh.y
	yacc $<
y.tab.o:	y.tab.c sh.h
	$(CC) -c $(CFLAGS) -DYYMAXDEPTH=$(YYMAXDEPTH) $*.c

# Etc.
clean:
	rm $(OBJ)

# end of /usr/src/cmd0/sh/Makefile
