OFILES	= alc.o hash.o input.o logic.o main.o util.o
CFLAGS	= -O
DESTDIR = /interim/bin/

tsort:	$(OFILES)
	$(CC) $(CFLAGS) -o $(DESTDIR)tsort $(OFILES)
	su root chown bin $(DESTDIR)tsort
	su root chgrp bin $(DESTDIR)tsort
	su root chmod 511 $(DESTDIR)tsort

clean:
	-rm $(OFILES)

$(OFILES):	tsort.h
