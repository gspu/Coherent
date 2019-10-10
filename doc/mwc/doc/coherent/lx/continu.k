.TH continue "" "" "C Keyword"
.PC "Force next iteration of a loop"
.PP
.B continue
forces the next iteration of a
.BR for ,
.BR while ,
or
.B do
loop.
For example,
.DM
	while ((foo = getchar()) != EOF) {
		if ((foo < 'a') || (foo > 'z'))
			continue;
		...		/* do something */
	}
.DE
.PP
forces the
.B while
loop to throw away everything except lower-case alphabetic characters.
.SH "See Also"
.Xr "C keywords," c_keyword
.Xr "for," for
.Xr "while" while
.br
\*(AS, \(sc6.6.6.2
