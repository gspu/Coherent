# Simple Coherent makefile for patch driver

INSTALL_DIR=..

all: $(INSTALL_DIR)/Driver.o $(INSTALL_DIR)/Space.c

$(INSTALL_DIR)/Driver.o : patch.c
	$(CC) $(CFLAGS) -o $(INSTALL_DIR)/Driver.o -c patch.c

$(INSTALL_DIR)/Space.c: Space.c
	ln Space.c $(INSTALL_DIR)

clean:
	rm -f $(INSTALL_DIR)/Driver.o
