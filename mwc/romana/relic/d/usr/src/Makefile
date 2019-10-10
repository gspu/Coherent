# Makefile for /usr/src. No compilations, just copying as these
# are not built into executables.

DESTDIR=/interim/usr/src/
DESTDIR1=/interim/usr/src/alien/
DESTDIR2=/interim/usr/src/liby/
DESTDIR3=/interim/usr/src/local/
DESTDIR4=/interim/usr/src/sample/
DESTDIR5=/interim/usr/src/libl/
DESTDIR6=/interim/usr/src/misc/
DESTDIR7=/interim/usr/src/csu/

all: crts alienstuff libystuff localstuff samplestuff liblstuff miscstuff examplestuff
     >all

crts:
	cp -d csu/*.s $(DESTDIR7)

alienstuff:
        cp -d alien/* $(DESTDIR1)

libystuff:
        cp -d liby/yy*.c $(DESTDIR2)
	cp -d liby/Makefile $(DESTDIR2)

localstuff:
        cp -d local/Makefile $(DESTDIR3)
	cp -d local/kermit.c $(DESTDIR3)
	cp -d local/me.tar.Z $(DESTDIR3)

samplestuff:
        cp -d sample/atod.c $(DESTDIR4)
	cp -d sample/example1.c $(DESTDIR4)
	cp -d sample/example2.c $(DESTDIR4)
	cp -d sample/example3.c $(DESTDIR4)
	cp -d sample/factor.c $(DESTDIR4)
	cp -d sample/makefile $(DESTDIR4)

liblstuff:
	cp -d libl/* $(DESTDIR5)

miscstuff:
	mv misc/RCS RCS
	tar cvf misc.tar misc
	compress misc.tar
	mv RCS misc/RCS
	mv misc.tar.Z $(DESTDIR)
	su root chmog 644 bin bin $(DESTDIR)misc.tar.Z

examplestuff:
	mv examples/RCS RCS
	tar cvf examples.tar examples
	compress examples.tar
	mv RCS examples/RCS
	mv examples.tar.Z $(DESTDIR)
	su root chmog 644 bin bin $(DESTDIR)examples.tar.Z
