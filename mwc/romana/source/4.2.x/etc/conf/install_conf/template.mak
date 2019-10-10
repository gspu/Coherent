# Template for kernel build makefile.
# Below, %C -> compile rules, %l -> link files, %b = "before" files,
# %B -> "before" rules, %a = "after" files, %A -> "after" rules,
# %r -> clean files and %% -> %.

PREFLAGS = 
POSTFLAGS = $(TARGET)

DRVOBJS = obj/conf.o \
	%l
DRVLIBS = %L

all: pre $(TARGET) post

pre:
	for i in %b; do [ -x $$i ] && $$i $(PREFLAGS); done

$(TARGET): $(K386LIB)/k0.o $(DRVOBJS) $(DRVLIBS) $(K386LIB)/k386.a \
	   $(FORCE_KERNEL)
	set -f; /bin/ld -K $$($(KEEP_FILE)) -o $(TARGET) -e stext $(K386LIB)/k0.o \
		$(DRVOBJS) $(DRVLIBS) $(K386LIB)/k386.a >$(TARGET).sym

post:
	for i in %a; do [ -x $$i ] && $$i $(POSTFLAGS); done

obj/conf.o : conf.c conf.h
	$(CC) $(CFLAGS) -o obj/conf.o -c conf.c

conf.h: $(MTUNE_FILE) $(STUNE_FILE)
	echo confpath=$(CONFPATH)
	$(CONFPATH)/bin/devadm -x $(MTUNE_FILE) -y $(STUNE_FILE) -t
	rm -f obj/*

conf.c: $(MDEV_FILE) $(SDEV_FILE)
	@echo "This shouldn't happen, but if you see this message then it is"
	@echo "likely that some files have dates set in the future or some"
	@echo "similar configuration anomaly."
	exit 1

%A%B%C

clean:
	rm conf.c conf.h
	[ "%r" ] && rm -f %r

# The following allows the macro FORCE_KERNEL to be used to force a relink
# of the target file; doing this allows driver developers to use their own
# style when it comes to kernel rebuilds. We use this in preference to some
# more brute-force technique such as removing an old kernel to allow people
# more flexibility with permissions.

in_case_we_are_not_forcing $(FORCE_KERNEL): this_does_not_exist

this_does_not_exist:

