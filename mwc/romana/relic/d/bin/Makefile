# CFLAGS are not needed for i8086
CFLAGS= -O -DNEWTTYS 
#DESTDIR= /interim/bin/
DESTDIR= /bin386/bin/
DESTDIR2= /interim/lib/
DESTDIR386= /bin386/bin/
DESTDIR386B= /bin386/etc/
CMDS= expr/expr conv/conv as/as awk/awk  \
      bc/bc check/check compress/compress dc/dc diff/diff dos/dos dump/dump \
      ed/ed find/find grep/grep lex/lex lmail smail make/make sed/sed sh/sh \
      test/test tsort/tsort yacc/yacc nroff/nroff ld debugger detab printers \
      login passwd su ac ar at bad banner basename c cal calendar cat \
      cmp col comm cp cpdir crypt date dd deroff df du echo egrep \
      factor file fixstack from head help join kill lc ln look ls m4 mail man \
      mesg mkdir msg mv newgrp nm od pr prep prof pwd quot ranlib rev rm rmdir \
      sa scat size sleep sort split strings strip stty sum sync tail tar tee \
      time touch tr tty typo uniq units wc who write yes qfind umount true \
      shutdown phone mount fnkey false msgs clear epson cgrep uucheckperms \
      which chroot env chmog

all:	$(CMDS)
	>all
# include file deftty.h defines the default ERASE, KILL and INTERRUPT 
# characters for ttys.

env:	env.c execvep.c
	cc $< -o $(DESTDIR)$@
	su root chmog 511 bin bin $(DESTDIR)$@

env386:	env.c execvep.c
	cc $< -o $(DESTDIR386)env
	su root chmog 511 bin bin $(DESTDIR386)env

chroot:	chroot.c
	cc $< -o $(DESTDIR)$@
	su root chmog 500 root root $(DESTDIR)$@

chroot386:	chroot.c
	cc $< -o $(DESTDIR386)chroot
	su root chmog 500 root root $(DESTDIR386)chroot
	
printers:
	export SRCPATH=.. ; cd lpr/objects ; make

debugger:
	cd db ; make

compiler:
	export SRC=/newbits/bin/cc/c ; . /newbits/bin/cc/c/.profile ; cd cc/work ; make
	su root mv cc/work/bin/cc $(DESTDIR)cc
	su root chmog 511 bin bin $(DESTDIR)cc
	su root mv cc/work/bin/cc0 $(DESTDIR2)cc0
	su root chmog 511 bin bin $(DESTDIR2)cc0
	su root mv cc/work/bin/cc1 $(DESTDIR2)cc1
	su root chmog 511 bin bin $(DESTDIR2)cc1
	su root mv cc/work/bin/cc2 $(DESTDIR2)cc2
	su root chmog 511 bin bin $(DESTDIR2)cc2
	su root mv cc/work/bin/cc3 $(DESTDIR2)cc3
	su root chmog 511 bin bin $(DESTDIR2)cc3
	su root mv cc/work/bin/cpp $(DESTDIR2)cpp
	su root chmog 511 bin bin $(DESTDIR2)cpp

ld:
	cd load/objects ; export SRCPATH=.. ; make

login:	login.c lock.c deftty.h
	$(CC) $(CFLAGS) $< -o $(DESTDIR)$@
	su root chmog 4511 root root $(DESTDIR)$@

login.BBS:	login.c lock.c numtab.c chk_srlno.c deftty.h
	$(CC) $(CFLAGS) -DBBS $< -o $@
	su root chmog 4511 root root $@

passwd:	passwd.c
	$(CC) $(CFLAGS) -o $(DESTDIR)passwd passwd.c
	su root chmog 4511 root root $(DESTDIR)$@

su:	su.c
	$(CC) $(CFLAGS) -o $(DESTDIR)su su.c
	su root chmog 4511 root root $(DESTDIR)$@

ac:	ac.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ac ac.c
	su root chmog 511 bin bin $(DESTDIR)$@

ar:	ar.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ar ar.c
	su root chmog 511 bin bin $(DESTDIR)$@

at:	at.c
	$(CC) $(CFLAGS) -o $(DESTDIR)at at.c
	su root chmog 511 bin bin $(DESTDIR)$@

bad:	bad.c
	$(CC) $(CFLAGS) -o $(DESTDIR)bad bad.c
	su root chmog 511 bin bin $(DESTDIR)$@

banner:	banner.c
	$(CC) $(CFLAGS) -o $(DESTDIR)banner banner.c
	su root chmog 511 bin bin $(DESTDIR)$@

basename: basename.c
	$(CC) $(CFLAGS) -o $(DESTDIR)basename basename.c
	su root chmog 511 bin bin $(DESTDIR)$@

c:	c.c
	$(CC) $(CFLAGS) -o $(DESTDIR)c c.c
	su root chmog 511 bin bin $(DESTDIR)$@

cal:	cal.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cal cal.c
	su root chmog 511 bin bin $(DESTDIR)$@

calendar: calendar.c
	$(CC) $(CFLAGS) -o $(DESTDIR)calendar calendar.c
	su root chmog 511 bin bin $(DESTDIR)$@

cat:	cat.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cat cat.c
	su root chmog 511 bin bin $(DESTDIR)$@

chmog:	chmog.c
	$(CC) -v -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@
	su root ln -f $(DESTDIR)$@ $(DESTDIR)chown
	su root ln -f $(DESTDIR)$@ $(DESTDIR)chgrp
	su root ln -f $(DESTDIR)$@ $(DESTDIR)chmod

chgrp:	chgrp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)chgrp chgrp.c
	su root chmog 511 bin bin $(DESTDIR)$@

chmod:	chmod.c
	$(CC) $(CFLAGS) -o $(DESTDIR)chmod chmod.c
	su root chmog 511 bin bin $(DESTDIR)$@

chown:	chown.c
	$(CC) $(CFLAGS) -o $(DESTDIR)chown chown.c
	su root chmog 511 bin bin $(DESTDIR)$@

cmp:	cmp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cmp cmp.c
	su root chmog 511 bin bin $(DESTDIR)$@

col:	col.c
	$(CC) $(CFLAGS) -o $(DESTDIR)col col.c
	su root chmog 511 bin bin $(DESTDIR)$@

comm:	comm.c
	$(CC) $(CFLAGS) -o $(DESTDIR)comm comm.c
	su root chmog 511 bin bin $(DESTDIR)$@

cp:	cp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cp cp.c
	su root chmog 511 bin bin $(DESTDIR)$@

cpdir:	cpdir.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cpdir cpdir.c
	su root chmog 511 bin bin $(DESTDIR)$@

crypt:	crypt.c
	$(CC) $(CFLAGS) -o $(DESTDIR)crypt crypt.c
	su root chmog 511 bin bin $(DESTDIR)$@

date:	date.c
	$(CC) $(CFLAGS) -o $(DESTDIR)date date.c
	su root chmog 511 bin bin $(DESTDIR)$@

dd:	dd.c
	$(CC) $(CFLAGS) -o $(DESTDIR)dd dd.c
	su root chmog 511 bin bin $(DESTDIR)$@

deroff:	deroff.c
	$(CC) $(CFLAGS) -o $(DESTDIR)deroff deroff.c
	su root chmog 511 bin bin $(DESTDIR)$@

df:	df.c
	$(CC) $(CFLAGS) -o $(DESTDIR)df df.c
	su root chmog 511 bin bin $(DESTDIR)$@

df386:	df.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)df $<
	su root chmog 511 bin bin $(DESTDIR386)df

du:	du.c
	$(CC) $(CFLAGS) -o $(DESTDIR)du du.c
	su root chmog 511 bin bin $(DESTDIR)$@

echo:	echo.c
	$(CC) $(CFLAGS) -o $(DESTDIR)echo echo.c
	su root chmog 511 bin bin $(DESTDIR)$@

egrep:	egrep.c
	$(CC) $(CFLAGS) -o $(DESTDIR)egrep egrep.c
	su root chmog 511 bin bin $(DESTDIR)$@

factor:	factor.c
	$(CC) $(CFLAGS) -o $(DESTDIR)factor factor.c -lm
	su root chmog 511 bin bin $(DESTDIR)$@

file:	file.c
	$(CC) $(CFLAGS) -o $(DESTDIR)file file.c
	su root chmog 511 bin bin $(DESTDIR)$@

fixstack: fixstack.c
	$(CC) $(CFLAGS) -o $(DESTDIR)fixstack fixstack.c
	su root chmog 511 bin bin $(DESTDIR)$@

from:	from.c
	$(CC) $(CFLAGS) -o $(DESTDIR)from from.c
	su root chmog 511 bin bin $(DESTDIR)$@

head:	head.c
	$(CC) $(CFLAGS) -o $(DESTDIR)head head.c
	su root chmog 511 bin bin $(DESTDIR)$@

help:	help.c
	$(CC) $(CFLAGS) -o $(DESTDIR)help help.c
	su root chmog 511 bin bin $(DESTDIR)$@

join:	join.c
	$(CC) $(CFLAGS) -o $(DESTDIR)join join.c
	su root chmog 511 bin bin $(DESTDIR)$@

kill:	kill.c
	$(CC) $(CFLAGS) -o $(DESTDIR)kill kill.c
	su root chmog 511 bin bin $(DESTDIR)$@

kill386:	kill.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)kill $<
	su root chmog 511 bin bin $(DESTDIR386)kill

lc:	lc.c
	$(CC) $(CFLAGS) -o $(DESTDIR)lc lc.c
	su root chmog 511 bin bin $(DESTDIR)$@

ln:	ln.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ln ln.c
	su root chmog 511 bin bin $(DESTDIR)$@

look:	look.c
	$(CC) $(CFLAGS) -o $(DESTDIR)look look.c
	su root chmog 511 bin bin $(DESTDIR)$@

ls:	ls.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ls ls.c
	su root chmog 511 bin bin $(DESTDIR)$@
	su root ln -f $(DESTDIR)$@ $(DESTDIR)l
	su root ln -f $(DESTDIR)$@ $(DESTDIR)lf
	su root ln -f $(DESTDIR)$@ $(DESTDIR)lr
	su root ln -f $(DESTDIR)$@ $(DESTDIR)lx

ls386:	ls.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)ls $<
	su root chmog 511 bin bin $(DESTDIR386)ls
	su root ln -f $(DESTDIR386)ls $(DESTDIR386)l
	su root ln -f $(DESTDIR386)ls $(DESTDIR386)lf
	su root ln -f $(DESTDIR386)ls $(DESTDIR386)lr
	su root ln -f $(DESTDIR386)ls $(DESTDIR386)lx

m4:	m4.c
	$(CC) $(CFLAGS) -o $(DESTDIR)m4 m4.c
	su root chmog 511 bin bin $(DESTDIR)$@

lmail:
	export SRCPATH=.. ; cd mail/usermail/objects ; make
	export SRCPATH=.. ; cd mail/lmail/objects ; make

smail:
	export SRCPATH=.. ; cd mail/smail/objects ; make

man:	man.c
	$(CC) $(CFLAGS) -o $(DESTDIR)man man.c
	su root chmog 511 bin bin $(DESTDIR)$@

mesg:	mesg.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mesg mesg.c
	su root chmog 511 bin bin $(DESTDIR)$@

mkdir:	mkdir.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mkdir mkdir.c
	su root chmog 511 bin bin $(DESTDIR)$@

msg:	msg.c
	$(CC) $(CFLAGS) -o $(DESTDIR)msg msg.c
	su root chmog 511 bin bin $(DESTDIR)$@

mv:	mv.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mv mv.c
	su root chmog 511 bin bin $(DESTDIR)$@

mv386:	mv.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)mv $<
	su root chmog 511 bin bin $(DESTDIR386)mv
	su root ln -f $(DESTDIR386)mv $(DESTDIR386B)mvdir

newgrp:	newgrp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)newgrp newgrp.c
	su root chmog 4511 root root $(DESTDIR)$@

nm386:	nm.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)nm nm.c
	su root chmog 511 bin bin $(DESTDIR386)nm

nm:	nm.c
	$(CC) $(CFLAGS) -o $(DESTDIR)nm nm.c
	su root chmog 511 bin bin $(DESTDIR)$@

od:	od.c
	$(CC) $(CFLAGS) -o $(DESTDIR)od od.c
	su root chmog 511 bin bin $(DESTDIR)$@

pr:	pr.c
	$(CC) $(CFLAGS) -o $(DESTDIR)pr pr.c
	su root chmog 511 bin bin $(DESTDIR)$@

prep:	prep.c
	$(CC) $(CFLAGS) -o $(DESTDIR)prep prep.c
	su root chmog 511 bin bin $(DESTDIR)$@

prof386:	prof.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)prof prof.c
	su root chmog 511 bin bin $(DESTDIR386)prof

prof:	prof.c
	$(CC) $(CFLAGS) -o $(DESTDIR)prof prof.c
	su root chmog 511 bin bin $(DESTDIR)$@

pwd:	pwd.c
	$(CC) $(CFLAGS) -o $(DESTDIR)pwd pwd.c
	su root chmog 511 bin bin $(DESTDIR)$@

quot:	quot.c
	$(CC) $(CFLAGS) -o $(DESTDIR)quot quot.c
	su root chmog 511 bin bin $(DESTDIR)$@

ranlib:	ranlib
	$(CC) $(CFLAGS) -o $(DESTDIR)ranlib ranlib.c
	su root chmog 511 bin bin $(DESTDIR)$@

rev:	rev.c
	$(CC) $(CFLAGS) -o $(DESTDIR)rev rev.c
	su root chmog 511 bin bin $(DESTDIR)$@

rm:	rm.c
	$(CC) $(CFLAGS) -o $(DESTDIR)rm rm.c
	su root chmog 511 bin bin $(DESTDIR)$@

rm386:	rm.c
	$(CC) $(CFLAGS) -o $(DESTDIR386)rm $<
	su root chmog 511 bin bin $(DESTDIR386)rm

rmdir:	rmdir.c
	$(CC) $(CFLAGS) -o $(DESTDIR)rmdir rmdir.c
	su root chmog 511 bin bin $(DESTDIR)$@

sa:	sa.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sa sa.c
	su root chmog 511 bin bin $(DESTDIR)$@

scat:	scat.c
	$(CC) $(CFLAGS) -o $(DESTDIR)scat scat.c
	su root chmog 511 bin bin $(DESTDIR)$@

size:	size.c
	$(CC) $(CFLAGS) -o $(DESTDIR)size size.c
	su root chmog 511 bin bin $(DESTDIR)$@

sleep:	sleep.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sleep sleep.c
	su root chmog 511 bin bin $(DESTDIR)$@

sort:	sort.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sort sort.c
	su root chmog 511 bin bin $(DESTDIR)$@

split:	split.c
	$(CC) $(CFLAGS) -o $(DESTDIR)split split.c
	su root chmog 511 bin bin $(DESTDIR)$@

strings: strings.c
	$(CC) $(CFLAGS) -o $(DESTDIR)strings strings.c
	su root chmog 511 bin bin $(DESTDIR)$@

strip:	strip.c
	$(CC) $(CFLAGS) -o $(DESTDIR)strip strip.c
	su root chmog 511 bin bin $(DESTDIR)$@

stty:	stty.c
	$(CC) $(CFLAGS) -o $(DESTDIR)stty stty.c
	su root chmog 511 bin bin $(DESTDIR)$@

sum:	sum.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sum sum.c
	su root chmog 511 bin bin $(DESTDIR)$@

sync:	sync.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sync sync.c
	su root chmog 511 bin bin $(DESTDIR)$@

tail:	tail.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tail tail.c
	su root chmog 511 bin bin $(DESTDIR)$@

tar:	tar.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tar tar.c
	su root chmog 511 bin bin $(DESTDIR)$@

tee:	tee.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tee tee.c
	su root chmog 511 bin bin $(DESTDIR)$@

time:	time.c
	$(CC) $(CFLAGS) -o $(DESTDIR)time time.c
	su root chmog 511 bin bin $(DESTDIR)$@

touch:	touch.c
	$(CC) $(CFLAGS) -o $(DESTDIR)touch $<
	su root chmog 511 bin bin $(DESTDIR)$@

tr:	tr.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tr tr.c
	su root chmog 511 bin bin $(DESTDIR)$@

tty:	tty.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tty tty.c
	su root chmog 511 bin bin $(DESTDIR)$@

typo:	typo.c
	$(CC) $(CFLAGS) -o $(DESTDIR)typo typo.c
	su root chmog 511 bin bin $(DESTDIR)$@

uniq:	uniq.c
	$(CC) $(CFLAGS) -o $(DESTDIR)uniq uniq.c
	su root chmog 511 bin bin $(DESTDIR)$@

units:	units.c
	$(CC) $(CFLAGS) -o $(DESTDIR)units units.c
	su root chmog 511 bin bin $(DESTDIR)$@

wc:	wc.c
	$(CC) $(CFLAGS) -o $(DESTDIR)wc wc.c
	su root chmog 511 bin bin $(DESTDIR)$@

who:	who.c
	$(CC) $(CFLAGS) -o $(DESTDIR)who who.c
	su root chmog 511 bin bin $(DESTDIR)$@

write:	write.c
	$(CC) $(CFLAGS) -o $(DESTDIR)write write.c
	su root chmog 511 bin bin $(DESTDIR)$@

yes:	yes.c
	$(CC) $(CFLAGS) -o $(DESTDIR)yes yes.c
	su root chmog 511 bin bin $(DESTDIR)$@

qfind:	qfind.c
	$(CC) $(CFLAGS) -o $(DESTDIR)qfind qfind.c
	su root chmog 511 bin bin $(DESTDIR)$@

as/as:
	./make.in as

awk/awk:
	./make.in awk

bc/bc:
	./make.in bc

check/check:
	./make.in check

compress/compress:
	export SRCPATH=.. ; cd compress/objects ; make

dc/dc:
	./make.in dc

diff/diff:
	./make.in diff

dos/dos:
	./make.in dos

dump/dump:
	./make.in dump

ed/ed:
	./make.in ed

find/find:
	./make.in find

grep/grep:
	./make.in grep

lex/lex:
	./make.in lex

make/make:
	./make.in make

sed/sed:
	./make.in sed

sh/sh:
	./make.in sh

test/test:
	./make.in test

tsort/tsort:
	./make.in tsort

yacc/yacc:
	./make.in yacc

expr/expr:
	./make.in expr

conv/conv:
	./make.in conv

nroff/nroff:
	./make.in nroff

mount:
	co $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 755 $(DESTDIR)$@

umount:
	co $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 755 $(DESTDIR)$@

false:
	su root co $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 555 $(DESTDIR)$@

true:
	su root touch $(DESTDIR)$@
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 555 $(DESTDIR)$@

shutdown:
	su root co $(DESTDIR)$@
	su root chmog 500 root root $(DESTDIR)$@

ps:	ps.c
	$(CC) $(CFLAGS) -o $(DESTDIR)$@ ps.c
	su root chmog 6511 sys sys $(DESTDIR)$@

phone:
	su root co $(DESTDIR)$@
	su root chmog 555 bin bin $(DESTDIR)$@

fnkey:	fnkey.c
	$(CC) $(CFLAGS) -o $(DESTDIR)$@ fnkey.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

msgs: msgs.c
	cc -o $(DESTDIR)$@ msgs.c
	su root fixstack 1000 $(DESTDIR)$@
	su root chown msgs $(DESTDIR)$@
	su root chgrp msgs $(DESTDIR)$@
	su root chmod 6511 $(DESTDIR)$@

clear:	clear.c
	cc -O -o $(DESTDIR)$@ clear.c -lterm
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

epson:	epson.c
	cc -O -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

cgrep: cgrep.c
	cc -O -o $(DESTDIR)$@ cgrep.c -lmisc
	su root chmog 511 bin bin $(DESTDIR)$@

detab:
	export SRCPATH=.. ; cd /newbits/usr/bin/emacs/objects ; make detab

uucheckperms:
	cd uucheck ; make

which:	which.c
	cc -O -o $(DESTDIR)$@ $<
	su root chmog 511 bin bin $(DESTDIR)$@

# /bin/login for mwcbbs
bbslogin:	login.o chk_srlno.o numtab.o
	cc -DBBS -DNEWTTYS -o $(DESTDIR)bbs.login login.o chk_srlno.o numtab.o
	su root chmog 6511 root root $(DESTDIR)bbs.login

chk_srlno.o : chk_srlno.c unumber.h

login.o:	login.c
	cc -DBBS -DNEWTTYS -c login.c
