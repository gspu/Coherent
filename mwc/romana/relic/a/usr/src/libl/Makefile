# /usr/src/libl/Makefile 2/14/91

OBJ=\
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
libl.a:	$(OBJ)
	ar rckus $@ $?

# Secondary targets.
auto.o:		auto.c lexlib.h lextype.h
back.o:		back.c lexlib.h
ctab.o:		ctab.c lexlib.h
ctxt.o:		ctxt.c lexlib.h
error.o:	error.c
less.o:		less.c lexlib.h
look.o:		look.c lexlib.h
ltab.o:		ltab.c lexlib.h
main.o:		main.c
more.o:		more.c lexlib.h
next.o:		next.c lexlib.h
rjct.o:		rjct.c lexlib.h
wrap.o:		wrap.c

clean:	libl.a $(OBJ)
	rm libl.a $(OBJ)

# end of /usr/src/libl/Makefile
