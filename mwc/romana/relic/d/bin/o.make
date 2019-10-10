# CFLAGS are not needed for i8086
CFLAGS= -O -DNEWTTYS
DESTDIR= /interim/bin/
CMDS= expr/expr conv/conv as/as awk/awk \
      bc/bc check/check compress/compress dc/dc diff/diff dos/dos dump/dump \
      ed/ed find/find grep/grep lex/lex mail/mail make/make sed/sed sh/sh \
      test/test tsort/tsort yacc/yacc \
      login passwd su ac ar at bad banner basename c cal calendar cat chgrp \
      chmod chown cmp col comm cp cpdir crypt date dd deroff df du echo egrep \
      factor file fixstack from head help join kill lc ln look ls m4 mail man \
      mesg mkdir msg mv newgrp nm od pr prep prof pwd quot ranlib rev rm rmdir \
      sa scat size sleep sort split strings strip stty sum sync tail tar tee \
      time touch tr tty typo uniq units wc who write yes qfind 

all:	$(CMDS)
	>all
# include file deftty.h defines the default ERASE, KILL and INTERRUPT 
# characters for ttys.

login:	login.c deftty.h
	$(CC) $(CFLAGS) -o $(DESTDIR)login login.c
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 4511 $(DESTDIR)$@

passwd:	passwd.c
	$(CC) $(CFLAGS) -o $(DESTDIR)passwd passwd.c
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 4511 $(DESTDIR)$@

su:	su.c
	$(CC) $(CFLAGS) -o $(DESTDIR)su su.c
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 4511 $(DESTDIR)$@

ac:	ac.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ac ac.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

ar:	ar.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ar ar.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

at:	at.c
	$(CC) $(CFLAGS) -o $(DESTDIR)at at.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

bad:	bad.c
	$(CC) $(CFLAGS) -o $(DESTDIR)bad bad.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

banner:	banner.c
	$(CC) $(CFLAGS) -o $(DESTDIR)banner banner.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

basename: basename.c
	$(CC) $(CFLAGS) -o $(DESTDIR)basename basename.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

c:	c.c
	$(CC) $(CFLAGS) -o $(DESTDIR)c c.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

cal:	cal.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cal cal.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

calendar: calendar.c
	$(CC) $(CFLAGS) -o $(DESTDIR)calendar calendar.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

cat:	cat.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cat cat.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

chgrp:	chgrp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)chgrp chgrp.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

chmod:	chmod.c
	$(CC) $(CFLAGS) -o $(DESTDIR)chmod chmod.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

chown:	chown.c
	$(CC) $(CFLAGS) -o $(DESTDIR)chown chown.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

cmp:	cmp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cmp cmp.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

col:	col.c
	$(CC) $(CFLAGS) -o $(DESTDIR)col col.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

comm:	comm.c
	$(CC) $(CFLAGS) -o $(DESTDIR)comm comm.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

cp:	cp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cp cp.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

cpdir:	cpdir.c
	$(CC) $(CFLAGS) -o $(DESTDIR)cpdir cpdir.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

crypt:	crypt.c
	$(CC) $(CFLAGS) -o $(DESTDIR)crypt crypt.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

date:	date.c
	$(CC) $(CFLAGS) -o $(DESTDIR)date date.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

dd:	dd.c
	$(CC) $(CFLAGS) -o $(DESTDIR)dd dd.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

deroff:	deroff.c
	$(CC) $(CFLAGS) -o $(DESTDIR)deroff deroff.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

df:	df.c
	$(CC) $(CFLAGS) -o $(DESTDIR)df df.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

du:	du.c
	$(CC) $(CFLAGS) -o $(DESTDIR)du du.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

echo:	echo.c
	$(CC) $(CFLAGS) -o $(DESTDIR)echo echo.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

egrep:	egrep.c
	$(CC) $(CFLAGS) -o $(DESTDIR)egrep egrep.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

factor:	factor.c
	$(CC) $(CFLAGS) -o $(DESTDIR)factor factor.c -lm
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

file:	file.c
	$(CC) $(CFLAGS) -o $(DESTDIR)file file.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

fixstack: fixstack.c
	$(CC) $(CFLAGS) -o $(DESTDIR)fixstack fixstack.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

from:	from.c
	$(CC) $(CFLAGS) -o $(DESTDIR)from from.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

head:	head.c
	$(CC) $(CFLAGS) -o $(DESTDIR)head head.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

help:	help.c
	$(CC) $(CFLAGS) -o $(DESTDIR)help help.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

join:	join.c
	$(CC) $(CFLAGS) -o $(DESTDIR)join join.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

kill:	kill.c
	$(CC) $(CFLAGS) -o $(DESTDIR)kill kill.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

lc:	lc.c
	$(CC) $(CFLAGS) -o $(DESTDIR)lc lc.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

ln:	ln.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ln ln.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

look:	look.c
	$(CC) $(CFLAGS) -o $(DESTDIR)look look.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

ls:	ls.c
	$(CC) $(CFLAGS) -o $(DESTDIR)ls ls.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

m4:	m4.c
	$(CC) $(CFLAGS) -o $(DESTDIR)m4 m4.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

mail:	mail.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mail mail.c
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 4511 $(DESTDIR)$@
	su root ln -f $(DESTDIR)$@ $(DESTDIR)rmail

man:	man.c
	$(CC) $(CFLAGS) -o $(DESTDIR)man man.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

mesg:	mesg.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mesg mesg.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

mkdir:	mkdir.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mkdir mkdir.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

msg:	msg.c
	$(CC) $(CFLAGS) -o $(DESTDIR)msg msg.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

mv:	mv.c
	$(CC) $(CFLAGS) -o $(DESTDIR)mv mv.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

newgrp:	newgrp.c
	$(CC) $(CFLAGS) -o $(DESTDIR)newgrp newgrp.c
	su root chown root $(DESTDIR)$@
	su root chgrp root $(DESTDIR)$@
	su root chmod 4511 $(DESTDIR)$@

nm:	nm.c
	$(CC) $(CFLAGS) -o $(DESTDIR)nm nm.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

od:	od.c
	$(CC) $(CFLAGS) -o $(DESTDIR)od od.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

pr:	pr.c
	$(CC) $(CFLAGS) -o $(DESTDIR)pr pr.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

prep:	prep.c
	$(CC) $(CFLAGS) -o $(DESTDIR)prep prep.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

prof:	prof.c
	$(CC) $(CFLAGS) -o $(DESTDIR)prof prof.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

pwd:	pwd.c
	$(CC) $(CFLAGS) -o $(DESTDIR)pwd pwd.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

quot:	quot.c
	$(CC) $(CFLAGS) -o $(DESTDIR)quot quot.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

ranlib:	ranlib
	$(CC) $(CFLAGS) -o $(DESTDIR)ranlib ranlib.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

rev:	rev.c
	$(CC) $(CFLAGS) -o $(DESTDIR)rev rev.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

rm:	rm.c
	$(CC) $(CFLAGS) -o $(DESTDIR)rm rm.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

rmdir:	rmdir.c
	$(CC) $(CFLAGS) -o $(DESTDIR)rmdir rmdir.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

sa:	sa.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sa sa.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

scat:	scat.c
	$(CC) $(CFLAGS) -o $(DESTDIR)scat scat.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

size:	size.c
	$(CC) $(CFLAGS) -o $(DESTDIR)size size.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

sleep:	sleep.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sleep sleep.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

sort:	sort.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sort sort.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

split:	split.c
	$(CC) $(CFLAGS) -o $(DESTDIR)split split.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

strings: strings.c
	$(CC) $(CFLAGS) -o $(DESTDIR)strings strings.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

strip:	strip.c
	$(CC) $(CFLAGS) -o $(DESTDIR)strip strip.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

stty:	stty.c
	$(CC) $(CFLAGS) -o $(DESTDIR)stty stty.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

sum:	sum.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sum sum.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

sync:	sync.c
	$(CC) $(CFLAGS) -o $(DESTDIR)sync sync.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

tail:	tail.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tail tail.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

tar:	tar.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tar tar.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

tee:	tee.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tee tee.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

time:	time.c
	$(CC) $(CFLAGS) -o $(DESTDIR)time time.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

touch:	touch.c
	$(CC) $(CFLAGS) -o $(DESTDIR)touch time.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

tr:	tr.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tr tr.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

tty:	tty.c
	$(CC) $(CFLAGS) -o $(DESTDIR)tty tty.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

typo:	typo.c
	$(CC) $(CFLAGS) -o $(DESTDIR)typo typo.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

uniq:	uniq.c
	$(CC) $(CFLAGS) -o $(DESTDIR)uniq uniq.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

units:	units.c
	$(CC) $(CFLAGS) -o $(DESTDIR)units units.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

wc:	wc.c
	$(CC) $(CFLAGS) -o $(DESTDIR)wc wc.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

who:	who.c
	$(CC) $(CFLAGS) -o $(DESTDIR)who who.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

write:	write.c
	$(CC) $(CFLAGS) -o $(DESTDIR)write write.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

yes:	yes.c
	$(CC) $(CFLAGS) -o $(DESTDIR)yes yes.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

qfind:	qfind.c
	$(CC) $(CFLAGS) -o $(DESTDIR)qfind qfind.c
	su root chown bin $(DESTDIR)$@
	su root chgrp bin $(DESTDIR)$@
	su root chmod 511 $(DESTDIR)$@

as/as:
	./make.in as

awk/awk:
	./make.in awk

bc/bc:
	./make.in bc

check/check:
	./make.in check

compress/compress:
	./make.in compress

dc/dc:
	./make.in dc

diff/diff:
	./make.in diff

dos/dos:
	./make.in dos

dump/dump:
	./make.in dos

ed/ed:
	./make.in ed

find/find:
	./make.in find

grep/grep:
	./make.in grep

lex/lex:
	./make.in lex

mail/mail:
	./make.in mail

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
