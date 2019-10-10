.TH if "" "" "C Keyword"
.PC "Introduce a conditional statement"
.PP
.B if
is a C keyword that introduces a conditional statement.
For example,
.DM
	if (i==10)
		dosomething();
.DE
.PP
will
.B dosomething
only if
.B i
equals ten.
.PP
.B if
statements can be used with the statements \fBelse if\fR
and \fBelse\fR to create a chain of conditional statements.
Such a chain can include any number of \fBelse if\fR statements, but
only one \fBelse\fR statement.
.SH "See Also"
.Xr "C keywords," c_keyword
.Xr "else" else
.br
\*(AS, \(sc6.6.4.1
