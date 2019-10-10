# pacman/Makefile 6/10/92
# This Makefile halfheartedly preserves dal's original braindamaged notion
# of machine independence.
# His sources build either a PDP-11 or an IBM PC version,
# using links to get the right one.
# The pac.c also #includes other C sources, not really kosher,
# but for now this goes with the flow rather than cleaning it all up.
DESTDIR=/interim/usr/games/
DESTDIR386=/bin386/usr/games/

OBJ=ibmpac.o ibmout.o
OBJ386=ibmpac.o

pacman:	$(OBJ)
	cc -o $(DESTDIR)chase $(OBJ)

pac386:	$(OBJ386)
	cc -o $(DESTDIR386)chase $(OBJ386)

ibmpac.o:	ibmpac.c term.c io.c
	cc -c ibmpac.c

ibmout.o:	ibmout.s
	cc -c ibmout.s

term.c:	ibm.c
	ln -f ibm.c term.c

io.c:	ibmio.c
	ln -f ibmio.c io.c

ibmpac.c:	pac.c
	ln -f pac.c ibmpac.c
