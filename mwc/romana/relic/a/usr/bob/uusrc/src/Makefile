#
#  12/7/90
#  Makefile for various auxiliary uucp executables
#

IDIR=../include
ADIR=../aux
#DESTDIR=/interim/usr/bin/
#DESTDIR2=/interim/usr/lib/uucp/

CFLAGS= -O -I$(IDIR)
LFLAGS= -n

ALIB=	$(ADIR)/libuuaux.a
LIBX=	/usr/lib/libndir.a

all: support uucp uudecode uuencode uulog uuname uux uumkdir uutouch \
     uuxqt scripts uucico \
     @

uucp:	uucp.o $(ALIB)
	$(CC) $(CFLAGS) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 uucp uucp $(DESTDIR)uucp

uudecode: uudecode.o
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog  511 bin  bin  $(DESTDIR)uudecode

uuencode: uuencode.o
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog  511 bin  bin  $(DESTDIR)uuencode

uulog:	uulog.o $(ALIB)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog  511 bin  bin  $(DESTDIR)uulog

uuname:	uuname.o $(ALIB)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 uucp uucp $(DESTDIR)uuname

uux:	uux.o $(ALIB)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 uucp uucp $(DESTDIR)uux


uumkdir: uumkdir.o $(ALIB)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 root root $(DESTDIR)$@

uutouch: uutouch.o $(ALIB)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $<
	su root chmog 6511 uucp uucp $(DESTDIR)uutouch

uuxqt:	uuxqt.o $(ALIB) $(LIBX)
	$(CC) -o $(DESTDIR)$@ $(LFLAGS) $< -lmisc
	su root chmog 6511 uucp uucp $(DESTDIR)uuxqt

scripts:
	(cd ../script ; su root cp * $(DESTDIR))
	su root chmog 600 uucp uucp $(DESTDIR)L.sys
	su root chmog 600 uucp uucp $(DESTDIR)L-devices
	su root chmog 600 uucp uucp $(DESTDIR)Permissions

uucico:
	(cd ../dcp/objects ; export SRCPATH=.. ; make)

support:
	(cd ../aux; make)
	(cd ../modemcap ; make)
