# lex/Makefile 2/6/91
# Does not include header file dependencies.
#DESTDIR=/interim/bin/
DESTDIR=/tmp/
DESTDIR386=/bin386/bin/
SRC=/newbits/bin/lex

OBJ=\
	lex0.o\
	lex1.o\
	lex2.o\
	lex3.o

# Primary target.
lex:	$(OBJ)
	cc -o $(DESTDIR)$@ $(OBJ)
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

lex386:	$(OBJ)
	cc -o $(DESTDIR386)lex $(OBJ)
	su root chown bin $(DESTDIR386)lex
	su root chgrp bin $(DESTDIR386)lex
	su root chmod 511 $(DESTDIR386)lex

# Secondary targets.
lex0.o:	$(SRC)/lex0.c
lex1.o:	$(SRC)/lex1.c
lex2.o:	$(SRC)/lex2.c
lex3.o:	$(SRC)/lex3.c

clean:
	rm $O

# end of lex/Makefile
