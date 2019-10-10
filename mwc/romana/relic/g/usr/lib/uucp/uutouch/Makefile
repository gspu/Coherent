DESTDIR=/bin386/usr/bin/
LIBS=-lmisc
CFLAGS=-DTAYLOR_UUCP

uutouch:	uutouch.c taylor.c
	cc $(CFLAGS) $< -o $(DESTDIR)$@ $(LIBS)
	su root chmog 6511 uucp uucp $(DESTDIR)$@
