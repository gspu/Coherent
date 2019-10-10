# /usr/src/cmd/grep/Makefile 2/20/91
DESTDIR=/bin386/bin/
OBJ=\
	grep1.o\
	grep2.o

grep:	$(OBJ)
	$(CC) $(LDFLAGS) -o $(DESTDIR)$* $(OBJ)
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

grep1.o: grep.h grep1.c
grep2.o: grep.h grep2.c

clean:
	rm $(OBJ)

# end of /usr/src/cmd/grep/Makefile
