.TH uucp "" "" Command
.PC "Spool files for transmission to other systems"
\fBuucp [ \-cCdfmr ] [\-n\fIuser\^\fB] [\-x\fIactivity\^\fB] \fIsource ... dest\fR
.PP
.HS
.SH Options:
.IC "\fB\-c\fR"
Do not copy source to spool directory; rather use the file itself
.IC "\fB\-C\fR"
Copy source file to spool directory (default; same as \fB\-p\fR)
.IC "\fB\-d\fR"
Create directories as required on destination
.IC "\fB\-f\fR"
Do not make intermediate directories; fail if they do not exist
.IC "\fB\-I \fIfile\fR"
Use \fIfile\fR as configuration file
.IC "\fB\-j\fR"
Report job's process identifier
.IC "\fB\-m\fR"
Send mail to requester when file is sent
.IC "\fB\-n\fIuser\fR"
Notify \fIuser\fR (on destination system) when file received
.IC "\fB\-p\fR"
Copy source file to spool directory (default; same as \fB\-C\fR)
.IC "\fB\-R\fR"
Copy directories recursively
.IC "\fB\-r\fR"
Spool transfer request, do not initiate \fBuucico\fR
.IC "\fB\-s \fIfile\fR"
Write status upon completion of job into \fIfile\fR
.IC "\fB\-u \fIuser\fR"
Set user name to \fIuser\fR
.IC "\fB\-W\fR"
Do not add the current directory to file names on the remote system
.IC "\fB-x\fIactivity\fR"
Log a given \*(UU \fIactivity\fR, for debugging purposes
.HE
The command
.B uucp
spools every file
.I source
for copying to
.IR dest .
.I source
and
.I dest
can specify a remote system.
.PP
.B uucp
recognizes the following options:
.IP \fB\-C\fR 0.3i
Copy the source file into spool directory; same as option
.BR \-p .
This is the default.
.IP \fB\-c\fR
Do not copy the source file into spool directory; rather, use the file itself.
The file must be readable both by yourself and by the daemon
.BR uucico .
If the file is removed before
.B uucico
processes it, the transmission of the file will fail.
.IP \fB\-d\fR
Create directories as required on the destination system.
This is the default.
.IP \fB\-f\fR
Do not create any directories on the remote system.
If directories do not already exist, abort copying the file.
.IP \fB\-g\fIgrade\fR
Assign a grade (a single ASCII character, from `0' through `z')
to indicate the importance of the file being transmitted.
The lower the ASCII value of
.IR grade ,
the more important the file; thus, `0' is the highest grade and `z' the lowest.
.IP "\fB\-I \fIfile\fR"
Read the configuration for the remote system from
.I file
instead of from
.BR /usr/lib/uucp/sys ,
which is the default.
.IP "\fB\-j\fR"
Report the job's process identifier.
If you wish, you can use this identifier with the command
.B uustat
to kill the job.
.IP \fB\-m\fR
Send mail to requester when the file is sent; report whether the job
was executed successfully.
.IP \fB\-n\fIuser\fR
Send mail to
.I user
on destination system when the file is received.
.I user
can contain a path.
Note that
.I user
is relative to the destination machine, not to originating machine or to
any intervening machine.
For example, consider the command:
.DM
	uucp -nlepanto!fred myfile joe!/tmp
.DE
.IP
Here, you are copying
.B myfile
from your machine
into directory
.B /tmp
on machine
.BR joe ,
and sending notification to user 
.B fred
on machine
.BR lepanto .
If, however, machine
.B joe
does not know how to address machine
.BR lepanto ,
then
.B fred
will never be notified of the transfer.
.IP \fB\-p\fR
Copy the source file into spool directory; same as
.BR \-C .
This is the default.
.IP "\fB\-R\fR"
Copy directories recursively.
.IP \fB\-r\fR
Spool transfer request, but do not initiate
.BR uucico .
.IP "\fB\-s \fIfile\fR"
Write status upon completion of job into \fIfile\fR.
.IP "\fB\-u \fIuser\fR"
Set user name to \fIuser\fR.
.IP "\fB\-W\fR"
Do not prefix the file's name with the name of the current directory.
.IP \fB\-x\fIactivity\fR
Log a given
.IR activity .
These logs can help you debug problems with \*(UU.
.B uucp
recognizes the following activities:
.RS
.IP \fBabnormal\fR
Log abnormal events that occur while spooling a file for copying.
.IP \fBconfig\fR
Log problems that arise with reading or interpreting the configuration files
.BR dial ,
.BR port ,
and
.BR sys .
.IP \fBexecute\fR
Log each step
.B uucp
takes as it executes.
.IP \fBspooldir\fR
Log activity that involved the \*(UU spooling directory
.B /usr/spool/uucp
and its subdirectories.
.RE
.IP
.II "uucp^logging file"
.II /usr/spool/uucp/.Admin/audit.local
.B uucp
writes its logging information into file
.BR /usr/spool/uucp/.Admin/audit.local .
.SH Examples
The first example copies file
.B foo
to directory
.B /bar
on system
.BR george :
.DM
	uucp foo george!/bar
.DE
.PP
The next example copies file
.B /foo
from system
.B george
into directory
.B /tmp
on your system:
.DM
	uucp george!/foo /tmp
.DE
.PP
The next example copies file
.B /foo
from system
.B george
into file or directory
.B /bar
on system
.BR ivan :
.DM
	uucp george!/foo ivan!/bar
.DE
.PP
Note that this assumes your system can talk to both
.B george
and
.B ivan
and that your system has permission to read file
.B /foo
on system
.B george
as well as to write file
.B /bar
on system
.BR ivan .
.PP
The next example downloads files
.B /foo
and
.B /bar
from remote systems
.B ivan
and
.B george
into directory
.B /tmp
on your system:
.DM
	uucp ivan!/foo george!/bar /tmp
.DE
.PP
The last example downloads file
.B foo
from system
.B ivan
via system
.BR george :
.DM
	uucp george!ivan!foo
.DE
.PP
For an example of using the command
.B find
with
.B uucp
to spool files automatically, see the entry for
.BR find .
.SH Files
\fB/usr/lib/uucp/sys\fR \(em List of reachable systems
.br
\fB/usr/spool/uucp/.Log/*/\fIsitename\fR \(em \fBuucp\fR activities log files for \fIsitename\fR
.br
\fB/usr/spool/uucp/\fIsitename\fR \(em Spool directory for work
.SH "See Also"
.Xr "commands," commands
.Xr "mail," mail
.Xr "uucico," uucico
.Xr "UUCP," uucp
.Xr "uudecode," uudecode
.Xr "uuencode," uuencode
.Xr "uutouch," uutouch
.Xr "uuxqt" uuxqt
.SH Notes
.II "Taylor, Ian Lance"
.B uucp
was written by Ian Lance Taylor (ian@airs.com).
