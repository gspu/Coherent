#
#	Makefile for 'conf' commands
#
#					Angus Telfer (January 1987)
#

DESTDIR=/interim/conf/

CC=exec /bin/cc
CFLAGS=-i -s -O

patch:	patch.c
		$(CC) $(CFLAGS) -o $(DESTDIR)patch patch.c
		su root chmog 500 root root $(DESTDIR)$@
