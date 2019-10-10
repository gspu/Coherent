#
#	Makefile for the mail system.
#
#	CFLAGS = -O -DHPCOHERENT	: if on Inetco HP Coherent
#	CFLAGS = -O			: if on MWC Coherent
#
#DESTDIR=/bin386/bin/
CFLAGS=-DDEBUG
O = send.o mail.o util.o copyright.o
S = send.c mail.c util.c mail.h Makefile

mail : $O
	$(CC) -n -o $(DESTDIR)mail $O
	su root chmog 511 bin bin $(DESTDIR)mail

copyright.o : copyright.c

mail.a : $S
	rm mail.a
	ar q mail.a $S
