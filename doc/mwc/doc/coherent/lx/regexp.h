.TH regexp.h "" "" "Header File"
.PC "Header file for regular-expression functions"
.B "#include <regexp.h>"
.PP
Header file
.B <regexp.h>
is used with regular-expression function
.BR regcomp() ,
.BR regexec() ,
and
.BR regsub() .
These functions manipulate a regular expression, which is stored in structure
.BR regexp .
.B <regexp.h>
defines this structure as follows:
.DM
	typedef struct regexp {
		char *startp[NSUBEXP];
		char *endp[NSUBEXP];
		char regstart;
		char reganch;
		char *regmust;
		int regmlen;
		char program[1];
	} regexp;
.DE
.PP
Fields
.B regstart
through
.B program
are used internally, and should not be manipulated by a user's program.
Fields
.B startp[]
and
.B endp[]
are arrays of pointers to sub-strings within the expression.
For details on how these pointers are used, see the Lexicon entry for
.BR regexec() .
.B NSUBEXP
gives the number of sub-strings that can be addressed at one time;
as of this writing, it is set to ten.
.SH "Syntax of a Regular Expression"
.II "regular expression^definition"
The following describes the rules with which the
.B regexp
functions define a regular expression.
.PP
A regular expression consists of zero or more \fIbranches\fR.
Branches are separated from each other by a pipe character `|'.
A string matches an expression when it matches any branch within the expression.
.PP
A branch, in turn, consists of zero or more \fIpieces\fR, which are
concatenated.
.\"When a regular\-expression function matches a string with a branch,
.\"it matches the first piece, then the second piece, and so on.
.\"It matches a match for the first, followed by a match for the second, etc.
Each piece is a string, or
.IR atom ,
which can be followed by `*', `+', or `?'.
An atom followed by `*' can be matched with a sequence of zero or more matches
of the atom.
An atom followed by `+' can be matched with a sequence of one or more matches
of the atom.
An atom followed by `?' can be matched with either the atom or the null string.
.PP
An atom, in turn, is built from the following:
.IP \fB(\fIexpression\^\fB)\fR
A regular expression between parentheses
This matches a match for the regular expression.
.IP \fB[\fIstring\^\fB]\fR
Match any character within
.IR string .
If string contains a hyphen `\-', this represents a range of characters.
For example, ``0\-9'' represents all digits; or ``a\-z'' represents all
lower-case characters.
To include a literal `\-' within
.IR string ,
make it the first or last character within
.IR string .
To include a literal `]' in the sequence, make it the first character,
after a possible `^'.
.IP \fB[^\fIstring\^\fB]\fR
Match any character that is
.I not
in
.IR string .
 a \fIrange\fR (see below), `.'
.IP \fB^\fR
Match the null string at the beginning of the input string.
.IP \fB$\fR
Match the null string at the end of the input string.
.IP \fB\e\fIc\fR
Match the single character
.I c
literally; ignore any special significance that
.I c
might have.
.SH Ambiguity
A string can match more than one part of an regular expression.
The following rules describe how to choose which part to match.
.PP
The basic rule is that if a regular expression could match two parts
of a string, it matches the one that begins earlier.
.PP
If both parts begin in the same place but match different lengths
of the expression, or match the same length in different ways,
life gets messier, as follows.
.PP
In general, the possibilities in a list of branches are considered in
left-to-right order, the possibilities for `*', `+', and `?' are
considered longest-first, nested constructs are considered from the
outermost in, and concatenated constructs are considered leftmost-first.
The match that is chosen is the one that uses the earliest
possibility in the first choice that has to be made.
If there is more than one choice, the next will be made in the same manner
(earliest possibility) subject to the decision on the first choice.
.PP
For example, ``(ab|a)b*c'' could match ``abc'' in one of two ways.
The first choice is between ``ab'' and `a'; since ``ab'' is earlier, and
lead to a successful overall match, it is chosen.
Since the `b' is already spoken for,
the ``b*'' must match its last possibility \(em the empty string \(em because
it must respect the earlier choice.
.PP
In the particular case where no `|'s are present and there is only one
`*', `+', or `?', the net effect is that the longest possible
match will be chosen.
So ``ab*'', presented with ``xabbbby'', will match ``abbbb''.
Note that if ``ab*'' is tried against ``xabyabbbz'', it
will match ``ab'' just after `x', due to the begins-earliest rule.
In effect, the decision on where to start the match is the first choice
to be made, hence subsequent choices must respect it even if this leads them
to less-preferred alternatives.
.SH "See Also"
.Xr "header files," header_fi
.Xr "regcomp()," regcomp
.Xr "regerror()," regerror
.Xr "regexec()," regexec
.Xr "regsub()" regsub
.SH "Notes"
.II "Spencer, Harry"
The code used for the
.B regexp()
was written by Harry Spencer at the University of Toronto.
It is copyright \(co 1986 by the University of Toronto.
These routines are intended to be compatible with the Bell System-8
.B regexp() ,
but are not derived from Bell code.
The above description of regular expressions is derived from the
manual page written by Harry Spencer.

