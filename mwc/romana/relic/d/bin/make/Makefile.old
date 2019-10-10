OBJS	= make.o
CFLAGS	= -O
#For non-PDP11 systems: LDFLAGS = -n -i
LDFLAGS	= -i

make:	$(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o make

make.o: make.h make.c

make.v.o:
	exec version `pwd` make make.c make.h > make.v.s
	exec as -o make.v.o make.v.s
	exec rm make.v.s

install:	make makemacros makeactions
	exec /bin/cp make /bin
	exec /bin/cp makemacros makeactions /usr/lib

