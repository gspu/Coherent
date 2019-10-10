DESTDIR=/interim/usr/bin/

all:	cut paste
	>done

cut:	cut.o strtol.o
	cc -o $(DESTDIR)$@ $< $(LIB)
	su root chmog 511 bin bin $(DESTDIR)$@

cut.o:	cut.c
	cc $(CFLAGS) -c $(INC) $<

strtol.o:	strtol.c
	cc $(CFLAGS) -c $(INC) $<

paste:	paste.o
	cc -o $(DESTDIR)$@ $< $(LIB)
	su root chmog 511 bin bin $(DESTDIR)$@

paste.o:	paste.c
	cc $(CFLAGS) -c $(INC) $<
