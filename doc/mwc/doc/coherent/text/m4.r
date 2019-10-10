.II "m4^tutorial"
.ds TL "m4 Macro Processor"
.NH "Introduction to the m4 Macro Processor" 1
.PP
.B m4
is a macro processor for the \*(CO system.
It is a powerful and flexible text processing tool.
You can tell it, with a great degree of generality,
to search for macro names and replace them with other strings.
Macros can also take arguments.
.PP
.II 4GL
.II "fourth-generation language"
.B m4
provides a useful front end for programming languages such as
fourth-generation lanaguages (4GLs)
which commonly have no built-in macro facility.
.B m4
also has powerful facilities for manipulating files,
making decisions conditionally,
selecting substrings, and performing arithmetic,
so it is useful for processing forms.
.PP
The command
.DS
	\fBm4\fR [ \fIfile\fR ... ]
.DE
invokes \fBm4\fR.
\fBm4\fR reads each \fIfile\fR in the order given on the command line;
if no \fIfile\fR is given, \fBm4\fR reads from the standard input.
The \fIfile\fR `\-' also indicates the standard input;
this allows you to perform interactive input while
.B m4
is processing files.
.B m4
reports any
.I file
that it cannot open, and eliminates it from the input stream.
.PP
.B m4
writes its output to the standard output stream.
As with other \*(CO commands,
the optional output redirection specification \fB>\fIoutfile\fR
on the command line redirects the output into
.I outfile.
To leave
.BR m4 ,
type
.BR <ctrl-D> .
.Sh "Definitions and Syntax"
.PP
.B m4
reads text one line at a time from its input stream.
When it reads a line of text, it scans the line for a macro
that you have defined.
A legal macro name is a string of alphanumeric characters
(letters, digits, underscore `_'), the first of which is not a digit.
.B m4
recognizes the macro name only if it is surrounded by
nonalphanumeric characters (i.e., spaces or newline characters)
on both sides.
.PP
When
.B m4
finds a macro, it removes it from the input stream and
replaces it with its definition.
It then writes the resulting modified text (called
\fIreplacement text\^\fR), onto the input stream.
.B m4
then reads another line from the input stream, and continues processing.
.PP
.II "m4^quoted text"
.II "m4^unquoted text"
Text that is contained within single quotation marks is quoted
(i.e., is contained between
a grave mark \(ga on the left and an apostrophe \' on the right).
All other text is \fIunquoted\fR.
.B m4
searches only unquoted text for macros.
.PP
A
.I "macro call"
can be either a macro or a macro immediately followed by a set of
arguments:
.DM
	macroname(arg1, ..., argn)
.DE
A set of arguments must start with a left parenthesis
that follows the macro immediately (i.e., no space can come between
the macro and the left parenthesis).
The entire argument set must be enclosed by balanced, unquoted parentheses:
parentheses may appear within the text of an argument, but they must
always come in balanced pairs.
A single left or right parenthesis may be passed
by quoting it, e.g. \(ga\^(\^\' or \(ga\^)\^\'.
.PP
Arguments are separated by commas
that are neither within apostrophes nor
within an inner set of unquoted parentheses.
.B m4
strips from each argument all leading unquoted spaces, tabs, and newlines.
It processes the text of each argument in the same manner that it
processes ordinary text; that is,
it removes, evaluates, and replaces any recognized macro calls
.I before
it stores the argument text
for possible use within the replacement text.
If you wish
to pass a macro name or an entire macro call as an argument, it must be quoted.
.B m4
stores the values of the first nine arguments for possible use
in the replacement text.
It processes arguments after the ninth, but throws away the results.
.PP
.II "m4^nestable quotes"
.II "m4^quote marks^removing"
.B m4
does not search quoted text for macros.
Instead, it removes the quotation marks and copies the text to the
standard output unchanged.
Quotes can be nested; that is, quoted text can contain other blocks
of quoted text.
.B m4
removes only the outermost level of quotation marks
each time it reads a piece of quoted text.
This aids in delaying macro expansion in text
until the second (or later) time the text is read by
.BR m4 .
.PP
.B m4
includes numerous predefined macros,
which perform various functions.
The remainder of this document describes the predefined macros in detail.
The Lexicon entry for
.B m4
summarizes each predefined macro.
.Sh "Defining Macros"
.PP
.II "m4^define"
The macro
.DM
	define(\(ganame\', \(gadefinition\')
.DE
defines a macro
.I name
and its replacement text
.IR definition .
.B m4
replaces every subsequent unquoted occurrence of
.I name
with
.IR definition ,
as described above.
For example, the
.B m4
input
.DM
	define(\(gaher\', \(gaCOHERENT\')
	To know, know, know her
	Is to love, love, love her ...
.DE
produces the output
.DM
	To know, know, know COHERENT
	Is to love, love, love COHERENT ...
.DE
.I name
should usually be quoted.
If it is not quoted and it is being redefined,
.B m4
sees its old
.I definition
as the first argument to
.BR define ,
which will not have the intended effect.
Similarly,
.I definition
should be quoted if the macro names that occur in it should not be replaced.
.PP
Any legal macro name may be the first argument of a
.BR define .
If you redefine a predefined macro,
its original function is lost and cannot be recovered.
.PP
.II "m4^macro name recognition"
As noted above,
.B m4
recognizes a macro name only if it is surrounded by non-alphanumeric
characters.
For example,
.DM
	define(\(gaher\', \(gaCOHERENT\')
	Coherent software is reliable software.
.DE
produces the output
.DM
	Coherent software is reliable software.
.DE
.B m4
does not recognize the characters
.B her
in the word
.B Coherent
as a macro name.
.PP
The value of the
.B define
macro is the null or empty string (the string which contains no characters).
In other words,
.B m4
puts nothing (the null string) back on its input stream when it processes a
.B define
call.
.PP
.II "m4^argument"
Like predefined macros, user-defined macros may take arguments.
.B m4
replaces the string
.I $n
in the macro definition with the value of the \fIn\fRth argument,
where
.I n
is a digit (1 to 9).
It replaces $0 with the macro name.
If the argument set contains fewer than
.I n
arguments,
.B m4
replaces
.I $n
with the null string.
.B m4
uses functional notation to specify argument sets.
Unlike a normal function, however, an
.B m4
macro does not require a fixed number of arguments.
The same macro may be called with or without an argument set,
or with argument sets containing different numbers of arguments.
.PP
.II "m4^argument substitution"
The following macro concatenates its arguments:
.DM
	define(\(gacat\', $1$2$3$4$5$6$7$8$9)
.DE
Then
.DM
      cat(one, \(gatwo\', \(ga\(gathree\'\', \(gafour, four \',
          five(also,),,seven)
.DE
becomes
.DM
	onetwothreefour, four five(also,)seven
.DE
A more complex definition is:
.DM
	define(\(gacomma\', \(ga\(ga$0 (which looks like \(ga,\')\'\')
.DE
This turns each subsequent unquoted occurrence of
.DM
	comma
.DE
into
.DM
	comma (which looks like \(ga,\')
.DE
Two sets of quotation marks around the replacement text are necessary.
When
.B m4
reads this call to macro
.BR define ,
the resultant argument text is:
.DM
	comma
.DE
for the \fIname\fR and
.DM
	\(ga$0 (which looks like \(ga,\')\'
.DE
for the \fIdefinition\fR.
When
.B m4
sees the text
.DM
	comma that is not quoted
.DE
it evaluates and replaces the now-defined macro name
.B comma
to produce the text
.DM
	\(gacomma (which looks like \(ga,\')\' that is not quoted
.DE
on the \fIinput\fR stream.
Because
.B comma
appears inside a set of quotation marks,
.B m4
does not treat it as a macro name.
For the same reason, the string `,' also passes through unmodified.
The final output is:
.DM
	comma (which looks like \(ga,\') that is not quoted
.DE
.II "m4^dumpdef"
When the predefined macro
.B dumpdef
is used without arguments,
it returns the names and definitions of all defined macros.
For each macro, it returns its quoted name, a tab character,
and then its quoted definition;
no definition is given for a predefined macro.
When used with arguments,
.DM
	dumpdef(name)
.DE
returns the quoted definition of each macro name that
appears as an argument.
.PP
.II "m4^undefine"
The predefined macro
.DM
	undefine(\(ganame\')
.DE
removes a macro definition.
As noted for
.B define
above, the argument must be quoted to have the desired effect.
\fBundefine\fP ignores arguments which are not defined macro names.
The value of the
.B undefine
call is the null string.
If a predefined macro is undefined, its original function cannot be recovered.
.Sh "Input Control"
.PP
.II "m4^changequote"
The predefined macro \fBchangequote\fP
changes the quote characters.
For example:
.DM
	changequote( {, })
.DE
makes the quote characters the left and right braces.
It also removes the effect of the previously defined
quotation characters.
Missing arguments default to \(ga for open quotation and \'
for close quotation.
Thus, \fBchangequote\fP without arguments restores the
original quote characters \(ga and \'\^.
If the arguments are identical, the nesting ability
of quotation marks is temporarily lost.
Instead, the first instance of the new quote character turns on quoting
and the next instance turns off quoting.
The value of the
.B changequote
call is the null string.
.PP
.II "m4^dnl"
The predefined macro \fBdnl\fP
(delete to newline) ``eats'' all characters from the input stream
up to and including the next newline
and returns the null string.
It is particularly useful in a string of \fBdefine\fP macro calls.
Although
.B m4
replaces each
.B define
by the null string, newlines often separate macro definitions, and
.B m4
copies the newlines to the output stream unchanged.
Two ways of using
.B dnl
are:
.DM
	define(this, that)dnl
	define(something, else)dnl
.sp
	dnl(define(this, that), define(something, else))
.DE
.II "m4^extra newlines"
The first examples use \fBdnl\fR without arguments.
The final example uses
.B dnl
with an argument set, which
.B m4
processes (performing each
.BR define )
and subsequently ignores.
The following section describes an alternative
(and generally preferable)
method of eliminating extraneous newlines in a
sequence of \fBdefine\fP calls.
.PP
.II "m4^decision-making macro"
.II "m4^ifdef"
.B m4
includes two decision-making macros:
.B ifdef
and
.BR ifelse .
.PP
.B ifdef
checks whether a macro is defined.
It has the following form:
.DM
	ifdef(macro,defvalue,undefvalue)
.DE
If
.I macro
is defined,
.B ifdef
returns
.IR defvalue ;
otherwise, it returns
.IR undefvalue .
.PP
.B ifelse
compares pairs of arguments.
It has the following form:
.DM
	ifelse(arg1,arg2,arg3, ... , arg9)
.DE
.B ifelse
compares
.I arg1
with
.IR arg2 .
If they are the same,
it returns
.IR arg3 .
If not, and if
.I arg4
is the last argument, it returns
.IR arg4 .
Otherwise, it repeats the process, comparing
.I arg4
with
.IR arg5 ,
and so on.
Like other
.B m4
macros, this takes a maximum of nine arguments.
.PP
.II "m4^sinclude"
In addition to each
.I file
specified in the command line,
any other accessible file may be included in the input stream
with the predefined macro
.DM
	include(file)
.DE
.II "m4^include"
.B m4
replaces this macro call on the input stream with the entire
contents of the specified
.IR file .
If
.I file
cannot be accessed, \fBinclude\fP causes a fatal error;
.B m4
prints an error message and exits.
The alternative predefined macro
.DM
	sinclude(file)
.DE
functions exactly like
.BR include ,
except that it does not print an error message and stop processing if
.I file
is inaccessible.
.Sh "Output Control"
.PP
.II "m4^output stream"
.II "m4^divert"
.B m4
maintains ten output streams, numbered zero through nine.
Stream 0 is the standard output, where
.B m4
normally directs its output.
Streams 1 through 9 are temporary files.
The predefined macro
.DM
	divert(n)
.DE
diverts output away from stream 0,
appending it instead to stream
.IR n .
Any
.I n
outside the range 0 to 9 causes output to be thrown
away until the next \fBdivert\fP call.
.B divert
without any arguments or with a nonnumeric argument is equivalent to
.BR divert(0) .
The value of a
.B divert
call is the null string.
.PP
.II "m4^extra newlines"
The preceding section described the use of
.B dnl
to eliminate extraneous newlines on the output stream when
processing a sequence of
.B define
calls.
A more readable method of eliminating the newlines is to precede
the definitions with
.B "divert(\-1)"
and follow them with
.BR "divert" .
.B m4
then diverts the extraneous newlines to the nonexistent stream \-1.
.PP
.II "m4^undivert"
The predefined macro
.DM
	undivert(streams)
.DE
fetches text diverted to one or more temporary streams.
It appends the text from the specified
.I streams
in the given order to the
.I current
output stream.
.B m4
does not allow diverted text to be undiverted back to the same stream.
.B undivert
with no arguments undiverts all diversions in numerical order.
The value of
.B undivert
is the null string; undiverted text is
.I not
scanned for macro calls, but is simply moved from one place to another.
.B m4
automatically undiverts all diversions in numerical order
to the standard output (stream 0) at the end of processing.
.PP
To illustrate the use of
.B divert
and
.BR undivert ,
invoke
.B m4
and type:
.DM
	define(\(gacount', $1$2)
.DE
And to see what macro
.B count
does, type:
.DM
	count(one, three)
.DE
The output on the screen reads:
.DM
	onetwo
.DE
Now type:
.DM
	divert(1)
.DE
This diverts device 1 (the standard output)
into a temporary file.
Now type:
.DM
	count(one, three)
.DE
Nothing appears on the screen.
.B divert
sent the output of the macro
.B "count(one, three)"
into a temporary file.
Thus, the output is not lost, as you might have thought.
To demonstrate the existence of that output, type:
.DM
	divert
.DE
to reset the standard output to be the screen.
See for yourself.
Now, when you type
.DM
	count(one, four)
.DE
.B m4
replies on the screen:
.DM
	onefour
.DE
As you can see, the standard output is again directed to the screen.
To retrieve the diverted
output of
.BR "count(one, three)" ,
and send it to the screen, type:
.DM
	undivert(1)
.DE
which produces:
.DM
	onethree
.DE
.II "m4^divnum"
The predefined macro \fBdivnum\fP returns the current diversion number.
.PP
The predefined macro
.DM
	errprint(message)
.DE
sends the given
.I message
to the standard error stream.
.II "m4^errprint"
The value of \fBerrprint\fP is the null string.
.Sh "String Manipulation"
.PP
.II "m4^substr"
The predefined macro
.DM
	substr(string, start, count)
.DE
returns a substring of a string of characters.
The first argument
.I string
can be anything.
The second argument
.I start
is a number giving the starting position of the desired substring in
.IR string .
Position 0 is the leftmost character of
.IR string ,
position 1 is the next character to the right, and so on.
If
.I start
is negative, the orientation switches to the right.
Position \-1 is the rightmost character of
.IR string ,
position \-2 is the character to its left, and so on.
The third argument
.I count
specifies the length and direction of the substring.
Zero returns the null string.
A positive
.I count
returns a substring consisting of the character addressed by
.I start
and \fIcount\fR\-1 characters to the right of it.
A negative number does the same thing, but to the left.
If
.I count
is omitted, it is assumed to be of the same sign as
.I start
and large enough to extend to the end of
.I string
in that direction.
If
.I start
is omitted, it is assumed to be 0 if
.I count
is positive or omitted, or \-1 if
.I count
is negative.
For example:
.DM
	define(\^\(ga\^alpha\^\'\^, \(ga\^abcdefghijklmnopqrstuvwxyz\^\'\^)
	substr(alpha, , )
.DE
returns
.DM
	abcdefghijklmnopqrstuvwxyz
.DE
Here both
.I start
and
.I count
are omitted and are therefore assumed to be 0 and 26, respectively.
.DM
	substr(alpha, 0, 6)
	substr(alpha, , 6)
.DE
both return
.DM
	abcdef
.DE
Similarly,
.DM
	substr(alpha, , -6)
	substr(alpha, 21, )
.DE
both return
.DM
	uvwxyz
.DE
Finally,
.DM
	substr(alpha, -6, )
	substr(alpha, 0, 21)
.DE
both return
.DM
	abcdefghijklmnopqrstu
.DE
.PP
.II "m4^translit"
The predefined macro
.DM
	translit(string, characters, replacements)
.DE
transliterates single characters within a string.
It returns
.I string
with every occurrence of a character specified in
.I characters
replaced with the corresponding character from
.IR replacements .
If there is no corresponding character, \fBtranslit\fP
simply deletes the character.
For example:
.DM
	define(liquorjugs, \(gapack my box with five dozen liquor jugs')
	translit(liquorjugs, aeiou, 1234)
.DE
returns:
.DM
	p1ck my b4x w3th f3v2 d4z2n l3q4r jgs
.DE
.Sh "Numeric Manipulation"
.PP
.B m4
can simulate the long integer variables typical of most programming languages
by using
.B define
as the assignment operator.
Whenever the defined macro name appears unquoted,
.B m4
immediately replaces it by its numeric value.
.PP
.II "m4^incr"
.II "m4^decr"
The predefined macros \fBincr\fP
and \fBdecr\fP return their argument incremented or decremented by 1.
Thus,
.DM
	define(\(gax\', 1234)
	incr(x)
.DE
returns:
.DM
	1235
.DE
Note that
.B incr
and
.B decr
do not change the value of the simulated variable
.BR x ,
or of any other variable.
They return only that value plus or minus 1;
.B x
itself retains its value of
.BR 1234 .
.PP
.B incr
and
.B decr
initialize to zero all arguments that are omitted or not a valid number.
Thus, the example
.DM
 	incr(a34/87)
.DE
returns
.BR 1 ;
but
.DM
	incr(123.67)
.DE
returns
.BR 124 .
As you can see,
.B incr
truncates floating-point numbers.
The same applies to a variable that you have
.BR define d
to have a floating-point value.
.PP
.II "m4^eval"
.II "m4^expression evaluation"
More generally, the predefined macro
.DM
	\fBeval\fR(\fIexpression\fR)
.DE
evaluates an integer-value arithmetic
.I expression
and returns the resulting value.
The operators available, in order of decreasing precedence, are:
.DS
.ta 0.5i 2.0i
	( )	Parentheses for grouping
	+ \-	Unary plus, negation
	^ **	Exponentiation
	* / %	Multiplication, division, modulus
	+ \-	Addition, subtraction
	> < >= <= == !=	Comparisons
	!	Logical negation
	&& &	Logical and
	|| |	Logical or
.DE
.II "m4^eval"
The comparisons and logical operators return either 0 (false) or 1 (true).
\fBeval\fP performs all arithmetic in
.B long
integers.
.B eval
reports an error if its argument is not a well-formed expression.
.PP
.II "m4^string length"
The predefined macro
.DM
	\fBlen\fR(\fIstring\fR)
.DE
returns a numeric value corresponding to the length of
.IR string .
.PP
.II "m4^index"
The predefined macro
.DM
	\fBindex\fR(\fIstring\fR, \fIpattern\fR)
.DE
returns a numeric value corresponding to the first position where
.I pattern
appears in
.IR string .
If it does not appear, \fBindex\fP returns \-1.
Both
.I pattern
and
.I string
may be arbitrary strings of any length.
.PP
.II "m4^repeat"
The following example defines a macro \fBrepeat\fP
that repeats its first argument the number of times specified
by its second argument.
.DM
   define(\(garepeat\',
	\(gaifelse(eval($2<=0),1,,\(garepeat($1,decr($2) )'$1)')
.DE
.II "m4^ifelse"
.II "m4^eval"
The definition is recursive;  that is, \fBrepeat\fP
calls itself within its own definition.
The entire definition is quoted to defer the evaluation of \fBifelse\fP
from when
.B m4
encounters the definition to when it encounters a
.B repeat
macro call.
Similarly, the recursive
.B repeat
call is quoted to defer its evaluation within the
.BR ifelse .
\fBeval\fP checks if the first argument is less than or equal to 0;
if so, it returns 1 (true) and
.B ifelse
returns the null string.
.II "m4^decr"
Otherwise, \fBdecr\fP decrements the count,
so each successive recursive call has a smaller second argument,
and each call appends a copy of the first argument to the
previous result.
For example:
.DM
	repeat(\(gaHo! \',3)
.DE
produces
.DM
	Ho! Ho! Ho! 
.DE
From this example, you can see that the lowered value of the second
argument \(em generated by the macro
.BR decr \(em
is ``kept in mind'' successively.
Nevertheless,
.B decr 
and
.B incr
never change the value of a variable.
For example, consider:
.DM
	define(\(gaturns', 10)
.DE
We now have a variable called
.B turns
whose value is ten.
Typing
.DM
	repeat(\(gaHo! ', turns)
.DE
produces:
.DM
	Ho! Ho! Ho! Ho! Ho! Ho! Ho! Ho! Ho! Ho!
.DE
Within
.BR repeat ,
.B decr
lowered the current value of the second
argument (i.e., \fBturns\fR), until it becomes zero.
But when we type
.DM
	turns
.DE
we see:
.DM
	10	
.DE
As you can see, the value of
.B turns
remained ten, despite that variable's having been used in a
.B decr
statement.
.SH "COHERENT System Interface"
.PP
.II "m4^maketemp"
The predefined macro
.DM
	maketemp(string)
.DE
creates a unique file name for a temporary file.
.I string
is a six-character string that is normally initialized to \fBXXXXXX\fR;
.B maketemp
replaces all of the \fBX\fRs with a pattern of six numerals
that form a unique file name in the directory where temporary
files are being written.
It is the same as the C library routine
.BR mktemp .
It returns the null string if its argument is less than six characters long.
.PP
.II "m4^syscmd"
The predefined macro
.DM
	syscmd(command)
.DE
performs the given \*(CO
.I command
and returns the null string.
It is the same as the C library routine
.BR system .
.PP
A common use of
.B syscmd
is to create a file which
.B m4
subsequently reads with an
.BR include .
For example, to get the output from the \*(CO
.B date
command:
.DM
	define(\(gatempfile\', maketemp(/tmp/m4XXXXXX))
	define(\(gaget_date\',
		\(gasyscmd(date >tempfile)\'\(gainclude(tempfile)\')
.DE
In subsequent input,
.B m4
replaces each occurrence of
.B get_date
with the system date information.
The definition of
.B tempfile
is unquoted, so
.B m4
executes the
.B maketemp
call only once (when it processes the
.BR define ),
and it creates only one temporary file.
On the other hand, the definition of
.B getdate
is quoted, so
.B m4
executes
.B syscmd
and
.B include
to get the current time and date each time it processes a call to
.BR get_date .
The temporary file should be removed with
.DM
	syscmd(rm tempfile)
.DE
at the end of the
.B m4
program.
.PP
The following example is more complex.
It defines a macro
.BR save ,
which appends a macro definition to a file:
.DM
	define(\(gasave',\(gasyscmd(\(gacat>>$2 <<\e#
	define(\(ga$1\',\'dumpdef(\(ga$1\')\(ga)
	#
	\')\')
.DE
The arguments to
.B define
are the
.I name
.DM
	save
.DE
and the
.I definition
.DM
	syscmd(\(gacat >>$2 <<\e#
	define(\(ga$1\',\'dumpdef(\(ga$1\')\(ga)
	#
	\')
.DE
(Note that the body of macro
.B syscmd
uses the shell operator \fB<<\fR to create a ``here document''.
For more information on here documents, see the tutorial
\fIIntorducing sh, the Bourne Shell\fR.)
A typical call of this macro is:
.DM
	save(\(gasample\',\(gadefs.m4\')
.DE
which saves the macro definition of
.B sample
in a \*(CO file
.B defs.m4
containing macro definitions.
.II "m4^dumpdef"
When
.B m4
processes this call, the argument of
.B syscmd
becomes
.DM
	cat >>defs.m4 <<\e#
	define(\(gasample\',
.DE
followed by the definition of
.B sample
returned by \fBdumpdef\fP, followed by
.DM
	)
	#
.DE
.II "m4^syscmd"
Then \fBsyscmd\fP executes the \*(CO
.B cat
command to append the here document delimited by #
to the macro definition file
.BR defs.m4 .
The leading # delimiter of the here document is quoted with \e
to prevent interpretation by the \*(CO shell.
Because
.B save
uses the character # to delimit the here document,
it does not work correctly for macro definitions containing #.
For example,
.DM
	save(\(gasave\',\(gadefs.m4\')
.DE
does not work as expected.
.PP
Note that you can only use
.B save
when you run
.B m4
interactively \(em you cannot use it in a script.
Furthermore,
.B save
does not always save a definition literally.
For example:
.DM
	save(\(gatempfile', \(gadefs.m4')
.DE
saves the
.B tempfile
definition in
.B defs.m4
as:
.DM
	define(\(gatempfile', \(ga/tmp/m400074a') #
.DE
where, as you can see, the
.B XXXXXX
has been replaced with a hexadecimal number
(which may differ from the one you ).
Likewise, the definition of
.B get_date
will look like this:
.DM
	define(\(gaget_date', \(gasyscmd(date >tempfile)include(tempfile)') #
.DE
To load a saved definition into
.BR m4 ,
simply type
.B m4
at the shell's command-line prompt to invoke it interactively; and then type:
.DM
	sinclude(defs.m4)
.DE
From now on, you can use any definition that you had saved into file
.BR defs.m4 .
.Sh "Errors"
.PP
.B m4
reports all errors to the standard error stream.
An error produces a line of the form
.DM
	m4: line: message
.DE
where
.I line
is a decimal line number and
.I message
describes the error.
For example, the error message
.DM
	m4:  7:  illegal macro name: ab*c
.DE
indicates an attempt to
.B define
a macro with the illegal macro name
.B ab*c
in line 7 of the input stream.
.PP
The following error messages may occur:
.DM
	cannot open file
	eval: invalid expression
	eval: missing or unknown operator
	eval: missing value
	illegal macro name: name
	out of space
	/tmp open error
	unexpected EOF
.DE
The
.I file
or
.I name
will be the file name or macro name which caused the error, or
.B {NULL}
if the required argument is omitted.
.PP
.B m4
does not recognize (and therefore does not report) the most common of
.B m4
errors, namely invoking recursive macro definitions that
never terminate.
A simple example is the definition
.DM
	define(\(garecursive\', \(garecursive\')
.DE
.II "m4^endless loop"
When
.B m4
subsequently encounters a call of
.B recursive
in its input stream, it replaces it on the input stream with its definition.
Because the definition is another call to
.BR recursive ,
.B m4
replaces it in turn with its definition;
the process never terminates.
More complicated examples may involve many macro definitions
and may be difficult to discover.
If
.B m4
enters an endless loop, you can terminate it from the keyboard
by typing the interrupt character (normally
.BR <ctrl-C> )
or the kill character (normally <\fBctrl-\e\fR>).
If
.B m4
enters an endless loop while being run in the background, you
can terminate it with the
.B kill
command.
.SH "For More Information"
.PP
The Lexicon entry for
.B m4
gives a summary of its functions and options.
