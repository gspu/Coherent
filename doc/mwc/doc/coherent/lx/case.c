.TH case "" "" Command
.PC "Execute commands conditionally according to pattern"
\fBcase \fItoken \fBin [\fIpattern\fB [|\fIpattern\fB] ...) \fIsequence \fB;;] ... esac\fR
.PP
.HS
The shell executes \fBcase\fR directly.
.HE
.B case
is a construct that used by the shell.
It tells the shell to execute commands conditionally, according to a pattern.
It tests the given
.I token
successively against each
.IR pattern ,
in the order given.
It then executes the commands in the
.I sequence
corresponding to the first matching pattern.
Optional `|' clauses specify additional patterns corresponding
to a single
.I sequence.
If no
.I pattern
matches the
.IR token ,
the
.B case
construct executes no commands.
.PP
Each
.I pattern
can include text characters (which match themselves),
special characters `?' (which matches any character except newline)
and `*' (which matches any sequence of non-newline characters),
and character classes enclosed in brackets `[ ]';
ranges of characters within a class may be separated by `-'.
In particular, the last
.I pattern
in a
.B case
construct is often `*', which will match any
.IR token .
.PP
The shell
executes
.B case
directly.
.SH Example
The following example prints a string in response to a command-line option:
.DM
	case $1 in
		FOO)	echo "This is option FOO";;
		BAR)	echo "This is option BAR";;
		BAZ)	echo "This is option BAZ";;
		*)	echo "An asterisk marks the default option";;
	esac
.DE
.SH "See Also"
.Xr commands, commands
.Xr ksh, ksh
.Xr sh sh
