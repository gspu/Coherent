.TH utmp.h "" "" "Header File"
.PC "Login accounting information"
.B "#include <utmp.h>"
.PP
.II utmp
.II /etc/utmp
.II wtmp
.II /usr/adm/wtmp
Header file
.B <utmp.h>
defines the types and constants that are used to
manipulate the system-adminstration files
.B /etc/utmp
and
.BR /usr/adm/wtmp .
The former file describes every user who is currently logged into your system;
the latter records when each user logged into your system and logged out again.
.PP
Each of these files consists of records, each of which has are objects of
type
.BR utmp ,
which
.B <utmp.h>
defines as follows:
.DM
struct	utmp {
	char ut_user[8];
	char ut_id[4];
	char ut_line[12];
	short ut_pid;
	short ut_type;
	struct exit_status {
		short e_termination;
		short e_exit;
	} ut_exit;
	time_t ut_time;
};
.DE
.PP
The following describes each field in
.BR utmp :
.IP \fBut_user\fR
The login identifier of the user.
.IP \fBut_id\fR
The user's identifier, as taken from
.BR /etc/init .
.IP \fBut_line\fR
The device through which the user logged in.
.IP \fBut_pid\fR
The process identifier of the user's shell.
.IP \fBut_type\fR
Type of entry in this file.
This can be any of the following values:
.DS
.ta 0.5i 2.0i
	\fBEMPTY\fR	An empty entry
	\fBRUN_LVL\fR	Run level
	\fBBOOT_TIME\fR	Boot time
	\fBOLD_TIME\fR	
	\fBNEW_TIME\fR	
	\fBINIT_PROCESS\fR	Process spawned by \fBinit\fR
	\fBLOGIN_PROCESS\fR	A \fBgetty\fR waiting for a login
	\fBUSER_PROCESS\fR	A user process
	\fBDEAD_PROCESS\fR	
	\fBACCOUNTING\fR	
.DE
.IP \fBut_exit\fR
The process's exit status.
It consists of the following fields:
.RS
.IP \fBe_termination\fR
Process's termination status.
.IP \fBe_exit\fR
Process's exit status.
.RE
.IP \fBut_time\fR
The time the user logged on.
.PP
The following functions use this header file:
.LB
\fBendutent()\fR	Close the logging file.
\fBgetutent()\fR	Read the next entry from \fB/etc/utmp\fR.
\fBgetutid()\fR	Find an entry in \fB/etc/utmp\fR by login identifier.
\fBgetutline()\fR	Find an entry in \fB/etc/utmp\fR by login device.
\fBpututline()\fR	Write a record into \fB/etc/utmp\fR.
\fBsetutent()\fR	Rewind the input stream that is reading \fB/etc/utmp\fR
\fButmpname()\fR	Manipulate a file other than \fB/etc/utmp\fR.
.PP
Each function is described in its own Lexicon entry.
.SH Files
.B /etc/utmp
.br
.B /usr/adm/wtmp
.SH "See Also"
.Xr "ac," ac
.Xr "header files," header_fi
.Xr "login," login
.Xr "utmp," utmp
.Xr "who," who
.Xr "wtmp" wtmp
.R
