# libl/Makefile 10/17/90
# Does not include header file dependencies.

OBJECTS=\
	auto.o \
	back.o \
	ctab.o \
	ctxt.o \
	error.o \
	less.o \
	look.o \
	ltab.o \
	main.o \
	more.o \
	next.o \
	rjct.o \
	wrap.o

# Primary target.
libl.a:	$(OBJECTS)
	ar rcvs libl.a $(OBJECTS)

clean:	libl.a $(OBJECTS)
	rm libl.a $(OBJECTS)
