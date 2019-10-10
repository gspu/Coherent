#
#  12/2/90
#  Makefile for dcp (a UUCP clone) which implements a uucico
#  type file transfer and remote execution protocol.
#

IDIR=../include
ADIR=../aux
MDIR=../modemcap
#DESTDIR=/interim/usr/lib/uucp/

CFLAGS= -O -I$(IDIR) -I$(MDIR)
LFLAGS= -f

OBJS=	dcpmisc.o dcpxfer.o dcpxf2.o dcputil.o gpkt.o
SOBJS=	dcpS.o dcpunixS.o dcpsys.o $(MDIR)/fixttyS.o
TOBJS=	dcpT.o dcpunixT.o dcpsys.o $(MDIR)/fixttyT.o
BOBJS=	dcpB.o dcpunixS.o dcpsysB.o $(MDIR)/fixttyS.o

ALIB=	$(ADIR)/libuuaux.a
MLIB=	$(MDIR)/libmodemcap.a
LIBX=	/usr/lib/libndir.a

all: uucico.S uucico.T uucico.B
	@

uucico.S: $(OBJS) $(SOBJS) $(MLIB) $(ALIB) $(LIBX)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 uucp uucp $(DESTDIR)uucico.S

uucico.T: $(OBJS) $(TOBJS) $(MLIB) $(ALIB) $(LIBX)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 uucp uucp $(DESTDIR)uucico.T

uucico.B: $(OBJS) $(BOBJS) $(MLIB) $(ALIB) $(LIBX)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 uucp uucp $(DESTDIR)uucico.B

support:
	(cd ..; make support)


#
#  how to make some specific objects
#

dcpS.o:	dcp.c
	$(CC) $(CFLAGS) -DSGTTY -c -o $@ $<

dcpT.o: dcp.c
	$(CC) $(CFLAGS) -DTERMIO -c -o $@ $<

dcpB.o: dcp.c
	$(CC) $(CFLAGS) -DBBS -c -o $@ $<

dcpunixS.o: dcpunix.c
	$(CC) $(CFLAGS) -DSGTTY -c -o $@ $<

dcpunixT.o: dcpunix.c
	$(CC) $(CFLAGS) -DTERMIO -c -o $@ $<

dcpsysB.o: dcpsys.c
	$(CC) $(CFLAGS) -DBBS -c -o $@ $<
