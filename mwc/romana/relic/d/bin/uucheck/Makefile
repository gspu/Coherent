# Makefile for checkperms, permissions checking interpreter
# part of uucheck the uucp sanity checker.
#
# Destination directory for installing executable files
DESTDIR=/interim/bin/
BIN=./install/bin/
# Destination directory for installing data files
# BTW if LIB= anything other than /usr/lib/uucheck/, you will have to edit
# uucheck by hand.
LIB=./install/usr/lib/uucheck/

SUBPROGS=uucheck checkperms/checkperms uucheckname/uucheckname uuchecklock/uuchecklock
#
uucheck:	$(SUBPROGS)
	cp uucheck $(DESTDIR)$@
	su root chmog 511 bin bin $(DESTDIR)$@

checkperms/checkperms:
	(cd checkperms; make)

uucheckname/uucheckname:
	(cd uucheckname; make)

uuchecklock/uuchecklock:
	(cd uuchecklock; make)

install: uucheck
	(cd checkperms; make install)
	(cd uucheckname; make install)
	(cd uuchecklock; make install)
	cp -d uucheck $(BIN)
	chown bin $(BIN)/uucheck
	chgrp bin $(BIN)/uucheck

clean:
	(cd checkperms; make clean)
	(cd uucheckname; make clean)
	(cd uuchecklock; make clean)
	rm -f *.o core
