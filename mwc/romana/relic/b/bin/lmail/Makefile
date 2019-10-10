#
#	Makefile for the mail system.
#
#	CFLAGS = -O -DHPCOHERENT	: if on Inetco HP Coherent
#	CFLAGS = -O			: if on MWC Coherent
#
DESTDIR=/bin386/bin/
LIBS=-lmisc
CFLAGS=-O
O = send.o lmail.o util.o copyright.o
S = send.c lmail.c util.c mail.h Makefile

lmail : $O
	${CC} -n -o $(DESTDIR)lmail $O ${LIBS}
#	fixstack 1000 $(DESTDIR)lmail
	su root chmog 511 bin bin $(DESTDIR)lmail

copyright.o : copyright.c
