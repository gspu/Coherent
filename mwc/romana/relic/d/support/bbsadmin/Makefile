LDFLAGS= -lcurses -lterm
CFLAGS= -c -v -A

OBJECTS= globals.o main.o maillist.o display_mail.o states.o lite.o getfilename.o \
	 add_rec.o map_command.o display_rec.o uucp_upd.o

bbsadmin:	$(OBJECTS)
	cc -v -A $< $(LDFLAGS) -o $@

globals.o:	globals.c
	cc $(CFLAGS) $@ $<

main.o:	main.c
	cc $(CFLAGS) $< -o $@	
	
maillist.o:	maillist.c
	cc $(CFLAGS) $< -o $@

display_mail.o:	display_mail.c
	cc $(CFLAGS) $< -o $@

states.o: states.c
	cc $(CFLAGS) $< -o $@

lite.o:	lite.c
	cc $(CFLAGS) $<  -o $@

getfilename.o:	getfilename.c
	cc $(CFLAGS) $<  -o $@

add_rec.o:	add_rec.c
	cc $(CFLAGS) $<  -o $@

map_command.o:	map_command.c
	cc $(CFLAGS) $< -o $@

display_rec.o:	display_rec.c
	cc $(CFLAGS) $<  -o $@

uucp_upd.o:	uucp_upd.c
	cc $(CFLAGS) $<  -o $@
