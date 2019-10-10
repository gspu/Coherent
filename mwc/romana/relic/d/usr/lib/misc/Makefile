# misc/Makefile 10/11/90

# Objects.
O = \
	alloc.o \
	approx.o \
	ask.o \
	banner.o \
	crc16.o \
	fatal.o \
	getline.o \
	is_fs.o \
	lcase.o \
	match.o \
	memxch.o \
	newcpy.o \
	pathn.o \
	picture.o \
	qsort.o \
	randl.o \
	replace.o \
	regexp.o \
	regsub.o \
	regerror.o \
	showflag.o \
	skip.o \
	span.o \
	splitter.o \
	strchrtr.o \
	strcmpl.o \
	trim.o \
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

install: libmisc.a
	su root cp -d libmisc.a /usr/lib
	su root cp -d `srcpath *.h`  /usr/include
