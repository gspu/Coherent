.TH acct.h "" "" "Header File"
.PC "Format for process-accounting file"
.B "#include <acct.h>"
.PP
.II "process accounting, file format"
.II "file format, processing accounting"
.I "Process accounting"
is a feature of the \*(CO system that allows it record what processes
each user executes and how long each process takes.
These data can be used to track how much each user uses the system.
.PP
The function
.B acct()
turns process accounting off or on.
When process accounting has been turned on,
the \*(CO system writes raw process-accounting information
into an accounting file as each process terminates.
Each entry in the accounting file, normally
.BR /usr/adm/acct ,
has the following form, as defined in the header file
.BR acct.h :
.DM
.ta 0.5i 1.25i
struct acct {
	char	ac_comm[10];
	comp_t	ac_utime;
	comp_t	ac_stime;
	comp_t	ac_etime;
	time_t	ac_btime;
	short	ac_uid;
	short	ac_gid;
	short	ac_mem;
	comp_t	ac_io;
	dev_t	ac_tty;
	char	ac_flag;
};
.DE
.DM
.ta 0.75i 1.5i 2.0i
/* Bits from ac_flag */
#define	AFORK	01	/* has done fork, but not exec */
#define	ASU	02	/* has used superuser privileges */
.DE
.PP
Every time a process calls
.BR exec() ,
the contents of
.B ac_comm
are replaced with the first ten characters of the file name.
The fields
.B ac_utime
and
.B ac_stime
represent the CPU time used in the user program and in the system,
respectively.
.B ac_etime
represents the elapsed time since the process started running,
whereas
.B ac_btime
is the time the process started.
The effective user id and effective group id are
.B ac_uid
and
.B ac_gid.
.B ac_mem
gives the average memory usage of the process.
.B ac_io
gives the number of blocks of input-output.
.B ac_tty
gives the controlling typewriter device major and minor numbers.
.PP
For some of the above times, the
.B acct
structure uses the special representation
.B comp_t,
defined in the header file
.BR types.h .
It is a floating point representation with
three bits of base-8 exponent and 13 bits of fraction, so it fits in a
.B short
integer.
.SH "See Also"
.Xr acct(), acct
.Xr accton, accton
.Xr "header files," header_fil
.Xr sa sa
