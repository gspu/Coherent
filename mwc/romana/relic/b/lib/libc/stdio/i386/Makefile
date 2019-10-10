# /usr/src/libc/stdio/i386/Makefile 1/17/92

OBJ=\
	fgetw.o\
	fputw.o\
	getw.o\
	putw.o

# Primary target.
STDIOI386:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
	: libc/stdio/i386 done.

# Secondary targets.
fgetw.o:	fgetw.c
fputw.o:	fputw.c
getw.o:		getw.c
putw.o:		putw.c

# end of libc/stdio/i386/Makefile
