# c/n0/Makefile 3/26/92

CC0=\
	cc0.$X\
	cc0key.$X\
	cc0sym.$X\
	cpp.$X\
	dbgt0.$X\
	ddecl.$X\
	dope.$X\
	double.$X\
	etc.$X\
	expand.$X\
	expr.$X\
	fold.$X\
	gcandt.$X\
	gdecl.$X\
	get.$X\
	init.$X\
	lex.$X\
	locals.$X\
	sharp.$X\
	size.$X\
	stat.$X
CC0MCH=\
	bind.$X

# Primary target.
cc0:	$(CC0) $(CC0MCH) $(CLIB)
	$(CC) $(LDFLAGS) -o cc0 $<

# Monolithic compiler cc0 library.
cc0.a:	$(CC0) $(CC0MCH)
	$(AR) rucks $@ $<

# Machine independent.
cc0.$X:		cc0.c
cc0key.$X:	cc0key.c
cc0sym.$X:	cc0sym.c
cpp.$X:		cpp.c
dbgt0.$X:	dbgt0.c
ddecl.$X:	ddecl.c
dope.$X:	dope.c
double.$X:	double.c
etc.$X:		etc.c
expand.$X:	expand.c
expr.$X:	expr.c
fold.$X:	fold.c
gcandt.$X:	gcandt.c
gdecl.$X:	gdecl.c
get.$X:		get.c
init.$X:	init.c
lex.$X:		lex.c
locals.$X:	locals.c
sharp.$X:	sharp.c
size.$X:	size.c
stat.$X:	stat.c

# machine dependent
bind.$X:	bind.c

# end of c/n0/Makefile
