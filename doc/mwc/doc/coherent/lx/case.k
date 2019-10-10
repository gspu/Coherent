.TH case "" "" "C Keyword"
.PC "Introduce entry in switch statement"
.PP
The C keyword
.B case
is a label within a
.B switch
statement.
For example:
.DM
	while ((int = getchar()) != EOF)
		switch (foo) {
		case 'q':
		case 'Q':
			exit(0);
		case ' ':
			n++;
		default:
			break;
		}
.DE
.PP
.B case
labels each of the three possibilities recognized by the
.B switch
statement:
a space, \*(Qlq\*(Qr, and \*(QlQ\*(Qr.
The statements that follow a
.B case
statement behave as if they were enclosed within braces.
.PP
Note that a
.B case
statement is simply a label:
it sets a point to which the
.B switch
statement jumps,
and execution continues from that point.
Once a
.B switch
statement jumps to the point marked by a given
.B case
label, execution continues until an
.BR exit ,
.BR break ,
or
.B return
is read, or the closing brace of the
.B switch
statement is encountered.
.SH "See Also"
.Xr break, break
.Xr "C keywords," c_keyword
.Xr switch switch
.br
\*(AS, \(sc6.6.4.2
