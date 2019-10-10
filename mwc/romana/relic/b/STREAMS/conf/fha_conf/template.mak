# Template for kernel build makefile.
# Below, %C -> compile rules, %l -> link files, %b = "before" files,
# %B -> "before" rules, %a = "after" files, %A -> "after" rules,
# %r -> clean files and %% -> %.

PREFLAGS = 
POSTFLAGS = $(TARGET)

DRVOBJS = obj/conf.o %l

all: pre $(TARGET) post

pre: %b
	for i in $<; do if [ -x $$i ]; then $$i $(PREFLAGS); fi; done

$(TARGET): $(K386LIB)/k0.o $(DRVOBJS) $(K386LIB)/k386.a
	ld -K -o $(TARGET) -e stext $(K386LIB)/k0.o $(DRVOBJS) \
			$(K386LIB)/k386.a

post: %a
	for i in $<; do if [ -x $$i ]; then $$i $(POSTFLAGS); fi; done

obj/conf.o : conf.c conf.h
	$(CC) $(CFLAGS) -o obj/conf.o -c conf.c

conf.h: mtune stune
	echo confpath=$(CONFPATH)
	$(CONFPATH)/bin/devadm -t
	if [ "obj/*" ] ; then rm -f obj/* ; else true ; fi

conf.c: mdevice sdevice
	@echo "You need to run \"make\" in response to a configuration change"
	exit 1

%A%B%C

clean:
	rm conf.c conf.h
	if [ "%r" ] ; then rm -f %r ; else true ; fi

