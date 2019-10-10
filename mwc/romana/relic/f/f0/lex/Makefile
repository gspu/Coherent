# lex/Makefile 2/6/91
# Does not include header file dependencies.

DESTDIR386=/bin386/bin/

OBJ=\
	lex0.o\
	lex1.o\
	lex2.o\
	lex3.o

# Primary target.

lex386:	$(OBJ)
	cc -o $(DESTDIR386)lex $(OBJ)
	su root chmog 511 bin bin $(DESTDIR386)/lex

# Secondary targets.
lex0.o:	lex0.c
lex1.o:	lex1.c
lex2.o:	lex2.c
lex3.o:	lex3.c

clean:
	rm $O

# end of lex/Makefile
