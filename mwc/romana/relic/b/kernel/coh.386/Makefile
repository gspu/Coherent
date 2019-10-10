#
#	Makefile for Coherent (TM) 386 base system
#

CC=exec /bin/cc
USRINC=/usr/include

#.c.o:	
#	@echo "$(CC) $(CFLAGS) -co $(K386OBJ)/$*.o $<"
#	$(CC) $(CFLAGS) -co $(K386OBJ)/$*.o $<
#	exit $$?

COHOBJ= $(K386OBJ)/alloc.o \
	$(K386OBJ)/bio.o \
	$(K386OBJ)/clist.o \
	$(K386OBJ)/clock.o \
	$(K386OBJ)/clocked.o \
	$(K386OBJ)/exec.o \
	$(K386OBJ)/fd.o \
	$(K386OBJ)/fifo.o \
	$(K386OBJ)/fs1.o \
	$(K386OBJ)/fs2.o \
	$(K386OBJ)/fs3.o \
	$(K386OBJ)/main.o \
	$(K386OBJ)/misc.o \
	$(K386OBJ)/null.o \
	$(K386OBJ)/pipe.o \
	$(K386OBJ)/poll.o \
	$(K386OBJ)/printf.o \
	$(K386OBJ)/proc.o \
	$(K386OBJ)/rlock.o \
	$(K386OBJ)/seg.o \
	$(K386OBJ)/sig.o \
	$(K386OBJ)/support.o \
	$(K386OBJ)/sys1.o \
	$(K386OBJ)/sys2.o \
	$(K386OBJ)/sys3.o \
	$(K386OBJ)/sys4.o \
	$(K386OBJ)/sys5.o \
	$(K386OBJ)/syscoh.o \
	$(K386OBJ)/timeout.o \
	$(K386OBJ)/var.o

all:	$(COHOBJ)
	@echo "coh objects up to date"

$(K386OBJ)/alloc.o: alloc.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/bio.o: bio.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/clist.o: clist.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/clock.o: clock.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/clocked.o: clocked.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/fd.o: fd.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/sys/param.h \
	$(USRINC)/errno.h \
	$(USRINC)/fcntl.h \
	$(USRINC)/sys/fd.h \
	$(USRINC)/sys/inode.h
	$(CC) $(CFLAGS) -c -o $@ fd.c

$(K386OBJ)/fifo.o: fifo.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/fs1.o: fs1.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/sys/buf.h \
	$(USRINC)/canon.h \
	$(USRINC)/dirent.h \
	$(USRINC)/errno.h \
	$(USRINC)/sys/filsys.h \
	$(USRINC)/sys/ino.h \
	$(USRINC)/sys/inode.h \
	$(USRINC)/sys/io.h \
	$(USRINC)/sys/mount.h \
	$(USRINC)/sys/stat.h 
	$(CC) $(CFLAGS) -c -o $@ fs1.c

$(K386OBJ)/fs2.o: fs2.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/sys/acct.h \
	$(USRINC)/sys/buf.h \
	$(USRINC)/canon.h \
	$(USRINC)/sys/con.h \
	$(USRINC)/errno.h \
	$(USRINC)/sys/filsys.h \
	$(USRINC)/sys/ino.h \
	$(USRINC)/sys/inode.h \
	$(USRINC)/sys/io.h \
	$(USRINC)/sys/mount.h \
	$(USRINC)/sys/proc.h \
	$(USRINC)/sys/stat.h
	$(CC) $(CFLAGS) -c -o $@ fs2.c

$(K386OBJ)/fs3.o: fs3.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/sys/buf.h \
	$(USRINC)/canon.h \
	$(USRINC)/sys/con.h \
	$(USRINC)/errno.h \
	$(USRINC)/sys/filsys.h \
	$(USRINC)/sys/mount.h \
	$(USRINC)/sys/io.h \
	$(USRINC)/sys/ino.h \
	$(USRINC)/sys/inode.h \
	$(USRINC)/sys/stat.h
	$(CC) $(CFLAGS) -c -o $@ fs3.c

$(K386OBJ)/main.o: ../i386/version main.c
	$(CC) $(CFLAGS) -o $(K386OBJ)/main.o `../i386/version` -c main.c

$(K386OBJ)/misc.o: misc.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/pipe.o: pipe.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/errno.h \
	$(USRINC)/sys/filsys.h \
	$(USRINC)/sys/ino.h \
	$(USRINC)/sys/inode.h \
	$(USRINC)/sys/io.h \
	$(USRINC)/sys/proc.h \
	$(USRINC)/sys/sched.h \
	$(USRINC)/signal.h
	$(CC) $(CFLAGS) -c -o $@ pipe.c

$(K386OBJ)/poll.o: poll.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/printf.o: printf.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/proc.o: proc.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/rlock.o: rlock.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/errno.h \
	$(USRINC)/fcntl.h \
	$(USRINC)/sys/fd.h \
	$(USRINC)/sys/proc.h \
	$(USRINC)/sys/rlock.h \
	$(USRINC)/sys/sched.h \
	$(USRINC)/sys/uproc.h \
	$(USRINC)/unistd.h
	$(CC) $(CFLAGS) -c -o $@ rlock.c

$(K386OBJ)/seg.o: seg.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/sig.o: sig.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/support.o: support.c
	$(CC) $(CFLAGS) -c -o $@ $<

#$(K386OBJ)/swap.o: swap.c
#	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/elog.o: elog.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/sys1.o: sys1.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/sys2.o: sys2.c \
	$(USRINC)/sys/param.h
	$(CC) $(CFLAGS) -c -o $@ sys2.c

$(K386OBJ)/sys3.o: sys3.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/sys/buf.h \
	$(USRINC)/errno.h \
	$(USRINC)/fcntl.h \
	$(USRINC)/sys/fd.h \
	$(USRINC)/sys/filsys.h \
	$(USRINC)/sys/ino.h \
	$(USRINC)/sys/inode.h \
	$(USRINC)/sys/io.h \
	$(USRINC)/sys/mount.h \
	$(USRINC)/sys/stat.h
	$(CC) $(CFLAGS) -c -o $@ sys3.c

$(K386OBJ)/timeout.o: timeout.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/var.o: var.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/null.o: null.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/exec.o: exec.c \
	$(USRINC)/sys/coherent.h \
	$(USRINC)/sys/param.h \
	$(USRINC)/sys/acct.h \
	$(USRINC)/sys/buf.h \
	$(USRINC)/canon.h \
	$(USRINC)/sys/con.h \
	$(USRINC)/errno.h \
	$(USRINC)/fcntl.h \
	$(USRINC)/sys/filsys.h \
	$(USRINC)/sys/ino.h \
	$(USRINC)/sys/inode.h \
	$(USRINC)/a.out.h \
	$(USRINC)/l.out.h \
	$(USRINC)/sys/proc.h \
	$(USRINC)/sys/sched.h \
	$(USRINC)/sys/seg.h \
	$(USRINC)/signal.h \
	$(USRINC)/sys/uproc.h \
	$(USRINC)/sys/reg.h \
	$(USRINC)/sys/stat.h \
	$(USRINC)/sys/fd.h
	$(CC) $(CFLAGS) -c -o $@ exec.c

$(K386OBJ)/sys4.o: sys4.c \
	$(USRINC)/sgtty.h
	$(CC) $(CFLAGS) -c -o $@ sys4.c

$(K386OBJ)/sys5.o: sys5.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/syscoh.o: syscoh.c
	$(CC) $(CFLAGS) -c -o $@ $<
