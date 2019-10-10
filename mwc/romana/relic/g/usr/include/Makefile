#DESTDIR=/bin386/usr/include/
#DESTDIR2=/bin386/usr/include/sys/
DESTDIR=/usr/include/
DESTDIR2=/usr/include/sys/

INCLUDE=a.out.h access.h acct.h action.h ar.h arcoff.h \
        assert.h canon.h coff.h ctype.h curses.h dirent.h \
        dumptape.h ebcdic.h errno.h fcntl.h float.h fperr.h \
        grp.h l.out.h larges.h limits.h math.h memory.h \
        misc.h mnttab.h mon.h mprec.h mtab.h mtype.h \
        n.out.h path.h poll.h pwd.h setjmp.h sgtty.h \
        signal.h stdarg.h stddef.h stdio.h stdlib.h string.h \
        sys.s term.h terminfo.h termio.h time.h timef.h \
        unctrl.h unistd.h ustat.h utmp.h varargs.h
	
INCLUDE_SYS=alloc.h  ascii.h buf.h chars.h con.h const.h deftty.h \
            devices.h dir.h dirent.h fblk.h fd.h fdioctl.h fdisk.h \
            filsys.h hdioctl.h ino.h inode.h io.h ipc.h kb.h \
            kbscan.h lpioctl.h machine.h malloc.h mdata.h mount.h msg.h \
            msig.h mtioctl.h param.h poll.h proc.h rlock.h sched.h \
            seg.h select.h sem.h shm.h stat.h statfs.h stream.h timeb.h \
            timeout.h times.h tty.h types.h uproc.h utsname.h

INCL_MWC=dial.h ascii.h disp.h ieeefp.h

INCL_SYS_MWC= a_drive.h aha154x.h al.h alloc.h asy.h clist.h coherent.h \
	dmac.h errno.h fakeff.h fun.h i8086.h ins8250.h ktty.h m18hd.h \
	me.h mmu.h ms.h mwc_coh.h null.h oldstat.h oldtimeb.h oldtimes.h \
	poll_clk.h ptrace.h reg.h scsiwork.h sdioctl.h signal.h silo.h \
	ss.h stropts.h systab.h tnioctl.h vt.h

op_system: 
	co $(INCLUDE) ; su root cp $(INCLUDE) $(DESTDIR) ; su root rm $(INCLUDE)
	cd sys; co $(INCLUDE_SYS) ; su root cp $(INCLUDE_SYS) $(DESTDIR2) ; su root rm $(INCLUDE_SYS)

mwc:
	co $(INCLUDE) ; su root cp $(INCLUDE) $(DESTDIR) ; su root rm $(INCLUDE)
	cd sys; co $(INCLUDE_SYS) ; su root cp $(INCLUDE_SYS) $(DESTDIR2) ; su root rm $(INCLUDE_SYS)
	co $(INCL_MWC) ; su root cp $(INCL_MWC) $(DESTDIR) ; su root rm $(INCL_MWC)
	cd sys; co $(INCL_SYS_MWC) ; su root cp $(INCL_SYS_MWC) $(DESTDIR2) ; su root rm $(INCL_SYS_MWC)
	cd $(DESTDIR2) ; su root mv mwc_coh.h mwc_coherent.h
