# misc/Makefile 10/11/90

# Objects.
O = \
	alloc.o \
	approx.o \
	ask.o \
	banner.o \
	crc16.o \
	fatal.o \
	is_fs.o \
	lcase.o \
	match.o \
	newcpy.o \
	pathn.o \
	picture.o \
	randl.o \
	replace.o \
	showflag.o \
	skip.o \
	span.o \
	strchrtr.o \
	strcmpl.o \
	ucase.o \
	usage.o \
	virtual.o \
	xdump.o \
	xopen.o \
	yn.o

# Primary target.
libmisc.a: $O
	rm -f libmisc.a
	ar q libmisc.a $O

clean: $O
	rm $O
