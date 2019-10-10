# Simple makefile for 'devadm' under Coherent. This doesn't change very often,
# so keeping the .o's around doesn't seem like a win.

INSTALL_DIR=..

TMPDIR = ../obj

SOURCES = assign.c buildobj.c devadm.c ehand.c input.c lex.c mdev.c mkconf.c \
		mklink.c mktune.c mtune.c read.c sdev.c stune.c symbol.c

OBJS = $(TMPDIR)/assign.o $(TMPDIR)/buildobj.o $(TMPDIR)/devadm.o \
	$(TMPDIR)/ehand.o $(TMPDIR)/input.o $(TMPDIR)/lex.o $(TMPDIR)/mdev.o \
	$(TMPDIR)/mkconf.o $(TMPDIR)/mklink.o $(TMPDIR)/mktune.o \
	$(TMPDIR)/mtune.o $(TMPDIR)/read.o $(TMPDIR)/sdev.o \
	$(TMPDIR)/stune.o $(TMPDIR)/symbol.o

devadm: $(INSTALL_DIR)/devadm

$(INSTALL_DIR)/devadm:	$(OBJS)
	cc -o $@ $(OBJS)
	strip $@
	rm -f ../drvbld.mak

clean:
	rm -f $(OBJS)

$(TMPDIR)/assign.o : assign.c
	$(CC) $(CFLAGS) -o $@ -c assign.c

$(TMPDIR)/buildobj.o : buildobj.c
	$(CC) $(CFLAGS) -o $@ -c buildobj.c

$(TMPDIR)/devadm.o : devadm.c
	$(CC) $(CFLAGS) -o $@ -c devadm.c

$(TMPDIR)/ehand.o : ehand.c
	$(CC) $(CFLAGS) -o $@ -c ehand.c

$(TMPDIR)/input.o : input.c
	$(CC) $(CFLAGS) -o $@ -c input.c

$(TMPDIR)/lex.o : lex.c
	$(CC) $(CFLAGS) -o $@ -c lex.c

$(TMPDIR)/mdev.o : mdev.c
	$(CC) $(CFLAGS) -o $@ -c mdev.c

$(TMPDIR)/mkconf.o : mkconf.c
	$(CC) $(CFLAGS) -o $@ -c mkconf.c

$(TMPDIR)/mklink.o : mklink.c
	$(CC) $(CFLAGS) -o $@ -c mklink.c

$(TMPDIR)/mktune.o : mktune.c
	$(CC) $(CFLAGS) -o $@ -c mktune.c

$(TMPDIR)/mtune.o : mtune.c
	$(CC) $(CFLAGS) -o $@ -c mtune.c

$(TMPDIR)/read.o : read.c
	$(CC) $(CFLAGS) -o $@ -c read.c

$(TMPDIR)/sdev.o : sdev.c
	$(CC) $(CFLAGS) -o $@ -c sdev.c

$(TMPDIR)/stune.o : stune.c
	$(CC) $(CFLAGS) -o $@ -c stune.c

$(TMPDIR)/symbol.o : symbol.c
	$(CC) $(CFLAGS) -o $@ -c symbol.c

