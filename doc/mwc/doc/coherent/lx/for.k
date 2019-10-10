.TH for "" "" "C Keyword"
.PC "Control a loop"
\fBfor(\fIinitialization\^\fB; \fIendcondition\^\fB; \fImodification\^\fB)\fR
.PP
.B for
is a C keyword that introduces a loop.
It takes three arguments, which are separated by semicolons \*(Ql;\*(Qr.
.I initialization
is executed before the loop begins.
.I endcondition
describes the condition that ends the loop.
.I modification
is a statement that modifies
.I variable
to control the number of iterations of the loop.
For example,
.DM
	for (i=0; i<10; i++)
.DE
.PP
first sets the variable
.B i
to zero; then it declares that the loop will continue as long as
.B i
remains less than ten;
and finally, increments
.B i
by one after every iteration of the loop.
This ensures that the loop will iterate exactly ten times (from
.B i==0
through
.BR i==9 ).
The statement
.DM
	for(;;)
.DE
.PP
will loop until its execution is interrupted by a
.BR break ,
.BR goto ,
or
.B return
statement.
Also, either or both of
.I initialization
and
.I modification
may consist of multiple statements that are separated by commas.
For example,
.DM
	for (i=0, j=0; i<10; i++, j++)
.DE
.PP
initializes both
.I i
and
.IR j ,
and increments both with each iteration of the loop.
.SH "See Also"
.Xr "break," break.k
.Xr "C keywords," c_keyword
.Xr "continue," continuek
.Xr "while" while
.br
\*(AS, \(sc6.6.5.3
