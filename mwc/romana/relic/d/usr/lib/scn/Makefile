DESTDIR=/usr/lib/
# Parts of scn
O = y.tab.o lex.o

# Parts of libscn
L = 	yesNo.o \
	nonNull.o \
	numeric.o \
	userCtlc.o \
	getStuff.o \
	checkEnd.o \
	clearbak.o \
	cleararea.o \
	noWhite.o \
	noVerify.o \
	showbak.o \
 	showMsg.o \
	query.o \
	fatal.o \
	closeup.o \
	longfield.o \
	setup.o

CFLAGS = -O -A -I. -I..

all : scn libscn.a
	echo all done

install : libscn.a scn scn.h
	su root `srcpath installer`

libscn.a : $L
	rm -f $(DESTDIR)libscn.a
	ar rcs $(DESTDIR)libscn.a $L

getStuff.o : getStuff.c longfield.c

longfield.c : longfield scn
	./scn `srcpath longfield`

longfield.o : longfield.c

scn : $O
	cc -o scn $O -lmisc

y.tab.o : y.tab.c

y.tab.c : scn.y
	yacc $<

lex.o : lex.c y.tab.c scn.h
	cc -A -I. -I.. -c -O `srcpath lex.c`
