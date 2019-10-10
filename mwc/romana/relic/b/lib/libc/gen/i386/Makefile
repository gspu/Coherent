# /usr/src/libc/gen/i386/Makefile 7/8/92

OBJ=\
	abort.o\
	alloca.o\
	canon.o\
	l3tol.o\
	ldiv.o\
	ltol3.o\
	setjmp.o\
	signame.o

# Primary target.
GENI386:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
	: libc/gen/i386 done

# Secondary targets.
abort.o:	abort.c
alloca.o:	alloca.s
canon.o:	canon.s
l3tol.o:	l3tol.c
ldiv.o:		ldiv.s
ltol3.o:	ltol3.c
setjmp.o:	setjmp.s
signame.o:	signame.c

# end of /usr/src/libc/gen/i386/Makefile
