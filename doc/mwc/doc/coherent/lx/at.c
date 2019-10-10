.TH at "" "" Command
.PC "Execute commands at given time"
\fBat \fR[ \fB\-v \fR] [ \fB\-c \fIcommand\fR ] \fItime\fR [ [ \fIday \fR] \fIweek\fR ] [ \fIfile \fR]
.br
\fBat \fR[ \fB\-v \fR] [ \fB\-c \fIcommand\fR ] \fItime month day\fR [ \fIfile \fR]
.PP
.HS
.SH Options:
.IC \fB\-c\fR
Following argument gives command
.IC \fB\-v\fR
Print time for which command is set
.Pp
If \fIfile\fR is given, read commands from it.
If neither \fIfile\fR nor \fB\-c\fR is given, read commands from stdin.
.HE
.B at
executes commands at a given time in the future.
.PP
If the
.B \-c
option is used, \fBat\fR executes the following
.IR command .
If
.I file
is named,
.B at
reads the commands from it.
If neither is given,
.B at
reads the standard input for commands.
.PP
If
.I time
is a one-digit or two-digit number, \fBat\fR interprets it as
specifying an hour.
If
.I time
is a three-digit or four-digit number, \fBat\fR interprets it
as specifying an hour and minutes.
If \fItime\fR is followed by
\fBa\fR, \fBp\fR, \fBn\fR, or \fBm\fR,
.B at
assumes \fBAM\fR, \fBPM\fR, \fBnoon\fR, or \fBmidnight\fR, respectively;
otherwise,
it assumes that
.I time
indicates a 24-hour clock.
Note that you should
.I not
type a colon `:' in the time string.
.PP
For example, the command
.DM
	at -c "time | msg henry" 1450
.DE
.PP
set the \fBtime\fR command to be executed at 2:50 PM, and pipe \fBtime\fR's
output to the \fBmsg\fR command, which will pass it to the terminal
of user \fBhenry\fR.
The argument to the \fB\-c\fR option had to be enclosed in quotation
marks because it contains spaces and special characters;
if this were not done, \fBat\fR would
not be able to tell when the argument ended, and so would generate an
error message.
If you wish to pass
information to a user's terminal with the \fBat\fR command, you must
tell \fBat\fR to whom to send the information.
The command
.DM
	at 250p commandfile
.DE
.PP
sets the file \fBcommandfile\fR to be read and executed at 2:50 PM.
It is \fInot\fR necessary to use the file's full path name.
Also, if the suffix \fBp\fR were not appended to the time, the file
would be set to be read at 2:50 AM.
.PP
The time set in \fBat\fR's command line is \fInot\fR
the exact time that the command is executed.
.II atrun
Rather, the daemon
.B cron
periodically executes the command
.B /usr/lib/atrun
to see if any commands have been
scheduled commands to be executed at or before the present time.
The frequency with which
.B cron
executes
.B atrun
determines the ``granularity'' of
.B at
execution times.
To change when
.B cron
executes
.BR atrun ,
edit file
.BR /usr/spool/cron/crontabs/root .
For example, the entry
.DM
	0,5,10,15,20,25,30,35,40,45,50,55 * * * *   /usr/lib/atrun
.DE
.PP
sets
.B /usr/lib/atrun
to be executed every five minutes.
Thus, the
.B at
command that is set, for example, to 2:53 PM will
actually be executed at 2:55 PM.
.B atrun
executes specified commands when it discovers that the given time is
past; therefore,
.B at
commands are executed even if the system is down at the
specified time or if the system's time is changed.
.PP
The \fBat\fR command has two forms, as shown above.
In the first form, the option
.I day
names a day of the week (lower case, spelled out).
If
.B week
is specified,
.B at
interprets the given
.I time
and
.I day
as meaning that time and day the following week.
For example, the command
.DM
	at -c "time | msg henry" 1450 friday week
.DE
.PP
executes \fBtime\fR and sends its output to \fBhenry\fR's
terminal one week from Friday at 2:50 PM.
.PP
In the second form given above,
.I month
specifies a month name (lower case, spelled out)
and the number
.I day
specifies a day of the month.
For example, the command
.DM
	at 1450 july 4 commandfile
.DE
.PP
set the file \fBcommandfile\fR to be read at 2:50 PM on July 4.
.PP
If the
.B \-v
flag is given,
.B at
prints the time when the commands will be executed, giving you
enough information to plan for the execution of the command.
For example, if it is now August 13, 1990, at 2:30 PM, and you type the
command
.DM
	at -v -c "/usr/games/fortune | msg henry" 1435
.DE
.PP
\fBat\fR will reply:
.DM
	Tue Aug 13 14:35:00
.DE
.PP
indicating that the command will be executed five minutes from now.
However, if you type
.DM
	at -v -c "/usr/games/fortune | msg henry" 1435 august 10
.DE
.PP
\fBat\fR will reply
.DM
	Sun Aug 10 14:35:00 1991
.DE
.PP
which indicates that on Sunday, August 10 of next year, at 2:35 PM,
the \*(CO system will print a \fBfortune\fR onto your terminal.
.PP
Should you create such a long-distance \fBat\fR file by accident, you
can correct the error by simply deleting the file that encodes it from
the directory \fB/usr/spool/at\fR.
The file will be named after the time that it is set to execute, plus
a unique two-character suffix, should more than one command be scheduled
to run at the same time.
For example, the file for the above command would be named \fB9108101435.aa\fR.
.PP
Finally, note that the current working directory, exported shell variables,
file creation mask, user id, and group id are restored when the given
command is executed.
.SH Example
The following example invokes the command
.B wall
at 11 P.M. to confirm that the
.B at
command is working properly:
.DM
	at -c "echo 'testing to see if cron is working' | /etc/wall" 2300
.DE
.SH Files
\fB/bin/pwd\fR \(em To find current directory
.br
\fB/usr/lib/atrun\fR \(em Execute scheduled commands
.br
\fB/usr/spool/at\fR \(em Scheduled activity directory
.br
\fB/usr/spool/at/ \fIyymmddhhmm.xx\fR \(em Commands scheduled at given time
.SH "See Also"
.Xr at, at
.Xr commands, commands
.Xr cron cron
