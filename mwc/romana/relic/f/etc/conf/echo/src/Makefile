# Simple Coherent makefile for echo driver

INSTALL_DIR=..

all: $(INSTALL_DIR)/Driver.o

$(INSTALL_DIR)/Driver.o: echo.c
	$(CC) $(CFLAGS) -o $(INSTALL_DIR)/Driver.o -c echo.c

install:

clean:
	rm -f $(INSTALL_DIR)/Driver.o
