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
	$(CC) -so $@ us.o $(LDLIBS)

uk:	uk.o $(LDLIBS)
	$(CC) -so $@ uk.o $(LDLIBS)

german:	german.o $(LDLIBS)
	$(CC) -so $@ german.o $(LDLIBS)

french:	french.o $(LDLIBS)
	$(CC) -so $@ french.o $(LDLIBS)

belgian:	belgian.o $(LDLIBS)
	$(CC) -so $@ belgian.o $(LDLIBS)

italian:	italian.o $(LDLIBS)
	$(CC) -so $@ italian.o $(LDLIBS)

spanish:	spanish.o $(LDLIBS)
	$(CC) -so $@ spanish.o $(LDLIBS)

swedish:	swedish.o $(LDLIBS)
	$(CC) -so $@ swedish.o $(LDLIBS)

danish:	danish.o $(LDLIBS)
	$(CC) -so $@ danish.o $(LDLIBS)

norwegian:	norwegian.o $(LDLIBS)
	$(CC) -so $@ norwegian.o $(LDLIBS)

swiss_french:	swiss_french.o $(LDLIBS)
	$(CC) -so $@ swiss_french.o $(LDLIBS)

swiss_german:	swiss_german.o $(LDLIBS)
	$(CC) -so $@ swiss_german.o $(LDLIBS)

canadian:	canadian.o $(LDLIBS)
	$(CC) -so $@ canadian.o $(LDLIBS)

dutch:	dutch.o $(LDLIBS)
	$(CC) -so $@ dutch.o $(LDLIBS)

latin_am:	latin_am.o $(LDLIBS)
	$(CC) -so $@ latin_am.o $(LDLIBS)

portuguese:	portuguese.o $(LDLIBS)
	$(CC) -so $@ portuguese.o $(LDLIBS)

# end of /conf/kbd/Makefile
