DESTDIR=/bin386/usr/bin/

uudecode:	uudecode.c
	cc $< -o $(DESTDIR)$@
	strip $(DESTDIR)$@
	chmog 511 bin bin $(DESTDIR)$@

xargs:	xargs.c
	cc $< -o $(DESTDIR)$@
	strip $(DESTDIR)$@
	chmog 511 bin bin $(DESTDIR)$@

apropros:	apropros.c
	cc $< -o $(DESTDIR)$@
	strip $(DESTDIR)$@
	chmog 511 bin bin $(DESTDIR)$@
