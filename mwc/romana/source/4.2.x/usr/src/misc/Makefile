# misc/Makefile

CFLAGS= -O
DESTLIB=libmisc.a
MAKEFILE=Makefile
USRINC=/usr/include

# Objects.
O = \
	$(DESTLIB)(alloc.o) \
	$(DESTLIB)(approx.o) \
	$(DESTLIB)(ask.o) \
	$(DESTLIB)(banner.o) \
	$(DESTLIB)(basename.o) \
	$(DESTLIB)(bedaemon.o) \
	$(DESTLIB)(copyd.o) \
	$(DESTLIB)(crc16.o) \
	$(DESTLIB)(fatal.o) \
	$(DESTLIB)(fifo.o) \
	$(DESTLIB)(fifo_b.o) \
	$(DESTLIB)(flatten.o) \
	$(DESTLIB)(getargs.o) \
	$(DESTLIB)(getline.o) \
	$(DESTLIB)(getpty.o) \
	$(DESTLIB)(ifcoh.o) \
	$(DESTLIB)(is_fs.o) \
	$(DESTLIB)(jdtoti.o) \
	$(DESTLIB)(jdtotm.o) \
	$(DESTLIB)(kernelName.o) \
	$(DESTLIB)(lcase.o) \
	$(DESTLIB)(lock.o) \
	$(DESTLIB)(match.o) \
	$(DESTLIB)(metaphone.o) \
	$(DESTLIB)(newcpy.o) \
	$(DESTLIB)(pathn.o) \
	$(DESTLIB)(picture.o) \
	$(DESTLIB)(randl.o) \
	$(DESTLIB)(regerror.o) \
	$(DESTLIB)(regexp.o) \
	$(DESTLIB)(regsub.o) \
	$(DESTLIB)(replace.o) \
	$(DESTLIB)(select.o) \
	$(DESTLIB)(showflag.o) \
	$(DESTLIB)(skip.o) \
	$(DESTLIB)(span.o) \
	$(DESTLIB)(splitter.o) \
	$(DESTLIB)(strchrtr.o) \
	$(DESTLIB)(strcmpl.o) \
	$(DESTLIB)(strdlen.o) \
	$(DESTLIB)(titojd.o) \
	$(DESTLIB)(tmtojd.o) \
	$(DESTLIB)(tocont.o) \
	$(DESTLIB)(trim.o) \
	$(DESTLIB)(ucase.o) \
	$(DESTLIB)(usage.o) \
	$(DESTLIB)(virtual.o) \
	$(DESTLIB)(xdump.o) \
	$(DESTLIB)(xopen.o) \
	$(DESTLIB)(yn.o)

libmisc.a: $O

install: libmisc.a
	cp -d libmisc.a /usr/lib/libmisc.a

picture: picture.c
	$(CC) -f -DTEST $? -lm
	picture | scat

depend:
	echo > makedep
	grep "^\#include" *.c \
		| sed 's/\(.*\)\.c:.*"\(.*\)".*/\1.o: \2/' \
		| sed '/\#include/d' >> makedep
	grep "^\#include" *.c \
		| sed 's/\(.*\)\.c:.*<\(.*\)>.*/$$(DESTLIB)(\1.o): $$(USRINC)\/\2/' \
		| sed '/\#include/d' >> makedep
	grep ".include" *.s \
		| sed 's/\(.*\)\.s:.*"\(.*\)".*/$$(K386LIB)(\1.o): \2/' \
		| sed '/\#include/d' >> makedep
	echo '/^\# DO NOT DELETE THIS LINE/+1,$$d' > eddep
	echo '$$r makedep' >> eddep
	echo 'w' >> eddep
	ed $(MAKEFILE) < eddep
	rm makedep eddep
	echo >> $(MAKEFILE)
	echo "\# DEPENDENCIES MUST END AT END OF FILE:" \
	     "IF YOU PUT STUFF HERE, IT WILL GO AWAY" >> $(MAKEFILE)
	echo "\# See make depend, above" >> $(MAKEFILE)

# DO NOT DELETE THIS LINE - make depend needs it

jdtoti.o: local_misc.h
jdtotm.o: local_misc.h
lock.o: dcp.h
match.o: local_misc.h
newcpy.o: local_misc.h
picture.o: stdio.h
picture.o: math.h
regexp.o: local_misc.h
regsub.o: local_misc.h
replace.o: local_misc.h
skip.o: local_misc.h
span.o: local_misc.h
strcmpl.o: local_misc.h
titojd.o: time.h
titojd.o: local_misc.h
tmtojd.o: local_misc.h
virtual.o: local_misc.h
$(DESTLIB)(alloc.o): $(USRINC)/stdio.h
$(DESTLIB)(ask.o): $(USRINC)/stdio.h
$(DESTLIB)(banner.o): $(USRINC)/stdio.h
$(DESTLIB)(basename.o): $(USRINC)/stdio.h
$(DESTLIB)(bedaemon.o): $(USRINC)/stdio.h
$(DESTLIB)(bedaemon.o): $(USRINC)/signal.h
$(DESTLIB)(bedaemon.o): $(USRINC)/kernel/param.h
$(DESTLIB)(copyd.o): $(USRINC)/stdio.h
$(DESTLIB)(copyd.o): $(USRINC)/misc.h
$(DESTLIB)(fatal.o): $(USRINC)/stdio.h
$(DESTLIB)(fatal.o): $(USRINC)/errno.h
$(DESTLIB)(fifo.o): $(USRINC)/kernel/typed.h
$(DESTLIB)(fifo.o): $(USRINC)/stdio.h
$(DESTLIB)(fifo_b.o): $(USRINC)/kernel/typed.h
$(DESTLIB)(flatten.o): $(USRINC)/misc.h
$(DESTLIB)(getargs.o): $(USRINC)/stdio.h
$(DESTLIB)(getline.o): $(USRINC)/misc.h
$(DESTLIB)(getpty.o): $(USRINC)/errno.h
$(DESTLIB)(getpty.o): $(USRINC)/fcntl.h
$(DESTLIB)(ifcoh.o): $(USRINC)/sys/utsname.h
$(DESTLIB)(ifcoh.o): $(USRINC)/string.h
$(DESTLIB)(is_fs.o): $(USRINC)/sys/stat.h
$(DESTLIB)(is_fs.o): $(USRINC)/canon.h
$(DESTLIB)(is_fs.o): $(USRINC)/sys/filsys.h
$(DESTLIB)(kernelName.o): $(USRINC)/kernel/typed.h
$(DESTLIB)(kernelName.o): $(USRINC)/fcntl.h
$(DESTLIB)(lock.o): $(USRINC)/stdio.h
$(DESTLIB)(lock.o): $(USRINC)/sys/stat.h
$(DESTLIB)(lock.o): $(USRINC)/unistd.h
$(DESTLIB)(lock.o): $(USRINC)/stdio.h
$(DESTLIB)(metaphone.o): $(USRINC)/ctype.h
$(DESTLIB)(metaphone.o): $(USRINC)/misc.h
$(DESTLIB)(pathn.o): $(USRINC)/stdio.h
$(DESTLIB)(pathn.o): $(USRINC)/path.h
$(DESTLIB)(pathn.o): $(USRINC)/unistd.h
$(DESTLIB)(pathn.o): $(USRINC)/misc.h
$(DESTLIB)(regerror.o): $(USRINC)/stdio.h
$(DESTLIB)(regexp.o): $(USRINC)/stdio.h
$(DESTLIB)(regsub.o): $(USRINC)/stdio.h
$(DESTLIB)(select.o): $(USRINC)/poll.h
$(DESTLIB)(select.o): $(USRINC)/sys/select.h
$(DESTLIB)(select.o): $(USRINC)/sys/time.h
$(DESTLIB)(select.o): $(USRINC)/kernel/param.h
$(DESTLIB)(skip.o): $(USRINC)/stdio.h
$(DESTLIB)(skip.o): $(USRINC)/ctype.h
$(DESTLIB)(span.o): $(USRINC)/stdio.h
$(DESTLIB)(span.o): $(USRINC)/ctype.h
$(DESTLIB)(splitter.o): $(USRINC)/misc.h
$(DESTLIB)(strdlen.o): $(USRINC)/string.h
$(DESTLIB)(tocont.o): $(USRINC)/misc.h
$(DESTLIB)(trim.o): $(USRINC)/ctype.h
$(DESTLIB)(trim.o): $(USRINC)/stdio.h
$(DESTLIB)(usage.o): $(USRINC)/stdio.h
$(DESTLIB)(virtual.o): $(USRINC)/sys/stat.h
$(DESTLIB)(xopen.o): $(USRINC)/stdio.h
$(DESTLIB)(yn.o): $(USRINC)/misc.h


# DEPENDENCIES MUST END AT END OF FILE: IF YOU PUT STUFF HERE, IT WILL GO AWAY
# See make depend, above
