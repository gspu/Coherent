.TH write "" "" Command
.PC "Converse with another user"
\fBwrite \fIuser \fB[ \fItty \fB]\fR
.PP
.HS
Name the \fItty\fR if \fIuser\fR is logged in on more than one port.
.HE
The \*(CO system provides several commands that allow
users to communicate with each other.
.B write
allows two logged-in users to have an extended, interactive conversation.
.PP
.B write
initiates a conversation with
.IR user .
If
.I tty
is given,
.B write
looks for the
.I user
on that terminal; this is useful if a user is marked as
being logged in on more than one device.
Otherwise,
.B write
holds the conversation with the first instance of
.I user
found on any tty.
.PP
If found,
.B write
notifies
.I user
that you are beginning a conversation with him.
All subsequent lines typed into
.B write
are forwarded to the
.IR user 's
terminal, except lines beginning with `!',
which are sent to the shell
.BR sh .
Typing end of file (usually
.BR <ctrl-D> )
terminates
.B write
and sends
.I user
the message ``EOT'' to tell him that communication has ended.
.PP
Two users typing lines to
.B write
at about the same time can cause extreme confusion,
so users should agree on a protocol to limit when each is typing.
The following protocol is suggested.
One user initiates a
.B write
to another,
and waits until the other user replies before beginning.
The first user then types until he wishes a reply and suffixes ``o''
(over) to indicate he is through.
The other user does the same, and the conversation alternates
until one user wishes to terminate it.
This user types ``oo'' (over and out).
The other user replies in the same way, indicating he too is finished.
Finally each of the users leave
.B write
by typing end-of-file (usually
.BR <ctrl-D> ).
.PP
Any user may deny others the permission to
.B write
to his terminal by using the command
.BR mesg .
.SH Files
\fB/etc/utmp\fR
.br
.B /dev/*
.SH "See Also"
.Xr "commands," commands
.Xr "mail," mail.c
.Xr "mesg," mesg
.Xr "msg," msg
.Xr "sh," sh
.Xr "wall," wall
.Xr "who" who
.SH Notes
You should use
.B write
only for extended conversations.
Use
.B msg
to send brief communications to a logged in user, and
.B mail
to communicate with a user who is not logged in.
.B wall
broadcasts a message to all logged in users.
