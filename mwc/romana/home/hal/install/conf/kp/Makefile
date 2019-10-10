#
#	Makefile for kpatch command
#

MAKEFILE=Makefile
KPATCH_OBJ = y.tab.o lex.yy.o kpatch.o

INSTALL_DIR=..

RESULT=$(INSTALL_DIR)/kpatch

$(RESULT):	$(KPATCH_OBJ)
	$(CC) -s -o $@ $(KPATCH_OBJ) -ly -ll

y.tab.c: kpatch.yy patchl.h
	yacc kpatch.yy

y.tab.o: y.tab.c
	$(CC) -c y.tab.c

lex.yy.c: kpatch.ll patchl.h
	lex kpatch.ll

lex.yy.o: lex.yy.c
	$(CC) -c lex.yy.c

kpatch.o: kpatch.c
	$(CC) -c kpatch.c

depend:
	echo > makedep
	for OBJ in $(KPATCH_OBJ) ;\
	do \
		SRC=$${OBJ%.*}.c ;\
		gc -M $${SRC} >> makedep; \
	done
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

y.tab.o : y.tab.c patchl.h /usr/include/sys/patch.h /usr/include/common/feature.h \
  y.tab.h 
kpatch.o : kpatch.c /usr/include/stdio.h /usr/include/common/feature.h /usr/include/common/ccompat.h \
  /usr/include/common/_ccompat.h /usr/include/common/__off.h /usr/include/common/__valist.h \
  /usr/include/common/_size.h /usr/include/common/__size.h /usr/include/common/_null.h \
  /usr/include/common/whence.h /usr/include/errno.h /usr/include/sys/errno.h /usr/include/sys/patch.h \
  patchl.h 

# DEPENDENCIES MUST END AT END OF FILE: IF YOU PUT STUFF HERE, IT WILL GO AWAY
# See make depend, above
