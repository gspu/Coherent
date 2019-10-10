
DESTDIR=/interim/usr/dict/

all: american.Z british.Z digrams trigrams local spellhist common Run

american.Z:
	co $(DESTDIR)american
	cd $(DESTDIR) ; compress american

british.Z:
	co $(DESTDIR)british 
	cd $(DESTDIR) ; compress british

digrams:
	co $(DESTDIR)$@

trigrams:
	co $(DESTDIR)$@

local:
	touch $(DESTDIR)$@

spellhist:
	su root /etc/mknod $(DESTDIR)$@ c 0 0
	su root chmog 666 bin bin $(DESTDIR)$@

common:
	co $(DESTDIR)$@
	cd $(DESTDIR) ; compress common	

Run:
	co $(DESTDIR)$@
	su root chmog 555 bin bin $(DESTDIR)$@
