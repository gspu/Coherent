# /usr/src/libc/stdio/i8086/Makefile 2/14/91

OBJ=\
	fgetw.o\
	fputw.o\
	getw.o\
	putw.o

# Primary target.
STDIOI8086:	$(OBJ)
	ar rkus libc.a $?
	touch $@
	: libc/stdio/i8086 done.

# Secondary targets.
fgetw.o:	fgetw.c
fputw.o:	fputw.c
getw.o:		getw.c
putw.o:		putw.c

# end of libc/stdio/i8086/Makefile
