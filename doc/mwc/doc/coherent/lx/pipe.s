.TH pipe() "" "" "System Call (libc)"
.PC "Open a pipe"
.B "#include <unistd.h>"
\fBint pipe(\fIfd\^\fB)\fR
\fBint \fIfd\fB[2];\fR
.PP
A
.I pipe
is an interprocess communication mechanism.
.B pipe()
creates a pipe, typically to construct pipelines in the shell
.BR sh .
.PP
.B pipe()
fills in
.IB fd [0]
and
.IB fd [1]
with
.I read
and
.I write
file descriptors,
respectively.
The file descriptors allow the transfer of data from one or more writers
to one or more readers.
Pipes are buffered to 5,120 bytes.
If more than 5,120 bytes are written into the pipe, the
.B write()
call will not return until the reader has removed sufficient data for the
.B write()
to complete.
If a
.B read()
occurs on an empty pipe, its completion awaits the writing of data.
.PP
When all writing processe close their write file descriptors,
the reader receives an end of file indication.
A write on a pipe with no remaining readers generates a
.B SIGPIPE
signal to the caller.
.PP
.B pipe()
is generally called just before
.BR fork() .
Once the parent and child processes are created,
the unused file descriptors should be closed in each process.
.SH Example
The following example prints the word
.B Waiting
until a line of data is entered.
It illustrates how to use
.BR pipe() ,
.BR fstat() ,
and
.BR fork() .
.DM
#include <stdio.h>
#include <sys/stat.h>		/* for stat */
#include <sgtty.h>		/* for stty/gtty functions */
#include <unistd.h>
.DE
.DM
static int fd[2];		/* pipe array */
.DE
.DM
main()
{
	printf("This prints 'Waiting' every second until a 'q' is hit.\en");
.DE
.DM
	/*
	 * Pipe may also be constructed by /etc/mknod
	 * If it is desired to have tasks communicate where
	 * they are not parent and child. In this case make
	 * sure the constructed pipe has the correct owner and
	 * permissions. Such pipe may be used exactly like this
	 * but open()ed on each side.
	 */
.DE
.DM
	if (-1 == pipe(fd)) {
		fprintf(stderr, "Cannot open pipe\en");
		exit(EXIT_FAILURE);
	}
.DE
.DM
	if (fork())
		parentProcess();
	else
		childProcess();
	exit(EXIT_SUCCESS);
}
.DE
.DM
parentProcess()
{
	struct stat s;
	char buff;
.DE
.DM
	for (buff = ' '; 'q' != buff;) {
		fstat(fd[0], &s);	/* get status of pipe */
		if (s.st_size) {		/* char in the pipe */
			read(fd[0], &buff, sizeof(buff));
			printf("Got a '%c'\en", buff);
			continue;
		}
.DE
.DM
		/* 
		 * This can be any process, it can use system()
		 * or exec()
		 */
		printf("Waiting\en");
		sleep(1);
	}
}
.DE
.DM
childProcess()
{
	struct	sgttyb	os, ns;
	char buff;
.DE
.DM
	gtty(fileno(stdin), &os);	/* save old state */
	ns = os;			/* get base of new state */
	ns.sg_flags |= RAW;		/* process each character as entered */
	ns.sg_flags &= ~(ECHO|CRMOD);	/* no echo for now... */
	stty(fileno(stdin), &ns);	/* set mode */
.DE
.DM
	do {
		buff = getchar();	/* wait for the keyboard */
		write(fd[1], &buff, sizeof(buff));
	} while ('q' != buff);
.DE
.DM
	stty(fileno(stdin), &os);	/* reset mode */
}
.DE
.SH "See Also"
.Xr "close()," close
.Xr "libc," libc
.Xr "libsocket," libsocket
.Xr "mkfifo()," mkfifo
.Xr "mknod()," mknod.s
.Xr "read()," read.s
.Xr "sh," sh
.Xr "signal()," signal
.Xr "unistd.h," unistd.h
.Xr "write()" write.s
.br
\*(PX Standard, \(sc6.1.1
.SH Diagnostics
.B pipe()
returns zero on successful calls, or \-1 if it could not create the pipe.
.PP
If it is necessary to create a pipe between tasks that are
not parent and child, use
.B /etc/mknod
to create a named pipe.
These named pipes can be opened and used by different programs
for communication.
Remember to give them the correct owner and permissions.
.PP
If you attempt to open a pipe write only,
.B O_NDELAY
is set, and there are currently no readers on this pipe,
.B open()
returns immediately and sets
.B errno
to
.BR ENXIO .
