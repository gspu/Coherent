DESTDIR=/interim/bin/

conv:	conv.o	
	cc -O -o $(DESTDIR)conv conv.o -ly
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@
	rm conv.o y.tab.h

conv.o:	conv.y

