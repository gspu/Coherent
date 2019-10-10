# /usr/src/libc/gen/i8086/Makefile 1/9/92

OBJ=\
	abort.o\
	canon.o\
	div.o\
	inout.o\
	l3tol.o\
	ldiv.o\
	ltol3.o\
	modf.o\
	setjmp.o\
	signame.o

# atof source is in gen/atof.c.
# canon and setjmp are assembled from .m, not from old .s.
UNUSED=\
	atof.s\
	canon.s\
	setjmp.s

# Primary target.
GENI8086:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
	: libc/gen/i8086 done

# Secondary targets.
abort.o:	abort.s
canon.o:	canon.m
div.o:		div.m
inout.o:	inout.m
l3tol.o:	l3tol.c
ldiv.o:		ldiv.m
ltol3.o:	ltol3.c
modf.o:		modf.s
setjmp.o:	setjmp.m
signame.o:	signame.c

# end of /usr/src/libc/gen/i8086/Makefile
