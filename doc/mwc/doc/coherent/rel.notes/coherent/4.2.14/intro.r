.ds TL Introduction
.ds TI "RELEASE NOTES"
.NH "Introduction"
.PP
Congratulations on your purchase of \*(CO release 4.2.14!
\*(CO gives your 80386- or 80486-based system a fully multi-tasking,
multi-user operating system \(em at a fraction of the cost of other systems.
We think you will appreciate the fact that \*(CO is powerful
yet inexpensive, efficient, and easy to install.
.PP
This booklety shows you how to install \*(CO on your system.
It also includes information you should find helpful in using \*(CO.
This includes information on how to contact Mark Williams Technical
Support, and how to access the Mark Williams Bulletin Board.
.I
Read these notes carefully before you attempt to install or update
\*(CO on your computer.
.R
.SH "Frequently Asked Questions"
.PP
Before we begin, please note the following:
.IP \fB1.\fR 0.3i
If you need to telephone Mark Williams Technical Support,
dial 1-708-291-6700.
Please do not telephone on the Mark Williams sales number.
.IP \fB2.\fR
If you are updating your \*(CO system, you will
.I not
have received a card with a serial number on it.
Updating your system does not require that you enter your system's
serial number, so this will not be a problem.
.SH "How to Use This Booklet"
.PP
This booklet consists of the following sections:
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
This section is for users who already have \*(CO,
and are upgrading to \*(CO release 4.2.14.
.IP \fB5.\fR
Third-party software.
This section details software available for \*(CO,
including those available from Mark Williams Company and
from third-party vendors.
This chapter also describes how to get free \*(CO software from the
Internet and from the Mark Williams bulletin board.
.IP \fB6.\fR
Running third-party software.
This gives directions on to make any necessary minor configuration changes
so you can install and run the most popular third-party packages.
.\".IP \fB7.\fR
.\"Bulletin board and user groups.
.\"This section describes how to dial into the Mark Williams Bulletin Board.
.\"This BBS gives you access to public-domain software that has been
.\"ported to \*(CO.
.\"It gives new versions of device drivers,
.\"technical-support bulletins, product announcements, and bug fixes.
.\"You can also exchange mail with the Mark Williams technical support staff
.\"and other \*(CO users.
.\".IP
.\"This section also introduces \*(CO user groups.
.\"This section describes \*(CO user groups throughout the world,
.\"and how you can contact them.
.\"It also describes how you can form a \*(CO user group and register it
.\"with Mark Williams Company.
.IP \fB7.\fR
Notes and errata.
This section describes helpful hints
that were discovered after the manual went to press.
This section also lists bugs that have been found in this version of \*(CO,
but have not yet been fixed.
Please note that as soon as a bug is fixed, you can download the corrected
version of the program in question from the Mark Williams BBS.
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
This includes make and model for your motherboard, BIOS, hard drive,
and any add-in cards.
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
computer turned on, so we can ``walk you through''
any diagnostic or repair measures needed.
.IP \fB7.\fR
For information on how to contact Mark Williams Company,
see the copyright page of this booklet.
