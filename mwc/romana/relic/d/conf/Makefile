#Makefile to place proper sources in /conf/kbd and to
#build boot blocks
# Bob H. June 13, 1991

DESTDIR=/interim/conf/kbd/
DESTDIR2=/interim/conf/

POST=Coh_321.post

kb_tables=belgian canadian danish dutch french \
	   german italian latin_am norwegian \
	   portuguese spanish swedish swiss_french \
	   swiss_german uk us us_xt

kbmain.o: /newbits/kernel/USRSRC/i8086/drv/kbmain.c
	cc -c $< -o $(DESTDIR)$@

keyboards:
	(cd kbd/objects;export SRCPATH=..;make; mv $(kb_tables) $(DESTDIR))
	cp -d kbd/.list $(DESTDIR)

ldker:
	(co ldker ; mv ldker $(DESTDIR2)$@)
	su root chmog 500 root root $(DESTDIR2)$@

postfile:
	co $(POST) ; mv $(POST) $(DESTDIR2)
	chmog 500 root root $(DESTDIR2)$(POST)
	
