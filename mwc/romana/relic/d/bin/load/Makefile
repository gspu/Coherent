# ld/Makefile 5/14/91
# i8086 loader Makefile.

# Set BREADBOX to maximum size for in-memory load (e.g. 16384).
BREADBOX=16384

CFLAGS=-O -DBREADBOX=$(BREADBOX)
LFLAGS=-i -s

DESTDIR=/interim/bin/

OBJ=	main.o\
	data.o\
	message.o\
	pass1.o\
	pass2.o

# Primary target.
ld:	$(OBJ)
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

# Unlinked objects.
main.o:		main.c data.h
data.o:		data.c 	data.h
message.o:	message.c data.h
pass1.o:	pass1.c	data.h
pass2.o:	pass2.c	data.h

# Etc.
clean:
	rm *.o

# end of ld/Makefile
