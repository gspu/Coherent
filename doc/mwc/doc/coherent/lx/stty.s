.TH stty() "" "" "System Call (libc)"
.PC "Set terminal modes"
\fB#include <sgtty.h>\fR
\fBint stty(\fIfd, sgp\^\fB)\fR
\fBint \fIfd\^\fB;\fR
\fBstruct sgttyb *\fIsgp\^\fB;\fR
.PP
The \*(CO system call
.B stty()
sets a terminal's attributes.
See the Lexicon article for
.B stty
for information on terminal attributes and their legal values.
.SH Example
This example demonstrates both
.B stty()
and
.BR gtty() .
It sets terminal input to read one character at a time (that is,
it reads the terminal in ``raw'' form).
When you type `q', it restores the terminal to its previous settings,
and exits.
For an additional example, see the \fBpipe\fP Lexicon article.
.DM
#include <sgtty.h>
.DE
.DM
main()
{
.ta 0.5i 2.3i
	struct sgttyb os, ns;
	char buff;
.DE
.DM
.ta 0.5i 2.3i
	printf("Waiting for q\en");
	gtty(1, &os);	/* save old state */
	ns = os;	/* get base of new state */
	ns.sg_flags |= RAW;	/* prevent <ctl-c> from working */
	ns.sg_flags &= ~(ECHO|CRMOD);	/* no echo for now... */
	stty(1, &ns);	/* set mode */
.DE
.DM
.ta 0.5i 1.0i 2.5i
	do {
		buff = getchar();	/* wait for the keyboard */
	} while(buff != 'q');
.DE
.DM
.ta 0.5i 2.0i
	stty(1, &os);		/* reset mode */
}
.DE
.SH Files
\fB<sgtty.h>\fR \(em Header file
.SH "See Also"
.Xr "exec," exec
.Xr "gtty()," gtty
.Xr "ioctl()," ioctl
.Xr "libc," libc
.Xr "open()," open
.Xr "read()," read.s
.Xr "sgtty.h," sgtty.h
.Xr "stty," stty
.Xr "write()" write.s
.SH Notes
Please note that if you use
.B stty()
to change the baud rate on a port, you must first invoke
.BR sleep() .
If you do not, the port reverts back to its default settings.
