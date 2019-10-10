all: number unumber
	 > all_done
number : number.o numtab.o
	cc -o number number.o numtab.o
unumber : unumber.o numtab.o
	cc -o unumber unumber.o numtab.o
clean:
	rm -f  all_done *.o
unumber.o : unumber.c unumber.h
number.o  : number.c unumber.h
