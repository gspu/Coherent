DESTDIR=/bin386/usr/bin/
DESTDIR2=/bin386/
CC=cc
CFLAGS = -AO -I$(SRCPATH)
#For the PDP-11, do not bind shared:
LDFLAGS = -so
LFLAGS = -lcurses

OBJS=ansi.o basic.o buffer.o display.o error.o file.o fileio.o line.o main.o\
	random.o region.o search.o spawn.o termio.o tinfo.o window.o word.o\
	vt52.o lookup.o helplib.o bracket.o comtab.o execute.o tcap.o

SRCS=ansi.c basic.c buffer.c display.c error.c file.c fileio.c line.c\
	main.c random.c region.c search.c spawn.c tinfo.c tcap.c termio.c\
	vt52.c window.c word.c lookup.c helplib.c bracket.c ext.c execute.c\
	ed.h Makefile

emacs: $(OBJS)
	$(CC) $(LDFLAGS) $(DESTDIR)me $(OBJS) $(LFLAGS)

detab : detab.c
	$(CC) -o $(DESTDIR2)$@ $<

install : emacs detab
	strip $<
	su root cp emacs /bin/me
	su root cp detab /bin
 
comtab.c : ext.o
	$(CC) ext.o
	ext > comtab.c
	rm ext

ext.o: ext.c ed.h
	$(CC)  -VPSTR -c `srcpath ext.c`

comtab.o: comtab.c ed.h
execute.o: execute.c ed.h
ansi.o: ansi.c ed.h
basic.o: basic.c ed.h
buffer.o: buffer.c ed.h
display.o: display.c ed.h
error.o: error.c ed.h
file.o: file.c ed.h
fileio.o: fileio.c ed.h
line.o: line.c ed.h
main.o: main.c ed.h
random.o: random.c ed.h
region.o: region.c ed.h
search.o: search.c ed.h
spawn.o: spawn.c ed.h
tinfo.o: tinfo.c ed.h
tcap.o: tcap.c ed.h
termio.o: termio.c ed.h
vt52.o: vt52.c ed.h
window.o: window.c ed.h
word.o: word.c ed.h
lookup.o: lookup.c
helplib.o: helplib.c
