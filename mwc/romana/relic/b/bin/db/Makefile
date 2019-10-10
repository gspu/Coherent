# db/Makefile 3/30/93
# This version is i386-specific, though it could easily be generalized.

# CFLAGS must specify -VNDP so that get_fp_reg can read NDP registers.
# Other compile-time options include NOFP and NOCANON, cf. mdb.h.
CFLAGS=-VNDP

# db executable uses fp output conversion, must be linked with NDP library.
# This is not necessary for a NOFP version.
LFLAGS=-VNDP -f

# Machine-independent objects.
OBJ=\
	db0.o \
	db1.o \
	db2.o \
	db3.o \
	db4.o \
	db5.o \
	db6.o \
	db7.o \
	db8.o

# Machine-dependent objects.
MCHOBJ=\
	i386db0.o \
	i386db1.o \
	i386db2.o \
	i386db3.o \
	i386db4.o

# Primary target.
db:	$(OBJ) $(MCHOBJ)
	$(CC) $(LFLAGS) -o db $(OBJ) $(MCHOBJ)

# Secondary targets.
# These currently do not include header file dependencies.

# Machine-independent.
db0.o:		db0.c
db1.o:		db1.c
db2.o:		db2.c
db3.o:		db3.c
db4.o:		db4.c
db5.o:		db5.c
db6.o:		db6.c
db7.o:		db7.c
db8.o:		db8.c

# Machine-dependent.
i386db0.o:	i386db0.c
i386db1.o:	i386db1.c
i386db2.o:	i386db2.c
i386db3.o:	i386db3.c
i386db4.o:	i386db4.s

# Etc.
clean:
	rm $(OBJ) $(MCHOBJ)

# end of db/Makefile
