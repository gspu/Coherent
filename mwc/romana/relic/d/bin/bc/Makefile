DESTDIR=/interim/bin/
YFLAGS	= -hdr yy.h -st -terms 55 -nterms 34 -prods 109 -states 203
OFILES	= bcmch.o  bcmutil.o getnum.o globals.o gram.o grmact.o\
		interp.o lex.o main.o output.o putnum.o
HFILES	= bc.h bcmch.h bcsymtbl.h
SFILES	= globals.c main.c gram.y grmact.c interp.c bcmch.c bcmutil.c\
		lex.c getnum.c putnum.c output.c
CFLAGS= -AOs -I.. -I.
MFILES	= readme Makefile gram.fix

bc:	$(OFILES) 
	$(CC) $(CFLAGS) -o bc $(DESTDIR)$(OFILES) -lmp

install : bc
	su root cp bc /bin
	su root chmog 511 bin bin /bin/bc

bcprof:	$(OFILES)
	$(CC) $(CFLAGS) -o bcprof -p $(OFILES) bc.v.o -lmp

print:
	pr $(MFILES) $(HFILES) $(SFILES)

ofiles:		$(OFILES)

bc.v.o: $(SFILES)
	version `pwd` bc $(SFILES) > bc.v.s
	as -o bc.v.o bc.v.s
	rm bc.v.s

clean:
	-rm $(OFILES) yy.h

$(OFILES):	$(HFILES)

debug.o:	$(HFILES)
lex.o:		gram.o
# gram.fix rewrites the terminal name list into the input strings rather
# then the NAMES_ used in the grammar
gram.o: gram.y $(HFILES) gram.fix
	yacc $(YFLAGS) `srcpath gram.y`
	sed -f `srcpath gram.fix` < y.tab.c > gram.c
	rm y.tab.c
	$(CC) $(CFLAGS) -c gram.c
	rm gram.c
