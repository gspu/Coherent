# makefile for FP emulation.

INSTALL_DIR= ..

all: $(INSTALL_DIR)/Driver.o $(INSTALL_DIR)/Stub.o $(INSTALL_DIR)/Space.c

$(INSTALL_DIR)/Stub.o : stub.c
	$(CC) $(CFLAGS) -o $@ -c stub.c

$(INSTALL_DIR)/Driver.o: ../obj/em87.o
	ln $< $@

$(INSTALL_DIR)/Space.c: Space.c
	ln Space.c $(INSTALL_DIR)

clean:
	rm -f $(OBJS) $(INSTALL_DIR)/Stub.o
