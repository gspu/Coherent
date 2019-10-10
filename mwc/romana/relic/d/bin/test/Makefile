# /usr/src/cmd/test/Makefile 2/20/91
DESTDIR=/interim/bin/
DESTDIR386=/bin386/bin/

test386:	test.o
	cc -O -i -o $(DESTDIR386)test $< -ly
	su root chown bin $(DESTDIR386)test
	su root chown bin $(DESTDIR386)test
	su root chmod 511 $(DESTDIR386)test
	su root ln -f $(DESTDIR386)test $(DESTDIR386)[
test:	test.o
	cc -O -i -o $(DESTDIR)$@ $< -ly
	su root chown bin $(DESTDIR)test
	su root chown bin $(DESTDIR)test
	su root chmod 511 $(DESTDIR)test
	su root ln -f $(DESTDIR)test $(DESTDIR)[
test.o:	test.y testnode.h

# end of /usr/src/cmd/test/Makefile
