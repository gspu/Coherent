.ds TL Introduction
.NH "Introduction"
.PP
Congratulations on your purchase of \*(CO release 4.2!
\*(CO gives your 80386- or 80486-based system a fully multi-tasking,
multi-user operating system \(em at a fraction of the cost of other systems.
We think you will appreciate the fact that \*(CO is powerful
yet inexpensive, efficient, and easy to install.
.PP
This pamphlet shows you how to install \*(CO on your system.
It also includes information you should find helpful in using \*(CO.
This includes information on how to contact Mark Williams Technical
Support, and how to access the Mark Williams Bulletin Board.
\fIRead these notes carefully before you attempt to install or update
\*(CO on your computer.\fR
.SH "Frequently Asked Questions"
.PP
Before we begin, please note the following:
.IP \fB1.\fR 0.3i
If you need to telephone Mark Williams Technical Support, dial
1-708-291-6700.
Please do not telephone on the Mark Williams sales number.
.IP \fB2.\fR
If you are updating your \*(CO system, you will
.I not
have received a card with a serial number on it.
This is because you must use the serial number of your current \*(CO system.
If you do not know your system's serial number, read the file
.BR /etc/serialno
and write down its contents on a piece of paper.
Be sure to write the number correctly, or \*(CO will complain when you
attempt to update it.
.SH "Recent Enhancements"
.PP
\*(CO includes many new features:
.IP \(bu 0.3i
Improved support for third-party packages.
The UNIX System-V editions of Lotus and WordPerfect now work out of the box.
.IP \(bu
Support for X Windows.
An X Windows package is now available for \*(CO from Mark Williams Company.
.IP \(bu
A rewritten version of the \*(CO kernel.
The kernel now includes
\*(ST, the \*(UN system for portable device drivers;
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
With release 4.2, you can change kernel variables, add or delete drivers,
and modify driver variables, then build a new bootable kernel.
.IP \(bu
Support for peripheral devices has been expanded.
Devices support now include
SCSI tape and a variety of SCSI devices.
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
\fBalmanac\fR	Print a listing of the events on today's date in history
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
.SH "How to Use These Notes"
.PP
This pamphlet consists of the following sections:
.IP \fB1.\fR 0.3i
The introduction \(em that is, the section you are reading right now.
.IP \fB2.\fR
List of hardware.
This section lists hardware that we know works with \*(CO, and
equipment that we know does \fInot\fR work with \*(CO.
Check your equipment against this list
.I before
you begin installation.
.IP \fB3.\fR
Installation guide.
This section walks you through the process of installing \*(CO
on your computer.
\fIThis section is for users who do not yet have \*(CO
on their computers.\fR
In the off chance that something goes wrong during installation,
this section also describes problems that have occurred to some
users, and how to work around them.
.IP \fB4.\fR
Update guide.
\fIThis section is for users who already have \*(CO,
and are upgrading to \*(CO release 4.2.\fR
.IP \fB5.\fR
Third-party software.
This section details software available for \*(CO,
including those available from Mark Williams Company and
from third-party vendors.
Because \*(CO can run many packages
designed for \*(UN, the list of software
that runs on \*(CO is growing daily.
Check the Mark Williams BBS for the latest list of available software.
.IP \fB6.\fR
Running third-party software.
This gives directions on to make any necessary minor configuration changes
so you can install and run the most popular third-party packages.
.IP \fB7.\fR
Bulletin board and user groups.
This section describes how to dial into the Mark Williams Bulletin Board.
This BBS gives you access to public-domain software that has been
ported to \*(CO.
It gives new versions of device drivers,
technical-support bulletins, product announcements, and bug fixes.
You can also exchange mail with the Mark Williams technical support staff
and other \*(CO users.
.IP
This section also introduces \*(CO user groups.
This section describes \*(CO user groups throughout the world,
and how you can contact them.
It also describes how you can form a \*(CO user group and register it
with Mark Williams Company.
.IP \fB8.\fR
Notes and errata.
This section describes helpful hints
that were discovered after the manual went to press.
This section also lists bugs that have been found in this version of \*(CO,
but have not yet been fixed.
Please note that as soon as a bug is fixed, you can download the corrected
version of the program in question from the Mark Williams BBS.
It also summarizes corrections and changes in the Lexicon that have been
made since the manual was last printed, and that are available in the
on-line Lexicon.
.PP
Read these notes through before you do anything else.
Then check your hardware against the machines listed in the following section.
If you do not find a discrepancy between your machine and what is given in
this section, then continue on to the installation or upgrade section,
and load \*(CO onto your system.
.SH "Contacting Mark Williams Technical Support"
.PP
Mark Williams Company
has a team of technical support specialists who are skilled at helping you
solve the problems you encounter with \*(CO.
.PP
The technical support staff try to respond promptly to
all inquiries, whether by telephone, FAX, e-mail, or ordinary mail.
Sometimes, however,
we receive e-mail from a customer only to find that our reply message
is undeliverable.
Therefore, if you send us e-mail,
please also include a FAX number or telephone number, or mailing address.
This will help ensure that we can contact you with our reply;
after all, if we can't reach you, we can't help you solve your problem.
.PP
To get the most out of
your discussion with Technical Support, please follow these guidelines:
.IP \fB1.\fR 0.3i
Have your \*(CO serial number at hand.
.IP \fB2.\fR
Know the hardware configuration you are using.
This includes make and
model for your motherboard, BIOS, hard drive, and any add-in cards.
.IP \fB3.\fR
Have an exact description of the problem.
If you are getting an error message or \*(QLpanic\*(QR message,
write down \fIexactly\fR what appears on the screen.
If a certain file, such as a C program or \fBnroff\fR document,
causes a problem, try to whittle the file down to the smallest size that
duplicates the problem, then mail the file on a floppy disk to Mark Williams
Company.
.IP \fB4.\fR
If you are having problems with \*(UU, please log in as
.B root
(the super-user) and enter the command
.BR uulog .
If you continue to have problems, have available (and if possible, FAX
to us) the contents of the following files:
.BR /etc/ttys ,
.BR /usr/lib/uucp/sys ,
.BR /usr/lib/uucp/port ,
and
.BR /usr/lib/uucp/dial .
Also, execute the following command to log file permissions on
key \*(UN files:
.DM
	ls -lR /dev/com* /usr/lib/uucp /usr/spool/uucp >/tmp/MWC.info
.DE
Fax a hardcopy of file
.B /tmp/MWC.info
or copy the file onto a floppy
and mail it to the Mark Williams Company Technical Support department.
.IP \fB6.\fR
If possible, call us when you are at your computer, with the
computer turned on, so we can ``walk you through'
any diagnostic or repair measures needed.
.IP \fB7.\fR
For information on how to contact Mark Williams Company,
see the copyright page of this pamphlet.
