.ds TL "GNU Tools for COHERENT"
.ds TI ""
.NH "Introduction"
.PP
GNU Tools is a collection of utilities from the Free Software Foundation.
These tools are commonly used on various implementations of \*(UN and
other \*(UN-like operating systems.
.PP
GNU Tools can be compiled and run on any system that runs \*(CO
(that is, \*(CO version 4.2 or greater).
Your system must have at least three megabytes of RAM to run most
of these tools; four megabytes is recommended.
.PP
Mark Williams Company does not offer technical support for GNU Tools, other
than to assist you in installing it.
We will post on the MWC BBS notices of bugs and fixes as we are informed of them.
We would appreciate being informed of any bugs that you
have discovered and any fixes you have implemented.
.PP
Finally, if you are interested in further support of the tools included with
GNU Tools, you should subscribe the GNU news groups on the Internet, or
you can post to newsgroup
.BR comp.os.coherent .
.SH "Installation"
.PP
Before you begin, please note the following:
.PP
.B
If you are installing both GNU Tools and GNU C onto your system, be sure
to install GNU C \fIfirst\^\fP!
.R
If you fail to do so, problems may arise.
.PP
The rest of these notes assume that you either are installing only the GNU
Tools onto your system, or that you have already installed GNU C release 2.5.6
onto your system.
.PP
To install GNU Tools onto your \*(CO system, use the \*(CO
.B install
command.
You must have roughly 16,000 blocks (eight megabytes) free on
the file system that holds directory
.BR /u1 .
That usually is your root file system, but you can also mount a separate file
system onto that directory should you wish.
If you are going to make heavy use of GNU tools and sources, you may wish
to consider doing so.
.PP
To install the package from 5.25-inch floppy disks in floppy-disk drive 0
(i.e., drive A), use the following command:
.DM
	/etc/install \-c GNUtools /dev/fha0 3
.DE
To install the package from 3.5-inch floppy disks in floppy-disk drive 0,
use the following command:
.DM
	/etc/install \-c GNUtools /dev/fva0 2
.DE
.B install
prompts you to insert each floppy disk in turn into drive A.
It then copies each disk's contents onto your hard disk.
.PP
Finally, you should modify your file
.B /etc/profile
to change the setting of the environmental variable
.BR PATH .
If you want GNU tools to take precedence over the \*(CO commands, place
.B /u1/gnu/bin
at the
.I beginning
of your
.BR PATH ;
otherwise, place it at the end.
.SH Documentation
.PP
To see the suite of commands that are included with this package,
simply type the command:
.DM
	lc /u1/gnu/bin
.DE
This package includes standard manual pages for many of these commands.
It also includes the GNU command
.BR info .
This command is an interactive, Hypertext-like reader
for GNU documentation.
To learn how to use
.BR info ,
simply type:
.DM
	info info
.DE
This tells
.B info
to present a tutorial about itself.
It would be well worth your while to take a few minutes to learn
how to use this command:
once you have learned how to use
.BR info ,
you can easily learn how to use all of the other GNU tools.
.PP
Please note that before you can use
.BR info ,
you must have followed the directions on renaming the file
.BR dir.new ,
as presented during installation.
.SH Conclusion
.PP
Mark Williams Company has ported these tools to \*(CO.
Please note that these tools have been tested under \*(CO;
however, Mark Williams Company makes no guarantee that they are free
of bugs.
\fICaveat utilitor.\fR
.PP
We hope that you enjoy working with these tools.
If you have any comments, fixes, or requests, please mail or E-mail them to
.BR support@mwc.com .
