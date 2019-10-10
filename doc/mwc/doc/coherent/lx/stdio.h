.TH stdio.h "" "" "Header File"
.PC "Declarations and definitions for I/O"
.PP
.B stdio.h
is a header file that defines manifest constants used
in standard I/O,
prototypes the STDIO functions,
and defines numerous I/O macros, as follows:
.SH Types
.II FILE
.II "type^FILE"
.II stdin
.II stdout
.II stderr
.LB
\fBFILE\fR	Descriptor of file used by STDIO routines
\fBstderr\fR	Standard error device (by default, the screen)
\fBstdin\fR	Standard input device (by default, the keyboard)
\fBstdout\fR	Standard output device (by default, the screen)
.SH "Manifest Constants"
.II BUFSIZ
.II EOF
.II FILENAME_MAX
.II FOPEN_MAX
.LB
\fBBUFSIZ\fR	Default buffer size
\fBEOF\fR	End of file
\fBFILENAME_MAX\fR	Maximum length of a file name
\fBFOPEN_MAX\fR	Maximum number of of open files
\fBL_ctermid\fR	Length of \fBctermid()\fR
\fBL_tmpnam\fR	Length of a temporary file name
\fBP_tmpdir\fR	Default directory for temporary files
\fBTMP_MAX\fR		Maximum number of temporary file names
.SH "Functions and Macros"
.LB
\fBclearerr()\fR	Present status stream
\fBfclose()\fR	Close a file stream
\fBfdopen()\fR	Open a file stream for I/O
\fBfeof()\fR	Discover a file stream's status
\fBferror()\fR	Discover a file stream's status
\fBfflush()\fR	Flush an output buffer
\fBfgetc()\fR	Get a character
\fBfgetpos()\fR	Read the file-position indicator
\fBfgets()\fR	Get a string
\fBfgetw()\fR	Get a word
\fBfileno()\fR	Get a file descriptor from a \fBFILE\fR structure
\fBfopen()\fR	Open a file stream
\fBfprintf()\fR	Format and print to a file stream
\fBfputc()\fR	Output a character
\fBfputs()\fR	Output a string
\fBfputw()\fR	Output a word
\fBfread()\fR	Read a file stream
\fBfreopen()\fR	Open a file stream
\fBfscanf()\fR	Format and read from a file stream
\fBfseek()\fR	Seek in a file stream
\fBfsetpos()\fR	Set the file-position indicator
\fBftell()\fR	Return file pointer position
\fBfwrite()\fR	Write to a file stream
\fBgetc()\fR	Get a character
\fBgetchar()\fR	Get a character
\fBgets()\fR	Get a string
\fBgetw()\fR	Get a word
\fBpclose()\fR	Close a pipe
\fBpopen()\fR	Open a pipe
\fBprintf()\fR	Print a formatted string
\fBputc()\fR	Output a character
\fBputchar()\fR	Output a character
\fBputs()\fR	Output a string
\fBputw()\fR	Output a word
\fBrewind()\fR	Reset a file pointer
\fBscanf()\fR	Format and input from standard input
\fBsetbuf()\fR	Set alternative file-stream buffer
\fBsetvbuf()\fR	Set alternative file-stream buffer
\fBsprintf()\fR	Format and print to a string
\fBsscanf()\fR	Format and read from a string
\fBtmpfile()\fR	Create a temporary file
\fBungetc()\fR	Return character to file stream
\fBvfprintf()\fR	Format and print to a file stream
\fBvprintf()\fR	Print a formatted string
\fBvsprintf()\fR	Format and print to a string
.SH "See Also"
.Xr "header file," header_fi
.Xr "libc," libc
.Xr "STDIO" stdio
.br
\*(AS, \(sc7.9
.SH Notes
\*(CO release 4.2 has rewritten its version of
.B stdio.h
so that it conforms to the \*(AS.
For this reason, program that use STDIO and are compiled under
\*(CO release 4.2 (or subsequent releases) will not run
correctly under versions of \*(CO prior to release 4.2.
