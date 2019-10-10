# /usr/src/libc/crt/Makefile 2/14/91

OBJ=\
	_pow10.o\
	_prof.o

# Primary target.
all:	CRT crti8086
	: libc/crt done.

# Pseudotargets.
CRT:	$(OBJ)
	ar rkus libc.a $?
	touch $@
crti8086:
	make SRCPATH=$(SRCPATH)/i8086

# Secondary targets.
_pow10.o:	_pow10.c
_prof.o:	_prof.c

# end of /usr/src/libc/crt/Makefile
