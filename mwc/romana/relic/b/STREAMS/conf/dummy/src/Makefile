# Simple Coherent makefile for dummy driver

DEVICE=dummy

INSTALL_DIR=..

all: $(INSTALL_DIR)/Driver.o

$(INSTALL_DIR)/Driver.o: dummy.c
	$(CC) $(CFLAGS) -o $@ -c dummy.c

install:
	@:

clean:
	rm -f $(INSTALL_DIR)/Driver.o
