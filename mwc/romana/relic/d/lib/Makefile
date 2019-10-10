# Makefile to build the stuff in /lib

DESTDIR=/interim/lib/

all: 	yyparse.c libc.a libm.a crts0.o mcrts0.o
	>done

yyparse.c:
	co $(DESTDIR)$@
	su root chmog 444 bin bin $(DESTDIR)$@

libc.a:
	cd LIBCobjects; make SRCPATH=/newbits/lib/libc
	su root mv LIBCobjects/libc.a $(DESTDIR)
	su root chmog 444 bin bin $(DESTDIR)$@
	cd LIBCobjects ; rm C* S* G*

libm.a:
	su root rm $(DESTDIR)$@
	cd libm ; make

crts0.o:
	cd LIBCobjects
	cc -c /newbits/lib/libc/csu/crts0.s
	cp -d crts0.o $(DESTDIR)
	su root chmog 444 bin bin $(DESTDIR)crts0.o

mcrts0.o:
	cd LIBCobjects
	cc -c /newbits/lib/libc/csu/mcrts0.s
	cp -d mcrts0.o $(DESTDIR)
	su root chmog 444 bin bin $(DESTDIR)mcrts0.o
