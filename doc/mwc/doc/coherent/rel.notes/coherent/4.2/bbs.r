.ds TL "Bulletin Board"
.NH "Mark Williams Bulletin Board" 1
.PP
Mark Williams Company has set up a \*(CO \*(UU node for dial-up
use by our customers.
It serves three major purposes:
.IP \fB1.\fR 0.3i
As a remote site with which you can test your \*(UU configuration.
.IP \fB2.\fR
As a source of news, bug fixes, and public-domain software for you to download.
In addition, the BBS lists new \*(QLthird party\*(QR
software and hardware that work with \*(CO.
.IP \fB3.\fR
As a mail drop for customers who request an account.
If you wish,
you can can exchange electronic mail with MWC and other \*(CO users.
.SH "Accessing the MWC BBS"
.PP
Before you try to access the MWC BBS, note that you
.I must
use \*(UU.
You cannot use
.BR ckermit ,
.BR Procomm ,
or any other communications utility.
.PP
Before you proceed, please review the tutorial on \*(UU in the \*(CO manual.
It includes an exhaustive tutorial on this powerful utility and uses
.B mwcbbs
throughout it.
In most cases, the tutorial gives you all the information you need.
.PP
You can access
.B mwcbbs
through the following three telephone numbers:
.DS
.ta 0.5i 3.5i
	1200/2400-baud generic modem:	708-559-0412
	9600-baud Trailblazer modem:	708-559-0445
	9600-baud V.32 or HST modem:	708-559-0452
.DE
.SH "Downloading Files"
.PP
Once you have succeeded in contacting
.BR mwcbbs ,
you can begin to exchange files and mail with Mark Williams personnel,
and with other \*(CO users.
.PP
For your initial contact with \fBmwcbbs\fR, we recommend that you download
the introductory message \fBhowto.start\fR from directory
.BR /usr/spool/uucppublic/mwcnews .
For example, the command:
.DM
	uucp -r mwcbbs!/usr/spool/uucppublic/mwcnews/howto.start /usr/spool/uucppublic
.DE
queues up a request to copy file \fBhowto.start\fR into directory
.B /usr/spool/uucppublic
on your system.
This file contains information on available downloads, new \*(CO
developments, and how to obtain an individual account on the system.
If the time at which you type the \fBuucp\fR command is within the range
of times allowed in the entry for
.B mwcbbs
in
.BR /usr/lib/uucp/sys ,
your modem will call \fBmwcbbs\fR immediately \(em
you can check this with the commands \fBps\fR or \fBuulog\fR.
If you wish to force polling of mwcbbs at a time not specified in
.BR /usr/lib/uucp/sys ,
you can do so with the command:
.DM
	su uucp /usr/lib/uucp/uucico -Smwcbbs  &
.DE
You can then monitor the progress of the transfer via the command:
.DM
	uulog -f mwcbbs
.DE
To stop monitoring the call, type
.BR <ctrl-C> .
.SH "Electronic Mail Access"
.PP
The following electronic mail addresses are available to customers.
.PP
For issues relating to administration of the BBS node, send mail to
.BR mwcbbs!admin .
.PP
For matters relating to support of \*(CO and related
products, send mail to
.BR mwcbbs!support .
.PP
Additional mail aliases may appear from time to time.
Please remember that mail sent to either of the aforementioned addresses
will be sent to \fBmwcbbs\fR only when you invoke
.BR uucico .
You can do this interactively from the \*(CO shell,
as shown in the example above, or at regular intervals as a \fBcron\fR task.
The following gives a sample line for your
.B cron
file:
.DM
	15 20 * * * su uucp /usr/lib/uucp/uucico -smwcbbs
.DE
This line will check every day at 8:15 PM for
.B uucp
and
.B mail
requests which have been queued for
.BR mwcbbs ,
and, if any are found, will call \fBmwcbbs\fR.
See the Lexicon article
.B cron
for further information.
.PP
Please note that while the BBS supports inbound electronic mail from
your system to
.B mwcbbs
via anonymous \*(UU,
we cannot reply to your electronic mail messages unless
you have an account on
.BR mwcbbs .
To request an account on
.BR mwcbbs ,
download the file
.B howto.start
from directory
.B /usr/spool/uucppublic/mwcnews
on
.BR mwcbbs ,
and follow its directions.
