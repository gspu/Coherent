# Template for kernel build makefile. Note that all the comments in this
# template will disappear. Below, %C -> compile rules, %L -> link files,
# %R -> clean files and %% -> %.

CFLAGS = -O -I.

DRVOBJS = conf.o %L

$(K386LIB)/drivers.o: conf.o space $(DRVOBJS)
	ld -r -o $(K386LIB)/drivers.o $(DRVOBJS)

conf.o : conf.c conf.h
	$(CC) $(CFLAGS) -c conf.c

conf.h: mtune stune
	mkdev -t
	if [ "%R" ] ; then rm -f %R ; else true ; fi

space: %L

%C
clean:
	if [ "%R" ] ; then rm -f %R ; else true ; fi

