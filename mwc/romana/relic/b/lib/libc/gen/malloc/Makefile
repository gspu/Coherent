# /usr/src/libc/gen/malloc/Makefile 1/9/92

OBJ=\
	calloc.o\
	malloc.o\
	memok.o\
	notmem.o\
	realloc.o

UNUSED=trymallo.c

# Primary target.
GENMALLOC:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
	: libc/gen/malloc done

# Secondary targets.
calloc.o:	calloc.c
malloc.o:	malloc.c
memok.o:	memok.c
notmem.o:	notmem.c
realloc.o:	realloc.c

# end of /usr/src/libc/gen/malloc/Makefile
