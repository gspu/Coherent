# /usr/src/cmd/yacc/Makefile 2/20/91
# For i8086: LFLAGS=-n.

DESTDIR=/bin386/bin/
CFLAGS=-O
LDFLAGS=
LD=cc
CC=cc

O= y0.o y1.o y2.o y3.o y4.o y5.o y6.o

yacc:	$O
	$(LD) $(LDFLAGS) -so $(DESTDIR)$@ $O

install: yacc
	su root `srcpath installer`

y0.o y1.o y4.o:	yacc.h
y3.o:		yacc.h assert.h
y2.o y6.o:	yacc.h action.h
y5.o:		yacc.h assert.h action.h

# end of /usr/src/cmd/yacc/Makefile
