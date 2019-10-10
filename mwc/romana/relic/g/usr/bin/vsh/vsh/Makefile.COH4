#       Makefile for vsh (COHERENT 4.0)
DESTDIR=/bin386/usr/bin/
WINLIB = ../winlib
WININC = ../winlib
#CC = gcc
CC = cc
CFLAGS = -c -O -I$(WININC)
LFLAGS = -n

#       Additional flags which may be defined in OPTIONS:
#
#       -DPOSDIR        - use POSIX functions to read directorys
#
#	-DAIX		- must be set if compiling for AIX
#
#	-DSYSV4		- must be set if compiling for System V.4
#
#	-DSCO32		- must be set if compiling for SCO/386 3.2
#
#       -DDEMO=910931   - compiles a demo version which only runs until
#			  September, 31th 1991
#
#       -DONLY_THIS_NODE=\"lah\"
#			- compiles a version which only runs on a system
#			  where uname is "ibm"
#
#	-DCOH_SERIAL_NO=\"210\"
#			- compiles a version which only runs under COHERENT
#			  with the serial number 123
#
#       -DMAXUSE=4
#			- compiles a version where a maximum of 4 programs
#			  may run at the same time
#
#	-DSHAREWARE	- compiles a shareware version

OPTIONS = -DPOSDIR

#       defines for main menu characters and format of dates

# English:
CMDSE = -DM_FILE=\'F\' -DM_DIRECTORY=\'D\' -DM_OPTIONS=\'O\' \
	-DM_INSTALL=\'I\' -DM_COMMAND=\'C\' -DM_REFRESH=\'R\' \
	-DM_EXIT=\'E\' -DM_HELP=\'H\' -DUSDATE

# German:
CMDSD = -DM_FILE=\'D\' -DM_DIRECTORY=\'V\' -DM_OPTIONS=\'O\' \
	-DM_INSTALL=\'I\' -DM_COMMAND=\'B\' -DM_REFRESH=\'R\' \
	-DM_EXIT=\'E\' -DM_HELP=\'H\'

LIB = $(WINLIB)/winlib.a -lcurses
OBJ = vshfun.o vshmisc.o vshhlp.o vshsec.o vshsys.o match.o

all: $(WINLIB)/winlib.a vshe vshd tags
	@echo "all done"

# ----- English version -----

vshe: vshe.o $(OBJ) vshetxt.o $(WINLIB)/winlib.a
	$(CC) $(LFLAGS) -o $(DESTDIR)vsh vshe.o $(OBJ) vshetxt.o $(LIB)

vshe.o: vsh.c vsh.h $(WININC)/winfun.h
	$(CC) $(CFLAGS) $(OPTIONS) $(CMDSE) -c vsh.c
	mv vsh.o vshe.o

vshetxt.o: vshetxt.c $(WININC)/winfun.h
	$(CC) $(CFLAGS) $(OPTIONS) -c vshetxt.c

# ----- German version -----

vshd: vshd.o $(OBJ) vshdtxt.o $(WINLIB)/winlib.a
	$(CC) $(LFLAGS) -o vshd vshd.o $(OBJ) vshdtxt.o $(LIB)

vshd.o: vsh.c vsh.h $(WININC)/winfun.h
	$(CC) $(CFLAGS) $(OPTIONS) $(CMDSD) -c vsh.c
	mv vsh.o vshd.o

vshdtxt.o: vshdtxt.c $(WININC)/winfun.h
	$(CC) $(CFLAGS) $(OPTIONS) -c vshdtxt.c

# ----- language independent modules -----

vshfun.o: vshfun.c vsh.h $(WININC)/winfun.h
	$(CC) $(CFLAGS) $(OPTIONS) -c vshfun.c

vshmisc.o: vshmisc.c vsh.h $(WININC)/winfun.h
	$(CC) $(CFLAGS) $(OPTIONS) -c vshmisc.c

vshhlp.o: vshhlp.c $(WININC)/winfun.h
	$(CC) $(CFLAGS) -c vshhlp.c

vshsec.o: vshsec.c
	$(CC) $(CFLAGS) $(OPTIONS) -c vshsec.c

vshsys.o: vshsys.c vsh.h
	$(CC) $(CFLAGS) $(OPTIONS) -c vshsys.c

match.o: match.c
	$(CC) $(CFLAGS) -c match.c

# ----- make winlib -----

$(WINLIB)/winlib.a:
	cd $(WINLIB); make -f Makefile.COH4

# ----- administration -----

tags:
	ctags -r *.[hc] ../winlib/*.[hc]

clean:
	rm -f *.o vsh[ed] core tags refs
	cd $(WINLIB); make -f Makefile.COH4 clean
