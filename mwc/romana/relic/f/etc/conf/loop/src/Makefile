# Simple Coherent makefile for loop driver

INSTALL_DIR=..

all: $(INSTALL_DIR)/Driver.o $(INSTALL_DIR)/Space.c $(INSTALL_DIR)/loop.h

$(INSTALL_DIR)/Driver.o : loop.c
	$(CC) $(CFLAGS) -o $(INSTALL_DIR)/Driver.o -c loop.c

$(INSTALL_DIR)/Space.c: Space.c
	ln Space.c $(INSTALL_DIR)

$(INSTALL_DIR)/loop.h: loop.h
	ln loop.h $(INSTALL_DIR)

install:
	cp loop.h /usr/include
	chmog =r bin bin /usr/include/loop.h

clean:
	rm -f $(INSTALL_DIR)/Driver.o
