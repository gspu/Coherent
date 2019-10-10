# /usr/src/c/Makefile 4/25/91
# Assumes $(SRC) defines root libc source directory, e.g. /usr/src/c.
# To build new C compiler objects, move to an empty object directory and type:
#	export SRC=/usr/src/c		(or other pathname as appropriate)
#	. $SRC/.profile			(to set exported shell variables)
#	make dirs			(to build subdirectories)
#	make				(to build compiler)
# .profile sets exported shell variables required by subdirectory Makefiles:
#	MCH	machine type
#	CFLAGS	to -I compiler headers
#	CLIB	path to common library
#	OUTPUT	output writer
#	TABGEN	path to table generator executable
#	TFLAGS	flags for table generator
#	X	object file extension
# Takes about 20 minutes from scratch on 20 MHz 386 with RAM disk cc.

# Primary target.
# Build unstripped executables cc, cpp, cc[0123] linked to directory bin.
# Also builds common library common/lib.a, table generator coh/tabgen,
# code table sources n1/macros.c and m1/patern.c.
all:
	cd common;	make -e SRCPATH=$(SRC)/common:$(SRC)/common/$(MCH)
	cd coh;		make -e SRCPATH=$(SRC)/coh
	cd cpp;		make -e SRCPATH=$(SRC)/cpp
	cd n0;		make -e SRCPATH=$(SRC)/n0:$(SRC)/n0/$(MCH)
	cd n1;		make -e SRCPATH=$(SRC)/n1/$(MCH):$(SRC)/n1/$(MCH)/tables
	cd n1;		make -e SRCPATH=$(SRC)/n1:$(SRC)/n1/$(MCH)
	cd n2;		make -e SRCPATH=$(SRC)/n2:$(SRC)/n2/$(MCH)
	cd n3;		make -e SRCPATH=$(SRC)/n3:$(SRC)/n3/$(MCH)
	ln -f coh/cc cpp/cpp n0/cc0 n1/cc1 n2/cc2 n3/cc3 bin
	: Done.

# Strip executables.
strip:
	strip bin/cc bin/cpp bin/cc0 bin/cc1 bin/cc2 bin/cc3

# Build subdirectories.
dirs:
	mkdir bin coh common cpp n0 n1 n2 n3

# end of /usr/src/c/Makefile
