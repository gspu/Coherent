# /usr/src/cmd/dump/Makefile 2/20/91

CFLAGS = -O
#DESTDIR= /interim/bin/
DESTDIR= /bin386/bin/

EXE=\
	dump\
	dumpdate\
	dumpdir\
	restor

OBJ=\
	dump.o\
	dump.o\
	dump.o\
	dump.o\
	dump.o

# Primary target.
all:	$(EXE)
	: all done

# Executables.
dump:	dump.o discbuf.o
	$(CC) $(CFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

dumpdate: dumpdate.o
	$(CC) $(CFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

dumpdir: dumpdir.o
	$(CC) $(CFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

restor:	restor.o discbuf.o
	$(CC) $(CFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

# Secondary targets.
discbuf.o:	discbuf.h discbuf.c
dumpdate.o:	dumptape.h dumpdate.c
dumpdir.o:	dumptape.h dumpdir.c
dump.o:		dumptape.h discbuf.h dump.c
restor.o:	dumptape.h discbuf.h restor.c

# Etc.
clean:
	rm $(OBJ)

# end of /usr/src/cmd/dump/Makefile
