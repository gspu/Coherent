.ds TL "Introduction"
.ds TI COHULIP
.NH "Introduction"
.PP
This manual introduces the \*(CO User-Level Internet Program (\fBcohulip\fR).
.PP
.B cohulip
is a user-level application that lets you perform TCP/IP networking tasks
under the \*(CO operating system.
It supports the IP, ICMP, TCP, UDP, FTP, and TELNET protocols,
as well as the ARP protocol for address resolution.
.PP
Please note that
.B cohulip
is no substitute for true in-kernel networking;
however, for systems that have limited networking needs (e.g., a \*(CO
system in a user's basement), it can perform some useful networking tasks.
With it, you can connect to other systems via Ethernet, serial lines, or
modem; and other users on remote systems can access your system.
.PP
.BR cohulip 's
limited capacity is also helpful if you are new to networking.
Its relatively simple structure, plus the fact that it runs as a user-level
application, makes it easy to install, configure, and learn.
We have found
.B cohulip
to be a good way to introduce beginners to the confusing but fascinating world
of networking.
.PP
.II "Wright, Randy"
.B cohulip
was written by Randy Wright (rw@rwsys.wimsey.bc.ca).
.II "Karns, Phil"
It is derived from KA9Q, a package that implements TCP/IP over a
packet-radio network, which was written by Phil Karns plus many collaborators.
.B cohulip
differs from KA9Q in the following respects:
.IP \(bu 0.3i
All 16-bit objects have been altered to 32 bits.
.IP \(bu
All code to support packet radio has been removed, including
.B ax25
and
.BR netrom .
.IP \(bu
Code has been inserted to allow the package to interact with the
device-driver
.BR ne ,
which drives the NEC NE2000 Ethernet card and its clones.
.IP \(bu
Flow-control was rewritten to a form more suited to \*(CO (and \*(UU).
.PP
The result is a simpler, faster implementation that is suitable for LAN usage.
Under \*(CO, the speed of file transfer increases
approximately by a factor of ten, but with considerly less impact on
system performance.
However, due to these changes,
.B cohulip
cannot be compiled under \*(MD.
.SH "Using This Manual"
.PP
.II "Garbee, Bdale"
.II "Buswell, Joe"
.II "Butzen, Fred"
This manual was prepared by Fred Butzen of Mark Williams Company
(fred@lepanto.mwc.com).
It is based on the documentation assembled by Bdale Garbee and
revised by Joe Buswell.
The manual has been heavily edited and rewritten, both make it conform to
.BR cohulip 's
dimensions, and to improve its organization and readability.
.II "Hedrick, Charles L."
The chapter
.IR "TCP/IP History and Design"
is based on the document
.I "Introduction to the Internet Protocols"
written by Charles L. Hedrick of Rutgers University.
The portions of that chapter taken from
his document are copyright \(co 1988 by Charles Hedrick,
and are used by permission.
.PP
The chapters
.I "Installing cohulip"
and
.I "Advanced cohulip"
include much information culled from the oral tradition among
.B cohulip
users, especially as posted to newsgroup
.BR comp.os.coherent .
.II "Chalmers, Robert"
The editor wishes especially to thank Randy Wright for his
explanations of
.BR cohulip 's
internal workings,
and Robert Chalmers
(earth@nanguo.cstpl.com.au) of Mackay, Australia, for his detailed
and humorous accounts of his grapplings with earlier releases of
.BR cohulip .
.PP
Please note that all errors in this manual are the
responsibility of its editor, not of the original authors.
.PP
This manual contains the following sections:
.IP "\fITCP/IP History and Design\fR"
This gives an overview of TCP/IP.
Although much of this chapter is dry and technical, we strongly urge you
to read it.
It will help you understand just what networking and TCP/IP are, and
therefore why
.B cohulip
does what it does.
It will also help you understand some of the many obscure terms and anagrams
that shroud every discussion of networking.
.IP "\fIInstalling cohulip\fR"
This chapter describes how to install
.B cohulip
onto your system, and how to configure it for various combinations of
networks and hardware.
It also explains
.BR cohulip 's
suite of configuration files, and walks you through modifying them.
.IP "\fIConfiguration Files\fR"
This chapter introduces the configuration files that
.B cohulip
uses.
Configuring
.B cohulip
largely consists of editing these files.
.IP "\fIRunning cohulip\fR"
This chapter describes in detail the program
.BR net .
This program actually performs the networking tasks.
You can also use this chapter as a reference for
.B net
while you are running it.
.IP "\fIAdvanced cohulip\fR"
This discusses some advanced topics with
.BR cohulip ,
such as connecting it to \*(MD or connecting with two networks simultaneously.
.PP
This manual, like
.B cohulip
itself, is a work in progress.
The editor welcomes your comments, especially if you have found an error.
Please mail your jeremiads, philippics, and panegyrics to the editor at
fred@lepanto.mwc.com.
.SH "Bibliography"
.PP
The literature on TCP/IP is vast; much of it is impenetrable.
However, we strongly recommend that you acquire the following three books:
.IP \(bu 0.3i
Kehoe, Brendan P.:
\fIZen and Art of the Internet:
A Beginner's Guide to the Internet.\fR
This free book is unpublished, but is widely available on the Internet
in various forms and editions.
It is an excellent brief introduction to the Internet and its lore.
.IP \(bu
Hunt, Craig:
\fITCP/IP Network Administration.\fR
Sebastopol, Calif., O'Reilly & Associates, Inc., 1992.
This book details how to set up and administer a large TCP/IP-based network.
You may not have such a network yourself, but this book will greatly
help you work with one.
.IP \(bu
Krol, Ed:
\fIThe Whole Internet:
User's Guide & Catalog.\fR
Sebastopol, Calif., O'Reilly & Associates, Inc., 1993.
This book is an ``instant classic''.
It is by far the best one-volume guide to the Internet, and is
worthwhile for both beginners and experienced users.
If you buy no other book on this subject, buy this one.
