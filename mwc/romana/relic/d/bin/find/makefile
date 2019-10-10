DESTDIR=/interim/bin/

find:	find.o	findnode.h
	cc -O -o $(DESTDIR)find find.o -ly
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@
	rm find.o y.tab.h

find.o:	find.y

