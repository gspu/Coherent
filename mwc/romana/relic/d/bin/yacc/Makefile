# /usr/src/cmd/yacc/Makefile 2/21/92
# For i8086: LFLAGS=-n.

CFLAGS=-O
LDFLAGS=-i -n

DESTDIR=/interim/bin/

OBJ=\
	y0.o\
	y1.o\
	y2.o\
	y3.o\
	y4.o\
	y5.o\
	y6.o

yacc:	$(OBJ)
	$(CC) $(LDFLAGS) -o $(DESTDIR)$@ $(OBJ)
	su root chmog 511 bin bin $(DESTDIR)$@

cmp:
	cmp yacc /bin/yacc
	rm yacc *.o

cp:	all
	cp yacc /bin/yacc
	rm yacc *.o

y0.o y1.o y4.o:	yacc.h
y3.o:		yacc.h
y2.o y6.o:	yacc.h
y5.o:		yacc.h

# end of /usr/src/cmd/yacc/Makefile
