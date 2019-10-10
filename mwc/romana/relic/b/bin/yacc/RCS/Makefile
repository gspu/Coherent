head     1.4;
branch   ;
access   ;
symbols  ;
locks    bin:1.4;
comment  @# @;


1.4
date     93.07.08.12.45.18;  author bin;  state Exp;
branches ;
next     1.3;

1.3
date     93.01.26.12.43.59;  author bin;  state Exp;
branches ;
next     1.2;

1.2
date     92.05.29.13.24.15;  author bin;  state Exp;
branches ;
next     1.1;

1.1
date     92.04.07.08.26.11;  author bin;  state Exp;
branches ;
next     ;


desc
@Makefile for yacc (386 specific) by cef
@


1.4
log
@*** empty log message ***
@
text
@# /usr/src/cmd/yacc/Makefile 2/20/91
# For i8086: LFLAGS=-n.

DESTDIR=/bin386/bin/
CFLAGS=-O
LDFLAGS=

O= y0.o y1.o y2.o y3.o y4.o y5.o y6.o

yacc:	$O
	$(CC) $(LDFLAGS) -so $(DESTDIR)$@@ $O

install: yacc
	su root `srcpath installer`

y0.o y1.o y4.o:	yacc.h
y3.o:		yacc.h assert.h
y2.o y6.o:	yacc.h action.h
y5.o:		yacc.h assert.h action.h

# end of /usr/src/cmd/yacc/Makefile
@


1.3
log
@*** empty log message ***
@
text
@d4 1
a4 1
DESTDIR=
@


1.2
log
@bob h added destdir and modfied include file dependencies
@
text
@d4 1
a4 1
DESTDIR=/bin386/bin/
d17 3
a19 3
y3.o:		yacc.h
y2.o y6.o:	yacc.h
y5.o:		yacc.h
@


1.1
log
@Initial revision
@
text
@d4 1
a4 1
DESTDIR=
d17 3
a19 3
y3.o:		yacc.h assert.h
y2.o y6.o:	yacc.h action.h
y5.o:		yacc.h assert.h action.h
@
