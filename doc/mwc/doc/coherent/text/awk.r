.II "awk^tutorial"
.ds TL "The awk Language"
.NH "Introduction to the awk Language" 1
.PP
\fBawk\fR is a general-purpose language for processing text.
With \fBawk\fR, you can manipulate strings,
process records, and generate reports.
.PP
.II "Aho, A.V."
.II "Weinberger, P.J."
.II "Kernighan, Brian W."
.B awk
is named after its creators:
A. V. Aho, P. J. Weinberger, and Brian W. Kernighan.
Unfortunately, its name suggests that
.B awk
is awkward \(em whereas in truth the
.B awk
language is simple, elegant, and powerful.
With it, you can perform many tasks that would otherwise require
hours of drudgery.
.PP
\fBawk\fR uses a simple syntax.
Each statement in an \fBawk\fR program contains
either or both of two elements:
a \fIpattern\fR and an \fIaction\fR.
The pattern tells \fBawk\fR what lines to select from the input stream;
and the action tells \fBawk\fR what to do with the selected data.
.PP
This tutorial explains how to write
.B awk
programs.
It explains how to describe a pattern to \fBawk\fR.
It also describes the range of actions that
.B awk
can perform;
these include formatted printing of text,
assigning variables, defining arrays, and controlling the flow of data.
.SH "Example Files"
.PP
Before you begin to study
.BR awk ,
please take the time to type the following text files that
are used by the examples in this tutorial.
.PP
The first is some text from Shakespeare.
Use the command \fBcat\fR to type it into the file
.BR text1 ,
as follows.
Note that \fB<ctrl-D>\fR means that you should hold down the \fBCtrl\fR
(or \fBcontrol\fR) key and simultaneously press `D'.
Do not type it literally.
.DM
.II "Shakespeare, William"
	cat > text1
	When, in disgrace with fortune and men's eyes,
	I all alone beweep my outcast state,
	And trouble deaf heaven with my bootless cries,
	And look upon myself, and curse my fate,
	Wishing me like to one more rich in hope,
	Featured like him, like him with friends possest,
	Desiring this man's art and that man's scope,
	With what I most enjoy contented least.
	Yet in these thoughts myself almost despising,
	Haply I think on thee - and then my state,
	Like to the lark at break of day arising
	From sullen earth, sings hymns at heaven's gate;
	For thy sweet love remember'd such wealth brings
	That then I scorn to change my state with kings.
	<ctrl-D>
.DE
.II "Ruth, Babe"
The second example consists of some of Babe Ruth's batting statistics,
which we will use to demonstrate how \fBawk\fR processes tabular input.
Type it into file
.BR table1 ,
as follows:
.DM
	cat > table1
	1920	.376	54	158	137
	1921	.378	59	177	171
	1922	.315	35	94	99
	1923	.393	41	151	131
	1924	.378	46	143	121
	1925	.290	25	61	66
	1926	.372	47	139	145
	1927	.356	60	158	164
	1928	.323	54	163	142
	1929	.345	46	121	154
	<ctrl-D>
.DE
The columns give, respectively,
the season, the batting average, and the numbers of
home runs, runs scored, and runs batted in (RBIs).
.PP
The rest of this tutorial presents many examples that use these files.
Type them in and run them!
In that way, you can get a feel for
.B awk
into your fingers.
Experiment; try some variations on the examples.
Don't be afraid of making mistakes; this is one good way to learn the
limits (and the strengths) of a language.
.SH "Using awk"
.PP
\fBawk\fR reads input from the standard input
(entered from your terminal or from a file you specify),
processes each input line according to a specified \fBawk\fR program,
and writes output to the standard output.
This section explains the structure of an \fBawk\fR program
and the syntax of \fBawk\fR command lines.
.Sh "Command-line Options"
.PP
.II "awk^command-line options"
The complete form for the \fBawk\fR command line is as follows:
.DM
	\fBawk\fR  [\fB-y\^\fR] [\fB\-F\fIc\^\fR] [\fB\-f \fIprogfile\^\fR] [\fIprog\^\fR] [\fIfile1\^\fR] [\fIfile2\^\fR] ...
.DE
The following describes each element of the command line.
.IP \fB\-y\fR
Map
.I patterns
from lower case to both lower-case and upper-case letters.
For example, with this option the string
.B the
would match
.B the
or
.BR The .
.IP \fB\-F\fIc\fR
Set the field-separator character to the character
.IR c .
The field-separator character and its uses are described below.
.IP "\fB\-f \fIprogfile\fR"
Read the
.B awk
program from
.I progfile .
.IP \fIprog\fR
An
.B awk
program to execute.
If you do not use the
.B -f
option, you must enter
.BR awk 's
statements on its command line.
.sp \n(pDu
Note that if you include
.BR awk 's
program on its command line (instead of in a separate file),
you must enclose the program between apostrophes.
Otherwise, some of the
.B awk
statements will be modified by the shell before \fBawk\fR ever sees them,
which will make a mess of your program.
For example:
.DM
	awk 'BEGIN {print "sample output file"}
	            {print NR, $0}'
.DE
(The following sections explain
what the stuff between the apostrophes means.)
However, if you include the statement within a file that you pass to
.B awk
via its \fB\-f\fR option, you must \fInot\fR enclose the statements within
parentheses; otherwise,
.B awk
will become very confused.
If you were to put the statements in the above program into an
.B awk
program file, they would appear as follows:
.DM
	BEGIN {print "sample output file"}
        {print NR, $0}
.DE
.IP "\fIfile1 file2 ...\fR"
The files whose text you wish to process.
For example, the command
.DM
	awk '{print NR, $0}' text1
.DE
prints the contents of
.BR text1 ,
but precedes each line with a line number.
.sp \n(pDu
If you do not name an input file,
.B awk
processes what it reads from the standard input.
For example, the command
.DM
	awk '{print NR, $0}'
.DE
reads what you type from the keyboard and echoes it preceded with a line
number.
To exit from this program, type
.BR <ctrl-D> .
.SH "Structure of an awk Program"
.PP
.II "awk^statement"
An
.B awk
program consists of one or more statements of the form:
.DS
	\fIpattern \fB{ \fIaction \fB}\fR
.DE
Note that
.B awk
insists that the
action
be enclosed between braces, so that it can distinguish the action
from the pattern.
.PP
A program can contain as many statements
as you need to accomplish your purposes.
When
.B awk
reads a line of input,
it compares that line with the \fIpattern\fR in each statement.
Each time a line matches \fIpattern\fR,
\fBawk\fR performs the corresponding \fIaction\fR.
.B awk
then reads the next line of input.
.PP
A statement can specify an \fIaction\fR without a \fIpattern\fR.
In this case,
.B awk
performs the action on every line of input.
For example, the program
.DM
	awk '{ print }' text1
.DE
prints every line of
.B text1
onto the standard output.
.PP
An \fBawk\fR program may also specify a pattern without an action.
In this case, when an input line matches the pattern,
\fBawk\fR prints it on the standard output.
For example, the command
.DM
	awk 'NR > 0' table1
.DE
prints all of
.B table1
onto the standard output.
Note that you can use the same pattern in more than one statement.
Examples of this will be given below.
.PP
.BR awk 's
method of forming patterns uses
.I "regular expressions"
(also called
.IR patterns ),
like those used by the \*(CO commands
.BR sed ,
.BR ed ,
and
.BR egrep .
Likewise,
.BR awk 's
method of constructing actions is modelled after the C programming language.
If you are familiar with regular expressions and with C, you should have
no problem learning how to use
.BR awk .
However,
if you are not familiar with them, they will be explained in the following
sections.
.Sh "Records and Fields"
.PP
.II "awk^records"
.II "awk^fields"
\fBawk\fR divides its input into
.IR records .
It divides each record, in turn, into
.IR fields .
.II "awk^input-record separator"
.II "awk^input-field separator"
Records are separated by a character called
the
.IR "input-record separator" ;
likewise, fields are separated by the
.IR "input-field separator" .
.B awk
in effect conceives of its input as a table with an indefinite
number of columns.
.PP
The newline character is the default input-field separator,
so \fBawk\fR normally regards each input line as a separate record.
The space and the tab characters are
the default input-field separator,
so white space normally separates fields.
.PP
To address a field within a record, use the syntax \fB$\fIN\fR, where
.I N
is the number of the field within the current record.
The pattern \fB$0\fR addresses the entire record.
Examples of this will be given below.
.II "awk^output-record separator"
.II "awk^output-field separator"
In addition to input record and field separators,
\fBawk\fR provides output record and field separators,
which it prints between output records and fields.
The default output-field separator
is the newline character;
\fBawk\fR normally prints each output record as a separate line.
The space character is the default output-field separator.
.SH "Patterns"
.PP
This section describes how
.B awk
interprets the pattern section of a statement.
.Sh "Special Patterns"
.PP
.II "awk^special patterns"
.II "awk^pattern, special"
To begin,
.B awk
defines and sets a number of special \fIpattern\fRs.
You can use these patterns in your program for special purposes.
You can also redefine some of these patterns to suit your preferences.
The following describes the commonest such special
.IR patterns ,
and how they're used:
.II "awk^BEGIN"
.IP \fBBEGIN\fR
This pattern matches the beginning of the input file.
.B awk
executes all
.I actions
associated with this pattern before it begins to read input.
.II "awk^END"
.IP \fBEND\fR
This pattern matches the end of the input file.
.B awk
executes all
.I actions
associated with this pattern after it had read all of its input.
.II "awk^FILENAME"
.IP \fBFILENAME\fR
.B awk
sets this pattern to the name of the file
that it is currently reading.
Should you name more than one input file on the command line,
.B awk
resets this pattern as it reads each file in turn.
.II "awk^FS"
.IP \fBFS\fR
Input-field separator.
This pattern names the character that
.B awk
recognizes as the field separator for the records it reads.
.II "awk^NF"
.IP \fBNF\fR
This pattern gives the number of fields within the current record.
.II "awk^NR"
.IP \fBNR\fR
This pattern gives the number of the current record within the
input stream.
.II "awk^OFS"
.IP \fBOFS\fR
Output-field separator.
.B awk
sets this pattern to the character that it writes in its output to
separate one field from another.
.II "awk^ORS"
.IP \fBORS\fR
Output-record separator.
.B awk
sets this pattern to the character that it writes in its output to
separate one field from another.
.II "awk^RS"
.IP \fBRS\fR
Input-record separator.
.B awk
sets this pattern to the character by which it separates records
that it reads.
.Sh "Arithmetic Relational Expressions"
.II "awk^arithmetic operators"
.PP
An
.I operator
marks a task to be within an expression,
much as the `+' or `/' within an arithmetic expression indicates
that the numbers are to be, respectively, added or divided.
You can use \fBawk\fR's operators to:
.IP \(bu 0.3i
Compare a special pattern with a variable, a field, or a constant.
.IS \(bu
Assign a value to a variable or to a special pattern.
.IS \(bu
Dictate the relationship among two or more expressions.
.PP
The first type of operator to be discussed are
.IR "arithmetic relational operators" .
These compare the input text with an arithmetic value.
.B awk
recognizes the following arithmetic operators:
.DS
	\fB<\fR	Less than
	\fB<=\fR	Less than or equal to
	\fB==\fR	Equivalent
	\fB!=\fR	Not equal
	\fB>=\fR	Greater than or equal to
	\fB>\fR	Greater than
.DE
With these operators, you can compare a field with a constant;
compare one field with another;
or compare a special pattern with either a
field or a constant.
.PP
For example, the following
.B awk
program prints all of the years in which Babe Ruth hit more than
50 home runs:
.DM
	awk '$3 >= 50' table1
.DE
(As you recall, column 3 in the file
.B table1
gives the number of home runs.)
The program prints the following on your screen:
.DM
	1920	.376	54	158	137
	1921	.378	59	177	171
	1927	.356	60	158	164
	1928	.323	54	163	142
.DE
The following program, however, shows the years in which Babe Ruth scored
more runs than he drove in:
.DM
	awk '$4 > $5 { print $1 }' table1
.DE
Remember, field 4 in file
.B table1
gives the number of runs scored, and field 5 gives the number of
runs batted in.
You should see the following on your screen:
.DM
	1920
	1921
	1923
	1924
	1928
.DE
In the above program, expression
.DM
	{print $1}
.DE
defines the action to perform, as noted by the fact that expression is
enclosed between braces.
In this case, the program tells
.B awk
that if the input record matches the pattern, to print only the first
field.
However, to print both the season and the number of runs scored, use the
following program:
.DM
	awk '$4 > $5 { print $1, $4 }' table1
.DE
This prints the following:
.DM
	1920 158
	1921 177
	1923 151
	1924 143
	1928 163
.DE
Note that
.B $1
and
.B $4
are separated by a comma.
.II "awk^,"
The comma tells
.B awk
to print its default output-field separator between columns.
If we had left out the comma, the output would have appeared as follows:
.DM
	1920158
	1921177
	1923151
	1924143
	1928163
.DE
As we noted above, the special pattern
.B OFS
gives the output-field separator.
.B awk
by default defines this special pattern to the space character.
If we wish to redefine the output-field separator,
we can use an operator, plus the special pattern
.BR BEGIN ,
as follows:
.DM
	awk 'BEGIN { OFS = ":" }
		$4 > $5	{ print $1, $4 }' table1
.DE
This prints:
.DM
	1920:158
	1921:177
	1923:151
	1924:143
	1928:163
.DE
The first statement
.DM
	BEGIN { OFS = ":"}
.DE
tells
.B awk
to set the output-field separator (the special pattern \fBOFS\fR)
to `:' before it processes any input (as indicated by the special
pattern \fBBEGIN\fR).
.PP
Although we're getting a little ahead of ourselves, note that there's
no reason to print the fields in the order in which they appear in
the input record.
For example, if you wish to print the number of runs scored before the
season, use the command:
.DM
	awk 'BEGIN { OFS = ":"}
		$4 > $5	{ print $4, $1 }' table1
.DE
This prints:
.DM
	158:1920
	177:1921
	151:1923
	143:1924
	163:1928
.DE
As you recall, the special pattern
.B NR
gives the number of the current input record.
You can execute an action by comparing this pattern with a constant.
For example, the command
.DM
	awk 'NR > 12' text1
.DE
prints:
.DM
	For thy sweet love remember'd such wealth brings
	That then I scorn to change my state with kings.
.DE
That is, the program prints every line after line 12 in the input file.
As you recall, a statement that has a pattern but no action
prints the entire record that matches the pattern.
.PP
As we saw with the special patterns, some patterns can be defined to
be numbers and others to be text.
If you compare a number with a string,
.B awk
by default makes a string comparison.
The following example shows how \fBawk\fR
compares one field to part of the alphabet:
.DM
	awk '$1 <= "C"' text1
.DE
This program prints:
.DM
	And trouble deaf heaven with my bootless cries,
	And look upon myself, and curse my fate,
.DE
The statement
\fB$1 <= "C"\fR selected all records that begin
with an ASCII value less than or equal to that of the letter `C' (0x43) \(em
in this case, both lines that begin with `A' (0x41).
If we ran this example against
.BR table1 ,
it would print every record in the file.
This is because each record begins with the character `1' (0x31), which
matches the pattern \fB$1 <= "C"\fR.
.PP
Finally, you can use a numeric field plus a constant in a comparison
statement.
For example, the following program prints all of the seasons in which
Babe Ruth had at least 100 more runs batted in than home runs:
.DM
	awk '$3 + 100 < $5 {print $1}' table1
.DE
This prints the following:
.DM
	1921
	1927
	1929
.DE
.Sh "Boolean Combinations of Expressions"
.PP
.II "awk^Boolean operators"
.B awk
has a number of operators, called
.I Boolean
operators, that let you hook together several small expressions
into one large, complex expression.
.B awk
recognizes the following Boolean operators:
.DS
	\fB||\fR	Boolean OR (one expression or the other is true)
	\fB&&\fR	Boolean AND (both expressions are true)
	\fB!\fR	Boolean NOT (invert the value of an expression)
.DE
.II "Boole, George"
(The eponym ``Boolean'' comes from the English mathematician George Boole.)
In a Boolean expression,
.B awk
evaluates each sub-expression to see if it is true or false;
the relationship of sub-expressions (as set by the Boolean operator)
then determines whether the entire expression is true or false.
.PP
For example, the following program prints all seasons in which
Babe Ruth hit between 40 and 50 home runs:
.DM
	awk '$3 >= 40 && $3 <= 50 { print $1, $3 }' table1
.DE
This prints the following:
.DM
	1923 41
	1924 46
	1926 47
	1929 46
.DE
In the above program,
.B awk
printed its output only if the subexpression \fB$3 >= 40\fR
was true \fIand\fR (\fB&&\fR) the subexpression \fB$3 <= 50\fR
was true.
.PP
The next example demonstrates the Boolean OR operator.
It prints all seasons for which Babe Ruth hit fewer
than 40 home runs or more than 50 home runs:
.DM
	awk '$3 < 40 || $3 > 50 { print $1, $3}' table1
.DE
This example prints the following:
.DM
	1920 54
	1921 59
	1922 35
	1925 25
	1927 60
	1928 54
.DE
In this example,
.B awk
printed its output if the subexpression \fB$3 < 40\fR
was true \fIor\fR (\fB||\fR) the subexpression \fB$3 > 50\fR was true.
Note that the output would also printed if both subexpressions were true
(although in this case, this is impossible).
.PP
Finally, the Boolean operator `!' negates the truth-value of any expression.
For example, the expression \fB$1 = "And"\fR is true if the first field
in the current record equals ``And''; however, the expression
\fB$1 != "And"\fR is true if the first field does \fInot\fR equal ``And''.
For example, the program
.DM
	awk '$1 != "And"' text1
.DE
prints:
.DM
	When, in disgrace with fortune and men's eyes,
	I all alone beweep my outcast state,
	Wishing me like to one more rich in hope,
	Featured like him, like him with friends possest,
	Desiring this man's art and that man's scope,
	With what I most enjoy contented least.
	Yet in these thoughts myself almost despising,
	Haply I think on thee - and then my state,
	Like to the lark at break of day arising
	From sullen earth, sings hymns at heaven's gate;
	For thy sweet love remember'd such wealth brings
	That then I scorn to change my state with kings.
.DE
These are the 12 lines from \fBtext1\fR that do not begin with ``And''.
.PP
Note that
.B awk
evaluates all operators from left to right unless sub-expressions are
grouped together with parentheses, as is described in the following section.
.Sh "Patterns"
.II "awk^patterns"
.PP
The previous examples have all matched strings or numbers against predefined
fields in each input record.
This is fine for manipulating tabular information, like our table of
Babe Ruth's batting statistics, but it is not terribly useful when
you are processing free text.
Free text is not organized into predefined columns, nor are you likely to
know which field (that is, which word) will contain the pattern you're seeking.
.PP
.II ed
.II sed
To help you manage free text,
.B awk
has a pattern-matching facility that resembles those of the editors
.B ed
and
.BR sed .
.PP
The most common way to search for a pattern is to enclose it between slashes.
For example, the program
.DM
	awk '/and/' text1
.DE
prints every line in
.B text1
that contains the string ``and''.
.DM
	When, in disgrace with fortune and men's eyes,
	And look upon myself, and curse my fate,
	Desiring this man's art and that man's scope,
	Haply I think on thee - and then my state,
.DE
Note that ``and'' does not have to be a word by itself \(em it can be
a fragment within a word as well.
Note, too, that this pattern matches ``and'' but does not match ``And'' \(em
but it would if we were to use the \fB\-y\fR option on the \fBawk\fR command
line (described above).
.PP
You can use Boolean operators to search for more than one string at once.
For example, the program
.DM
	awk '/and/ && /or/' text1
.DE
finds every line in \fBtext1\fR that contains both ``and'' and ``or''.
There is only one:
.DM
	When, in disgrace with fortune and men's eyes,
.DE
Note that the ``or'' in this line is embedded in the word ``fortune''.
.PP
.B awk
can also scan for classes and types of characters.
To do so, enclose the characters within brackets and place the bracketed
characters between the slashes.
For example, the following program looks for every line in \fBtext1\fR
that contains a capital `A' through a capital `E':
.DM
	awk '/[A-E]/' text1
.DE
This prints the following:
.DM
	And trouble deaf heaven with my bootless cries,
	And look upon myself, and curse my fate,
	Desiring this man's art and that man's scope,
.DE
.II "awk^special characters"
In addition, you can use the following
special characters for further flexibility:
.DS
	\fB[ ]\fR	Class of characters
	\fB( )\fR	Grouping subexpressions
	\fB|\fR	Alternatives among expressions
	\fB+\fR	One or more occurrences of the expression
	\fB?\fR	Zero or more occurrences of the expression
	\fB*\fR	Zero, one, or more occurrences of the expression
	\fB.\fR	Any non-newline character
.DE
When adding a special character to a pattern, enclose the special
character as well as the rest of the pattern
within slashes.
.PP
To search for a string that contains one of
the special characters, you must precede the
character with a backslash.
For example, if you are looking for the string ``today?'',
use the following pattern:
.DM
	/today\e?/
.DE
When you need to find an expression in a
particular field, not just anywhere in the
record, you can use one of these operators:
.DS
	\fB~\fR	Contains the data in question
	\fB!~\fR	Does not contain the data in question
.DE
For example, if you need to find the digit `9'
in the fourth field of file \fBtable1\fR, use the following
program:
.DM
	awk '$4~/9/ {print $1, $4}' table1
.DE
This prints the following:
.DM
	1922 94
	1926 139
.DE
As you can see, the above program found every record with a `9' in its
fourth field, regardless of whether the `9' came at the beginning of the
field or its end.
.B awk
also recognizes two operators that let you set where a pattern is within
a field:
.DS
	\fB^\fR	Beginning of the record or field
	\fB$\fR	End of the record or field
.DE
For example, to find every record in \fBtable1\fR whose fourth field
\fIbegins\fR with a `9', run the following program:
.DM
	awk '$4~/^9/ {print $1, $4}' table1
.DE
This prints:
.DM
	1922 94
.DE
Finally, to negate a pattern use the operator \fB!~\fR.
For example, to print every record in
.B table1
whose fourth column does
.I not
begin with a `9', use the following program:
.DM
	awk '$4!~/^9/ {print $1, $4}' table1
.DE
This prints:
.DM
	1920 158
	1921 177
	1923 151
	1924 143
	1925 61
	1926 139
	1927 158
	1928 163
	1929 121
.DE
.Sh "Ranges of Patterns"
.PP
.II "awk^patterns, range"
You can tell \fBawk\fR to perform an action
on all records between two patterns.
For example, to print all records between the
.I patterns
\fB1925\fR and \fB1929\fR, inclusive,
enclose the strings in slashes and separate them with a comma,
then indicate the
.B print
action, as follows:
.DM
	awk '/1925/,/1929/ { print }' table1
.DE
You can also use the special pattern
.B NR
(or \fIrecord number\^\fR) to name a range of record numbers.
For example, to print records 5 through 10 of file
.BR text1 ,
use the following program:
.DM
	awk 'NR == 5, NR == 10 { print }' text1
.DE
.Sh "Resetting Separators"
.PP
.II "awk^separators, reset"
As noted above,
.B awk
recognizes certain characters by default to parse its input into
records and fields, and to separate its output into records and fields:
.II "awk^FS"
.IP \fBFS\fR
Input-field separator.
By default, this is one or more white-space characters (tabs or spaces).
.II "awk^OFS"
.IP \fBOFS\fR
Output-field separator.
By default, this is exactly one space character.
.II "awk^ORS"
.IP \fBORS\fR
Output-record separator.
By default, this is the newline character.
.II "awk^RS"
.IP \fBRS\fR
Input-record separator.
By default, this is the newline character.
.PP
By resetting any of these special patterns, you can change how
.B awk
parses its input or organizes its output.
Consider, for example, the command:
.DM
	awk 'BEGIN {ORS = "|"}
		/1920/,/1925/ {print $1, $5}' table1
.DE
This prints the following:
.DM
	1920 137|1921 171|1922 99|1923 131|1924 121|1925 66|
.DE
As you can see, this prints the season and the number of runs batted
in for the 1920 through 1925 season.
However,
.B awk
uses the pipe character `|' instead of the newline character to separate
records.
If you wish to change the output-field separator as well as the
output-record separator, use the program:
.DM
	awk 'BEGIN {ORS = "|" ;	OFS = ":"}
		/1920/,/1925/ {print $1, $5}' table1
.DE
This produces:
.DM
	1920:137|1921:171|1922:99|1923:131|1924:121|1925:66|
.DE
.II "awk^;"
As you can see,
.B awk
has used the colon `:' instead of a white-space character to separate
one field from another.
.PP
Note, too, that the semicolon `;' character separates expressions
in the action portion of the statement associated with the
.B BEGIN
pattern.
This lets you associate more than one action with a given pattern, so you
do not have to repeat that pattern.
This is discussed at greater length below.
.PP
You can also change the input-record separator from the newline
character to something else that you prefer.
For example, the following program changes the input-record separator
from the newline to the comma:
.DM
	awk 'BEGIN {RS = ","}
		{print $0}' text1
.DE
This yields the following:
.DM
	When
	 in disgrace with fortune and men's eyes
.sp \n(pDu
	I all alone beweep my outcast state
.sp \n(pDu
	And trouble deaf heaven with my bootless cries
.sp \n(pDu
	And look upon myself
	 and curse my fate
.sp \n(pDu
	Wishing me like to one more rich in hope
.sp \n(pDu
	Featured like him
	 like him with friends possest
.sp \n(pDu
	Desiring this man's art and that man's scope
.sp \n(pDu
	With what I most enjoy contented least.
	Yet in these thoughts myself almost despising
.sp \n(pDu
	Haply I think on thee - and then my state
.sp \n(pDu
	Like to the lark at break of day arising
	From sullen earth
	 sings hymns at heaven's gate;
	For thy sweet love remember'd such wealth brings
	That then I scorn to change my state with kings.
.DE
The blank lines resulted from a comma's occurring at the end of a line.
.PP
.II "awk^="
Note that by specifying the null string \fB(RS="")\fR,
you can make two consecutive newlines the record separator.
Note, too, that only one character can be the input-record separator.
If you try to reset this separator to a string,
.B awk
uses the first character in the string as the separator, and ignores the rest.
.PP
You can change the input-field separator by redefining
.BR FS .
The default
.B FS
is \fB<space>\et\fR exactly and in that order
(where \fB<space>\fR is the space character).
In this case,
.B awk
uses its ``white-space rule,'' in which
.B awk
treats any sequence of spaces and tabs as a single separator.
This is the default rule for
.BR FS .
If you set
.B FS
to anything else, including \fB\et<space>\fR, then each separator is separate.
For example, the following program changes the input-field separator
to the comma and prints the first such field it finds in each line from file
.BR text1 :
.DM
	awk 'BEGIN {FS = ","}
		{print $1}' text1
.DE
This produces:
.DM
	When
	I all alone beweep my outcast state
	And trouble deaf heaven with my bootless cries
	And look upon myself
	Wishing me like to one more rich in hope
	Featured like him
	Desiring this man's art and that man's scope
	With what I most enjoy contented least.
	Yet in these thoughts myself almost despising
	Haply I think on thee - and then my state
	Like to the lark at break of day arising
	From sullen earth
	For thy sweet love remember'd such wealth brings
	That then I scorn to change my state with kings.
.DE
As you can see, this program prints text up to the first comma in
each line.
.B awk
throws away the comma itself, because the input-field separator is not
explicitly printed.
.PP
You can define several characters to be input-field separators
simultaneously.
When you specify several characters within
quotation marks, each character becomes a field
separator, and all separators have equal precedence.
For example, you can specify the letters `i', `j', and `k' to be
input-field separators.
The following program does this, and prints the first field so
defined from each record in file
.BR text1 :
.DM
	awk 'BEGIN  {FS = "ijk"}
		{print $1}' text1
.DE
This prints:
.DM
	When, 
	I all alone beweep my outcast state,
	And trouble deaf heaven w
	And loo
	W
	Featured l
	Des
	W
	Yet 
	Haply I th
	L
	From sullen earth, s
	For thy sweet love remember'd such wealth br
	That then I scorn to change my state w
.DE
Note that if you set the input-record separator to a null string, you
can use the newline character as the input-field separator.
This is a handy way to concatenate clusters of lines into records that
you can then manipulate further.
.PP
One last point about the
.B FS
separator.
If the white-space rule is not invoked and
an assignment is made to a nonexistent field,
.B awk
can add the proper number of field separators.
For example if
\fBFS=":"\fR and the input line is \fBa:b\fR, then
the command \fB$5 = "e"\fR produces \fBa:b:::e\fR.
If the white-space rule were in effect,
.B awk
would add spaces as if each space were a separator,
and print a warning message.
In short, it would try to produce the sanest result from the error.
.PP
Finally, the variable
.B NR
gives the number of the current record.
The next example prints the total number of records in file
.BR text1 :
.DM
	awk 'END {print NR}' text1
.DE
The output is
.DM
	14
.DE
which is to be expected, since
.B text1
is a sonnet.
.SH "Actions"
.PP
.II "awk^actions"
The previous section described how to construct a
.I pattern
for
.BR awk .
For each pattern, there must be a corresponding
.IR action .
So far, the only action shown has been to print output.
However,
.B awk
can perform many varieties of actions.
In addition to printing, \fBawk\fR can:
.DS
	\(bu Execute built-in functions
	\(bu Redirect output
	\(bu Assign variables
	\(bu Use fields as variables
	\(bu Define arrays
	\(bu Use control statements
.DE
These actions are discussed in detail in the following sections.
.PP
As noted above, each
.B awk
statement must have an action.
If a statement does not include an action,
.B awk
assumes that the action is
.BR {print} .
.PP
Within each statement,
.B awk
distinguishes an action from its corresponding pattern by the fact
that the action is enclosed within braces.
Note that the action section of a statement may include several
individual actions;
however, each action must be separated from the others by
semicolons `;' or newlines.
.PP
.II "Free Software Foundation"
Some forms of
.BR awk ,
such as that provided by the Free Software Foundation (FSF),
allow user-defined functions.
The FSF version of
.B awk
is available from the MWC BBS as well as via COHware.
Note that your system must have at least two megabytes of RAM
to run the FSF version of
.BR awk .
.Sh "awk Functions"
.PP
.II "awk^functions"
\fBawk\fR includes the following functions with which you can
manipulate input.
You can assign a function to any variable or use it in a pattern.
The following lists
.BR awk 's
functions.
Note that an
.I argument
can be a variable, a field, a constant, or an expression:
.II "awk^abs"
.IP \fBabs(\fIargument\^\fB)\fR
Return the absolute value of \fIargument\fR.
.II "awk^exp"
.IP \fBexp(\fIargument\^\fB)\fR
.II "Euler, Leonhart"
Return Euler's number
.I e
(2.178...) to the power of \fIargument\fR.
.II "awk^index"
.IP \fBindex(\fIstring1\^\fB,\fIstring2\^\fB)\fR
Return the position of \fIstring2\fR within
\fIstring1\fR.
If \fIs2\fR does not occur in \fIs1\fR,
.B awk
returns zero.
This
.B awk
function resembles the \*(CO C function
.BR index() .
.II "awk^int"
.IP \fBint(\fIargument\^\fB)\fR
Return the integer portion of \fIargument\fR.
.II "awk^length"
.IP \fBlength\fR
Return the length, in bytes, of the current record.
.IP \fBlength(\fIargument\^\fB)\fR
Return the length, in bytes, of \fIargument\fR.
.II "awk^log"
.IP \fBlog(\fIargument\^\fB)\fR
Return the natural logarithm of \fIargument\fR.
.II "awk^print"
.IP "\fBprint(\fIargument1 argument2 ... argumentN\^\fB)\fR"
Concatenate and print
.I argument1
through
.IR argumentN .
.IP "\fBprint(\fIargument1\^\fB,\fIargument2\^\fB, \fI... argumentN\^\fB)\fR"
Print
.I argument1
through
.IR argumentN .
Separate each
.I argument
with the
.B OFS
character.
.II "awk^printf"
.IP "\fBprintf(\fIf\^\fB, \fIargument1\^\fB, \fI... argumentN\^\fB)\fR"
Format and print strings \fIargument1\fR through \fIargument\fR in the
manner set by the formatting string
.IR f ,
which can use \fBprintf()\fR-style formatting codes.
.IP "\fBsplit(\fIstr\^\fB, \fIarray\^\fB, \fIfs\^\fB)\fR"
Divide the string \fIstr\fR into fields associated with
\fIarray\fR.
The fields are separated by character \fIfs\fR or the default field separator.
.II "awk^sprintf"
.IP "\fBsprintf(\fIf\^\fB, \fIe1\^\fB, \fIe2\^\fB)\fR"
Format strings \fIe1\fR and \fIe2\fR in the
manner set by the formatting string
.IR f ,
and return the formatted string.
.I f
can use \fBprintf()\fR-style formatting codes.
.II "awk^sqrt"
.IP "\fBsqrt(\fIargument\^\fB)\fR"
Return the square root of \fIargument\fR.
.II "awk^substr"
.IP "\fBsubstr(\fIstr\^\fB, \fIbeg\^\fB, \fIlen\^\fB)\fR"
Scan string \fIstr\fR for position \fIbeg\fR;
if found, print the next \fIlen\fR characters.
If \fIlen\fR is not included, print from from \fIbeg\fR
to the end of the record.
.Sh "Printing with awk"
.PP
.II "awk^printing"
Printing is the commonest task you will perform in your
.B awk
programs.
.BR awk 's
printing functions
.B printf
and
.B sprintf
resemble the C functions
.B printf()
and
.BR sprintf() ;
however, there are enough differences to make a close reading of this
section worthwhile.
.PP
.B print
is the commonest, and simplest, \fBawk\fR function.
When used without any arguments,
.B print
prints all of the current record.
The following example prints every record in file
.BR text1 :
.DM
	awk '{print}' text1
.DE
You can print fields in any order you desire.
For example, the following program reverses the order of the season
and batting-average columns from file
.BR table1 :
.B
.DM
	awk '/1920/,/1925/ { print $2,$1 }' table1
.DE
The output is as follows:
.DM
	.376 1920
	.378 1921
	.315 1922
	.393 1923
	.378 1924
	.290 1925
.DE
Because the field names are separated by a comma,
\fBawk\fR inserts the
.B OFS
between the fields when it prints them.
If you do not separate field names with commas,
\fBawk\fR concatenates the fields when it printing them.
For example, the program
.DM
	awk '/1920/,/1925/ { print $2 $1 }' table1
.DE
produces:
.DM
	.3761920
	.3781921
	.3151922
	.3931923
	.3781924
	.2901925
.DE
When you use \fBawk\fR to process a column of text or numbers,
you may wish to specify a consistent format for the output.
The statement for formatting a column of numbers follows this
.IR pattern :
.DM
	{printf "format", expression}
.DE
where
.I format
prescribes how to format the output, and
.I expression
specifies the fields for \fBawk\fR to print.
.PP
The following table names and defines
the most commonly used of \fBawk\fR's format control characters.
Each character must be preceded by a percent sign `%'
and a number in the form of
.I n
or
.IR n.m .
.DS
	\fB%\fIn\fBd\fR	Decimal number
	\fB%\fIn\^\fB.\fIm\^\fBf\fR	Floating-point number
	\fB%\fIn\^\fB.\fIm\^\fBs\fR	String
	\fB%%\fR	Literal `%' character
.DE
When you use the
.B printf()
function, you must define the output-record separator within the
format string.
The following codes are available:
.DS
	\fB\en\fR	Newline
	\fB\et\fR	Tab
	\fB\ef\fR	Form feed
	\fB\er\fR	Carriage return
	\fB\e"\fR	Quotation mark
.DE
For example, the following program prints Babe Ruth's RBIs unformatted:
.DM
	awk '/1920/,/1925/ { print $1, $5 }' table1
.DE
The output appears as follows:
.DM
	1920 137
	1921 171
	1922 99
	1923 131
	1924 121
	1925 66
.DE
As you can see,
.B awk
right-justifies its output by default.
To left-justify the second column, use the following program:
.DM
	awk '/1920/,/1925/ { printf("%d %3d\en", $1, $5) }' table1
.DE
The output is as follows:
.DM
	1920 137
	1921 171
	1922  99
	1923 131
	1924 121
	1925  66
.DE
Note that the `3' in the string
.B %3d
specifies the minimum number of characters to be displayed.
If the size of the number exceeds the space allotted to it,
.B awk
prints the entire number.
A different rule applies when printing strings, as will be
shown below.
.PP
To print a floating-point number, you must specify the minimum number of
digits you wish to appear on either side of the decimal point.
For example, the following program gives the average number of RBIs
Babe Ruth hit in each game between 1920 and 1925:
.DM
	awk '/1920/,/1925/ { printf("%d %1.2f\en", $1, $5/154.0) }' table1
.DE
This prints the following:
.DM
	1920 0.89
	1921 1.11
	1922 0.64
	1923 0.85
	1924 0.79
	1925 0.43
.DE
Note the following points about the above program:
.IP \(bu 0.3i
To get the average number of runs batted in, we had to divide the
total number of RBIs in a season by the number of games in a season
(which in the 1920s was 154).
.B awk
permits you to use a constant to perform arithmetic on a field; this will
be discussed in more detail below.
.IP \(bu
To force
.B awk
to produce a floating-point number, the constant had to be in the format
of a floating-point number, i.e., ``154.0'' instead of ``154''.
Dividing an integer by another integer would not have produced what we wanted.
.PP
.B awk
rounds its output to match sensitivity you've requested \(em that is,
the number of digits to the right of the decimal point.
To see how sensitivity affects output, run the following program:
.DM
awk '/1920/,/1925/{printf("%1.2f %1.3f %1.4f\en",$5/154.0,$5/154.0,$5/154.0)}'\e
table1
.DE
This prints the following:
.DM
	0.89 0.890 0.8896
	1.11 1.110 1.1104
	0.64 0.643 0.6429
	0.85 0.851 0.8506
	0.79 0.786 0.7857
	0.43 0.429 0.4286
.DE
As an aside, the above example also shows that
you can break \fBawk\fR's command line across more than one
line using a backslash `\e' at the end of every line but the last.
Note, however, that you
.I cannot
break an
.B awk
statement across more than one line, or
.B awk
will complain about a syntax error.
.PP
One last example of floating-point numbers prints Babe Ruth's
ratio of runs scored to runs batted in between 1920 and 1925:
.DM
	awk '/1920/,/1925/{x = ($5*1.0) ; printf("%1.3f\en", $4/x)}' table1
.DE
This produces the following:
.DM
	1.153
	1.035
	0.949
	1.153
	1.182
	0.924
.DE
The expression
.B "x = ($5*1.0)"
was needed to turn field 5 (the divisor) into a floating-point number,
so we could obtain the decimal fraction that we wanted.
This is discussed further below, when we discuss how to manipulate
constants.
.PP
The function
.B sprintf()
also formats expressions; however, instead of printing
its output, it returns it for assignment to a variable.
For example, you could rewrite the previous example program to replace
the multiplication operation with a call to
.BR sprintf() :
.DM
	awk '/1920/,/1925/{x = sprintf("%3.1f", $5)
		printf("%1.3f\en", $4/x)}' table1
.DE
The output is the same as that shown above.
.PP
The
.B %s
formatting string can be used to align text in fields.
The digit to the left of the period gives the width of the field;
that to the right of the period gives the number of characters to write
into the field.
Note that if input is larger than the number of characters allotted to it,
.B awk
truncates the input.
For example, the following program aligns on seven-character fields
some words from file
.BR text1 :
.DM
	awk '{x=sprintf("%7.5s %7.5s %7.5s %7.5s", $1, $2, $3, $4)
		print x}' text1
.DE
The output is as follows:
.DM
	  When,      in   disgr    with
	      I     all   alone   bewee
	    And   troub    deaf   heave
	    And    look    upon   mysel
	  Wishi      me    like      to
	  Featu    like    him,    like
	  Desir    this   man's     art
	   With    what       I    most
	    Yet      in   these   thoug
	  Haply       I   think      on
	   Like      to     the    lark
	   From   sulle   earth   sings
	    For     thy   sweet    love
	   That    then       I   scorn
.DE
Note that fields (words) longer than five characters are truncated;
and every word is right-justified on a seven-character field.
.Sh "Redirecting Output"
.PP
.II "awk^redirecting output"
In addition to printing to the standard output,
.B awk
can redirect the output of an action into a file, or append it onto
an existing file.
With this feature, you can extract information from
a given file and construct new documents.
The following example shows an easy way to sift Babe Ruth's statistics
into four separate files, for further processing:
.DM
	awk '{	print $1, $2 > "average"
		print $1, $3 > "home.runs"
		print $1, $4 > "runs.scored"
		print $1, $5 > "rbi"}' table1
.DE
Note like as under the shell, the
operator `>' creates the named file if it does not exist,
or replaces its contents if it does.
To append
.BR awk 's
onto the end of an existing file, use the operator `>>'.
.PP
.B awk
can also pipe the output of an action to another program.
As under the shell, the operator `|' pipes the output of one
process into another process.
For example, if it is vital for user
.B fred
to know Babe Ruth's batting
average for 1925, you can mail it to him with the following command:
.DM
	awk '/1925/ {print $1, $2 | "mail fred"}' table1
.DE
.Sh "Assignment of Variables"
.PP
.II "awk^assigning variables"
A number of the previous examples assign values to variables.
.B awk
lets you create variables, perform arithmetic upon them,
and otherwise work with them.
.PP
An
.B awk
variable can be a string or a number,
depending upon the context.
Unlike C,
.B awk
does not require that you declare a variable.
By default, variables are set to the null
string (numeric value zero) on start-up of the \fBawk\fR program.
To set the variable
.B x
to the numeric value one, you can use the assignment operator `=':
.DM
	x = 1
.DE
To set x to the string
.B ted ,
also use the assignment operator:
.DM
	x = "ted"
.DE
When the context demands it, \fBawk\fR converts strings to numbers or numbers
to strings.
For example, the statement
.DM
	x = "3"
.DE
initializes to
.B x
to the string ``3''.
When an expression contains an
arithmetic operator such as the `-',
\fBawk\fR interprets the expression as numeric.
(Alphabetic strings evaluate to zero.)
Therefore, the expression
.DM
	x = "3" - "1"
.DE
assigns the numeric value two to variable
.B x ,
not the string ``2''.
.PP
When the operator is included within the
quotation marks, \fBawk\fR treats the operator as a
character in the string.
In the following example
.DM
	x = "3 - 1"
.DE
initializes
.B x
to the string ``3 - 1''.
.PP
A number of examples in the previous section showed you how to perform
arithmetic on fields.
.II "awk^arithmetic operators"
The following table gives \fBawk\fR's arithmetic operators:
.DS
	\fB+\fR	Addition
	\fB-\fR	Subtraction
	\fB*\fR	Multiplication
	\fB/\fR	Division
	\fB%\fR	Modulus
	\fB++\fR	Increment
	\fB--\fR	Decrement
	\fB+=\fR	Add and assign value
	\fB-=\fR	Subtract and assign value
	\fB*=\fR	Multiply and assign value
	\fB/=\fR	Divide and assign value
	\fB%=\fR	Divide modulo and assign value
.DE
Variables are often used with increment operators.
For example, the following program computes the average number of home
runs Babe Ruth hit each season during the 1920s:
.DM
	awk '	{ x += $3 }
		END { y = (NR * 1.0) 
			printf("Average for %d years: %2.3f.\en", NR, x/y) }' table1
.DE
The output is:
.DM
	Average for 10 years: 46.700.
.DE
.Sh "Field Variables"
.PP
.II "awk^field variables"
\fBawk\fR lets fields receive assignments, be used in arithmetic,
and be manipulated in string operations.
One task that has not yet been demonstrated is using 
a variable to address a field.
For example, the following program prints the
.BR NR th
field (word) from the first seven lines in file
.BR text1 :
.DM
	awk 'NR < 8 {print NR, $(NR)}' text1
.DE
The output is:
.DM
	1 When,
	2 all
	3 deaf
	4 myself,
	5 one
	6 with
	7 man's
.DE
.Sh "Control Statements"
.PP
.II "awk^control statements"
\fBawk\fR has seven defined control statements.
This section explains them and gives examples of their use.
.II "awk^if"
.II "awk^else"
.IP "\fBif (\fIcondition\^\fB) \fIaction1 \fB[else\fI action2 \fB]\fR"
If \fIcondition\fR is true, then execute
.IR action1 .
If the optional
.B else
clause is present and
.I condition
is false, then
execute
.IR action2 .
.sp \n(pDu
The following program keeps running totals of Babe Ruth's RBIs, for both
the years where his runs scored exceeded his RBIs and the years where they
did not:
.DM
	awk '{ if ( $4 > $5 )
		    gyear++
		else
		    lyear++
	     }
	     END { printf("Scored exceed RBIs: %d years.\n", gyear)
		   printf("Scored not exceed RBIs: %d years.\n", lyear)
	     }' table1
.DE
This produces:
.DM
	Scored exceed RBIs: 5 years.
	Scored not exceed RBIs: 5 years.
.DE
Note that if more than one action is associated with an
.B if
or
.B else
statement, you must enclose the statements between braces.
If you use braces with both the
.B if
and
.B else
statements, note that the beginning and closing braces
.I must
appear on the same line as the
.B else
statement.
For example:
.DM
	if (expr) {
		stuff
		stuff
	} else {
		stuff 
		stuff
	}
.DE
.II "awk^while"
.IS "\fBwhile (\fIcondition\^\fB) \fIaction\fR"
The
.B while
statement executes
.I action
as long as
.I condition
is true.
For example, the following program counts the number of times the word
.B the
appears in file
.BR text1 .
The
.B while
loop uses a variable to examine every word in every line:
.DM
	awk ' { i = 1
		while (i <= NF ) {
			if ($i == "the") j++
			i++
		}
	      }
	END   { printf ("The word \e"the\e" occurs %d times.\en", j) }' text1
.DE
.II "Shakespeare, William"
The result, as follows, shows Shakespeare's economy of language:
.DM
	The word "the" occurs 1 times.
.DE
By the way, note that if a control statement has more than one statement
in its action section,
enclose the action section between braces.
If you do not,
.B awk
will behave erratically or exit with a syntax error.
.II "awk^for"
.IP "\fBfor( \fIinitial\fB ; \fIend\fB ; \fIiteration\fB ) \fIaction\fR"
.IS "\fBfor( \fIvariable\fB in \fIarray\fB ) \fIaction\fR"
.BR awk 's
.B for
statement closely resembles the
.B for
statement in the C language.
The statement
.I initial
defines actions to be performed before the loop begins; this is usually
used to initialize variables, especially counters.
The statement
.I end
defines when the loop is to end.
The statement
.I iteration
defines one or more actions that are performed on every iteration of the
loop; usually this is used to increment counters.
Finally,
.I action
can be one or more statements that are executed on every iteration of
the loop.
.I action
need not be present, in which case only the action defined in the
.I iteration
portion of the
.B for
statement is executed.
.B for
is in fact just an elaboration of the
.B while
statement, but adjusted to make it a little easier to use.
The following example writes the previous example, but replaces the
.B while
loop with a
.B for
mechanism:
.DM
	awk ' { for (i = 1 ; i <= NF ; i++)
		    if ($i == "the") j++
	      }
	END   { printf ("The word \e"the\e" occurs %d times.\en", j) }' text1
.DE
The output is the same as the previous example, but the syntax is neater
and easier to read.
.sp \n(pDu
The second form of the
.B for
loop examines the contents of an array.
It is described in the following section, which introduces arrays.
.IP \fBbreak\fR
.II "awk^break"
The statement
.B break
immediately interrupts a
.B while
or
.B for
loop.
For example, the following program is the same as the previous example,
but counts only the first occurrence of the word
.B the
in each line of
.BR text1 .
Thus, it counts the number of lines in
.B text1
that contain
.BR the :
.DM
	awk '{ for (i = 1 ; i <= NF ; i++) {
		    if ($i == "the") {
		    	j++
			break
		    }
		 }
	      }
	END   {printf ("The word \e"the\e" occurs in %d lines.\en", j)}' text1
.DE
.II "awk^continue"
.IS \fBcontinue\fR
The statement
.B continue
immediately begins the next iteration of the nearest
.B while
or
.B for
loop.
For example, the following program prints all of Babe Ruth's statistics \(em
runs scored, runs batted, and home runs \(em in which he had more than 59
in one year:
.DM
	awk ' { for (i = 3 ; i <= NF ; i++)
		    if ($i <= 59)
			continue
		    else
			printf("%d, column %d: %d\en", $1, i, $i)
	      } ' table1
.DE
This produces the following:
.DM
	1920, column 4: 158
	1920, column 5: 137
	1921, column 4: 177
	1921, column 5: 171
	1922, column 4: 94
	1922, column 5: 99
	...
.DE
.II "awk^next"
.IS \fBnext\fR
The statement
.B next
forces
.B awk
abort the processing of the current record and
skip to the next input record.
Processing of the new input record begins with the first pattern, just as
if the processing of the previous record had concluded normally.
To demonstrate this, the following program skips all records in file
.B text1
that have an odd number of fields (words):
.DM
	awk ' { if (NF % 2 == 0) next }
	      { print $0 } ' text1
.DE
This produces:
.DM
	I all alone beweep my outcast state,
	Wishing me like to one more rich in hope,
	With what I most enjoy contented least.
	Yet in these thoughts myself almost despising,
	Like to the lark at break of day arising
.DE
.II "awk^exit"
.IS \fBexit\fR
Finally, the control statement
.B exit
forces the \fBawk\fP program to skip all remaining input and
execute the \fIactions\fR at the \fBEND\fR pattern, if any.
For example, the following program prints the year in which Babe Ruth
hit his 300th home run:
.DM
	awk ' { i =  $1 }
	      (j += $3) >= 300 { exit }
	      END {print "Babe Ruth hit his 300th homer in", i "."}' table1
.DE
This produces:
.DM
	Babe Ruth hit his 300th homer in 1926.
.DE
.SH "Arrays"
.PP
.II "awk^arrays"
.B awk
has a powerful feature for managing arrays.
Unlike C,
.B awk
automatically manages the size of an array, so you do not have to declare
the array's size ahead of time.
Also, unlike C,
.B awk
lets you address each element within an array by a label, not just by its
offset within the array.
This lets you generate arrays ``on the fly,'' which can be very useful
in transforming many varieties of data.
.PP
To declare an array, simply name it within a statement.
.B awk
recognizes as an array every variable that is followed by brackets `[\ ]'.
To initialize a row within an array, you must define its value and name its
label.
A label can be either a number or a string.
A value, too, can be a number or a string; if the value is a number, then
you can perform arithmetic upon it, as will be shown in a following example.
.Sh "Initializing an Array"
.PP
To demonstrate how an array works, use the line editor
.B ed
to add a line of text to the beginning of file
.BR table1 .
Type the following; please note that the token
.B <tab>
means that you should type a tab character:
.DM
	ed table1
	1i
	Year\fB<tab>\fPBA\fB<tab>\fPHRs\fB<tab>\fPScored\fB<tab>\fPRBIs
	.
	wq
.DE
This change writes a header into \fBtable1\fR that names each column.
Now, we can read these labels into an array and use them to
describe Babe Ruth's statistics.
For example, the following prints a summary of Babe Ruth's statistics for
the year 1926:
.DM
	awk '	NR == 1 { for (i=1; i <= NF; i++) header [i] = $i }
		$1 == 1926 { 
			for (i=1; i <= NF; i++)
				print header[i] ":\et", $i
		} ' table1
.DE
This produces:
.DM
.ta 0.5i 1.5i
	Year:	 1926
	BA:	 .372
	HRs:	 47
	Scored:	 139
	RBIs:	 145
.DE
The statement
.DM
	NR == 1 { for (i=1; i <= NF; i++) header [i] = $i }
.DE
reads the first line in
.BR table1 ,
which contains the column headers, and uses the headers to initialize the array
.BR header .
Each row is labeled with the contents of the variable
.BR i .
.PP
The loop
.DM
	for (i=1; i <= NF; i++)
		print header[i] ":\et", $i
.DE
prints the contents of
.BR header .
Because we labeled each row within
.B header
with a number, we can use a numeric loop to read its contents.
.Sh "The for() Statement With Arrays"
.PP
In the previous example, each element in the array was labeled with a number.
This permitted us to read the array with an ordinary
.B for
statement, which sets and increments a numeric variable.
However, the rows within an array can be labeled with strings, instead of
numbers.
To read the contents of such an array, you must use a special form of the
.BR for
statement, as follows:
.DS
	\fBfor ( \fIoffset \fBin \fIarray \fB)\fR
.DE
.I array
names the array in question.
.I offset
is a variable that you name at the time of contructing the
.B for
statement.
You can use the value of
.I offset
in any subordinate printing actions.
.PP
The following program demonstrates this new form of
.BR for ,
and (incidentally) to demonstrate the power of
.BR awk 's
array-handling feature.
It builds an array of each unique word in the file
.BR text1 ,
and notes the number of times that word occurs within the file:
.DM
	awk '	{ for (i = 1 ; i <= NF ; i++)
			words [$i]++ }
		END { for (entry in words)
			print entry ":", words[entry] }' text1 | sort
.DE
This prints:
.DM
	-: 1
	And: 2
	Desiring: 1
	Featured: 1
	For: 1
	From: 1
	Haply: 1
	I: 4
	Like: 1
	That: 1
	When,: 1
	Wishing: 1
	With: 1
	Yet: 1
	all: 1
	almost: 1
	...
.DE
As you can imagine, a similar program in C would require many more
lines of code.
However, a few features of this program are worth noting.
.PP
First, the expression
.DM
	{ for (i = 1 ; i <= NF ; i++)
		words [$i]++ }
.DE
declares the array
.B words .
Every time
.B awk
encounters a new field (word), it automatically adds another entry to the
array, and labels that entry with the word.
No work on your part is needed for this to happen.
.II "awk^++"
The `++' operator increments the value of the appropriate entry within
.BR words .
Because we did not initialize the entry, it implicitly contains a number.
.PP
The expression
.DM
	{ for (entry in words)
		print entry ":", words[entry] }
.DE
walks through the array
.BR words .
.B awk
initializes the variable
.B entry
to the label for each row in
.BR words ;
the
.B print
statement then prints
.B entry
and the contents of that row in the array \(em in this case, the number of
times the row appears in our input file.
.PP
Finally, we piped the output of this program to the command
.B sort
to print the words in alphabetical order.
.SH "For More Information"
.PP
This tutorial just gives a brief introduction to the power of
.BR awk .
To explore the language in depth, see \fIsed & awk\fR by Dale Dougherty
(Sebastopol, Calif, O'Reilly & Associates, Inc., 1985).
This book, however, describes a more complex version of
.B awk
than that provided with \*(CO.
.PP
The Lexicon's article on \fBawk\fR gives a quick summary of
its features and options.
