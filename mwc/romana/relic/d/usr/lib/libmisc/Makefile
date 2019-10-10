# misc/Makefile 10/11/90

DESTDIR=/interim/usr/lib/

# Objects.
O = \
	alloc.o \
	approx.o \
	ask.o \
	banner.o \
	bedaemon.o \
	crc16.o \
	fatal.o \
	is_fs.o \
	lcase.o \
	lock.o \
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
	su root rm -f $(DESTDIR)libmisc.a
	ar q $(DESTDIR)libmisc.a $O
	su root chmog 644 root root $(DESTDIR)$@
	su root ranlib $(DESTDIR)$@
	
