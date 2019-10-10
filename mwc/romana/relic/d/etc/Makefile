# CFLAGS are not needed for i8086
CFLAGS= -O -DNEWTTYS

ETCDIR = /newbits/etc
#DESTDIR = /interim/etc/
DESTDIR = /bin386/etc/
UTILS= getty init enable accton cron drvld fdformat timezone termcap \
	mkfs mknod mount umount unmkfs update wall utmp umount.all \
	shutdown serialno rc profile passwd newusr mtab mount.all motd \
	modemcap mnttab logmsg install.log group drvld.all \
	domain uucpname ddate checklist brc.build brc.coh brc.install boottime \
	badscan ATclock .profile build/build fsck/fsck reboot

all:	$(UTILS)
	>all
# include file deftty.h defines the default ERASE, KILL and INTERRUPT 
# characters for ttys.
getty:	getty.c deftty.h
	$(CC) $(CFLAGS) -s -n -i -o $(DESTDIR)getty $<
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

init:	init.c lock.c
	$(CC) $(CFLAGS) -n -i -o $(DESTDIR)init $<
	su root chmog 500 root root $(DESTDIR)$@

init386:	init.c
	$(CC) $(CFLAGS) -n -i -o $(DESTDIR386)init $< -lmisc
	su root chmog 500 root root $(DESTDIR386)init

enable:	enable.c
	$(CC) $(CFLAGS) -o $(DESTDIR)enable $<
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 4511 $(DESTDIR)$@
	su root ln -f $(DESTDIR)$@ $(DESTDIR)disable
	su root ln -f $(DESTDIR)$@ $(DESTDIR)ttystat

accton:	accton.c
	$(CC) $(CFLAGS) -o $(DESTDIR)accton $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

badscan: badscan.c
	$(CC) $(CFLAGS) -o $(DESTDIR)badscan $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

cron:	cron.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cron $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

drvld:	drvld.c
	$(CC) $(CFLAGS) -o $(DESTDIR)drvld $<
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

fdformat: fdformat.c
	$(CC) $(CFLAGS) -o $(DESTDIR)fdformat $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

mkfs386:	mkfs.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)mkfs $<
	su root chown bin $(DESTDIR386)mkfs
	su root chgrp bin $(DESTDIR386)mkfs
	su root chmod 511 $(DESTDIR386)mkfs

mkfs:	mkfs.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mkfs $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

mknod:	mknod.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mknod $<
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

mount386:	mount.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)mount $<
	su root chown bin $(DESTDIR386)mount
	su root chgrp bin $(DESTDIR386)mount
	su root chmod 511 $(DESTDIR386)mount

mount:	mount.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mount $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

reboot:	reboot.c ATrebootas.s
	$(CC) $(CFLAGS) -o $(DESTDIR)reboot $<
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

umount386:	umount.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)umount $<
	su root chown bin $(DESTDIR386)umount
	su root chgrp bin $(DESTDIR386)umount
	su root chmod 511 $(DESTDIR386)umount

umount:	umount.c
	$(CC) $(CFLAGS) -o $(DESTDIR)umount $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

unmkfs386:	unmkfs.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)unmkfs $<
	su root chown bin $(DESTDIR386)unmkfs
	su root chgrp bin $(DESTDIR386)unmkfs
	su root chmod 511 $(DESTDIR386)unmkfs

unmkfs:	unmkfs.c
	$(CC) $(CFLAGS) -o $(DESTDIR)unmkfs $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

update:	update.c
	$(CC) $(CFLAGS) -o $(DESTDIR)update $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

wall:	wall.c
	$(CC) $(CFLAGS) -o $(DESTDIR)wall $<
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

utmp:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

umount.all:
	su root touch $(DESTDIR)$@
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 755 $(DESTDIR)$@

mount.all:
	su root touch $(DESTDIR)$@
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 755 $(DESTDIR)$@

ttys:
	su root echo '1lPconsole' > $(DESTDIR)$@
	su root echo '0lPcom1l' >> $(DESTDIR)$@
	su root echo '0lPcom2l' >> $(DESTDIR)$@
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

timezone:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

termcap:
	cd $(ETCDIR) ; su root co termcap ; mv termcap $(DESTDIR)
	su root chmog 644 bin bin $(DESTDIR)$@

shutdown:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 500 root root $(DESTDIR)$@

serialno:
	su root touch $(DESTDIR)$@
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 444 $(DESTDIR)$@

rc:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 744 root root $(DESTDIR)$@

profile:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 644 root root $(DESTDIR)$@

passwd:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 644 root root $(DESTDIR)$@

newusr:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 500 root root $(DESTDIR)$@
	
mtab:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 666 $(DESTDIR)$@

motd:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

modemcap:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 644 bin bin $(DESTDIR)$@

mnttab:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 666 $(DESTDIR)$@

logmsg:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 666 bin bin $(DESTDIR)$@

install.log:
	su root touch $(DESTDIR)$@
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

helpfile:
	cd $(ETCDIR) ; su root co $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

helpindex:
	cd $(ETCDIR) ; su root co $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

group:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 644 bin bin $(DESTDIR)$@

drvld.all:
	su root rm -f $(DESTDIR)$@
	su root echo "/conf/kbd/us" > $(DESTDIR)$@
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 744 $(DESTDIR)$@

uucpname:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

domain:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

ddate:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

checklist:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 644 bin bin $(DESTDIR)$@

brc.build:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 544 root root $(DESTDIR)$@

brc.coh:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 744 root root $(DESTDIR)$@

brc.install:
	cd $(ETCDIR) ; su root co $@ ; mv $@ $(DESTDIR)
	su root chmog 544 root root $(DESTDIR)$@
	su root ln -f $(DESTDIR)$@ $(DESTDIR)brc

boottime:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 644 $(DESTDIR)$@

ATclock: ATclock.c ATclockas.s
	cc -s -i -O -o $(DESTDIR)ATclock $<
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 500 $(DESTDIR)$@

.profile:
	(co $@ ; cp $@ $(DESTDIR) ; rm $@)
	su root chmog 700 root root $(DESTDIR)$@

build/build:
	cd $(ETCDIR) ; ./make.in build

fsck/fsck:
	cd $(ETCDIR) ; ./make.in fsck

