.ds TL Introduction
.NH "Introduction"
.PP
This manual documents version 4.2 of the \*(CO Device-Driver Kit.
This kit is designed to help you write device drivers for \*(CO release 4.2.
It describes the contents of the kit, introduces the \*(CO kernel,
gives advice on how to go about writing a device driver,
gives examples of device drivers, and documents all of the
kernel's accessible functions in Lexicon format.
.PP
Before you continue, please read the following carefully:
.QS
.I
This kit will not teach you how to write a device driver.
It is to be used only by persons who are technically knowledgeable.
Due to the highly specialized nature of device drivers, this product is
not eligible for technical support from Mark Williams Company.
.R
.QE
If you discover a bug in the product or you have a suggestion on how
it can be improved, please contact Mark Williams Company.
If you run into a difficulty with the hardware for which you are
writing the driver, please consult that hardware's technical-reference
manual or contact its manufacturer.
.PP
Further, a bug in a device driver can inflict great damage on an operating
system and its files.
You should expect that during development, you will damage the contents
of your hard disk at least once.
Therefore, we implore you to practice defensive programming
in designing and testing your device driver, to protect irreplacable
files from damage or destruction.
This manual will give you suggestions on how to do this most easily.
.SH "Changes From Earlier Releases"
.PP
Release 4.2 completely changes the \*(CO Device-Driver Kit:
.IP \(bu 0.3i
Tools for modifying and relinking the \*(CO kernel are now included within
the \*(CO system itself.
.IP \(bu
.II DDI/DKI^definition
Release 4.2 introduces the \*(CO implementation of the \*(UN Device Driver
Interface/Driver-Kernel Interface (DDI/DKI), and \*(ST.
We now discourage programmers from using internal kernel calls within their
drivers unless absolutely necessary.
.SH "The Kit"
.PP
The \*(CO Device Driver Kit consists of the following:
.IP \(bu 0.3i
Source code for most \*(CO device drivers.
.IP \(bu
Configuration files for the device drivers.
.IP \(bu
This manual.
.SH "Installing the Device Driver Kit"
.PP
The \*(CO Device Driver Kit requires that you are currently
running a production copy of \*(CO release 4.2.
.PP
To install the \*(CO Device Driver Kit, log in as the superuser
.BR root .
Then type the command:
.DM
	/etc/install -c Drv_420 /dev/fha0 1
.DE
for a 5.25-inch floppy-disk drive, or
.DM
	/etc/install -c Drv_420 /dev/fva0 1
.DE
for a 3.5-inch floppy-disk drive.
.PP
The installation program will prompt you to insert the write-protected
floppy disk into the device you named on the command line.
After the installation completes, place your distribution disk in
a safe place, away from heat or magnetic fields.
.SH "Using This Manual"
.PP
This manual consists of six sections:
.IP \fB1.\fR 0.3i
The introduction \(em the section you are reading now.
.IP \fB2.\fR
A sketch of the \*(CO kernel, and how it works.
.IP \fB3.\fR
Writing a device driver using internal kernel calls.
This chapter describes methods that must be used to write block drivers.
.IP \fB4.\fR
Example of a block driver.
.IP \fB5.\fR
Example of a character driver.
.IP \fB6.\fR
The Lexicon.
This gives an entry for each DDI/DKI, \*(ST, or internal-kernel function
or macro.
It also contains overview articles, which introduce classes of functions
or macros, and articles that summarize technical information.
Note that you can use the command
.B man
to view these articles;
and you can view their calling conventions by invoking the command
.BR help .
.PP
As noted above, this manual will not teach you how to write a device driver.
If you are seeking a tutorial, we suggest you look at one of the volumes
listed below.
We hope, however, that you will find the tutorials a helpful guide to
the \*(CO kernel and resources, and the Lexicon a useful summary.
.SH Bibliography
.PP
The following reference manuals discuss the writing of \*(UN device drivers,
the Intel 80386 microprocessor, \*(ST, and related topics.
.PP
Intel Corporation:
\fI386 EX Programmer's Reference Manual\fR.
Santa Clara, Ca.: Intel Corporation, 1990 (part 230985-002).
.PP
Campbell, J.:
\fIC Programmers Guide to Serial Communication\fR.
Indianapolis:
Howard Sams & Company, 1989
(ISBN 0-67222-584-0).
.PP
Crawford, J.; Gelsinger, P.:
\fIProgramming the 80386\fR.
SYBEX Incorporated, 1987 (ISBN 0-89588-381-3).
.PP
Plauger, P.:
Evaluating device controllers.
\fIEmbedded Systems Programming,\fR
March 1991, pp 87-92.
.PP
Comer, D.:
\fIOperating System Design: The XINU Approach.\fR
Englewood Cliffs, NJ:
Prentice Hall, Incorporated, 1984 (ISBN 0-13-637539-1).
.PP
Egan, J.; Teixeira, T.:
\fIWriting A UNIX Device Driver.\fR
Englewood Cliffs, NJ:
John Wiley and Sons, Incorporated, 1988 (ISBN 0-471-62859-X).
.PP
\*(UN System Laboratories, Incorporated:
\fIDevice-Driver Interface/Driver-Kernel Interface Reference Manual for
Intel Processors.\fR
Englewood Cliffs, NJ:
Prentice-Hall, Incorportaed, 1992.
.PP
AT&T:
\fIUNIX System V STREAMS Primer.\fR
Englewood Cliffs, NJ:
Prentice-Hall, Incorporated, 1987.
.PP
AT&T:
\fIUNIX System V STEAMS Programmer's Guide.\fR
Englewood Cliffs, NJ:
Prentice-Hall, Incorporated, 1987.
.PP
\*(UN System Laboratories, Incorporated:
\fIUNIX System V Release 4 Programmer's Guide:
STREAMS.\fR
Englewood Cliffs, NJ:
Prentice-Hall, Incorporated, 1990.
.PP
Ritchie, D.:
A stream input-output system, in
\fIUnix Research System, Volume II: Papers\fR
(ed. 10).
Murray Hill, NJ:
Computing Research Center, AT&T Bell Laboratories, 1990.
