.TH path() "" "" "General Function (libc)"
.PC "Path name for a file"
\fB#include <path.h>\fR
\fB#include <stdio.h>\fR
\fBchar *path(\fIpath\^\fB, \fIfilename\^\fB, \fImode\^\fB);\fR
\fBchar *\fIpath\^\fB, *\fIfilename\^\fB;\fR
\fBint \fImode\^\fB;\fR
.PP
The function
.B path()
builds a path name for a file.
.PP
.I path
points to the list of directories to be searched for the file.
You can use the function
.B getenv()
to obtain the current definition of the environmental variable
.BR PATH ,
or use the default setting of
.B PATH
found in the header file
.BR path.h ,
or, you can define
.I path
by hand.
.PP
.I filename
is the name of the file for which \fBpath\fR is to search.
.I mode
is the mode in which you wish to access the file, as follows:
.DS
.ta 0.5i 1.25i
	\fBX_OK\fR	Execute the file
	\fBW_OK\fR	Write to the file
	\fBR_OK\fR	Read the file
.DE
.PP
.B path()
calls the function
.B access()
to check the access status of
.IR filename .
If
.B path()
finds the file you requested and the file is available in the mode that you
requested, it returns a pointer to a static area in which it has built the
appropriate path name.
It returns NULL if either
.I path
or
.I filename
are NULL, if the search failed, or if the requested file is not
available in the correct mode.
.SH Example
This example accepts a file name and a search mode.
It then tries to find the file in one of the directories named in the
.B PATH
environmental variable.
.DM
#include <path.h>
#include <stdio.h>
#include <stdlib.h>
.DE
.DM
void
fatal(message)
char *message;
{
	fprintf(stderr, "%s\en", message);
	exit(1);
}
.DE
.DM
main(argc, argv)
int argc; char *argv[];
{
	char *env, *pathname;
	int mode;
.DE
.DM
	if (argc != 3)
		fatal("Usage: findpath filename mode");
.DE
.DM
	if(((mode=atoi(argv[2]))>4) || (mode==3) || (mode<1))
		fatal("modes: 1=execute, 2=write, 4=read");
.DE
.DM
	env = getenv("PATH");
	if ((pathname = path(env, argv[1], mode)) != NULL) {
		printf("PATH = %s\en", env);
		printf("pathname = %s\en", pathname);
		return;
	} else
		fatal("search failed");
}
.DE
.SH "See Also"
.Xr "access()," access
.Xr "libc," libc
.Xr "PATH," path.e
.Xr "path.h" path.h
