.TH  ".profile" "" "" "System Administration"
.PC "Execute commands at login"
.B $HOME/.profile
.PP
The shell reads file
.B $HOME/.profile
whenever a user logs in.
The user can edit its contents to set up her
environment however she prefers, and to execute programs routinely upon login.
.PP
The following gives one user's \fB.profile\fR:
.DM
	MAIL=/usr/spool/mail/sally
	PATH=/usr/bin:/bin:/v/sally/bin:.
	EDITOR=me
	PS1="Sally(!) "
	PS2="MORE(!)> "
	PAGER=scat
	set -h
	set -o emacs
	echo "CALENDAR:"
	calendar
	echo ""
	/usr/games/fortune
.DE
.PP
The first six entries set environmental variables; note that these are
in addition to the variables set in
.BR /etc/profile .
.PP
The next two entries
.DM
	set -h
	set -o emacs
.DE
.PP
set two features of the Korn shell, which is used by the person.
The first turns on its hashing feature, and the second turns on
\*(ME-style editing of the command line.
.PP
The last four entries
.DM
	echo "CALENDAR:"
	calendar
	echo ""
	/usr/games/fortune
.DE
.PP
execute two programs upon login.
The two \fBecho\fR commands print, respectively, the word \fBCALENDAR\fR
and a blank line on the screen.
The command \fBcalendar\fR reads the user's personal calendar and prints
all entries the relate to today (or to the weekend, should today be a
Friday).
The command \fBfortune\fR prints a randomly selected (and, we hope, amusing)
select from file \fB/usr/games/lib/fortunes\fR.
.PP
This example is relatively simple.
A user's \fB.profile\fR can be turned into a complex shell program if you
wish.
.SH "See Also"
.Xr "Administering COHERENT," administe
.Xr "ksh," ksh
.Xr ".kshrc," kshrc.p
.Xr "profile," profile
.Xr "sh," sh
.Xr "Using COHERENT" using_coh
