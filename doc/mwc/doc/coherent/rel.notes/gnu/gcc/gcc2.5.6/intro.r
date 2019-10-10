.NH "GNU Software Development System"
.PP
Thank you for purchasing the \*(CO release of the GNU Software Development
System.
This package bundles a number of excellent GNU compilers and utilities
from the Free Software Foundation, ported to \*(CO by Mark Williams
Company.
.SH "Before You Install"
.PP
Please note the following:
.PP
.B
If you have purchased both the GNU Tools and GNU C from Mark Williams,
you \fImust\fP install GNU C first!
.R
.PP
If you have already installed GNU Tools onto your system, do the following:
.IP \(bu
Log in as the superuser
.BR root .
.IP \(bu
.B cd
to directory
.BR /usr/man .
.IP \(bu
.B mv
directory
.B gnu
to
.BR gnu.back .
.IP \(bu
.B mv
file
.B gnu.index
to
.BR gnu.index.back .
.PP
This will preserve files that otherwise would be overwritten by the
installation of GNU C.
A later section of these notes will show you how to reassemble these files
back together.
.SH "Contents of This Package"
.PP
This package consists of the following tools and utilities:
.DS
	\fBar\fR, the GNU archiver
	\fBas\fR, the GNU assembler
	\fBbison\fR, the GNU implementation of \fByacc\fR
	\fBflex\fR, the GNU implementation of \fBlex\fR
	\fBg++\fR, the GNU C++ compiler
	\fBgcc\fR, the GNU ANSI/Objective C compiler, release 2.5.6
	\fBgdb\fR, the GNU source-code debugger
	\fBgprof\fR, the GNU program profiler
	\fBident\fR, the GNU C beautifier
	\fBinfo\fR, the interactive documentation reader
	\fBld\fR, the GNU linker
	\fBmake\fR, the GNU implementation of the \*(UN programming discipline
	\fBnm\fR, the GNU symbol-table utility
	\fBobjcopy\fR, GNU tool to copy an object module
	\fBobjdump\fR, GNU tool to dump an object module
	\fBprotoize\fR, proto-ize a C program
	\fBranlib\fR, built a ranlib header on a library
	\fBsize\fR, display the size of a program and its components
	\fBunprotoize\fR, de-protoize a C program
.DE
.PP
This package also includes the following:
.IP \(bu 0.3i
A manual page for nearly every command.
.IP \(bu
C++ run-time libraries.
.IP \(bu
``info'' files for reading with the command
.BR info .
.SH "System Requirements"
.PP
To install this package, you must have 15 megabytes of disk space,
of which 11 megabytes are filled permanently and four megabytes
hold files temporarily.
You must also have at least eight megabytes of RAM.
.SH "How to Install"
.PP
Installation is in two parts.
The first updates \*(CO; and the second install the GNU development package.
.Sh "Updating COHERENT"
.PP
If you have \*(CO release 4.2.05, you must update your \*(CO system before
you install the GNU development package.
To see what version of \*(CO you have, type the command:
.DM
	uname -r
.DE
This command displays the version of \*(CO that you are using.
If the version is earlier than release 4.2.10,
then you must update \*(CO to release 4.2.10 or later
.I before
you install GNU C.
If, however, the version shown is release 4.2.10 or later, do
.I not
update your \*(CO system; instead, put aside the update disk and jump
to the next section of these notes, which describes how to install the GNU
developent package onto your system.
.PP
If you are reading this paragraph, we assume that you are running
\*(CO release 4.2.05.
To install the supplemental-update disk, insert into your disk drive the disk
that is labelled ``COHERENT Supplement''; then log in as the superuser
.B root
and type the following command:
.DM
	/etc/install CohSupl /dev/\fIdevice\fP 1
.DE
.I device
should name the floppy-disk device from which you are installing the update.
For example, if you are installing from a 3.5-inch floppy disk that is drive
0 on your machine, you would use the command:
.DM
	/etc/install CohSupl /dev/fva0 1
.DE
For a list of available floppy-disk drives, see the entry
.B "floppy disk"
in the \*(CO Lexicon.
.Sh "Installing GNU Package"
.PP
At this point, we assume that either you are running a release of \*(CO
that is later than release 4.2.05, or that you have installed the
supplemental-update disk, as described above.
.PP
Most of this package is installed into directory
.BR /u1/gnu .
The only exception are the manual pages, which are
installed into directory
.BR /usr/man/GNU .
If you have a spare partition available on your system, you should
consider giving it to this package:
simply build a new file system on the partition and mount it on
directory
.B /u1
before you install the package.
(For details on how to do this, see the Lexicon entries for the commands
.B mfks
and
.BR mount .)
.PP
To install this package from 3.5-inch disks on drive 0, use the
command:
.DM
	/etc/install CohGCC /dev/fva0 4
.DE
.PP
To install from 5.25-inch disks on drive 0, use the command:
.DM
	/etc/install CohGCC /dev/fha0 5
.DE
That's all there is to it.
.SH "Removing the Previous Release"
.PP
After you have installed the current release of GNU C, you may wish to
remove previous installations.
If your previous installation of GNU C came from Mark Williams Company,
remove the files:
.DM
	/usr/bin/g++
	/usr/bin/gcc
	/usr/bin/gdb
	/usr/bin/gmake
.DE
Then, remove the directory
.DM
	/usr/local/lib/gcc-lib
.DE
as well as all of the files and sub-directories under it.
.PP
If you do not remove the old release, be sure that you include
directory
.B /u1/gnu/bin
in your
.BR PATH ,
to ensure that you use the latest verison of the compiler.
.SH "Re-assembling Documentation"
.PP
If you had installed the GNU Tools for \*(CO
.I before
you installed GNU C, we assume that you followed the directions given at the
beginning of these notes to back up the manual pages that accompany that
package.
We will now describe how to reassemble the manual pages of the two
packages.
.IP \(bu 0.3i
Log in as the superuser
.B root
and
.B cd
to directory
.BR /usr/man .
.IP \(bu
Issue the following command to move all files from directory
.B gnu.back
to
.BR gnu :
.DM
	mv gnu.back/* gnu
	rmdir gnu.back
.DE
Directory
.B gnu.back
is removed because it is no longer needed.
.IP \(bu
Concatenate the indices of the two packages, as follows:
.DM
	cat gnu.index gnu.index.back | sort | uniq > tempfile
	mv tempfile gnu.index
	rm gnu.index.back
.DE
.PP
That's all there is to it.
.SH "Running GCC"
.PP
Please note the following twists before you attempt to use this
package on your \*(CO system.
.PP
First, many of the tools in this GNU package have the same names as
their \*(CO counterparts:
for example, both the GNU assembler and the \*(CO assembler are named
.BR as ,
both the GNU linker and the \*(CO linker are named
.BR ld ,
and so on.
Therefore, which tool is used by default depends upon which directory
is searched first when you issue a command; and this, in turn, depends
upon where you place directory
.B /u1/gnu/bin
in your
.BR PATH .
.PP
Assume, for example, that your
.B PATH
is as follows:
.DM
	/usr/bin:/bin:/usr/local/bin:/usr/X11/bin:.
.DE
.PP
If you wish to use the GNU tools as your default development tools,
you should place
.B /u1/gnu/bin
.I before
.B /bin
and
.B /usr/bin
in your
.BR PATH ,
so the GNU tools are used by default; and your
.B PATH
appears as follows:
.DM
	/u1/gnu/bin:/usr/bin:/bin:/usr/local/bin:/usr/X11/bin:.
.DE
.PP
If, however, you wish to use the \*(CO tools by default, change your
.B PATH
so that it appears as follows:
.DM
	/usr/bin:/bin:/u1/gnu/bin:/usr/local/bin:/usr/X11/bin:.
.DE
You may have to use direct path names if you use one set of tools by
default and wish to use the other for a special job.
.PP
The second twist is that although the GNU tools
can, in general, do everything the \*(CO tools can do, there is one
special job it cannot do:
you cannot use them to rebuild the \*(CO kernel.
If you wish to rebuild the \*(CO, you
.I must
use the \*(CO versions of
.BR make ,
of the linker
.BR ld ,
and of the assembler
.BR as .
.PP
If you are using the \*(CO tools by default, this is not a problem.
If, however, you are using the GNU tools by default, you must do one of
the following to ensure that you invoke the \*(CO tools instead of their
GNU counterparts:
.IP \fB1.\fR 0.3i
.B cd
to directory
.B /u1/gnu/bin
and rename
.BR ld ,
.BR as ,
and
.B make
to, respectively,
.BR ld.not ,
.BR as.not ,
and
.BR make.not .
.IP \fB2.\fR
Modify your
.B PATH
and remove directory
.B /u1/gnu/bin
from it.
.IP \fB3.\fR
If directory
.B /u1/gnu
is on its own partition, un-mount that partition.
.PP
After you have rebuilt your kernel, be sure to undo what you did,
so you can resume using the GNU tools by default.
.PP
Please note that a future release of \*(CO will simplify this process.
.SH "Documentation"
.PP
This package comes with two sets of documentation:
manual pages and info files.
.PP
The manual pages are installed into directory
.BR /usr/man/GNU .
You can read them with the \*(CO command
.BR man ,
just like any other manual page installed on your system.
.PP
The manual pages, however, do not contain much detail.
To study these tools in-depth, you must use the GNU command
.BR info ,
which is included with this package.
.B info
uses a Hyperscript-like structure that makes it easy for you to thread
your way through the documentation and find exacctly the information
you need.
.PP
The best way to learn about
.B info
is to use it.
To begin, just type:
.DM
	info
.DE
The command will display information about itself.
We suggest that you browse
.BR info 's
help files until you become familiar with this command; a little time
invested in studying this command will save you a great deal of time
later on, when you wish to use these GNU tools to their best advantage.
.SH "Objective C"
.PP
Objective C is an object-oriented version of C that is gaining
popularity.
This package has no special front end for Objective C;
rather, the source files have the extension
.B .m
and one invokes
.B gcc
to compile them.
.B gcc
does not have an option by which you can tell it that you
want to compile Objective C and not ANSI C.
.PP
To give you a taste of Objective C, the following implements
``hello world'' in that language.
Copy the following code into file
.B hello.m
and compile it with
.BR gcc :
.DM
#include <objc/objc.h>
#include <objc/Object.h>
.DE
.DM
static char     *p;
.DE
.DM
@interface Object (Private)
.DE
.DM
+ initialize;
- show;
@end
.DE
.DM
@implementation Object (Private)
.DE
.DM
+ initialize
{
	p = "Hello, the world\en";
	return self;
}
.DE
.DM
- show
{
	fprintf (stderr, "%s\en", p);
	return self;
}
.DE
.DM
@end
.DE
.DM
main (void)
{
	id  this_object;
.DE
.DM
	this_object = [Object new];
	[this_object show];
.DE
.DM
	exit (0);
}
.DE
.SH "Notes"
.PP
Please note that
.B gdb
cannot yet read a \*(CO dump file.
.PP
Mark Williams has designed this package to be easy to install onto your
system.
If you have a problem with installing this package or invoking its tools,
please contact our technical support team, and they will be glad to help
you.
Due to the highly technical nature of this product, we
cannot offer technical support for any program that you are attempting
to write, recompile, or debug with these tools.
.\" Caveat utilitor.
However,
if you enounter what you think is a bug in any of these tools, please mail
a detailed bug report to our technical support team.
You can reach them at the telephone number given the copyright page;
or you can send electronic mail to
.BR support@mwc.com .
We will do what we can to fix this problem, or forward your report to the
Free Software Foundation for correction.
.\" Don't hold your breath, however.
.PP
Finally, the binaries included with this package are protected by the GNU
``copyleft''.
The text of this agreement is kept in files
.B /u1/gnu/COPYING_2.0
and
.BR /u1/gnu/COPY.LIB .
We urge you to read these files to see what rights the Free Software
Foundation has granted you, and what restrictions it has placed upon
these products.
.PP
Finally, printed documentation on GNU C and its ancillary tools are
available from the Free Software Foundation.
If you are interested, we suggest that you contact the Free Software
Foundation, 675 Mass Avenue, Cambridge, MA 02139.
These documents are reasonably priced and well written; and by
purchasing them you can help support the Free Software Foundation in its
work of maintaining and extending their products.
