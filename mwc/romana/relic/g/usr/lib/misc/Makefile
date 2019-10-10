# misc/Makefile
#CFLAGS= -O -VNDP
CFLAGS= -O
DESTDIR=/usr/local/misc
# Objects.
O = \
	alloc.o \
	approx.o \
	ask.o \
	banner.o \
	basename.o \
	bedaemon.o \
	copyd.o \
	crc16.o \
	fatal.o \
	fifo.o \
	fifo_b.o \
	flatten.o \
	getargs.o \
	getline.o \
	getpty.o \
	ifcoh.o \
	is_fs.o \
	kernelName.o \
	lcase.o \
	lock.o \
	match.o \
	metaphone.o \
	newcpy.o \
	pathn.o \
	picture.o \
	randl.o \
	replace.o \
	regexp.o \
	regsub.o \
	regerror.o \
	select.o \
	showflag.o \
	skip.o \
	span.o \
	splitter.o \
	strchrtr.o \
	strcmpl.o \
	strdlen.o \
	titojd.o \
	tmtojd.o \
	jdtoti.o \
	jdtotm.o \
	tocont.o \
	trim.o \
	ucase.o \
	usage.o \
	virtual.o \
	xdump.o \
	xopen.o \
	yn.o

# Primary target.
.c.o:
	cc $(CFLAGS) -c $<

libmisc.a: $O
	rm -f $(DESTDIR)/libmisc.a
	ar crs $(DESTDIR)/libmisc.a $O

install: libmisc.a
	su root `srcpath installer`

picture: picture.c
	$(CC) -f -DTEST $< -lm
	picture | scat
