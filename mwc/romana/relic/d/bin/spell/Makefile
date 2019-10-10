# CFLAGS are not needed for i8086

DESTDIR=/interim/bin/
DESTDIR2=/interim/usr/dict/
DESTDIR3=/interim/usr/lib/

CFLAGS= -O 
UTILS= spellin spell spell.script

all:	$(UTILS)
	>all
# include file deftty.h defines the default ERASE, KILL and INTERRUPT 
# characters for ttys.

spell2.o: spell2.c
	$(CC) $(CFLAGS) spell2.c

spell.script:
	co $(DESTDIR)$@
	mv $(DESTDIR)$@ $(DESTDIR)spell
	su root chmog 555 bin bin $(DESTDIR)spell

spell:	spell.c spell2.o
	$(CC) $(CFLAGS) -o $(DESTDIR3)spell spell.c spell2.o
	su root chown bin $(DESTDIR3)spell
	su root chgrp bin $(DESTDIR3)spell
	su root chmod 555 $(DESTDIR3)spell

spellin: spellin.c spell2.o
	$(CC) $(CFLAGS) -o $(DESTDIR2)spellin spellin.c spell2.c
	su root chown bin $(DESTDIR2)spellin
	su root chgrp bin $(DESTDIR2)spellin
	su root chmod 511 $(DESTDIR2)spellin
