DESTDIR=/bin386/etc/

init:	init.c lock.c
	$(CC) $(CFLAGS) -n -i -o $(DESTDIR)init $< 
	su root chmog 500 root root $(DESTDIR)init
