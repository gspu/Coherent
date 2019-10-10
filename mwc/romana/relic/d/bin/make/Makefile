DESTDIR=/interim/bin/
DESTDIR2=/interim/usr/lib/
DESTDIR386=/bin386/bin/

# /usr/src/make/Makefile 2/14/91
# make's Makefile for make, what else?

make386:	make.o
	$(CC) $(LDFLAGS) -o $(DESTDIR386)make $?
	su root chown bin $(DESTDIR386)make
	su root chgrp bin $(DESTDIR386)make
	su root chmod 511 $(DESTDIR386)make

make:	make.o
	$(CC) $(LDFLAGS) -o $(DESTDIR)$@ $?
	su root chown bin $(DESTDIR)make
	su root chgrp bin $(DESTDIR)make
	su root chmod 511 $(DESTDIR)make

makemacros: makemacros
	cp $? $(DESTDIR2)$@

make.o: make.c make.h

clean:
	rm make.o

# end of /usr/src/make/Makefile
