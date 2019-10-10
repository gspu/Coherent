.TH while "" "" "C Keyword"
.PC "Introduce a loop"
\fBwhile(\fIcondition\^\fB)\fR
.PP
.B while
is a C keyword that introduces a conditional loop.
.I condition
is tested on reiteration of the loop, and the loop ends when
.I condition
is no longer satisfied.
For example,
.DM
	while (foo < 10)
.DE
.PP
introduces a loop that will continue until the variable
.B foo
is reset to ten or greater.
Note that the statement
.DM
	while (1)
.DE
.PP
will loop forever, unless interrupted by a
.BR break ,
.BR goto ,
or
.B return
statement.
.SH "See Also"
.Xr "break," break.k
.Xr "C keywords," c_keyword
.Xr "continue," continuek
.Xr "do," do
.Xr "for" for.k
.br
\*(AS, \(sc6.6.5.1
