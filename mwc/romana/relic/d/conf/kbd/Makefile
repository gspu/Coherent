#
# Build keyboard utilities from user-supplied keyboard tables
#
# Version 1.1
#
CFLAGS=-O
LDLIBS=kbmain.o
PRINT=pr
SPOOL=hp | hpr -B
TABLES=\
	belgian \
	canadian \
	danish \
	dutch \
	french \
	german \
	italian \
	latin_am \
	norwegian \
	portuguese \
	spanish \
	swedish \
	swiss_french \
	swiss_german \
	uk \
	us \

OBJS=\
	belgian.o \
	canadian.o \
	danish.o \
	dutch.o \
	french.o \
	german.o \
	italian.o \
	latin_am.o \
	norwegian.o \
	portuguese.o \
	spanish.o \
	swedish.o \
	swiss_french.o \
	swiss_german.o \
	uk.o \
	us.o \

SRCS=\
	belgian.c \
	canadian.c \
	danish.c \
	dutch.c \
	french.c \
	german.c \
	italian.c \
	latin_am.c \
	norwegian.c \
	portuguese.c \
	spanish.c \
	swedish.c \
	swiss_french.c \
	swiss_german.c \
	uk.c \
	us.c \

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

belgian:	belgian.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

italian:	italian.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

spanish:	spanish.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

swedish:	swedish.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

danish:	danish.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

norwegian:	norwegian.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

swiss_french:	swiss_french.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

swiss_german:	swiss_german.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

canadian:	canadian.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

dutch:	dutch.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

latin_am:	latin_am.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

portuguese:	portuguese.o $(LDLIBS)
	$(CC) -o $@ $<
	strip $@

# end of /conf/kbd/Makefile
