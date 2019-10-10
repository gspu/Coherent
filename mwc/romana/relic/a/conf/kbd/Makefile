#
# Build keyboard utilities from user-supplied keyboard tables
#
CFLAGS=-O -I/usr/include -I/usr/include/sys
LDLIBS=kbmain.o
PRINT=pr
SPOOL=hp | hpr -B
TABLES=\
	us \
	uk \
	german \
	french \

OBJS=\
	us.o \
	uk.o \
	german.o \
	french.o \

SRCS=\
	us.c \
	uk.c \
	german.c \
	french.c \

HEADERS=\
	kb.h \
	kbscan.h \

all:	$(TABLES)
	@echo 'Done!'

clean:
	rm $(OBJS)

print:
	$(PRINT) $(SRCS) $(HEADERS) | $(SPOOL)
#
# Insert any keyboard specific instructions below
#
us:	us.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

uk:	uk.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

german:	german.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

french:	french.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

