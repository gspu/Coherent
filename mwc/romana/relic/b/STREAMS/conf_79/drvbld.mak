# Kernel makefile
# This makefile was automatically generated. Do not hand-modify
# Generated at Fri Jul 16 16:28:28 1993





CFLAGS = -O -I.

DRVOBJS = conf.o ./streams.o ./streams/Driver.o ./loop.o ./loop/Driver.o ./dump.o ./dump/Driver.o ./echo/Driver.o 

$(K386LIB)/drivers.o: conf.o space $(DRVOBJS)
	ld -r -o $(K386LIB)/drivers.o $(DRVOBJS)

conf.o : conf.c conf.h
	$(CC) $(CFLAGS) -c conf.c

conf.h: mtune stune
	mkdev -t
	if [ "./streams.o ./loop.o ./dump.o " ] ; then rm -f ./streams.o ./loop.o ./dump.o  ; else true ; fi

space: ./streams.o ./streams/Driver.o ./loop.o ./loop/Driver.o ./dump.o ./dump/Driver.o ./echo/Driver.o 

./streams.o : ./streams/Space.c
	$(CC) $(CFLAGS) -o ./streams.o  -c ./streams/Space.c

./loop.o : ./loop/Space.c
	$(CC) $(CFLAGS) -o ./loop.o  -c ./loop/Space.c

./dump.o : ./dump/Space.c
	$(CC) $(CFLAGS) -o ./dump.o  -c ./dump/Space.c


clean:
	if [ "./streams.o ./loop.o ./dump.o " ] ; then rm -f ./streams.o ./loop.o ./dump.o  ; else true ; fi

