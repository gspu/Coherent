O = ld.o pass1.o message.o fake.o
LI = ld.c pass1.c message.c fake.c
S = Makefile ld.h ld.c pass1.c message.c fake.c script
D = ld.c pass1.c
CFLAGS = -O


DESTDIR=/bin386/bin/
DESTDIR2=/bin386/etc/

drvld: ld
	ln -f $(DESTDIR)ld $(DESTDIR2)drvld
	
ld : $O
	rm -f $(DESTDIR)ld
	cc -so $(DESTDIR)ld $O -lmisc

pick : $(LI)
	lint -I/usr/include co $(LI) > pick

# save on mwc!
save : $S
	tar cf - $< | compress -c > /tmp/newld.tar.Z
	uucp -r /tmp/newld.tar.Z mwc!/v/cef/

# list source files
list : $S
	ph $?
	touch list

# Build error message list in troff format.
doc : $D
	cgrep -d @ "(message|fatal|filemsg|modmsg|mpmsg|spmsg|spwarn)" $< | \
	 sort | \
	 sed -f `srcpath script` | \
	 fixdoc > doc

# Install new ld
install : ld
	su root `srcpath installer`

# end of ld/Makefile
