DESTDIR=/interim/bin/
O = send.o mail.o util.o
S = send.c mail.c util.c mail.h Makefile

mail : $O
	cc -o $(DESTDIR)mail $O
	su root chown root $(DESTDIR)mail
	su root chgrp root $(DESTDIR)mail
	su root chmod 6511 $(DESTDIR)mail
	su root ln -f $(DESTDIR)$@ $(DESTDIR)rmail
mail.a : $S
	rm mail.a
	ar q mail.a $S

