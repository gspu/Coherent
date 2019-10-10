# make ipcs
COMMON = ipcs.h Makefile

SRC = 	fifo.c \
	getipc.c \
	ipcs.c \
	pick_nfile.c \
	printipc.c \
	shmemul.c

OBJ = 	obj/fifo.o \
	obj/getipc.o \
	obj/ipcs.o \
	obj/pick_nfile.o \
	obj/printipc.o \
	obj/shmemul.o

bin/ipcs: $(OBJ) $(COMMON)
	cc $(OBJ) -o bin/ipcs 
	chmog 4511 sys sys bin/ipcs

obj/fifo.o:	fifo.c $(COMMON)
	cc -c fifo.c -o obj/fifo.o

obj/pick_nfile.o:	pick_nfile.c $(COMMON)
	cc -c pick_nfile.c -o obj/pick_nfile.o

obj/getipc.o : getipc.c $(COMMON)
	cc -c getipc.c -o obj/getipc.o

obj/ipcs.o : ipcs.c $(COMMON)
	cc -c ipcs.c -o obj/ipcs.o

obj/printipc.o : printipc.c $(COMMON)
	cc -c printipc.c -o obj/printipc.o

obj/shmemul.o : shmemul.c $(COMMON)
	cc -c shmemul.c -o obj/shmemul.o
