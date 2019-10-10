# makefile for i/o tools

asypatch:	asypatch.o coffpatch.o
	cc -o $@ $<
asypatch.286:	asypatch.o loutpatch.o
	cc -o $@ $<
asymkdev:	asymkdev.o
	cc -o $@ asymkdev.o
devload:	devload.o cohcall.o
	cc -o $@ $<
setfpe:		setfpe.o cohcall.o
	cc -o $@ $<
cohcall.o:	cohcall.s
