# c/n3/Makefile 4/25/91

CC3=\
	cc3.$X\
	dbgt3.$X\
	itree.$X

CC3MCH=\
	icode.$X\
	igen.$X

# Primary target.
cc3:	$(CC3) $(CC3MCH) $(CLIB)
	$(CC) $(LDFLAGS) -o cc3 $<

# Machine independent.
cc3.$X:		cc3.c
dbgt3.$X:	dbgt3.c
itree.$X:	itree.c

# Machine dependent.
icode.$X:	icode.c
igen.$X:	igen.c

# end of c/n3/Makefile
