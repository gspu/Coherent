DESTDIR=/interim/bin/

compress : compress.o is_fs.o
	cc -o $(DESTDIR)compress $<
	su root chmog 511 bin bin $(DESTDIR)compress
	su root ln -f $(DESTDIR)compress $(DESTDIR)zcat
	su root ln -f $(DESTDIR)compress $(DESTDIR)uncompress

compress.o : compress.c
	cc -c -O -DVIRTUAL -DBITS=16 $<

install : compress
	su root cp compress /bin
	su root ln -f /bin/compress /bin/zcat
	su root ln -f /bin/compress /bin/uncompress
