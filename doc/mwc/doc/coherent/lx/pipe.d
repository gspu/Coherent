.TH pipe "" "" "Definition"
.PC
.PP
A
.I pipe
directs the output stream of one program into the input stream
of another program, thus coupling the programs together.
With pipes, two or more programs (or \fIfilters\^\fR) can be coupled
together to perform complex transforms on streams of data.
For example, in the following command
.DM
	cat DATAFILE1 DATAFILE2 | sort | uniq -d
.DE
.PP
the filter
.B cat
opens two files and prints their contents.
Its output is piped to the filter
.BR sort ,
which sorts it.
The output of
.B sort
is piped, in turn, to the filter
.BR uniq ,
which (with the \fB\-d\fR option) prints a single copy of each line
that is duplicated within the file.
Thus, with this simple set of commands and pipes, a user can quickly
print a list of all lines that appear in both files.
.SH "See Also"
.Xr "filter," filter
.Xr "mkfifo()," mkfifo
.Xr "named pipe," named_pip
.Xr "pipe()," pipe.s
.Xr "Using COHERENT" using_coh
