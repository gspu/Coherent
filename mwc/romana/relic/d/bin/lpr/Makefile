# /usr/src/cmd/lpr/Makefile 11/13/90
# Make normal and hp versions of the line printer daemon and spooler.
# This also makes the hp filter,
# for converting nroff output to HP LaserJet printer escape sequences (PCL).
#DESTDIR=/interim/bin/
#DESTDIR2=/interim/usr/lib/
DESTDIR=/bin386/bin/
DESTDIR2=/bin386/usr/lib/

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
	cc -O -o $(DESTDIR)$@ hp.o
	su root chmog 511 bin bin $(DESTDIR)$@

hpd:	hpd1.o lpd2.o hpprint.o
	cc -o $(DESTDIR2)$@ hpd1.o lpd2.o hpprint.o
	su root chmog 6511 daemon daemon $(DESTDIR2)$@

hpr:	hpr.o
	cc -O -o $(DESTDIR)$@ hpr.o
	su root chmog 6511 daemon daemon $(DESTDIR)$@

hpskip:	hpskip.o
	cc -O -o $(DESTDIR)$@ hpskip.o
	su root chmog 6511 daemon daemon $(DESTDIR)$@

lpr:	lpr.o
	cc -O -o $(DESTDIR)$@ lpr.o
	su root chmog 6511 daemon daemon $(DESTDIR)$@

lpd:	lpd1.o lpd2.o print.o
	cc -o $(DESTDIR2)$@ $?
	su root chmog 6511 daemon daemon $(DESTDIR2)$@

lpskip:	lpskip.o
	cc -O -o $(DESTDIR)$@ lpskip.o
	su root chmog 6511 daemon daemon $(DESTDIR)$@

prps:	prps.o
	cc -O -o $(DESTDIR)$@ $?
	su root chmog 6511 daemon daemon $(DESTDIR)$@


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
