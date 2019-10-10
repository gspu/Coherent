.NH "Changes to COHERENT for Release 4.2.14"
.PP
The following describes how \*(CO release 4.2.14 differs from release
4.0.
.SH "Changes From Release 4.0 to 4.2.05"
.PP
Release 4.2.05 added the following new features to \*(CO:
.IP \(bu 0.3i
Improved support for third-party packages.
The UNIX System-V editions of Lotus and WordPerfect work out of the box.
.IP \(bu
Support for X Windows.
An X Windows package is now available for \*(CO from Mark Williams Company.
.IP \(bu
A rewritten version of the \*(CO kernel.
The kernel now includes \*(ST, the \*(UN system for portable device drivers.
.IP \(bu
Many new system calls, including support for the
.B sigset()
and
.B sigaction()
families of signal-handling system calls;
and \*(UN System-V interprocess comunications, including shared memory,
semaphores, and messages queues.
.IP \(bu
Tools for configuring and rebuilding the kernel.
Beginning with release 4.2.05, you can change kernel variables,
add or delete drivers, and modify driver variables,
then build a new kernel and boot it.
.IP \(bu
Support for peripheral devices has been expanded.
Devices support now include SCSI tape and a variety of SCSI devices.
The console has been rewritten to be more robust and to conform more closely
to accepted \*(UN standards.
The keyboard driver has been expanded and now
supports more languages, including Greek.
.IP \(bu
An improved compiler, linker, and assembler.
These include a completely restructured set of header files,
which bring \*(CO closer to the \*(PX standard;
support for mathematics co-processors;
and a greatly improved version of the \*(CO debugger
.BR db .
.IP \(bu
The visual shell
.BR vsh .
This shell uses drop-down menus, function keys, and single-key commands
to give you a simple, clear interface to the \*(CO system; yet it works
on a character-based terminal.
.IP \(bu
The Taylor \*(UU package.
This is a more robust and flexible version of \*(UU.
.IP \(bu
The MLP print spooler.
This spooler is modelled after the \*(UN command
.BR lp .
You now can have multiple printers of any type
plugged into your \*(CO systems, and access them via parallel ports,
serial ports, or the auxiliary port on a terminal.
.IP \(bu
A more robust version of the
.B curses
library, including support for eight-bit characters and color.
.IP \(bu
.B troff
now supports eight-bit character sets.
.B troff
now comes with tools to ``cook'' loadable PostScript fonts, and to
access fonts built into the Hewlett-Packard LaserJet III.
.IP \(bu
Improved versions of the Bourne shell
.B sh
and the Korn shell
.BR ksh .
Many bugs have been fixed, and the Bourne shell now supports shell functions.
.PP
The following new commands have been added to \*(CO:
.nf
.sp \n(pDu
.tc . 0.1i
.ta 1.25i
\fBalmanac\fR	Print a listing of events on today's date in history
\fBapropos\fR	Find the Lexicon entries that relate to a given topic
\fBasymkdev\fR	Create a node for asynchronous devices
\fBcancel\fR	Cancel a print job
\fBchreq\fR	Change a job's priority, life expectancy, or output device
\fBcohtune\fR	Change the value of a flag or variable in a \*(CO device driver
\fBcu\fR	\*(UN-style communications program
\fBfmt\fR	Adjust the length of lines in a file of text
\fBfwtable\fR	Build a \fBtroff\fR font-width table from a PCL or PostScript ``soft font''
\fBgnucpio\fR	GNU version of the \fBcpio\fR archiver
\fBgtar\fR	GNU version of the \fBtar\fR tape archiver
\fBgzip\fR	Industry-standard compression tool from the Free Software Foundation
\fBidenable\fR	Add a device driver to the \*(CO kernel, or delete a driver from it
\fBidmkcoh\fR	Build a new bootable kernel
\fBlcasep\fR	Convert text to lower case
\fBidtune\fR	Set the value of a ``tunable'' variable in the \*(CO kernel
\fBipcrm\fR	Remove an interprocess-communication item from memory
\fBipcs\fR	Display a snapshot of interprocess communications
\fBlp\fR	Spool a job for printing via the MLP spooler
\fBlpadmin\fR	Administer the MLP spooler
\fBlpsched\fR	Turn on the \fBlp\fR print spooler
\fBlpshut\fR	Turn off the \fBlp\fR print spooler
\fBlpstat\fR	Give the status of a printer or job spooled through the MLP spooler
\fBpclfont\fR	Prepare a PCL for downloading via MLP
\fBPSfont\fR	Cook an Adobe font into downloadable PostScript
\fBreprint\fR	Reprint a spooled job
\fBroute\fR	Show or reset a user's default printer
\fBtape\fR	Manipulate the tape device
\fBttytype\fR	Set the default types of remote terminals
\fBvsh\fR	Invoke the \*(CO visual shell
.PP
For more information on any command, see its entry in the Lexicon.
.SH "Changes From Release 4.2.05 to 4.2.10"
.PP
The following describes how \*(CO release 4.2.10 differs from
\*(CO release 4.2.05.
.Sh "Changes to the Kernel"
.PP
The following changes have been made to the \*(CO kernel and drivers:
.IP \(bu 0.3i
Some kernel bugs have been corrected that caused panics under
special conditions.
In particular, the bug that causes the ``Ring 1 Panic' has been traced
to the AT driver, and changes made to correct it.
.IP \(bu
Under \*(CO 4.2.05, the driver
.B asy
attempted to test for the presence of a UART.
However, the test did not always work correctly
because some newer 16550's no longer follow specification of IIR = xx00 xxxx.
The test has been rewritten to take this into account.
.IP \(bu
The driver
.B vtnkb
now supports Greek keyboards.
.IP \(bu
The driver
.B hai
has been changed to make its resetting more robust.
It now supports 255-head hard disks correctly.
Its default timeout has also been increased, to eliminate bogus
timeout messages when running slow drives.
.IP \(bu
The driver
.B at
has been modified to address a bug that caused panics and timeouts.
.IP \(bu
The kernel now clears the U area upon startup.
This problem had caused \*(CO installation to fail on a few systems.
.IP \(bu
Under certain circumstances, the kernel would crash if given a bad
physical address; it now complains and keeps running.
.IP \(bu
The kernel now recognizes Cyrix parts.
You can optionally enable the internal cache on the
SRx, SRx2, DRx, and DRx2 upgrade parts.
.IP \(bu
The file system has been changed to be more robust.
.IP \(bu
Global variable
.B RLOCKS
is now tunable.
.IP \(bu
In some instances, kernel error messages would print a bogus
number for a system call.
This has been fixed.
.IP \(bu
In some circumstances, the kernel used
.B rep
I/O instructions instead of explicit loops.
On some motherboards, this can cause the CPU to outrun the I/O bus.
The kernel has now removed all
.B rep
instructions and uses loops instead.
.IP \(bu
Variables
.B MSGQID_SPEC
and
.B MSGQSZ_SPEC
have been added to file
.BR /conf/install_conf/src/mtune .
.IP \(bu
Routines that report CPU idle times have been upgraded.
.IP \(bu
Under certain circumstances,
signalling did not work correctly.
This has been fixed.
.IP \(bu
Under \*(CO 4.2.05,
some screen contents would be lost
when the user switched sessions on a system with two video adapters.
This has been repaired.
.IP \(bu
Routines
.BR __kdenabio() ,
.BR __kddisabio() ,
.BR __kdaddio() ,
and
.B __kddelio()
have been added to the \*(CO device-driver kit.
.IP \(bu
Invoking
.B brk()
twice consecutively with the same arguments would cause a
.B PTOV
fault.
This has been fixed.
.IP \(bu
Changes to the kernel now permit
.B reboot
to reboot the system, rather than just bring it down and wait for the user
to press the reset the button.
.IP \(bu
The instruction
.DM
	while (sbrk(4096))
.DE
.IP
would crash the system under some circumstances.
This has been fixed.
.IP \(bu
The memory manager erroneously rounded down
byte lengths not a multiple of a page size,
which under some circumstances created problems.
This has been corrected.
.IP \(bu
The memory manager returned filler below the alignment address.
This has been corrected.
.Sh "Changes to Commands and Scripts"
.PP
The following changes have been made to \*(CO commands and scripts:
.IP \fB/bin/cc\fR
This version (4.5.1) fixes a bug in sign extension, which affected
the compilation of some X applications.
It also fixes option \fB\-g\fR, which generates debugging information.
Under previous releases, this option would break the COFF output.
Other changes correct inappropriate extension of arguments from
.B short
to
.BR int .
.IP \fB/bin/df\fR
This command now uses
.B fstat
and
.B fstatfs
instead of opening raw device.
Environmental variables
.BR DF_OLD ,
.BR OLDSTYLE,
etc., now no longer affect its behavior.
.IP \fB/bin/fwtable\fR
This version (1.6) fixes a bug that caused it to generate bogus tables under
certain conditions.
.IP \fB/bin/ld\fR
This version (4.2.3) fixes some minor bugs.
.IP \fB/bin/login\fR
This has been recompiled to use the latest version of
.BR ctime() .
.IP \fB/bin/make\fR
This command now correctly handles
.BR Makefile s
generated by
.BR imake .
.IP \fB/bin/man\fR
The version of this command shipped with \*(CO 4.2.05 could run out
of memory if it attempted to read too many index files.
This has been fixed:
now it can read an indefinite number of index files.
.IP \fB/bin/newgrp\fR
The version of this program under 4.2.05 incorrectly set no password in a
newly created group, thus creating a security hole.
This has been fixed.
Its default permissions have also been set to \fBr-s-x-x root bin\fR.
.IP \fB/bin/ps\fR
This now returns the proper exit status.
.IP \fB/bin/qfind\fR
This command would page fault under certain circumstances.
It has been repaired.
.IP \fB/bin/sed\fR
This command
features improved error reporting.
.IP \fB/bin/sh\fR
.B exec
has been repaired.
.IP \fB/bin/test\fR
This command now correctly handles empty arguments, and reacts correctly
when handed no arguments.
.IP \fB/bin/troff\fR
This command's suite of font-width tables have been rebuilt using new
.B .tfm
tables supplied by Hewlett-Packard.
These tables are stored in directory
.BR /usr/lib/roff/troff_pcl/fwt .
.IP \fB/bin/wc\fR
This now returns the proper exit status.
.IP \fB/etc/build\fR
This script now allow lets you skip running
.B badscan
on AT drives.
It also allows you to
.B badscan
SCSI drives; this is now allowed because of problems with bad blocks
reported by users who have removable SCSI media.
.IP \fB/etc/build.u\fR
Corrected the word ``Owervite'' to ``Overwrite'' in an error message.
.IP \fB/etc/fsck\fR
This command would page fault if accidentally run on a
.B cpio
backup disk.
This has been fixed.
.IP
The ``memory allocation failed'' error message has been expanded.
.IP \fB/etc/install\fR
This command now correctly handles partial installations.
If it finds a tag file in the root directory, it asks you whether to
continue, rather than simply aborting installation.
.IP \fB/etc/mklost+found\fR
This script is a new command that lets you build a larger
.B lost+found
directory than \*(CO utilities build by default.
.IP \fB/etc/newusr\fR
This script had a problem whereby it mistakenly used the identifier
.BR add_user .
This has been repaired.
.IP
Because
.B vsh
is not a login shell, this has been removed from the list of login shells
presented to the user.
.IP \fB/etc/profile\fR
This file now uses
.B cat
to display the file
.B /etc/motd
by default.
This is not done if
.B $HOME
is
.BR /etc .
.IP
Instruction
.DM
	[ -f /etc/motd ...
.DE
.IP
has been changed to:
.DM
	[ -s /etc/motd ...
.DE
.IP \fB/etc/reboot\fR
Changes to the kernel now permit
.B reboot
to reboot the system, rather than just bring it down and wait for the user
to press the reset the button.
Note that it still prompts you to press the reset button, but if you wait
a few moments it will (in all probability) reboot your system automatically.
.IP \fB/etc/wall\fR
This command has been changed so that it no longer hangs when
it encounters a bogus entry in
.BR /etc/utmp .
This corrects the problem that caused the script
.B /etc/shutdown
to hang after a user had run X.
.IP \fB/etc/conf/aha/mkdev\fR
This script has been added.
It walks you through the process of describing your SCSI devices to the driver
.B aha
(the alternate driver for Adaptec SCSI disks).
.IP \fB/etc/conf/asy/mkdev\fR
This scipt now walks you through the process of configuring a modem, as well
as describing a serial port.
Its prompts have been reworded to make them clearer.
.IP \fB/etc/conf/at/mkdev\fR
This script has been added.
It walks you through the process of describing your AT disks to the driver
.BR at .
.IP \fB/etc/conf/bin/devadm\fR
Identifier
.B STARTUP
has been corrected to
.BR START .
.IP \fB/etc/conf/cohmain/mkdev\fR
This script now asks you whether you wish to enable the Cyrix upgrade cache,
should your system have a Cyrix part.
A new global variable identifies CPU type.
.IP \fB/etc/conf/hai/mkdev\fR
The initial version of this script confused variables
.B ID
and
.BR i ,
which created problems under certain circumstances.
This has been repaired.
.IP
This script has also been modified to permit you to install a tape driver
even if you do not have a SCSI hard disk.
.IP
Its prompts have been reworded to make them clearer.
.IP \fB/etc/conf/install_conf/sdevice\fR
Defaults for drivers
.BR echo ,
.BR loop ,
and
.B dump
were incorrectly set to `Y'; now correctly set to `N'.
.IP \fB/etc/conf/lp/mkdev\fR
This script's prompts have been reworded to make them clearer.
.IP \fB/etc/conf/pty/mkdev\fR
This script has been added to the release.
It walks you through the task of configuring the driver
.BR pty ,
which handles pseudo-TTYs.
.IP \fB/etc/conf/ss/mkdev\fR
This script has been added.
It walks you through the configuring of the driver
.B ss
(the alternate driver for Seagate SCSI disks).
.IP \fB/etc/conf/bin/idbld\fR
This new script walks you through the process of building an entirely
new kernel.
It invokes each
.B mkdev
script in turn, then invokes
.BR /etc/conf/bin/idmkcoh
to build the newly reconfigured kernel.
.\".IP \fB/etc/install\fR
.\"This command now supports pre-files.
.\"That is, it executes file \fB/conf/\fItag\fB.pre\fR
.\"before
.IP \fB/usr/bin/cancel\fR
This now correctly cancels a job for the MLP spooler.
.IP \fB/usr/bin/elvis\fR
This has been updated to release 1.8.
.IP
The command
.B cc
in this editor did not work properly.
This has been repaired.
.IP
.B elvis
now invokes
.B elvprsv
from
.BR /etc/rc .
.B elvprsv
and
.B elvrec
have permissions \fB4755 root root\fR, and
.B /usr/preserve
now has permissions \fB755 root root\fR.
.IP \fB/usr/bin/gzip\fR
This has been updated to use the latest GNU sources.
.IP \fB/usr/bin/ksh\fR
The previous release of this shell dropped argument 1; this has
been fixed.
Its handling of
.B SIGINT
and
.B SIGQUIT
have also been fixed.
.IP \fB/usr/bin/lp\fR
.IS \fB/usr/bin/lpsched\fR
Several bugs have been fixed in the MLP spooler and daemon.
In particular, the option \fB\-m\fR now works.
.Sh "Changes to Library Routines and Macros"
.PP
The following library routines and macros have been changed or repaired.
.IP \fBftime()\fR
This has been restored as a COFF system call.
.IP \fBlibcurses\fR
This library has been updated to fix bugs and incorporated some previously
unsupported routines.
.IP \fBva_start()\fR
This routine, in header file
.BR <stdarg.h> ,
has been rewritten so that it can be used with the GNU C++ compiler
.BR g++ .
.IP \fBdiv()\fR
In-lining of this routine, which lives in header file
.BR <stdlib.h> ,
has been conditionalized.
This is done because
.B gcc
correctly handles such in-lined routines, but
.B g++
does not.
.IP \fBldiv()\fR
In-lining of this routine, which lives in header file
.BR <stdlib.h> ,
has been conditionalized.
This is done because
.B gcc
correctly handles such in-lined routines, but
.B g++
does not.
.IP \fBprintf()\fR
This has been updated to correct some minor errors.
.IP \fB/lib/ndp/libm.a\fR
This library lacked some floating-point op-codes for hardware floating-point
arithmetic.
This has been corrected.
.Sh "Changes to Header Files"
.PP
.IP \fB/usr/include/float.h\fR
The values for
.B DBL_MAX
and
.B DBL_MIN
have been corrected.
.IP \fB/usr/include/limits.h\fR
.B __SCHAR_MIN
was misspelled.
This has been corrected.
.IP \fB/usr/include/math.h\fR
This file now contains the following corrected prototypes for
.B frexp
and
.BR ldexp :
.DM
	double	frexp	__PROTO ((double _x, int * _iptr));
	double	ldexp	__PROTO ((double _x, int _i));
.DE
.IP \fB/usr/include/stdlib.h\fR
This header file has had the line
.DM
	#include <common/xdebug.h>
.DE
.IP
removed.
.IP \fB/usr/include/common/__stat.h\fR
The R4
.B stat
structure in this header file has been suppressed, so that it
works with the latest release of GNU C.
.IP \fB/usr/include/coh/proc.h\fR
.IS \fB/usr/include/coh/timeout.h\fR
These header files have been added to make the kernel more compatible with GCC.
.IP \fB/usr/include/sys/confinfo.h\fR
Added
.B EXTERN_CLOSE
declaration to
.BR DECLARE_OPEN_CLOSE .
.IP \fB/usr/include/sys/cyrix.h\fR
This header file has been added to help support the Cyrix part.
.IP \fB/usr/include/sys/mmu.h\fR
Symbols
.B SEG_PWT
and
.B SEG_PCD
have been added to this header file.
.IP \fB/usr/include/sys/stat.h\fR
The R4
.B stat
structure in this header file has been suppressed, so that it
works with the latest release of GNU C.
.IP \fB/usr/include/sys/msg.h\fR
.IS \fB/usr/include/sys/sem.h\fR
.IS \fB/usr/include/sys/shm.h\fR
BCS-compatible pid length has been restored.
.IP \fB/usr/include/sys/types.h\fR
Type
.B u_long
et al. have been added.
.Sh "Miscellaneous Changes"
.PP
The following miscellaneous changes have been made:
.IP  "Floppy-Disk Device Nodes\fR"
A number of changes have been made to floppy-disk device nodes.
The following nodes have been deleted:
.DM
	/dev/f9d0
	/dev/f9d1
	/dev/rf9d0
	/dev/rf9d1
.DE
.IP
The following nodes have been deleted and remade:
.DM
.ta 0.5i 2.0i
	/dev/dsk/f03d	/dev/rdsk/f03d
	/dev/dsk/f13h	/dev/rdsk/f13h
	/dev/dsk/f03d	/dev/rdsk/f03d
	/dev/dsk/f13h	/dev/rdsk/f13h
.DE
.IP "\fBSCSI Disk Device Nodes\fR"
Permissions on devices \fB/dev/{r}sd[a-d]\fR have been changed
\fB600 sys sys\fR.
Permissions on devices \fB/dev/{r}sdx\fR have been changed to
\fB600 root root\fR.
This matches the permissions for the \fBat\fR devices.
The
.B mkdev
scripts for these devices have also been updated to enforce these changes.
.IP \fB/lib/libc.a\fR
.IS \fB/lib/ndp/libc.a\fR
This version (4.2.3) fixes some bugs in some library routines, and include
a few routines that had been left out.
.IP \fB/lib/yyparse.c\fR
This has been updated to correct some minor bugs.
.IP "\fBManual Pages\fR"
These have been updated to include the latest changes and updates to the
software.
.IP \fB/usr/lib/makeactions\fR
.IS \fB/usr/lib/makemacros\fR
These have been updated to conform more closely to published standards.
.IP \fB/usr/lib/shell_lib.sh\fR
The routine
.B split_path
has had a minor bug corrected.
.IP \fB/usr/lib/uucp/sys\fR
The default version of this file
now includes the instructions
.DM
	remote-send
	remote-receive
	local-send
	local-receive
.DE
.IP
for the entry for site
.BR mwcbbs .
.IP \fB/usr/lib/libcurses.a\fR
.IS \fB/usr/lib/ndp/libcurses.a\fR
These incorporate fixes to minor bugs in the
.B curses
routines.
.IP \fBterminfo\fR
.IS \fBtermcap\fR
Some terminal descriptions have been repaired, in particular, the
.B xterm
entry in
.BR termcap .
.IP
Regional scrolling has been fixed in the
.B termcap
description for the console;
.B Blast
and GNU Emacs now scroll correctly.
.SH "Changes From Release 4.2.10 to 4.2.14"
The following describes how \*(CO release 4.2.14 differs from
\*(CO release 4.2.10.
.Sh "Changes to the Kernel"
.PP
The following changes have been made to the \*(CO kernel and drivers:
.IP \(bu 0.3i
The floppy-tape driver
.B ft
has been added to the kernel.
This supports floppy tapes in the QIC-40 or QIC-80 formats.
.IP \(bu
The AT driver has been rewritten to support large IDE drives \(em that is,
drives with more than 1,024 cylinders.
.\".IP \(bu
.\"Release 4.2.14 includes CD-ROM drivers.
.\"Driver
.\".B mcd
.\"supports a variety of Mitsumi drives, plugged into the Mitsumi interface card.
.\"Driver
.\".B cdu
.\"supports a variety of Sony drives, again plugged into their own interface
.\"card.
.\"The SCSI driver
.\".B hai
.\"now supports CD-ROM drives plugged into Adaptec, Seagate, or Future Domain
.\"host adapters.
.\"For details, use the
.\".B man
.\"command to view the manual page
.\".BR CD-ROM .
.\"Applications are also included to read CD-ROMs and play audio CDs.
.IP \(bu
Release 4.2.14 includes the trace driver
.BR tr .
This driver enables you to cat the output of the function
.B cmn_err()
from a trace buffer into a file; to do so, read the contents of device
.BR /dev/trace .
The trace buffer can hold up to four megabytes of traces and error messages.
This driver should be of great assistance to persons who are developing
device drivers.
.IP \(bu
The driver
.B hai
has been rewritten to use multi-sector transfers, thus increasing its
speed significantly.
.IP \(bu
.B hai
now includes modules to support tape devices and CD-ROMs plugged into
Future Domain and Seagate host adapters.
For details, use the command
.B man
to view the manual page for
.BR hai .
.IP \(bu
The driver
.B at
has been updated to increase its speed and make it more robust.
.IP \(bu
The System V features
.BR semaphores ,
.BR messages ,
and
.B "shared memory" 
have been rewritten to be more robust.
.IP \(bu
Device
.B /dev/freemem
has been added.
This device returns the amount of RAM that is free.
For details, use the command
.B man
to view the Lexicon entry
.BR freemem .
.IP \(bu
The kernel's handling of end-of-volume codes has been repaired.
This means, among other things, that multi-volume
.B tar
archives are now handled correctly.
.IP \(bu
The \*(CO kernel's scheduler has been rewritten.
The scheduler is the module which is responsible for allocating CPU time
to the various processes that are being executed.
The old scheduler would allocate CPU time to each process in turn, without
regard to the amount of resources it consumed.
Partially as a result of this scheme, the performance of programs that made
light use of the CPU (e.g., editors) would degrade when CPU-intensive programs
were invoked.
The new scheduler tends to favor programs that make light use of the CPU.
.IP \(bu
The kernel now supports the DDI/DKI routines
.BR physiock() ,
.BR getrbuf() ,
and
.BR freerbuf() .
.IP \(bu
The console driver now prints italicized text as white text on a red field.
This change was made to help the console driver conform to published standards.
.IP \(bu
The console driver used to lose track of cursor invisibility when the user
switched virtual consoles.
This has been repaired.
.IP \(bu
A bug in the polling routines has been fixed.
This bug could cause applications that attempted to perform I/O through
a pseudo-tty to hang under some circumstances.
Under X, this bug would cause keyboard lockups and cause the mouse
occasionally to enter ``jumping-bean'' mode.
It has been repaired.
.IP \(bu
A bug in the kernel could cause an imbalance in the lock count.
This has been repaired.
.IP \(bu
A bug in the kernel's signal handler would sometimes cause programs to
crash with an error message that indicated a problem in the signal frame buffer.
This has been repaired.
.IP \(bu
The signal
.B usigsuspend
did not work correctly in previous releases.
This has been corrected.
.IP \(bu
The tunable kernel variables
.B CON_BEEP_SPEC
and
.B SEP_SHIFT
have been added.
The former variable lets you turn off beeping on the console.
The latter lets each virtual console have its own shift-key settings.
For details on how to set and these variables, use the command
.B man
to view the Lexicon entry
.BR console .
.\".IP \(bu
.\"The drivers
.\".B kb
.\"and
.\".B nkb
.\"are no longer included with \*(CO.
.\"The need for them has vanished as the drivers
.\".B vtkb
.\"and
.\".B vtnkb
.\"have been made more robust.
.\"They are still available throught the Internet or the MWC BBS should you
.\"still need them.
.Sh "Changes to the Header Files"
.PP
The following changes have been made to \*(CO's suite of header files:
.IP \(bu 0.3i
Header files
.BR <coh/bio.h> ,
.BR <coh/defer.h> ,
.BR <coh/i386lib.h> ,
.BR <coh/misc.h> ,
.BR <coh/proc.h> ,
and
.B <coh/timeout.h>
have been added to \*(CO.
These prototype the internal-kernel routines used in the kernel.
.IP \(bu
Header file
.B <shadow.h>
has been added.
This supports the functions that manipulate the shadow-password file
.BR /etc/shadow .
These functions are described below.
.IP \(bu
Header file
.B <termios.h>
has been added.
This supports the \*(PX-compatible extended
.B termio
routines.
.IP \(bu
Header files
.BR <gdbm.h> ,
.BR <dbm.h> ,
and
.B <ndbm.h>
have been added.
These support the library
.BR libgdbm ,
which implements functions for building and manipulating a \*(UN-style DBM
data base.
.IP \(bu
Header file
.B <netdb.h>
and related header files have been added.
These prototype and support the routines in the socket library
.BR libsocket.a .
.IP \(bu
Header file
.B utime.h
has been moved from
.B /usr/include/sys
to
.BR /usr/include ,
to conform with standard usage.
.IP \(bu
Minor fixes have been made to header files
.B <sys/dmac.h>
and
.BR <sys/fdc765.h> .
.IP \(bu
Constant
.B OPEN_MAX
has been added to header file
.BR <limits.h> .
.Sh "Changes to Compiler and Libraries"
.PP
The following changes have been made to the \*(CO compiler and libraries:
.IP \(bu 0.3i
The functions
.BR endspent() ,
.BR getspent() ,
.BR getspnam() ,
and
.B setspent()
have been added to
.BR libc .
This manipulate the shadow-password file
.BR /etc/shadow .
.IP \(bu
A bug in the function
.B ctime()
caused systems in the southern hemisphere not to set daylight savings time
correctly.
This has been fixed.
.IP \(bu
Routines
.BR getutent() ,
.BR getutline() ,
et al. have been added to
.BR libc .
These manipulate the archiving files
.BR utmp .
For details, use the command
.B man
to view the Lexicon entry for
.BR getutline() .
.IP \(bu
Functions have been added to
.B libc
to implement the
.B termios
routines.
For details, use the command
.B man
to view the Lexicon entry for
.BR termios .
.IP \(bu
The regular-expression (\fBregexp\fR) routines have been moved from library
.B libmisc
into library
.BR libc .
Programs that use these routines no longer have to link in
.B libmisc
to obtain them.
.IP
.IP \(bu
The Berkeley functions
.BR bcopy() ,
.BR bcmp() ,
and
.B bzero()
have been added to
.BR libc .
These are included to support existing code.
.IP \(bu
A set of routines to generate 48-bit random numbers have been added to
.BR libc .
For details, use the command
.B man
to view the Lexicon entry
.BR drand48() .
.IP \(bu
Under \*(CO release 4.2.10, the version of the library
.BR /usr/lib/libl.a ,
which is used by the application
.BR lex ,
was incorrect.
The correct version is included in release 4.2.14.
.IP \(bu
The compiler
.B cc
had a bug that caused it to write incorrect output when invoked with its option
.BR \-g .
This has been fixed.
.Sh "Changes to Applications"
.PP
The following changes have been made to \*(CO's applications:
.IP \(bu 0.3i
The mail system has been updated to
.B smail
release 3.1.28.
This release is more robust than the older version of
.B smail
that had been included with \*(CO, and includes many more features.
.IP \(bu 0.3i
The \*(UU implementation has been upgraded to Taylor \*(UU release 1.05.
For a fuller description of how this implementation works, use the command
.B man
to view the Lexicon entry
.BR UUCP .
.IP \(bu
The command
.B ideinfo
has been added.
This command displays information about the configuration of an IDE drive.
For details, use the
.B man
command to view its manual page.
.\".IP \(bu
.\"The command
.\".B cdplayer
.\"has been added.
.\"This lets you play an audio CD on your CD-ROM drive.
.\".IP \(bu
.\"The commands
.\".B cdv
.\"and
.\".B cdview
.\"have been added.
.\"They let you read files from a CD-ROM.
.IP \(bu
Command
.B makedepend
has been added to the release.
This command builds a table of dependencies that can be used by
.BR make .
For details, use
.B man
to view its manual page.
.IP \(bu
Command
.B findmouse
has been added to \*(CO.
This command is used during installation of the \*(CO version of X Windows,
and supercedes the version that is included with that package.
.IP \(bu
Commands
.B sort
and
.B diff
have been recompiled as 32-bit binaries.
Users can now use them to sort or compare extremely large files.
The extremely annoying ``jackpot'' message has been removed from
.BR sort .
.IP \(bu
The editor
.B elvis
has been updated to release 1.8pl3.
As part of this update, the commands
.B elvprsv
and
.B elvrc
have been added; these help you recover modifications to a file
in the case of a catastrophic failure of the system during an editing session.
Manual pages for each can be viewed with the
.B man
command.
.IP \(bu
.B gnucpio
has been updated to release 2.3.
.IP \(bu
The command
.B ftbad
is now included.
This lets you edit the list of bad blocks on a floppy-tape cartridge.
It works with the floppy-tape driver
.BR ft .
.IP \(bu
Command
.B /etc/conf/bin/idbld
is now included in this release.
This command walks you through all configuration scripts for the kernel;
in effect, you can entirely reconfigure your kernel with one command.
.IP \(bu
Command
.B makeboot
builds a bootable floppy disk for \*(CO system.
We strongly suggest you build such a disk and put it into a safe place,
to help your system recover from a catastrophic error.
.IP \(bu
Commands
.BR zcmp ,
.BR zdiff ,
.BR zforce ,
.BR zgrep ,
.BR zmore ,
and
.B znew
manipulate compressed and ``zipped'' files.
They let you compare or read compressed files without having to go through
the trouble of uncompressing them first.
.IP \(bu
As part of the updating the \*(UU package, the command
.B uucheck
has been replaced with the command
.B uuchk .
.IP \(bu
The \*(CO implementation of
.B awk
has been replaced by
.BR gawk ,
from the Free Software Foundation.
This a fuller, more robust implementation of the AWK language.
For details, use the
.B man
command to view the manual page for
.BR gawk .
.IP \(bu
The shells
.B sh
and
.B ksh
have been repaired:
they no longer ``leak'' file descriptors.
.IP \(bu
The MLP print spooler has been revised.
Numerous bugs have been fixed; in particular, the spooler daemon no longer
dies when asked to print a large number of files simultaneously.
The daemon has also been revised to pass a fourth argument to the backend
scripts.
This fourth argument normally is used for the title of the document being
printed, but you can use it pass all manner of information as well, thus
greatly increasing the flexibility of MLP backend scripts.
For details, use
.B man
to view the Lexicon entry for
.BR lpsched .
.IP \(bu
Commands
.B man
and
.B apropos
have been rewritten; they are now shell scripts instead of C programs.
Both now support the environmental variable
.BR MANPATH ,
and so can now read manual pages kept in locations other than
.BR /usr/man .
For details, use
.B man
to read the Lexicon entry for
.BR man .
.IP \(bu
The command
.B epson
has been revised to write its output to the standard output instead of
to device
.B /dev/lp .
You can now use it as a filter within an MLP backend script.
.IP \(bu
Program
.B cdmp
has been revised to fix some bugs.
.IP \(bu
Command
.B tape
has been added to the release.
This command lets you manipulate tape devices, either floppy or SCSI;
for example, you can use it rewind a tape.
.IP \(bu
.B nroff
had a bug that caused it to interpret the escape sequence \fB\es0\fR
incorrectly.
This has been repaired.
.IP \(bu
Commands
.B nroff
and
.B troff
now support the primitive
.BR .sy ,
which executes a command under the shell.
For details, use
.B man
to view the Lexicon page for
.BR nroff .
.IP \(bu
Command
.B hmon
has been added to \*(CO.
This command uses a graphical display to show you what processes are running;
you can interactively send signals to a process just by selecting it from
the display and pressing a single key.
For details, use the command
.B man
to view its Lexicon entry.
.IP \(bu
Command
.B hpfilter
has been dropped from the MLP package, as have backend scripts
.B hppar
and
.BR hpserial .
The filter did not work properly, and is obviated by the commands
.B prps
and
.BR hp .
.IP \(bu
Minor bugs in commands
.B prps
and
.B fwtable
have been fixed.
.IP \(bu
Commands
.B unmkfs
and
.B cvmail
have been dropped from the \*(CO release.
.Sh "Miscellaneous Changes"
.PP
The following miscellaneous changes have been made:
.IP \(bu 0.3i
The macro package
.B tmac.an
has been rewritten.
When used with \*(CO's implementation of
.BR nroff ,
it now correctly formats most \*(UN manual pages without modification.
.IP \(bu
The file
.B specials.r
used by
.B troff
and
.B nroff
has been modified to encode many of the extended characters defined in
the character set ISO Latin 1.
Each extended character is named after its hexadecimal value in ISO Latin 1.
For details, read the file
.BR /usr/lib/roff/*/specials.r .
.IP \(bu
Both MicroEMACS and
.B help
now read files
.BR /usr/lib/help* .
This eliminates the need to link these files into directory
.BR /etc .
.IP \(bu
Command
.B almanac
is now included in the default
.BR /etc/profile .
.IP \(bu
User
.B remacc
has been removed from the default version of
.BR /etc/passwd .
That ``user'' used to govern romote access to the system, but has been
replaced by the shadow-password mechanism.
For details, use the command
.B man
to view the Lexicon entry for
.BR login .
.IP \(bu
A number of entries have been added to the on-line version of the
\*(CO Lexicon, or have been revised heavily.
Each can be viewed through \*(CO's command
.BR man .
For a complete list of additions to the Lexicon, use the
.B man
command to view the Lexicon entry
.BR Lexicon .
