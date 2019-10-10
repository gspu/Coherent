# makefile for i/o tools

DESTDIR1=/bin386/etc/
DESTDIR2=/bin386/conf/

asypatch:	asypatch.o coffpatch.o
	cc -o $(DESTDIR1)$@ $<
asypatch.286:	asypatch.o loutpatch.o
	cc -o $@ $<
asymkdev:	asymkdev.o
	cc -o $(DESTDIR)$@ asymkdev.o
devload:	devload.o cohcall.o
	cc -o $@ $<
setfpe:		setfpe.o cohcall.o
	cc -o $@ $<
setbp:		setbp.o cohcall.o
	cc -o $@ $<
reboot:		reboot.o cohcall.o
	cc -o $(DESTDIR1)$@ $<
cohcall.o:	cohcall.s
