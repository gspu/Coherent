.TH tr "" "" Command
.PC "Translate characters"
\fBtr [\-cds] \fIstring1 \fB[\fIstring2\^\fB]\fR
.PP
.HS
.SH Options:
.IC "\fB\-c\fR"
Complement the characters in \fIstring1\fR
.IC "\fB\-d\fR"
Delete characters found in \fIstring1\fI (no \fIstring2\fR needed)
.IC "\fB\-s\fR"
Squeeze multiple output mappings onto one character
.Pp
Both strings may contain ranges.
Characters may have form \fI\ennn\fR.
.HE
.B tr
reads characters from the standard input,
possibly translates each to another value or deletes it,
and writes to standard output.
.PP
Each specified
.I string
may contain literal characters of the form
.I a
or
.I \eb
(where
.I b
is non-numeric),
octal representations of the form
.I \eooo
(where
.I o
is an octal digit),
and character ranges of the form
.IR "X-Y" .
.B tr
rewrites each
.I string
with the appropriate conversions and range expansions.
.PP
If an input character is in
.IR string1 ,
.B tr
outputs the corresponding character of
.IR string2 .
If
.I string2
is shorter than
.IR string1 ,
the result is the last character in
.I string2.
.PP
The following flags control how \fBtr\fR translates characters:
.RS
.IP \fB\-c\fR 0.3i
Replace
.I string1
by the set of characters not in
.I string1.
.IP \fB\-d\fR
Delete characters in
.I string1
rather than translating them.
.IP \fB\-s\fR
The \*(QLsqueeze\*(QR option:
map a sequence of the same character from
.I string1
to one output character.
.SH Example
The following example
prints all sequences of four or more spaces
or printing characters from
.BR infile :
.DM
	tr \-cs ' \-~' '\e12' <infile | grep ....
.DE
.PP
Here
.I string1
is the range from \fB<space>\fR to `~',
which includes all printing characters.
Because this example uses the flags
.BR \-cs ,
.B tr
maps sequences of nonprinting characters to newline (octal 12).
.SH "See Also"
.Xr "ASCII," ascii
.Xr "commands," commands
.Xr "ctype.h," ctype.h
.Xr "sed" sed
.SH Notes
Beginning with \*(CO 4.2, the command
.DM
	echo "This is a test." | tr
.DE
.PP
returns
.DM
	This is a test.
.DE
.PP
This behavior does not conform with \*(PX, but is required by a number
of third-party packages.
