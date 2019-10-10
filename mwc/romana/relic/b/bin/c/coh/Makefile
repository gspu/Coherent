# c/coh/Makefile 2/11/93

# Monolithic compiler phases as archives.
MLIB=\
	cc.o\
	../n0/cc0.a\
	../n1/cc1.a\
	../n2/cc2.a\
	../n3/cc3.a

# all for coherent
all:	cc tabgen
	: all done

# native cc driver
cc:	cc.o $(CLIB)
	$(CC) $(LDFLAGS) -o cc cc.o $(CLIB)
cc.o:	cc.c

# Monolithic cc.
mcc:	$(MLIB) $(CLIB)
	$(CC) -o $@ $<

# i8086 cross compiler
i8086cc: cc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o i8086cc -DI8086CC cc.c $(CLIB)
# m68000 cross compiler
m68000cc: cc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o m68000cc -DM68000CC cc.c $(CLIB)
# z8001 cross compiler
z8001cc: cc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o z8001cc -DZ8001CC cc.c $(CLIB)
# code table generator
tabgen: tabgen.o
	$(CC) $(LDFLAGS) -o tabgen tabgen.o
tabgen.o: tabgen.c
# intel 8086 omf disassembler
ojed:	ojed.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ojed ojed.c
# intel 80286 omf disassembler
ojd286:	ojd286.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ojd286 ojd286.c
# coherent new l.out disassembler
ndis:	ndis.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ndis ndis.c $(CLIB)

# end of c/coh/Makefile
