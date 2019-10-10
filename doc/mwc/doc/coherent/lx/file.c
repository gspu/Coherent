.TH file "" "" Command
.PC "Guess a file's type"
\fBfile\fI file ...\fR
.PP
.B file
examines each
.I file
and takes an educated guess as to its type.
.B file
recognizes the following classes of text files:
files of commands to the shell;
files containing the source for a C program;
files containing
.B yacc
or
.B lex
source; files containing assembly language source;
files containing unformatted documents that can be passed to
.BR nroff ;
and plain text files that fit into none of the above categories.
.PP
.B file
recognizes the following classes of non-text or binary data files:
the various forms of archives, object files, and link modules for
various machines, and miscellaneous binary data files.
.SH "See Also"
.Xr "commands," commands
.Xr "ls," ls
.Xr "size" size
.SH Notes
Because
.B file
only reads a set amount of data to determine the class of a text file,
mistakes can happen.
