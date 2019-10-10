DESTDIR=/bin386/bin/

T = ar cdmp nm strip size nohup uname sa id dirname stty cpdir cgrep test \
    ipcrm msgs file man wc PSfont df pclfont

CFLAGS= -D__COHERENT__

all : $T
	echo all done

S = Makefile cdmp.c ar.c nm.c strip.c size.c

save : $S
	tar cf - $< | compress > /tmp/coff.tar.Z
	uucp -r /tmp/coff.tar.Z mwc!/v/cef

install : $T
	su root cp $T /bin

size : size.c
	cc -A -I/usr/include/sys -o $(DESTDIR)$@ $< -lmisc

cdmp : cdmp.c
	cc -A -I/usr/include/sys -o $(DESTDIR)$@ $< -lmisc

ar : ar.c
	cc -A -I/usr/include/sys -o $(DESTDIR)$@ $<

nm : nm.c
	cc -A -I/usr/include/sys -o $(DESTDIR)$@ $< -lmisc

strip : strip.c
	cc -A -I/usr/include/sys -o $(DESTDIR)$@ $< -lmisc

nohup:	nohup.c
	cc -v -o $(DESTDIR)$@ $<

uname:	uname.c
	cc -v -o $(DESTDIR)$@ $<

sa:	sa.c
	cc -v -o $(DESTDIR)$@ $<

id:	id.c
	cc -v -o $(DESTDIR)$@ $<

dirname:	dirname.c
	cc -v -o $(DESTDIR)$@ $<

stty:	stty.c
	cc -v -o $(DESTDIR)$@ $<

cpdir:	cpdir.c
	cc -v -o $(DESTDIR)$@ $<

cgrep:	cgrep.c
	cc -v -o $(DESTDIR)$@ $< -lmisc

test:	test.c
	cc -DTEST -D__VOID__=char -o $(DESTDIR)$@ $<

ipcrm:	ipcrm.c
	cc -o $(DESTDIR)$@ $<

msgs:	msgs.c
	cc $(CFLAGS) -o $(DESTDIR)$@ $<
	su root chmog 6511 msgs msgs $(DESTDIR)$@

file:	file.c
	cc $(CFLAGS) -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

readPCL:	readPCL.c
	cc $(CFLAGS) -f -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

man:	man.c
	cc $(CFLAGS) -f -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

wc:	wc.c
	cc $(CFLAGS) -f -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

PSfont:	PSfont.c
	cc $(CFLAGS) -f -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

df:	df.c
	cc $(CFLAGS) -f -o $(DESTDIR)$@ $<
	su root chmog 6511 sys sys $(DESTDIR)$@

pclfont:	pclfont.c
	cc $(CFLAGS) -f -o $(DESTDIR)$@ $<
	su root chmog 6511 sys sys $(DESTDIR)$@
