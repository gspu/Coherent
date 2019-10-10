# c/n1/Makefile 3/26/92
# This presumes that the code macros and patterns macros.c and patern.c
# have been made in the current directory by n1/$(MACHINE)/Makefile.

CC1=\
	cc1.$X\
	cc1sym.$X\
	code.$X\
	mtree0.$X\
	mtree1.$X\
	mtree3.$X\
	node.$X\
	out.$X\
	pool.$X\
	reg0.$X\
	sel0.$X\
	sel1.$X\
	snap0.$X\
	tree0.$X\
	tree1.$X
CC1MCH=\
	altemp.$X\
	amd.$X\
	fixtop.$X\
	gen1.$X\
	gen2.$X\
	mtree2.$X\
	mtree4.$X\
	outmch.$X\
	reg1.$X\
	sel2.$X\
	snap1.$X\
	table0.$X\
	table1.$X
CC1TABLES=\
	macros.$X\
	patern.$X

# primary target
cc1:	$(CC1) $(CC1MCH) $(CC1TABLES) $(CLIB)
	$(CC) $(LDFLAGS) -o cc1 $<

# monolithic compiler cc1 library
cc1.a:	$(CC1) $(CC1MCH) $(CC1TABLES)
	$(AR) rucks $@ $<

# machine independent
cc1.$X:		cc1.c
cc1sym.$X:	cc1sym.c
code.$X:	code.c
mtree0.$X:	mtree0.c
mtree1.$X:	mtree1.c
mtree3.$X:	mtree3.c
node.$X:	node.c
out.$X:		out.c
pool.$X:	pool.c
reg0.$X:	reg0.c
sel0.$X:	sel0.c
sel1.$X:	sel1.c
snap0.$X:	snap0.c
tree0.$X:	tree0.c
tree1.$X:	tree1.c

# machine dependent
altemp.$X:	altemp.c
amd.$X:		amd.c
fixtop.$X:	fixtop.c
gen1.$X:	gen1.c
gen2.$X:	gen2.c
mtree2.$X:	mtree2.c
mtree4.$X:	mtree4.c
outmch.$X:	outmch.c
reg1.$X:	reg1.c
sel2.$X:	sel2.c
snap1.$X:	snap1.c
table0.$X:	table0.c
table1.$X:	table1.c

# code macros and patterns
macros.$X:	macros.c
patern.$X:	patern.c

# end of c/n1/Makefile
