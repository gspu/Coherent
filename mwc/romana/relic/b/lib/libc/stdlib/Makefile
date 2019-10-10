# /usr/src/libc/stdlib/Makefile 11/19/92

# This directory should contain all the ANSI <stdlib.h> sources
# but does not, simply for historical reasons.
# They should be moved here in the future.

OBJ=\
	atexit.o

# Primary target.
all:	STDLIB
	: libc/stdlib done.

# Pseudotargets.
STDLIB:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@

# Secondary targets.
atexit.o:	atexit.c

# end of /usr/src/libc/stdlib/Makefile
