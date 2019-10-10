# /usr/src/libc/crt/Makefile 1/13/93

OBJ=\
	_prof.o

# Primary target.
all:	CRT crt$(MCH)
	: libc/crt done.

# Primary NDP target.
ndp:	CRT crti387
	: libc/crt done.

# Pseduotargets.
CRT:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
crt$(MCH):
	SRCPATH=$(SRCPATH)/$(MCH) make -e
crti387:
	SRCPATH=$(SRCPATH)/$(MCH) make -e CRTI386NDP
	SRCPATH=$(SRCPATH)/i387 make -e

# Secondary targets.
_prof.o:	_prof.c

# end of /usr/src/libc/crt/Makefile
