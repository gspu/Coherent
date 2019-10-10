.TH FILE "" "" Definition
.PC "Descriptor for a file stream"
.B "#include <stdio.h>"
.PP
.II "binary files"
.II "ASCII file"
.II "file"
.B FILE
describes a
.I "file stream" ,
which can be either a file on disk
or a peripheral device through which data flow.
It is defined in the header file
.BR stdio.h .
.PP
A pointer to
.B FILE
is returned by
.BR fopen() ,
.BR freopen() ,
.BR fdopen() ,
and related functions.
.PP
The \fBFILE\fR structure is as follows:
.DM
	typedef struct	FILE
	{
		unsigned char *cp,
			      *dp,
			      *bp;
		int	cc;
		int	(*gt)(),
			(*pt)();
		int	ff;
		char	fd;
		int	uc;
	} FILE;
.DE
.PP
.B cp
points to the current character in the file.
.B dp
points to the start of the data within the buffer.
.B bp
points to the file buffer.
.B cc
is the number of unprocessed characters in the buffer.
.B gt
and
.B pt
point, respectively, to the functions
.B getc()
and
.BR putc() .
.B ff
is a bit map that holds the various file flags, as follows:
.DS
	\fB_FINUSE\fR	0x01	Unused
	\fB_FSTBUF\fR	0x02	Used by macro \fBsetbuf()\fR
	\fB_FUNGOT\fR	0x04	Used by \fBungetc()\fR
	\fB_FEOF\fR	0x08	Tested by macro \fBfeof()\fR
	\fB_FERR\fR	0x10	Tested by macro \fBferror()\fR
.DE
.PP
.B fd
is the file descriptor, which is used by low-level routines like
.BR open() ;
it is also used by
.BR reopen() .
Finally,
.B uc
is the character that has been \*(QLungotten\*(QR by the function
\fBungetc()\fR, should it be used.
.SH "See Also"
.Xr "fopen()," fopen
.Xr "freopen()," freopen
.Xr "Programming COHERENT," programmi
.Xr "stdio.h," stdio.h
.Xr "stream" stream
.br
\*(AS, \(sc7.9.1
