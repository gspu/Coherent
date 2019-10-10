CFLAGS=	-O -DCM_N -DCM_GT -DCM_B -DCM_D

DESTDIR=/interim/usr/lib/

libterm.a: termcap.o tgoto.o tputs.o
	$(AR) rucks $(DESTDIR)libterm.a termcap.o tgoto.o tputs.o

libtermcap: termcap.c tgoto.c tputs.c
	cc -v -c $<
	(cd /newbits/usr/lib/libcurses/obj386 ; make libtermcap)

libtermcap.NDP: termcap.c tgoto.c tputs.c
	cc -VNDP -v -c $<
	(cd /newbits/usr/lib/libcurses/obj386 ; make libtermcap.NDP)

install: libterm.a
	cp libterm.a ${DESTDIR}/usr/lib/libterm.a

clean:
	-rm -f *.o
	-rm -f libterm.a
	-rm -f etc_termcap

# reorder gives an editor command for most common terminals
# (in reverse order from n'th to 1'st most commonly used)
# to move them to the front of termcap

/etc/termcap: etc_termcap
	cp etc_termcap /etc/termcap
	rm etc_termcap

etc_termcap: termcap
	ed - termcap < reorder

tabset:
	-mkdir /usr/lib/tabset

# who knows ...
vgrind:
	${VGRIND} -n -h "Terminal Capibility Data Base" \
	termcap reorder makefile
