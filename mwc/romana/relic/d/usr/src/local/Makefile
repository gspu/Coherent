# /usr/src/local/Makefile 4/17/90

# Primary target.
all:	kermit me/me

# Secondary targets.
kermit:	kermit.o
	$(CC) $(LDFLAGS) $?

me/me:	me.tar.Z
	-zcat $? | tar xvfm -
	cd me; make
