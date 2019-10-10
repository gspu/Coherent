.TH kill "" "" Command
.PC "Signal a process"
\fBkill [\- \fIsignal \fB] \fIpid ...\fR
.PP
\*(CO assigns each active process a unique process id, or
.IR pid ,
and uses the pid to identify the process.
.B kill
sends
.I signal
to each
.IR pid .
.I signal
must be one of the numbers described in the header file
.BR <signal.h> .
The signal can be given by number or by name, as \fBdefine\fRd in these
header files.
By default,
.I signal
is
.BR SIGTERM ,
which terminates a given process.
.PP
.II "process group"
If
.I pid
is zero,
.B kill
signals each process in the same process group (that is,
every process started by the same user from the same tty).
.PP
If
.I pid
is negative (but not \-1),
.B kill
signals every process in the process group whose ID equals the absolute
value of
.IR pid .
.PP
If
.I pid
is \-1,
.B kill
signals each process that you own.
If you are logged in as the superuser
.BR root ,
this signals every process except processes 0 (the kernel) and 1 (\fBinit\fR).
.PP
The shell prints the process id of a process if the command is detached.
The command
.B ps
prints a list of all active processes,
with process ids and command-line arguments.
.PP
A user can kill only the processes he owns;
the superuser, however, can kill anything.
A process cannot ignore or catch
.BR SIGKILL .
.PP
See the Lexicon article for
.B signal()
for a table of the signals and what each means.
.SH Files
\fB<signal.h>\fR \(em Signals
.SH "See Also"
.Xr "commands," commands
.Xr "getpid()," getpid
.Xr "init," init
.Xr "kill()," kill.s
.Xr "ksh," ksh
.Xr "ps," ps
.Xr "sh," sh
.Xr "signal()" signal
