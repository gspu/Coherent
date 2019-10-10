#*********************************************************************
#                         COPYRIGHT NOTICE                           *
#*********************************************************************
#        This software is copyright (C) 1982 by Pavel Curtis         *
#                                                                    *
#        Permission is granted to reproduce and distribute           *
#        this file by any means so long as no fee is charged         *
#        above a nominal handling fee and so long as this            *
#        notice is always included in the copies.                    *
#                                                                    *
#        Other rights are reserved except as explicitly granted      *
#        by written permission of the author.                        *
#                Pavel Curtis                                        *
#                Computer Science Dept.                              *
#                405 Upson Hall                                      *
#                Cornell University                                  *
#                Ithaca, NY 14853                                    *
#                                                                    *
#                Ph- (607) 256-4934                                  *
#                                                                    *
#                Pavel.Cornell@Udel-Relay   (ARPAnet)                *
#                decvax!cornell!pavel       (UUCPnet)                *
#********************************************************************/

# RCSid: $Header: /src386/usr/lib/ncurses/RCS/Makefile,v 1.7 93/04/12 14:07:28 bin Exp Locker: bin $
DESTDIR=/bin386/usr/lib/
DESTDIR2=/bin386/usr/bin/

SRCDIR= /usr/lib/terminfo
LIB=/usr/lib
INCL=/usr/include

#
# Additional flags which could be defined in CFLAGS:
#	TRACE	compile library with trace function
#	RCSHDR	compile the RCS headers into the functions
#
CFLAGS= -O -DSRCDIR='"$(SRCDIR)"'
#CFLAGS= -O -DSRCDIR='"$(SRCDIR)"' -VNDP

SRCS=	comp_main.c comp_hash.c comp_captab.c comp_scan.c comp_error.c \
	comp_parse.c read_entry.c \
	dump.c dump_names.c  \
	captoinfo.c getcap.c putinfo.c \
	lib_setup.c lib_fixterm.c lib_tparm.c lib_tputs.c \
	lib_unctrl.c lib_raw.c lib_vidattr.c lib_trace.c \
	lib_beep.c lib_doupdate.c lib_refresh.c lib_initscr.c lib_newwin.c \
	lib_addch.c lib_addstr.c lib_scroll.c lib_clreol.c lib_touchwin.c \
	lib_mvcur.c lib_delwin.c lib_endwin.c lib_clrbot.c lib_move.c \
	lib_printw.c lib_scanw.c lib_erase.c lib_getch.c lib_options.c \
	lib_box.c lib_clear.c lib_delch.c lib_deleteln.c lib_insch.c \
	lib_insertln.c lib_getstr.c lib_mvwin.c lib_longname.c lib_tstp.c \
	lib_newterm.c lib_set_term.c lib_overlay.c lib_scrreg.c lib_setacs.c \
	lib_napms.c lib_color.c lib_tget.c

#
# Normal, optimised object files
#
LIBOBJS= lib_setup.o lib_fixterm.o lib_tparm.o read_entry.o lib_tputs.o \
	lib_unctrl.o lib_raw.o lib_vidattr.o lib_trace.o lib_beep.o \
	lib_doupdate.o lib_refresh.o lib_initscr.o lib_newwin.o lib_addch.o \
	lib_addstr.o lib_scroll.o lib_clreol.o lib_touchwin.o lib_mvcur.o \
	lib_delwin.o lib_endwin.o lib_clrbot.o lib_move.o lib_printw.o \
	lib_scanw.o lib_erase.o lib_getch.o lib_options.o \
	lib_box.o lib_clear.o lib_delch.o lib_deleteln.o lib_insch.o \
	lib_insertln.o lib_getstr.o lib_mvwin.o lib_longname.o lib_tstp.o \
	lib_newterm.o lib_set_term.o lib_overlay.o lib_scrreg.o lib_setacs.o \
	lib_napms.o lib_color.o lib_tget.o

COMPOBJS= comp_main.o comp_hash.o comp_captab.o comp_scan.o comp_error.o \
	comp_parse.o read_entry.o

CTOIOBJS= captoinfo.o getcap.o putinfo.o

all:	lib tic infocmp captoinfo
	@echo "all done"

install: all
	su root installer

lib:	libcurses.a	
	@echo "lib done"

libcurses.a:	$(LIBOBJS)
	rm -f $(DESTDIR)libcurses.a
	ar rcs $(DESTDIR)libcurses.a $(LIBOBJS)

term.h: Caps MKterm.h.awk
	awk -f MKterm.h.awk Caps > term.h

tic: $(COMPOBJS)
	cc -s -o $(DESTDIR2)tic $(COMPOBJS)

comp_captab.c: Caps MKcaptab.awk
	awk -f MKcaptab.awk Caps > comp_captab.c

comp_captab.o: comp_captab.c

captoinfo:	$(CTOIOBJS) libcurses.a
		cc -s $(CFLAGS) $(CTOIOBJS) -o $(DESTDIR2)captoinfo $(DESTDIR)libcurses.a

putinfo.c: Caps infotail.src MKinfo.awk
	awk -f MKinfo.awk Caps
	cat boolnames numnames strnames infotail.src > putinfo.c
	-rm -f boolnames numnames strnames

captoinfo.o:	captoinfo.c

getcap.o:	getcap.c

putinfo.o:	putinfo.c

infocmp: dump.o dump_names.o read_entry.o
	cc -s -o $(DESTDIR2)infocmp dump.o dump_names.o read_entry.o

dump.o: dump.c term.h compiler.h

dump_names.o: dump_names.c

dump_names.c: Caps MKnames.awk
	awk -f MKnames.awk Caps
	cat boolnames numnames strnames > dump_names.c
	-rm -f boolnames numnames strnames

lint: $(SRCS)
	lint $(CFLAGS) $(SRCS) > lint.out

clean:
	-rm -f *.[od] term.h comp_captab.c dump_names.c putinfo.c tags \
	tic infocmp captoinfo libcurses.a lint.out

tags:	*.c *.h
	ctags *.c *.h

save : $(SRCS) Makefile
	tar cf - $< | compress -c > /tmp/ncurses.tar.Z
	uucp -r /tmp/ncurses.tar.Z mwc!/v/cef

list : $(SRCS) Makefile
	ph $?
	touch list

depend:	$(SRCS)
	grep "^\#include" $< \
		| sed 's/\([^:]*\).c:[^"]*"\([^"]*\)".*/\1.o: \2/' \
		| sed '/\#include/d' > makedep
	echo '/^\# DO NOT DELETE THIS LINE/+2,$$d' > eddep
	echo '$$r makedep' >> eddep
#	echo '/^\# DO NOT DELETE THIS LINE/+2,$$s/\.o/.d/' >> eddep
#	echo '$$r makedep' >> eddep
	echo 'w' >> eddep
	cp Makefile Makefile.bak
	ed Makefile < eddep
	rm makedep eddep
	echo >> Makefile
	echo "\# DEPENDENCIES MUST END AT END OF FILE" >> Makefile
	echo "\# IF YOU PUT STUFF HERE, IT WILL GO AWAY" >> Makefile
	echo "\# See make depend, above" >> Makefile

# DO NOT DELETE THIS LINE - make depend needs it

comp_main.o: compiler.h
comp_hash.o: compiler.h
comp_hash.o: term.h
comp_captab.o: compiler.h
comp_captab.o: term.h
comp_scan.o: compiler.h
comp_error.o: compiler.h
comp_parse.o: compiler.h
comp_parse.o: term.h
comp_parse.o: object.h
read_entry.o: term.h
read_entry.o: object.h
dump.o: compiler.h
dump.o: term.h
lib_setup.o: curses.h
lib_setup.o: curses.priv.h
lib_setup.o: term.h
lib_fixterm.o: curses.h
lib_fixterm.o: curses.priv.h
lib_fixterm.o: term.h
lib_tparm.o: curses.h
lib_tparm.o: curses.priv.h
lib_tparm.o: term.h
lib_tputs.o: curses.h
lib_tputs.o: curses.priv.h
lib_tputs.o: term.h
lib_raw.o: curses.h
lib_raw.o: curses.priv.h
lib_raw.o: term.h
lib_vidattr.o: curses.h
lib_vidattr.o: curses.priv.h
lib_vidattr.o: term.h
lib_trace.o: term.h
lib_trace.o: curses.h
lib_trace.o: curses.priv.h
lib_beep.o: curses.h
lib_beep.o: curses.priv.h
lib_beep.o: term.h
lib_doupdate.o: curses.h
lib_doupdate.o: curses.priv.h
lib_doupdate.o: term.h
lib_refresh.o: curses.h
lib_refresh.o: curses.priv.h
lib_initscr.o: curses.h
lib_initscr.o: curses.priv.h
lib_newwin.o: term.h
lib_newwin.o: curses.h
lib_newwin.o: curses.priv.h
lib_addch.o: curses.h
lib_addch.o: curses.priv.h
lib_addch.o: terminfo.h
lib_addch.o: unctrl.h
lib_addstr.o: curses.h
lib_addstr.o: curses.priv.h
lib_scroll.o: curses.h
lib_scroll.o: curses.priv.h
lib_clreol.o: curses.h
lib_clreol.o: curses.priv.h
lib_touchwin.o: curses.h
lib_touchwin.o: curses.priv.h
lib_mvcur.o: term.h
lib_mvcur.o: curses.h
lib_mvcur.o: curses.priv.h
lib_delwin.o: curses.h
lib_delwin.o: curses.priv.h
lib_endwin.o: term.h
lib_endwin.o: curses.h
lib_endwin.o: curses.priv.h
lib_clrbot.o: curses.h
lib_clrbot.o: curses.priv.h
lib_move.o: curses.h
lib_move.o: curses.priv.h
lib_printw.o: curses.h
lib_printw.o: curses.priv.h
lib_scanw.o: curses.h
lib_scanw.o: curses.priv.h
lib_erase.o: curses.h
lib_erase.o: curses.priv.h
lib_getch.o: curses.h
lib_getch.o: curses.priv.h
lib_options.o: term.h
lib_options.o: curses.h
lib_options.o: curses.priv.h
lib_box.o: curses.h
lib_box.o: curses.priv.h
lib_clear.o: curses.h
lib_clear.o: curses.priv.h
lib_delch.o: curses.h
lib_delch.o: curses.priv.h
lib_delch.o: term.h
lib_deleteln.o: curses.h
lib_deleteln.o: curses.priv.h
lib_insch.o: curses.h
lib_insch.o: curses.priv.h
lib_insertln.o: curses.h
lib_insertln.o: curses.priv.h
lib_getstr.o: curses.h
lib_getstr.o: curses.priv.h
lib_getstr.o: unctrl.h
lib_mvwin.o: curses.h
lib_mvwin.o: curses.priv.h
lib_longname.o: curses.h
lib_longname.o: curses.priv.h
lib_tstp.o: term.h
lib_tstp.o: curses.h
lib_tstp.o: curses.priv.h
lib_newterm.o: curses.h
lib_newterm.o: term.h
lib_newterm.o: curses.priv.h
lib_set_term.o: curses.h
lib_set_term.o: curses.priv.h
lib_set_term.o: term.h
lib_overlay.o: curses.h
lib_overlay.o: curses.priv.h
lib_scrreg.o: curses.h
lib_scrreg.o: curses.priv.h
lib_setacs.o: curses.h
lib_setacs.o: term.h
lib_color.o: curses.h
lib_color.o: curses.priv.h
lib_color.o: term.h

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE, IT WILL GO AWAY
# See make depend, above
