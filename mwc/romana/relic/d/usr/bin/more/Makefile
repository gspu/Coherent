DESTDIR= /bin386/usr/bin/
PROG=	more
HELP=	more.help
CFLAGS=	-I../regexp -O
#LIBS=	../regexp/regexp.o -lterm
LIBS=	-ltermcap -lmisc

$(PROG):	$(PROG).o
	cc -o $(DESTDIR)$(PROG) $(PROG).o $(LIBS)
	su root strip $(DESTDIR)$(PROG)
	su root chown bin $(DESTDIR)$(PROG)
	su root chgrp bin $(DESTDIR)$(PROG)
	su root chmod 511 $(DESTDIR)$(PROG)
	su root co /newbits/usr/bin/more/$(HELP) ; mv $(HELP) /interim/usr/lib/
	su root chown bin /interim/usr/lib/$(HELP)
	su root chgrp bin /interim/usr/lib/$(HELP)
	su root chmod 444 /interim/usr/lib/$(HELP)

$(PROG).o:	more.c pathnames.h
	cc -c $(CFLAGS) more.c

