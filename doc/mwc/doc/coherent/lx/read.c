.TH read "" "" Command
.PC "Assign values to shell variables"
\fBread \fIname ...\fR
.PP
.HS
Reads a line from stdin and assign each token
to corresponding shell variable \fIname\fR.
The shell executes \fBread\fR directly.
.HE
.B read
reads a line from the standard input.
It assigns each token of the input to the corresponding shell variable
.IR name .
If the input contains fewer tokens than the number of names specified,
.B read
assigns the null string to each extra variable.
If the input contains more tokens than the number of names specified,
.B read
assigns the last
.I name
in the list the remainder of the input.
.PP
.B read
normally returns an exit status of zero.
If it encounters end of file or is interrupted while
reading the standard input, it returns one.
.PP
The shell executes
.B read
directly.
.SH Example
The command
.DM
	read foo bar baz
	hello how are you
.DE
.PP
parses the line ``hello how are you'' and assigns the tokens to,
respectively, the shell variables
.BR foo ,
.BR bar ,
and
.BR baz .
If you further type
.DM
	echo $foo
	echo $bar
	echo $baz
.DE
.PP
you will see:
.DM
	hello
	how
	are you
.DE
.SH "See Also"
.B
.Xr "commands," commands
.Xr "ksh," ksh
.Xr "sh" sh
