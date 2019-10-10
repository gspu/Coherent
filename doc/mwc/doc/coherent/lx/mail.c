.TH mail "" "" Command
.PC "Send or read mail"
\fBmail [\-mpqrv] [\-f \fIfile\^\fB] [\fIuser ...\fB]\fR
.PP
.HS
.SH Options:
.IC "\fB\-f \fIfile\fR"
Print mail from \fIfile\fR instead of default
.IC \fB\-m\fR
Notify each logged-in recipient when mail is sent
.IC \fB\-p\fR
Print mail non-interactively
.IC \fB\-q\fR
Exit on interrupt, leaving mail unchanged
.IC \fB\-r\fR
Print mail in reverse order
.IC \fB\-v\fR
Verbose mode:
show version and expanded aliases
.Pp
If \fIuser\fR is present, send each a mail message read from standard input.
Mail message ends with EOF, a line containing only `.',
or a line containing only `?'; the last moves the message into
editor for further editing processing before transmission.
.SH Commands:
.IC \fBd\fR
Delete current message; print the next
.IC "\fBm [\fIuser ...\fB]\fR"
Mail current message to each \fIuser\fR
.IC \fBp\fR
Print this message again
.IC \fBq\fR
Quit and update mailbox
.IC \fBr\fR
Reverse direction of scan through mailbox
.IC "\fBs [\fIfile ...\fB]\fR"
Save current message with header in each \fIfile\fR
.IC "\fBt [\fIuser ...\fB]\fR"
Send message from stdin to each \fIuser\fR
.IC "\fBw  [\fIfile ...\fB]\fR"
Write current message without header in each \fIfile\fR
.IC \fBx\fR
Exit without updating mailbox
.IC \fB<newline>\fR
Print the next message
.IC \fB\-\fR
Print the previous message
.IC EOF
Quit and update mailbox; same as \fBq\fR command
.IC \fB?\fR
Print a command summary
.IC \fB!\fIcommand\fR
Pass \fIcommand\fR to the shell for execution
.HE
.B mail
allows you to exchange electronic mail with other \*(CO system users,
either on your own system or on other systems via \*(UU.
Depending upon its form, this command can be used either to send mail
to other users or to read the mail that other users have sent to you.
.SH "Sending Mail"
If you name one or more
.IR user s,
.B mail
assumes that you wish to send a mail message to each
.IR user .
.B mail
first prints the prompt
.DM
	Subject:
.DE
.PP
on the screen, requesting that you give the message a title.
.PP
.B mail
then reads what you type on the standard input.
A message is terminated by \fB<ctrl-D>\fR,
by a line that contains only the character \*(Ql.\*(Qr,
or by a line that contains only the character \*(Ql?\*(Qr.
Ending with a question mark prompts
.B mail
to feed the message into an editor for further editing.
The editor used is the one named in the environmental variable
.BR EDITOR .
If this variable is not defined,
.B mail
uses \fBed\fR.
.PP
If you have defined environmental variable \fBASKCC\fP to \fBYES\fP,
.B mail
asks you, after a message is ended, for a list of
users to whom you wish to send a copy of the message.
.PP
Finally,
.B mail
prepends the date and the sender's name, and sends the result to each
.I user
named either on the command line or on the carbon-copy list with the
.B rmail
command.
.PP
Each
.I user
who has received mail
is greeted by the message ``You have mail.'' when she logs in.
.B mail
normally changes the contents of the mailbox as the user
works with them; however,
.B mail
has options that allow the contents of the mailbox to remain
unchanged if the user desires.
.SH "Reading Mail"
If no
.I user
is named on its command line,
.B mail
reads and displays the user's mail, message by message.
If environmental variable
.B PAGER
is defined,
.B mail
will \*(QLpipe\*(QR each message through the command it names.
For example, the \fB.profile\fP command line:
.DM
	export PAGER="exec /bin/scat -1"
.DE
.PP
invokes
.B /bin/scat
for each mail message with the command-line argument
.B "-1"
(the digit one).
.PP
While reading mail, the user can use any of the following commands
to save, delete, or send each message to another user interactively.
.IP \fBd\fR 0.5i
Delete the current message and print the next message.
.IP "\fBm\fR\ [\fIuser\fP\ ...]"
Mail the current message to each
.I user
given (default: yourself).
.IP \fBp\fR
Print the current message again.
.IP \fBq\fR
Quit, and update mailbox file to reflect changes.
.IP \fBr\fR
Reverse the direction in which the mailbox is being scanned.
.IP "\fBs\fR\ [\fIfile\ ...\fP]"
Save the current mail message with the usual header in each
.I file
(default:
.BR $HOME/mbox ).
.IP "\fBt\fR\ [\fIuser\fP\ ...]"
Send a message read from the standard input,
terminated by an end-of-file character
or by a line containing only `.' or `?', to each
.I user
(default: yourself).
.IP "\fBw\fR\ [\fIfile\ ...\fP]"
Write the current message without the usual header in each
.I file
(default:
.BR $HOME/mbox ).
.IP \fBx\fR
Exit without updating the mailbox file.
.IP \fB<newline>\fR
Print the next message.
.IP \fB\-\fR
Print the previous message.
.IP \fBEOF\fR
Quit, updating mailbox; same as \fBq\fR.
.IP \fB?\fR
Print a summary of available commands.
.IP \fB!\fIcommand\fR
Pass
.I command
to the shell for execution.
.PP
The following command line options control the sending and reading of mail.
.IP "\fB\-f \fIfile\fR" 0.5i
Read mail from
.I file
instead of from the default,
\fB/usr/spool/mail/\fIuser\fR.
.IP \fB\-m\fR
Send a message to the terminal of
.I user
if he is logged into the system when mail is sent.
.IP \fB\-p\fR
Print all mail without interaction.
.IP \fB\-q\fR
Quit without changing the mailbox if an interrupt character is typed.
Normally, an interrupt character stops printing of the current message.
.IP \fB\-r\fR
Reverse the order of printing messages.
Normally,
.B mail
prints messages
in the order in which they were received.
.IP \fB\-v\fR
Verbose mode.
Show the version number of the
.B mail
program, and display expanded aliases.
.PP
.II .signature
If you wish, you can create a signature file,
.BR .signature ,
in your home directory.
.B mail
appends the contents of the signature file to the end of every mail
message you send, as a signature.
A signature can be your system's path name (for
.B uucp
messages), your telephone number, an amusing \fIbon mot\fR, or what you will.
.SH Files
\fB$HOME/dead.letter\fR \(em Message that \fBmail\fR could not send
.br
\fB$HOME/mbox\fR \(em Default saved mail
.br
\fB$HOME/.signature\fR \(em Signature file
.br
\fB/etc/domain\fR \(em Name of your system's domain
.br
\fB/etc/uucpname\fR \(em Name of your system
.br
\fB/tmp/mail*\fR \(em Temporary and lock files
.br
\fB/usr/spool/mail\fR \(em Mailbox directory, filed by user name
.SH "See Also"
.Xr "aliases," aliases
.Xr "ASKCC," askcc
.Xr "commands," commands
.Xr "EDITOR," editor
.Xr "\&.forward," forward
.Xr "mkfnames," mkfnames
.Xr "msg," msg
.Xr "nptx," nptx
.Xr "PAGER," pager
.Xr "paths," paths
.Xr "rmail," rmail
.Xr "smail," smail
.Xr uux uux
.SH Notes
Note that before you can send mail, either locally or to a remote site,
you must run the program
.B uuinstall
and use its `S' option to set the name of your local site and domain.
Your local system must, of course, also have permission to log into any
remote site to which you wish to send mail.
See the tutorial and Lexicon articles on \*(UU for details on using \*(UU
to exchange mail and files with remote sites.
