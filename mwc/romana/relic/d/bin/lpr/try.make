# /usr/src/cmd/lpr/Makefile 11/13/90
# Make normal and hp versions of the line printer daemon and spooler.
# This also makes the hp filter,
# for converting nroff output to HP LaserJet printer escape sequences (PCL).
DESTDIR=/f1/bin/

ALL=\
	hp\
	hpd\
	hpr\
	hpskip\
	lpd\
	lpr\
	lpskip\
	prps
OBJ=\
	hp.o\
	hpr.o\
	hpskip.o\
	hpd1.o\
	hpprint.o\
	lpd1.o\
	lpd2.o\
	lpr.o\
	lpskip.o\
	print.o\
	prps.o

# Primary target.
all:	$(ALL)
	: all done

# Secondary targets.
hp:	hp.o
	cc $?
hpd:	hpd1.o lpd2.o hpprint.o
	cc -o $(DESTDIR)$@ hpd1.o lpd2.o hpprint.o
hpr:	lpr.c
	cc -o -DLASER $(DESTDIR)$@ lpr.c
hpskip:	lpskip.c
	cc -o -DLASER $(DESTDIR)$@ lpskip.c
lpr:	lpr.c
	cc -o $(DESTDIR)$@ lpr.c
lpd:	lpd1.o lpd2.o print.o
	cc -o $(DESTDIR)$@ $?
lpskip:	lpskip.c
	cc -o $(DESTDIR)$@ lpskip.c
prps:	prps.c
	cc -o $(DESTDIR)$@ prps.c

# Unlinked objects.
hpr.o:	lpr.c
	cc -c -o $@ -DLASER $?
hpskip.o:	lpskip.c
	cc -c -o $@ -DLASER $?
hpd1.o:	lpd1.c
	cc -c -o $@ -DLASER $?

# Etc.
clean:
	rm $(ALL) $(OBJ)
strip:	$(ALL)
	strip $(ALL)

# end of /usr/src/cmd/lpr/Makefile
