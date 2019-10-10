# /usr/src/cmd/ed/Makefile 2/20/91

# for i8086 LDFLAGS = -i -n
CFLAGS=-O
DESTDIR= /bin386/bin/
LDFLAGS=-i -n

OBJ=ed0.o ed1.o ed2.o ed3.o

ed: $(OBJ)
	cc $(LDFLAGS) -o $(DESTDIR)$@ $(OBJ)
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

ed0.o:	ed.h ed0.c
ed1.o:	ed.h ed1.c
ed2.o:	ed.h ed2.c
ed3.o:	ed.h ed3.c

clean:
	rm $(OBJ)

# end of /usr/src/cmd/ed/Makefile
