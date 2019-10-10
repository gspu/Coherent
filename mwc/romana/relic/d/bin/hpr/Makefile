# hp/Makefile 11/12/90
# Make hp versions of the line printer daemon and spooler.
# This also makes the hp filter,
# for converting nroff output to HP LaserJet printer escape sequences (PCL).
# This Makefile and this directory should be integrated with lpr.

DESTDIR=/f1/bin/
ALL=\
	hp\
	hpd\
	hpr\
	hpskip
OBJ=\
	hp.o\
	hpr.o\
	hpskip.o\
	hpd1.o\
	hpd2.o\
	hpprint.o

# Primary target.
all:	$(ALL)
	: all done

# Secondary targets.
hp:	hp.o
	cc $?
hpd:	hpd1.o hpd2.o hpprint.o
	cc -o $@ hpd1.o hpd2.o hpprint.o
hpr:	hpr.o
	cc $?
hpskip:	hpskip.o
	cc $?

# Unlinked objects.
hp.o:	hp.c
hpr.o:	lpr.c
	cc -c -o $(DESTDIR)$@ -DLASER $?
hpskip.o:	lpskip.c
	cc -c -o $(DESTDIR)$@ -DLASER $?
hpd1.o:	lpd1.c
	cc -c -o $(DESTDIR)$@ -DLASER $?
hpd2.o:	lpd2.c
	cc -c -o $(DESTDIR)$@ $?
hpprint.o:	hpprint.c

clean:
	rm $(ALL) $(OBJ)
strip:	$(ALL)
	strip $(ALL)
	
