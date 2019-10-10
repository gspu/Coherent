# awk/Makefile 2/20/91

# Include source directory in CFLAGS for y.tab.c.
CFLAGS=-I$(SRCPATH)
# Link with floating point output.
LDFLAGS=-f
YFLAGS=  -prods 110 -terms 67 -nterms 30 -states 200

DESTDIR=/interim/bin/
DESTDIR386=/bin386/bin/

OBJ=\
	awk.o\
	awk0.o\
	awk1.o\
	awk2.o\
	awk3.o\
	awk4.o\
	awk5.o\
	awk6.o

# Primary target.
awk:	$(OBJ)
	cc $(LDFLAGS) -o $(DESTDIR)awk $(OBJ) -lm
	su root chmog 511 bin bin $(DESTDIR)awk

awk386:	$(OBJ)
	cc $(LDFLAGS) -o $(DESTDIR386)awk $(OBJ) -lm
	su root chmog 511 bin bin $(DESTDIR386)awk

# Secondary targets.
awk.o:	awk.y
awk0.o:	awk.h y.tab.h awk0.c
awk1.o:	awk.h y.tab.h awk1.c
awk2.o:	awk.h y.tab.h awk2.c
awk3.o:	awk.h y.tab.h awk3.c
awk4.o:	awk.h y.tab.h awk4.c
awk5.o:	awk.h y.tab.h awk5.c
awk6.o:	awk.h y.tab.h awk6.c

clean:
	rm $(OBJ)

# end of awk/Makefile
