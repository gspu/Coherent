DESTDIR=/bin386/usr/bin/
SRCDIR= /usr/lib/terminfo
#CFLAGS= -A -O -DSRCDIR='"$(SRCDIR)"' -DTRACE	# compile with trace
CFLAGS= -A -O -DSRCDIR='"$(SRCDIR)"'
LIB=/usr/lib
INCL=/usr/include

SRCS=	read_entry.c dump.c dump_names.c  

infocmp: dump.o dump_names.o read_entry.o
	cc -o $(DESTDIR)infocmp dump.o dump_names.o read_entry.o

dump.o: dump.c term.h compiler.h

dump_names.o: dump_names.c

read_entry.o: term.h object.h
dump.o: compiler.h term.h
