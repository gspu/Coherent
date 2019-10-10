# Simple Coherent makefile for dump driver

INSTALL_DIR=..

all: $(INSTALL_DIR)/Driver.o $(INSTALL_DIR)/Space.c $(INSTALL_DIR)/dump.h

$(INSTALL_DIR)/Driver.o: dump.c
	$(CC) $(CFLAGS) -o $(INSTALL_DIR)/Driver.o -c dump.c

$(INSTALL_DIR)/Space.c: Space.c
	ln Space.c $(INSTALL_DIR)

$(INSTALL_DIR)/dump.h: dump.h
	ln dump.h $(INSTALL_DIR)

install:
	cp dump.h /usr/include
	chmog =r bin bin /usr/include/dump.h

clean:
	rm -f $(INSTALL_DIR)/Driver.o
