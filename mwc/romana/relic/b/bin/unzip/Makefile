#==============================================================================
# Makefile for UnZip, ZipInfo & FUnZip:  Unix, OS/2, MS-DOS ("real" makes only)
# Version:  5.0 (inflate,explode)                                20 August 1992
#==============================================================================
#
#
# INSTRUCTIONS (such as they are):
#
# "make vax"	-- makes UnZip on a VAX 11-780 BSD 4.3 in current directory
#		   (or a SysV VAX, or an 8600 running Ultrix, or...)
# "make"	-- uses environment variable SYSTEM to set the type
#		   system to compile for.  This doesn't work for some
#		   particularly brain-damaged versions of make (VAX BSD,
#		   Gould, and SCO Unix are in this group).  If SYSTEM not
#		   set, gives instructions on what to try instead.
# "make list"	-- lists all supported systems (targets), including related
#		   utilities' targets
# "make wombat" -- Chokes and dies if you haven't added the specifics
#		   for your Wombat 68000 (or whatever) to the systems list.
#
# CF are flags for the C compiler.  LF are flags for the loader.  LF2 are
# more flags for the loader, if they need to be at the end of the line
# instead of at the beginning (for example, some libraries).  LOCAL_UNZIP
# is an environment variable that can be used to add default C flags to
# your compile without editing the Makefile (e.g., -DDEBUG_STRUC, or -FPi87
# on PCs).
#
# My host (a VAX 11-780 running BSD 4.3) is hereafter referred to as "my host."
#
# My host's /usr/include/sys/param.h defines BSD for me.  You may have to add
# "-DBSD" to the list of CF for your system.
#
# Some versions of make do not define the macro "$(MAKE)" (my host did not).
# The makefile should now handle such systems correctly, more or less; the
# possible exception to this is if you've used a make command-line option
# (for example, the one which displays the commands which WOULD be executed,
# but doesn't actually execute them).  It probably needs some more tinkering.
# If things still don't work, use "make" instead of "$(MAKE)" in your system's
# makerule.  Or try adding the following line to your .login file:
#   setenv MAKE "make"
# (It didn't help on my host.)
#
# Memcpy and memset are provided for those systems that don't have them;
# they're found in misc.c and will be used if -DZMEM is included in the list
# of CF.  These days *almost* all systems have them (they're mandated by
# ANSI), but older systems might be lacking.  And at least one machine's
# version results in some serious performance degradation...
#
# Be sure to test your nice new UnZip; successful compilation does not always
# imply a working program.


#####################
# MACRO DEFINITIONS #
#####################

# Defaults most systems use (use LOCAL_UNZIP in environment to add flags, 
# such as -DNOMEMCPY).

CRYPTF =
CRYPTO =
# Uncomment next two lines for decryption version:
CRYPTF = -DCRYPT
CRYPTO = crypt$O

# UnZip flags
CC = cc#	try using "gcc" target rather than changing this (if you do,
LD = cc#	you MUST change LD, too--else "unresolved symbol:  ___main")
LOC = $(LOCAL_UNZIP) $(CRYPTF)
CF = -O $(LOC)
LF = -o unzip
LF2 = -s

# ZipInfo flags
ZC = -DZIPINFO
ZL = -o zipinfo
ZL2 = -s

# FUnZip flags
FC = # not used
FL = -o funzip
FL2 = -s

# general-purpose stuff
LN = rm -f misc_.c; ln
RM = rm -f
E =
O = .o
SHELL = /bin/sh
INSTALL = cp#	 	   probably can change this to 'install' if you have it
BINDIR = /usr/local/bin#   target directory - where to install executables

# object files
OBJS1 = unzip$O $(CRYPTO) envargs$O explode$O extract$O file_io$O inflate$O
OBJS2 = mapname$O match$O misc$O unreduce$O unshrink$O
OBJS = $(OBJS1) $(OBJS2)
LOBJS = $(OBJS)
OS2_OBJS = $(OBJS:.o=.obj) os2unzip.obj
OBJZ = zipinfo$O envargs$O match$O misc_$O
OS2_OBJZ = $(OBJZ:.o=.obj) os2zinfo.obj
OBJF = funzip$O $(CRYPTO) inflate$O
OS2_OBJF = # not yet supported
UNZIPS = unzip$E # zipinfo$E funzip$E	# zipinfo, funzip not fully supported
#					#  yet (next release)

# list of supported systems/targets in this version
SYSTEMS1 = 386i 3Bx 7300 amdahl apollo aviion bsd bull c120 c210 coherent
SYSTEMS2 = convex cray cray_cc cray_v3 cyber_sgi dec dnix encore eta
SYSTEMS3 = gcc gcc_dos generic generic2 gould hk68 hp indigo linux
SYSTEMS4 = minix mips msc_dos next osf1 p_iris pyramid rs6000 rtaix
SYSTEMS5 = sco sco_dos sco_x286 sequent sgi stellar sun sysv sysv6300
SYSTEMS6 = tahoe ultrix vax wombat xos

SYS_UTIL1 = zi_dos zi_gcc zi_indigo zipinfo fu_gcc funzip
# SYS_UTIL2 = ship ship_dos ship_sysv


####################
# DEFAULT HANDLING #
####################

# The below will try to use your shell variable "SYSTEM" as the type system
# to use (e.g., if you type "make" with no parameters at the command line).
# The test for $(MAKE) is necessary for VAX BSD make (and Gould, apparently),
# as is the "goober" (else stupid makes see an "else ;" statement, which they
# don't like).  "goober" must then be made into a valid target for machines
# which DO define MAKE properly (and have SYSTEM set).  Quel kludge, non?
# And to top it all off, it appears that the VAX, at least, can't pick SYSTEM
# out of the environment either (which, I suppose, should not be surprising).
# [Btw, if the empty "goober" target causes someone else's make to barf, just
# add an "@echo > /dev/null" command (or whatever).  Works OK on the Amdahl
# and Crays, though.]

default:
	@if test -z "$(MAKE)"; then\
		if test -z "$(SYSTEM)";\
		then make help;\
		else make $(SYSTEM) MAKE="make";\
		fi;\
	else\
		if test -z "$(SYSTEM)";\
		then $(MAKE) help;\
		else $(MAKE) $(SYSTEM) goober;\
		fi;\
	fi

goober:

help:
	@echo
	@echo\
 "  If you're not sure about the characteristics of your system, try typing"
	@echo\
 '  "make generic".  If the compiler barfs and says something unpleasant about'
	@echo\
 '  "timezone redefined," try typing "make clean" followed by "make generic2".'
	@echo\
 '  One of these actions should produce a working copy of unzip on most Unix'
	@echo\
 '  systems.  If you know a bit more about the machine on which you work, you'
	@echo\
 '  might try "make list" for a list of the specific systems supported herein.'
	@echo\
 '  And as a last resort, feel free to read the numerous comments within the'
	@echo\
 '  Makefile itself.  Note that to compile the decryption version of UnZip,'
	@echo\
 '  you must obtain crypt.c separately, in addition to uncommenting two lines'
	@echo\
 '  in Makefile (see the main Contents file for ftp and mail-server sites).'
	@echo\
 '  Have an excruciatingly pleasant day.'
	@echo

list:
	@echo
	@echo\
 'Type "make <system>", where <system> is one of the following:'
	@echo
	@echo  "	$(SYSTEMS1)"
	@echo  "	$(SYSTEMS2)"
	@echo  "	$(SYSTEMS3)"
	@echo  "	$(SYSTEMS4)"
	@echo  "	$(SYSTEMS5)"
	@echo  "	$(SYSTEMS6)"
	@echo
	@echo\
 'Otherwise set the shell variable SYSTEM to one of these and just type "make".'
	@echo\
 'Targets for related utilities (ZipInfo) include:'
	@echo
	@echo  "	$(SYS_UTIL1)"
#	@echo  "	$(SYS_UTIL2)"
	@echo
	@echo\
 'For further (very useful) information, please read the comments in Makefile.'
	@echo


###############################################
# BASIC COMPILE INSTRUCTIONS AND DEPENDENCIES #
###############################################

.c.o:
	$(CC) -c $(CF) $*.c

unzips:		$(UNZIPS)

unzip$E:	$(OBJS)
	$(LD) $(LF) $(LOBJS) $(LF2)

crypt$O:        crypt.c unzip.h zip.h	# may or may not be in distribution
envargs$O:      envargs.c unzip.h
explode$O:      explode.c unzip.h
extract$O:      extract.c unzip.h
file_io$O:      file_io.c unzip.h
funzip$O:       funzip.c unzip.h
inflate$O:      inflate.c unzip.h
mapname$O:      mapname.c unzip.h
match$O:        match.c unzip.h
misc$O:         misc.c unzip.h
os2unzip$O:     os2unzip.c unzip.h	# for OS/2 only
os2zinfo$O:     os2unzip.c unzip.h	# for OS/2 only
unreduce$O:     unreduce.c unzip.h
unshrink$O:     unshrink.c unzip.h
unzip$O:        unzip.c unzip.h

all:	generic_msg generic zipinfo

generic_msg:
	@echo
	@echo\
 '  Attempting "make generic" and "make zipinfo" now.  If this fails for some'
	@echo\
 '  reason, type "make help" and/or "make list" for suggestions.'
	@echo

install:	$(UNZIPS)
	$(INSTALL) $(UNZIPS) $(BINDIR)

clean:
	rm -f $(OBJS) unzip$E $(OBJZ) zipinfo$E


################################
# INDIVIDUAL MACHINE MAKERULES #
################################

# these are the makerules for various systems
# TABS ARE REQUIRED FOR MANY VERSIONS OF "MAKE"!


# ---------------------------------------------------------------------------
#   Generic targets (can't assume make utility groks "$(MAKE)")
# ---------------------------------------------------------------------------

generic:	unzip	# first try if unknown

generic2:		# second try if unknown:  hope make is called "make"...
	make unzip CF="$(CF) -DBSD"

# ---------------------------------------------------------------------------
#   "Normal" group (both big- and little-endian, structure-padding or not):
# ---------------------------------------------------------------------------

386i:		unzip	# sun386i, SunOS 4.0.2
3Bx:		unzip	# AT&T 3B2/1000-80; should work on any WE32XXX machine
7300:		unzip	# AT&T 7300 (M68000/SysV)
apollo:		unzip	# Apollo Domain/OS machines
bull:		unzip	# Bull DPX/2, BOS 2.00.45 (doesn't require -Xk switch)
coherent:	unzip	# Coherent 3.10, Mark Williams C
cray_cc:	unzip	# Cray-2 and Y-MP, using default (possibly old) compiler
dec:		unzip	# DEC 5820 (MIPS RISC), test version of Ultrix v4.0
encore:		unzip	# Multimax
eta:		unzip	# ETA-10P*, hybrid SysV with BSD 4.3 enhancements
gould:		unzip	# Gould PN9000 running UTX/32 2.1Bu01
hp:		unzip	# HP 9000 series (68020), 4.3BSD or HP-UX A.B3.10 Ver D
hp_ux:		unzip	# (to match zip's makefile entry)
mips:		unzip	# MIPS M120-5(?), SysV.3 [error in sys/param.h file?]
pyramid:	unzip	# Pyramid 90X, prob. all, under >= OSx4.1, BSD universe
rtaix:		unzip	# IBM RT 6150 under AIX 2.2.1
sco:		unzip	# Xenix/386 (tested on 2.3.1); SCO Unix 3.2.0.
stellar:	unzip	# gs-2000
sun:		unzip	# Sun 3, 4; SunOS 4.x (SOME SYSTEMS ARE SYSTEM V!)
tahoe:		unzip	# tahoe (CCI Power6/32), 4.3BSD
ultrix:		unzip	# VAXen, DEC 58x0 (MIPS guts), DECstation 2100; v4.x
vax:		unzip	# general-purpose VAX target (not counting VMS)

# ---------------------------------------------------------------------------
#   BSD group (for timezone structs [struct timeb]):
# ---------------------------------------------------------------------------

bsd:		_bsd	# generic BSD (BSD 4.2 & Ultrix handled in unzip.h)

_bsd:
	$(MAKE) unzip CF="$(CF) -DBSD"

# ---------------------------------------------------------------------------
#   SysV group (for extern long timezone and ioctl.h instead of sgtty.h):
# ---------------------------------------------------------------------------

amdahl:		_sysv	# Amdahl (IBM) mainframe, UTS (SysV) 1.2.4 and 2.0.1
aviion:         _sysv	# Data General AViiONs, DG/UX 4.3x
sgi:		_sysv	# Silicon Graphics Iris 4D, Irix SysV rel. 3.3.2
sysv:		_sysv	# generic System V Unix
xos:		_sysv	# Olivetti LSX-3005..3045, X/OS 2.3 and 2.4

_sysv:
	$(MAKE) unzip CF="$(CF) -DSYSV -DTERMIO"

# ---------------------------------------------------------------------------
#   "Unique" group (require non-standard options):
# ---------------------------------------------------------------------------

# Apparently the C-120 has an optimization bug, and possibly another
# bug in the (SysV?) time routines which adds 11 years to the date.
# -DCONVEX not needed?  [RZM:  The remark above the C-120 entry 
# about a bug may not be true.  I think it is rather time procedures
# uncompatibility between unixes.] [GRR:  So is -O2 ok for c120?]
#
c120:			# Convex C-120, OS 9.0, with non-vectorizing cc 4.0
	$(MAKE) unzip CF="-O1 $(LOC) -Dunix -DBSD"

c210:			# Convex C-210, OS 9.0, cc 4.0
	$(MAKE) unzip CF="-O2 $(LOC) -Dunix -DBSD"

# Enclosed you'll find a context diff for the unzip41 makefile
# which enhances compilation on a convex.  The previous version
# probably worked great a couple of years ago, and would still do
# so if one compiles in our "backward compatible" pcc mode.   The
# following allows it to work better in a modern convexian environment.
# [This target results in the following error on various Convex's, 
# however:  "cc: Error on line 79 of file_io.c: 'ioctl' redeclared:
# incompatible types."]
#
convex:			# previous target was tested on C200/C400
	$(MAKE) unzip CF="$(CF) -Dunix -DCONVEX -ext" LF="$(LF) -ext"

# Cray-2 and Y-MP, running Unicos 5.1 to 6.1 (SysV + BSD enhancements)
# and Standard (ANSI) C compiler 1.5, 2.0 or 3.0.
cray:
	$(MAKE) unzip CC="scc" LD="scc"

# Ditto, for Cray Standard C 3.0 or later.
cray_v3:
	$(MAKE) unzip CC="scc" LD="scc" CF="$(CF) -h scalar3 -h vector3"

# The unzip41 build on a Cyber 910/SGI running Irix v3.3.3 was successful
# with the following change to Makefile:
cyber_sgi:
	$(MAKE) unzip CF="$(CF) -I/usr/include/bsd"\
	 LF="-lbsd $(LF)"

# The DIAB dnix 5.3 compiler does not define __STDC__ but understands
# prototypes, void, etc., anyway.  It also does not provide any predefined
# macros to detect this (aside from "unix" and the four file, line, time
# and date macros).  Thus we must define MODERN and PROTO by hand.
#
dnix:		# 680X0, DIAB dnix 5.2/5.3 (a Swedish System V clone)
	$(MAKE) unzip CF="$(CF) -DPROTO -DMODERN"

# Generic BSDish Unix gcc.  ``The -O2 only works with the latest version of
# gcc; you may have to use -O only for earlier versions.  I have no idea why
# -s causes this bug in gcc.''  [Bug:  "nm: unzip: no name list", "collect:
# /usr/bin/nm returned 1 exit status".]  If you don't have strip, don't
# worry about it (it just makes the executable smaller).
#
gcc:
	$(MAKE) unzip CC=gcc LD=gcc CF="-O2 $(LOC)" LF2=""
	strip unzip

# MS-DOS with D.J. Delorie's djgcc 1.06.  Note that go32 doesn't support
# dos function 0x38 (yet); to fix, append to line 400 of exphdlr.c (go32)
# the following:  "case 0x38:".
#
gcc_dos:	# may need to add -Uunix to CF
	$(MAKE) unzip CC=gcc LD=gcc CF="-O2 -Wall $(LOC)"\
	 LF="-s" LF2="-o unzip"
	aout2exe unzip

# Heurikon HK68 (68010), UniPlus+ System V 5.0, Green Hills C-68000
hk68:
	$(MAKE) unzip CC="gcc" LD="gcc" LF="-n $(LF)" \
	CF="-ga -X138 $(LOC) -Dlocaltime=localti -Dtimezone=timezon"

# Rules needed to build the unzip program for an Iris Indigo running
# Irix Version 4.0.1
indigo:
	$(MAKE) unzip CF="-cckr $(CF) -DTERMIO"

# Linux is almost sysv but not quite
linux:                # Linux pre-0.96 with gcc 2.1
	$(MAKE) unzip CF="$(CF) -DTERMIO -DLINUX" CC=gcc LD=gcc

# Minix 1.5 PC for the 386 with gcc or bcc
minix:
	$(MAKE) unzip CC=gcc CF="$(CF) -DMINIX"

# PCs (IBM-type), running MS-DOS, Microsoft C 6.0 and NMAKE.  Can't use
# SYSTEM environment variable:  "default" target is > 200 characters.
# "nmake msc_dos" works fine, aside from (possibly) an irrelevant message
# about the creation of a temporary file.  Environment variable LOCAL_UNZIP
# should be set via "SET LOCAL_UNZIP=-FPi87" if you use the 80x87 library;
# also add -G2 or -G3 if using a 286/386/486 system.
#
#msc_dos:
#	$(MAKE) unzip.exe\
#	 CF="-Ox $(LOC) -nologo -G2" CC=cl LD=link E=.exe O=.obj\
#	 LF="/noi/nol" LF2=",unzip;"

msc_dos:	rsp
	$(MAKE) unzip.exe CF="-Oait -Gs $(LOC) -nologo" CC=cl LD=link E=.exe\
	 O=.obj LOBJS="" LF="@rsp" LF2=""
	del rsp

rsp:
	echo $(OBJS1:.o=.obj)+ > rsp
	echo $(OBJS2:.o=.obj)/noi/e/st:0x1000; >> rsp

# $(LOCAL_UNZIP):  math libraries and/or any other personal or debugging
#                  definitions:  e.g., SET LOCAL_UNZIP=-FPi87 -DDEBUG_STRUC
# $(NOD):  intended to be used as   SET NOD=-link /nod:slibcep   to allow the
#          use of default library names (slibce.lib) instead of protected-mode
#          names (slibcep.lib), but it fails:  MSC adds its own /nod qualifier,
#          and there seems to be no way to override this.  Typical...
#
msc_os2:		# 16-bit OS/2 (1.x) with MSC 6.00 (use makefile.os2)
	$(MAKE) -nologo unzip.exe zipinfo.exe CC=cl LD=cl E=.exe O=.obj\
	 OBJS="$(OS2_OBJS)" OBJZ="$(OS2_OBJZ)"\
	 CF="-nologo -AC -Ocegit -G2s -DOS2 -DMSC $(LOC)"\
	 LF="-nologo -AC $(LOC) -Lp -F 2000"\
	 LF2="unzip.def -o unzip.exe $(NOD)" LN="copy" RM="del"\
	 ZL="-nologo -AC $(LOC) -Lp -Fb" ZL2="zipinfo.def -o zipinfo.exe"

# NeXT 2.x: make the executable smaller.
next:			# 68030 BSD 4.3+Mach
	$(MAKE) unzip LF2="-object -s"

# Rules to build the unzip program on a DecStation running DEC OSF/1 V1.0.
# This machine hasn't got ftime(3) in the standard C library.
osf1:
	$(MAKE) unzip LF2="-lbsd"

# I successfully compiled and tested the unzip program (v30) for the
# Silicon Graphics environment (Personal Iris 4D20/G with IRIX v3.2.2)
p_iris:
	$(MAKE) unzip CF="$(CF) -I/usr/include/bsd -DBSD"\
	 LF="-lbsd $(LF)"

# I have finished porting unzip 3.0 to the Pyramid 90X under OSX4.1.
# The biggest problem was the default structure alignment yielding two
# extra bytes.  The compiler has the -q option to pack structures, and
# this was all that was needed.  To avoid needing ZMEMS we could compile
# in the AT&T universe, but it runs more slowly!
#
#UnZip 5.0f:  moved to regular targets as test
#pyramid:	# Pyramid 90X, probably all, under >= OSx4.1, BSD universe
#	make unzip CF="$(CF) -q"

# IBM RS/6000 under AIX 3.2
rs6000:
	$(MAKE) unzip CF="$(CF) -DBSD -D_BSD -DUNIX" LF="-lbsd $(LF)"

# SCO cross compile from unix to DOS. Tested with Xenix/386 and OpenDeskTop.
# Should work with xenix/286 as well. (davidsen)  Note that you *must* remove
# the unix objects and executable before doing this!  (Piet Plomp:  gcc won't
# recognize the -M0 flag which forces 8086 code.)
#
sco_dos:	# uncomment zipinfo in UNZIPS if desired
	$(MAKE) unzips CF="-O $(LOC) -DNO_ERRNO -dos -M0" LF="-dos -F 2000"\
	 LF2="-o unzip.exe" ZL="-dos" ZL2="-o zipinfo.exe"

# SCO Xenix/286 2.2.1
sco_x286:
	$(MAKE) unzip CF="$(CF) -Ml2" LF="$(LF) -Ml2"

# Sequent Symmetry is a 386 but needs -DZMEM
# This should also work on Balance but I can't test it just yet.
sequent:	# Sequent w/Dynix
	$(MAKE) unzip CF="$(CF) -DBSD -DZMEM"

# AT&T 6300+, running System V.? Unix:  out-of-memory error if don't use -Ml
sysv6300:
	$(MAKE) unzip CF="$(CF) -Ml -DTERMIO" LF="$(LF) -Ml"

# I didn't do this.  I swear.  No, really.
wombat:		# Wombat 68000 (or whatever)
	@echo
	@echo  '	Ha ha!  Just kidding.'
	@echo


#####################
# ZIPINFO MAKERULES #
#####################

# ZipInfo section:  less hand-holding here, but it should be pretty
# straightforward by now.

zipinfo$O:	zipinfo.c unzip.h
	$(CC) -c $(CF) zipinfo.c

misc_$O:	misc.c unzip.h
	$(LN) misc.c misc_.c
	$(CC) -c $(CF) $(ZC) misc_.c
	$(RM) misc_.c

os2zinfo$O:	os2unzip.c unzip.h
	$(LN) os2unzip.c os2zinfo.c
	$(CC) -c $(CF) $(ZC) os2zinfo.c
	$(RM) os2zinfo.c

zipinfo$E:	$(OBJZ)
	$(LD) $(ZL) $(OBJZ) $(ZL2)

zi_gcc:			# GNU gcc under Unix (if no strip, don't worry)
	$(MAKE) zipinfo CC=gcc LD=gcc ZL2=""
	strip zipinfo

zi_indigo:		# SGI Iris Indigo
	$(MAKE) zipinfo CF="-cckr -O -DUNIX $(LOC)"

zi_dos:			# MSC 6.0 + nmake, MS-DOS
	$(MAKE) zipinfo.exe CF="-Oait -Gs -nologo $(LOC)" CC=cl\
	 LD=link E=.exe O=.obj ZL="/noi /nol" ZL2=",zipinfo;"\
	 LN="copy" RM="DEL"


####################
# FUNZIP MAKERULES #
####################

# FUnZip section:  FUnZip (Filter UnZip) is a last-minute addition to the
# UnZip suite and is still VERY raw.  Its purpose is to take a zipfile from 
# stdin and decompress the first entry to stdout.  Only non-encrypted, stored
# or deflated files are allowed at present.  FUnZip may be absorbed into
# regular UnZip in a subsequent release.  This target should work for some
# Unix systems but is not guaranteed to work for all (or even most).

funzip$E:	$(OBJF)
	$(LD) $(FL) $(OBJF) $(FL2)

fu_gcc:			# GNU gcc under Unix (if no strip, don't worry)
	$(MAKE) funzip CC=gcc LD=gcc FL2=""
	strip funzip


################
# ATTRIBUTIONS #
################

# Thanks to the following people for their help in testing and/or porting
# to various machines (and thanks to the many others who aren't listed
# here but should be):
#
#  (original Unix port:  Carl Mascott <cmascott@world.std.com>)
#  386i:	Richard Stephen <stephen@corp.telecom.co.nz>
#  3Bx:		Bob Kemp <hrrca!bobc@cbnewse.att.com>
#  7300:	Richard H. Gumpertz <rhg@cpsolv.CPS.COM>
#		Greg Roelofs <roelofs@amelia.nas.nasa.gov>
#  amdahl:	Kim DeVaughn <ked01@juts.ccc.amdahl.com>, Greg Roelofs
#  apollo:	Tim Geibelhaus
#  aviion:	Bruce Kahn <bkahn@archive.webo.dg.com>
#  bull:	Matt D'Errico <doc@magna.com>
#  c120:	Rafal Maszkowski <sgumk%pltumk11.bitnet>
#  coherent:	David Fenyes <dfenyes@thesis1.med.uth.tmc.edu>
#  convex:	Randy Wright <rwright@convex.com>
#  cray:	Greg Roelofs, Paul Borman <prb@cray.com>
#  cray_cc:	Greg Roelofs
#  cray_v3:	Greg Roelofs
#  cyber_sgi:	Clint Pulley <u001@cs910.cciw.ca>
#  dec:		"Moby" Dick O'Connor <djo7613@u.washington.edu>
#  dnix:	Bo Kullmar <bk@kullmar.se>
#  eta:		Greg Flint <afc@klaatu.cc.purdue.edu>
#  gcc:		Jean-loup Gailly <jloup@chorus.fr>
#  gcc_dos:	Onno van der Linden <linden@fwi.uva.nl>
#  gcc_os2:	Kai Uwe Rommel <rommel@informatik.tu-muenchen.de>
#  gould:	Onno van der Linden
#  hk68:	John Limpert <gronk!johnl@uunet.UU.NET>
#  hp:		Randy McCaskile <rmccask@seas.gwu.edu> (HP-UX)
#		Gershon Elber <gershon@cs.utah.edu> (HP BSD 4.3)
#  icc_os2:	Kai Uwe Rommel
#  indigo:	Kjetil Wiekhorst J|rgensen <jorgens@lise.unit.no>
#  linux:	Humberto Ortiz-Zuazaga <zuazaga@ucunix.san.uc.edu>
#  minix:	Kai Uwe Rommel (Minix 1.5)
#  mips:	Peter Jones <jones@mips1.uqam.ca>
#  msc_dos:	Greg Roelofs <roe2@ellis.uchicago.edu>
#		Piet W. Plomp <piet@icce.rug.nl>
#  msc_os2:	Wim Bonner <wbonner@yoda.eecs.wsu.edu>
#		Kai Uwe Rommel, Greg Roelofs
#  next:	Mark Adler <madler@piglet.caltech.edu>
#  osf1:	Kjetil Wiekhorst J{\o}rgensen
#  p_iris:	Valter V. Cavecchia <root@itnsg1.cineca.it>
#  pyramid:	James Dugal <jpd@usl.edu>
#  rs6000:	Filip Gieszczykiewicz <fmg@smi.med.pitt.edu>
#		Trevor Paquette <tpaquett@ita.lgc.com>
#  rtaix:	Erik-Jan Vens
#  sco:		Onno van der Linden (SCO Unix 3.2.0)
#   		Bill Davidsen <davidsen@crdos1.crd.ge.com> (Xenix/386)
#  sco_dos:	Bill Davidsen, Piet W. Plomp
#  sco_x286:	Ricky Mobley <ddi1!lrark!rick@uunet.UU.NET>
#  sequent:	Phil Howard <phil@ux1.cso.uiuc.edu>
#  sgi:		Greg Roelofs (Iris 4D/380?)
#  sun:		Onno van der Linden (Sun 4), Greg Roelofs (Sun 3, 4)
#  sysv:	Greg Roelofs
#  sysv6300:	Peter Mauzey <ptm@mtdcc.att.com>
#  tahoe:	Mark Edwards <mce%sdcc10@ucsd.edu>
#  ultrix:	Greg Flint (VAX)
#		Michael Graff <explorer@iastate.edu> (DECstation 2100?)
#		Greg Roelofs (DEC 5810)
#		Alex A Sergejew <aas@brain.wph.uq.oz.au>
#  vax:		Forrest Gehrke <feg@dodger.att.com> (SysV)
#		David Kirschbaum <kirsch@usasoc.soc.mil> (BSD 4.3)
#		Jim Steiner <steiner@pica.army.mil> (8600+Ultrix)
#  wombat:	Joe Isuzu <joe@trustme.isuzu.com>
#  xos:		Fulvio Marino <fulvio@iconet.ico.olivetti.com>
#  zi_dos:	Greg Roelofs
#  zi_icc:	Kai Uwe Rommel
#  zi_os2:	Greg Roelofs, Kai Uwe Rommel
#  zipinfo:	Greg Roelofs
