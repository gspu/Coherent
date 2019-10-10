# /usr/src/liby/Makefile 2/12/92

DESTDIR=/interim/usr/lib/
DESTDIR386=/bin386/usr/lib/

OBJ=\
	yyerror.o \
	yylex.o \
	yymain.o

liby.a:	$(OBJ)
	$(AR) rckus $(DESTDIR)$@ $?

liby386.a:	$(OBJ)
	$(AR) rcus $(DESTDIR386)liby.a $?

# end of /usr/src/liby/Makefile
