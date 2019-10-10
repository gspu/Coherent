# Makefile to build utilities to convert Coherent uucp to Taylor 
# uucp configuration format.

DESTDIR=
#CFLAGS=-DDEBUG

CMDS=cvldev cvmodem cvlsys

LDEVSRC= ldev/cvldev.c
MDMSRC= modem/cvmodem.c
LSYSSRC= lsys/cvlsys.c lsys/rdperms.c lsys/getACU.c

LDEVOBJ=cvldev.o
MDMOBJ=cvmodem.o
LSYSOBJ=cvlsys.o rdperms.o getACU.o

all: $(CMDS)
	mv *.o objects/
	>done!

cvldev:	$(LDEVOBJ)
	cc $(LDEVSRC) -o $(DESTDIR)$@ 

cvmodem: $(MDMOBJ)
	cc $(MDMSRC) -o $(DESTDIR)$@

cvlsys:  $(LSYSOBJ)
	cc $(LSYSSRC) -o $(DESTDIR)$@

cvldev.o: $(LDEVSRC)
	cc -c $(CFLAGS) $@ $<

cvmodem.o: $(MDMDSRC)
	cc -c $@ $<

$(LSYSOBJ):	$(LSYSSRC)
	cc -c $(CFLAGS) $@ $<
