# /usr/src/cmd/check/Makefile 2/20/91

# CFLAGS for PDP11
CFLAGS= -O
#DESTDIR= /interim/bin/
#DESTDIR2 = /interim/etc/
DESTDIR= /bin386/bin/
DESTDIR2 = /bin386/etc/
UTILS=\
	check\
	clri\
	dcheck\
	icheck\
	ncheck
OBJ=\
	check.o\
	clri.o\
	dcheck.o\
	icheck.o\
	ncheck.o

# Primary target.
all:	$(UTILS)
	: all done

check:	check.o
	$(CC) $(LDFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)check
	su root chgrp bin $(DESTDIR)check
	su root chmod 511 $(DESTDIR)check
clri:	clri.o
	$(CC) $(LDFLAGS) -o $(DESTDIR2)$@ $<
	su root chown bin $(DESTDIR2)clri
	su root chgrp bin $(DESTDIR2)clri
	su root chmod 511 $(DESTDIR2)clri

dcheck:	dcheck.o
	$(CC) $(LDFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)dcheck
	su root chgrp bin $(DESTDIR)dcheck
	su root chmod 511 $(DESTDIR)dcheck

icheck:	icheck.o
	$(CC) $(LDFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)icheck
	su root chgrp bin $(DESTDIR)icheck
	su root chmod 511 $(DESTDIR)icheck

ncheck:	ncheck.o
	$(CC) $(LDFLAGS) -o $(DESTDIR)$@ $<
	su root chown bin $(DESTDIR)ncheck
	su root chgrp bin $(DESTDIR)ncheck
	su root chmod 511 $(DESTDIR)ncheck

check.o:	check.h check.c
clri.o:		clri.c
dcheck.o:	check.h dcheck.c
icheck.o:	check.h icheck.c
ncheck.o:	ncheck.c

clean:
	rm $(OBJ)

# end of /usr/src/cmd/check/Makefile
