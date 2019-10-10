#makefile by bob h. to build sources for /usr/lib on romana

DESTDIR=/interim/usr/lib/
DESTDIR2=/interim/usr/lib/mail/
DESTDIR3=/interim/usr/src/

CMDS: atrun units  \
      copyright crontab helpfile makeactions makemacros tmac.an tmac.s \
      libl.a lib.b libcurses.a libmisc.src \
      aliases fonts

all: $(CMDS) $(SCRIPTS) $(LIBRARIES)
      >all

fonts:
	su root cpdir -ade /newbits/usr/lib/roff /interim/usr/lib/roff

libmisc.src:
	cp -d misc/misc.tar.Z $(DESTDIR3)
	su root chmog 644 bin bin $(DESTDIR3)misc.tar.Z

atrun:
	cc -O -o $(DESTDIR)$@ atrun.c
	su root chmog 6511 root root $(DESTDIR)$@

libl.a:
	cd libl ; make

units:
	cc -o $(DESTDIR)$@ units.c
	su root chmog 511 bin bin $(DESTDIR)$@

copyright:
	co $(DESTDIR)$@
	su root chmog 444 bin bin $(DESTDIR)$@

crontab:
	co $(DESTDIR)$@
	su root chmog 644 bin bin $(DESTDIR)$@

helpfile:
	co $(DESTDIR)$@
	su root chmog 444 bin bin $(DESTDIR)$@

lib.b:
	co $(DESTDIR)$@
	su root chmog 444 bin bin $(DESTDIR)$@

libcurses.a:
	cd libcurses ; make

makeactions:
	co $(DESTDIR)$@
	su root chmog 444 bin bin $(DESTDIR)$@

makemacros:
	co $(DESTDIR)$@
	su root chmog 444 bin bin $(DESTDIR)$@

tmac.an:
	co $(DESTDIR)$@
	su root chmog 444 bin bin $(DESTDIR)$@

tmac.s:
	co $(DESTDIR)$@
	su root chmog 644 bin bin $(DESTDIR)$@

aliases:
	co $(DESTDIR2)$@
	su root chmog 644 bin bin $(DESTDIR2)$@
