.II "lex^tutorial"
.ds TL "lex Lexical Analyzer"
.NH "Introduction to lex, the Lexical Analyzer" 1
.PP
Many computer applications involve reading text strings.
This is especially true for man-machine communication.
.PP
For some forms of textual input, a programmer can design a program
by hand to process it.
However, it is much easier to implement such programs
when you use a software tool that will automatically
construct a program to process the data.
The \*(CO command \fBlex\fR is such a tool.
.PP
.II "lex^program generator"
\fBlex\fR accepts expressions that describe the text input,
and generates a program to process it.
In computer-ese,
\fBlex\fR is a ``lexical scanner program generator''.
.PP
This document tells you how to use \fBlex\fR.
It presents
many simple examples to illustrate how to use its
features and how to use the generated program with other
tools provided with \*(CO, notably the parser generator \fByacc\fR.
.PP
Readers of this document are presumed to be familiar with the C programming
language and the use of the \*(CO system.
Related documents include
\fIUsing the COHERENT System\fR and the tutorial to
\fRyacc\fR, the \*(CO parser generator.
.SH "How To Use lex"
.PP
\fBlex\fR generates lexical scanners for compilers, to do
statistical analysis of text, and to generate filters for many
diverse tasks.
This section gives examples of how to use \fBlex\fR.
Later sections discuss the concepts used in these examples in detail.
.Sh "Translating Strings"
.PP
The first example tells \fBlex\fR to match an input string
and replace it with a different string \(em in this case, replace
the misspelled word ``removeable'' with the correctly spelled ``removable''.
The program outputs unchanged all strings that it does not recognize.
Enter the following program into the file \fBrmv.lex\fR.
.DM
	%%
	removeable	printf ("removable");
.DE
.II "lex^lex specification"
This creates the \fBlex\fP specification.
Use the following command line to
pass this specification through \fBlex\fR:
.DM
	lex rmv.lex
.DE
This produces a C program named \fBlex.yy.c\fR,
which you can compile by typing:
.DM
	cc lex.yy.c -ll -o rmv
.DE
The executable program \fBrmv\fR is now ready to use.
To illustrate its use, type:
.DM
	rmv
	Is this file removeable?
	<ctrl-D>
.DE
\fBrmv\fR replies:
.DM
	Is this file removable?
.DE
Note that the generated program reads from standard input and writes to
standard output.
.Sh "Remove Blanks From Input"
.PP
The next example deletes all blanks and tabs from the input.
Type the following \fBlex\fR program into file \fBnosp.lex\fR:
.DM
	%%
	[ \et]+  ;
.DE
Generate and compile the program with the following commands:
.DM
	lex nosp.lex
	cc lex.yy.c -ll -o nosp
.DE
To invoke the program, type
.BR nosp .
Now, test it by typing the following:
.DM
	This may be hard to read after processing.
	<ctrl-D>
.DE
.B nosp
outputs:
.DM
	Thismaybehardtoreadafterprocessing.
.DE
.Sh "Trimming Blanks"
.PP
The previous example can be rewritten to remove strings of blanks or tabs and
replace them with one space.
Type the following into file \fBonesp.lex\fR:
.DM
	%%
	[ \et]+  printf (" ");
.DE
Generate and compile this with the following commands:
.DM
	lex onesp.lex
	cc lex.yy.c -ll -o onesp
.DE
Invoke your program with the command \fBonesp\fR.
Now, type the following text to test the program;
be sure to separate each word by two spaces:
.DM
	This  should  be  easier  to  read.
	<ctrl-D>
.DE
.B onesp
prints the following:
.DM
	This should be easier to read.
.DE
.SH "lex Specification Form"
.PP
This section discusses the form of the \fBlex\fR specification.
.Sh "Simple Form"
.PP
The examples shown above use the simplest form of a
\fBlex\fR program.
Consider the text of the example \fBrmv.lex\fR:
.DM
	%%
	removeable	printf ("removable");
.DE
The symbol
.DM
	%%
.DE
.II "lex^%%"
divides sections of the \fBlex\fR specification.
Not all specifications need to be present, but at least one
\fB%%\fR must appear in a \fBlex\fR program.
.PP
.II "lex^definitions"
.II "lex^rules"
This symbol separates \fBlex\fR \fIdefinitions\fR from \fIrules\fR.
With nothing before the \fB%%\fR, there are no definitions.
Rules follow the \fB%%\fR.
No definitions are needed in the simplest of \fBlex\fR specifications.
.Sh "Rules in lex"
.PP
The format of a \fBlex\fR rule is simple.
Every rule has two parts.
Refer to the program \fBrmv\fR:
.DM
	removeable	printf ("removable");
.DE
The first part begins at the beginning of the line and ends with a space or tab.
In the example rule, the first part is
.DM
	removeable
.DE
.II "lex^pattern"
This part is called the \fIpattern\fR.
.PP
.II "lex^action"
The second part follows the space or tab, and is called the \fIaction\fR.
The action in this example is:
.DM
	printf ("removable");
.DE
When the pattern specified by the rule is found in the input,
the corresponding action is performed.
Thus, this rule detects every appearance of \fIremoveable\fR
and outputs the correct spelling.
.PP
A
.B lex
program tries each rule's pattern in turn, and performs the
associated action if and only if the pattern matches.
Actions often modify the input that matched the pattern;
they may also do nothing for certain patterns.
To illustrate this, type the following specification
into file \fBerase.lex\fR:
.DM
	%%
	erase	;
.DE
Then compile the generated program with the following commands:
.DM
	lex erase.lex
	cc lex.yy.c -ll -o erase
.DE
This program copies all its input to its output, except for any appearance of
the string \fBerase\fR.
Invoke the program by typing
.BR erase ,
and then test it by typing:
.DM
	Have you erased the blackboard?
	<ctrl-D>
.DE
.B erase
then prints:
.DM
	Have you d the blackboard?
.DE
If the input contains patterns that do not match any of the patterns
in the suite of rules you typed into
.BR lex ,
they are simply output unchanged.
Usually, you will want to write a rule to cover every case.
.Sh "Statements in lex"
.PP
.II "lex^statements"
As noted earlier, \fBlex\fR is a program generator.
It reads the specifications that you prepare for it,
and writes a C program that is used with the \fBlex\fR library.
Many of the actions in the rules you specify, such as
.DM
	printf ("removable");
.DE
are themselves C statements.
These statements are included in the resulting program, along with other
statements that \fBlex\fR provides so the program can run.
.PP
You can include
other statements, should the program need them, by placing them in
appropriate places.
The following program, called
.BR count.lex ,
shows how this is done.
It counts the number of \fItokens\fR, or strings of non-blank characters.
Type the following into the file \fBcount.lex\fR exactly as printed:
.DM
 		int count;
	%%
	[^ \et\en]+	count++;
	[ \et\en]+	;
	%%
	yywrap ()
	{
	 	printf ("Number of tokens:%d\en", count);
	 	return (1);
	}
.DE
Statements other than rule actions appear in two places in the program.
The first such statement is in the definition section, which precedes the
rule section delimiter \fB%%\fR:
.DM
		int count;
.DE
This C statement declares the variable \fBcount\fR to be an integer variable.
Notice that it is preceded by a tab;
a tab or a space indicates to \fBlex\fR that an input line is not a rule.
.PP
The second kind of non-rule statement follows the second \fB%%\fR,
which marks the end of the rules section.
.B lex
regards anything that follows the second delimiter as being
source statements.
.PP
.II "lex^yywrap"
The above example includes a function named \fByywrap\fR.
\fBlex\fR programs always call this function at the end of processing.
The above program fills this function with code that prints the
number of tokens in the text.
If you do not include a routine named \fByywrap\fR,
\fBlex\fR will use a standard one from its library.
.PP
Compile the program by typing the following commands:
.DM
	lex count.lex
	cc lex.yy.c -ll -o count
.DE
Run the program by typing:
.DM
	count <count.lex
.DE
This counts the tokens in the \fBcount.lex\fR file itself.
.B count
will print the following:
.DM
	Number of tokens:21
.DE
.Sh "Groups of Statements"
.PP
.II "lex^statements multiple"
In previous examples, the C statement in the action part of the rule is a
single statement.
In many \fBlex\fR applications, however, you will need to use
more than one statement per rule.
.PP
.II "lex^braces"
.II "lex^repetitions, zero or more"
To do so, enclose the statements in the braces \fB{\fR\ \fB}\fR.
The following example illustrates grouping.
This \fBlex\fR specification generates
a program to add numbers found in the input and
print the total whenever it reads an asterisk `\fB*\fR'.
Type the following program into \fBnsum.lex\fR:
.DM
	 	int number, sum;
	%%
	[0-9]+	{
		sscanf (yytext, "%d", &number);
		sum += number;
		printf ("%s", yytext);
		}
	"*"	{
		printf ("%s", yytext);
		printf ("%d", sum);
		sum = 0;
		}
.DE
Compile the program by typing:
.DM
	lex nsum.lex
	cc lex.yy.c -ll -o nsum
.DE
To run the generated \fBnsum\fR program, enter a sample data file by typing:
.DM
	cat >numbers
	one two three
	1 2 3 4 * 1 2 3 5 *
	*
	done
	<ctrl-D>
.DE
Run the program by typing:
.DM
	nsum <numbers
.DE
.B nsum
will print:
.DM
	one two three
	1 2 3 4 *10 1 2 3 5 *11
	*0
	done
.DE
The statements that follow the definitions
.DM
	[0-9]+
.DE
and
.DM
	*
.DE
are enclosed in braces, because each action triggers several statements.
Consider the first of these:
.DM
	[0-9]+	{
		sscanf (yytext, "%d", &number);
		sum += number;
		printf ("%s", yytext);
		}
.DE
The pattern looks for strings of digits.
.B sscanf
converts each such string into a number and saves it in the variable
\fBnumber\fR.
Now, consider the second rule:
.DM
	"*"	{
		printf ("%s", yytext);
		printf ("%d", sum);
		sum = 0;
		}
.DE
This specifies that upon detection of \fB*\fR
in the input, the program is to print the sum of the numbers
and then reset the counter to zero.
In both of these rules, the statement
.DM
	 printf ("%s", yytext);
.DE
prints the number or \fB*\fR so that the output shows
the input as well as the total.
.II "lex^yytext"
.B lex
defines
the variable \fByytext\fR.
It always contains the string that matches the pattern.
.PP
If the input is neither a number nor an asterisk, no pattern specifically
matches it.
Therefore, the program echoes it unchanged to the standard output.
.Sh "Using the Same Action"
.PP
.II "lex^rules, with same action"
To make it easier for you to write actions, \fBlex\fR allows you to
.I abbreviate
rules; that is, you have to write only once any action
that is performed upon detection of several patterns.
To abbreviate rules represented symbolically by
.DM
	p1	action1
	p2	action1
.DE
use the vertical bar operator:
.DM
	p1	|
	p2	action1
.DE
The vertical bar means ``use the action from the
first rule that declares an action.''
An example is given in the section on macro abbreviations, below.
.SH "Patterns"
.PP
.II "lex^patterns"
.II "lex^regular expressions"
The first part of each rule in the \fBlex\fR rules section
is a pattern that may match parts of the input.
This section describes how to construct these patterns, sometimes called
\fIregular\fR \fIexpressions\fR.
If you are familiar with \fBed\fR and how its patterns work,
this will be familiar to you.
.Sh "Simple Patterns"
.PP
The simplest kind of pattern is a string of characters that matches itself.
A previous section presented an illustration of this:
.DM
	%%
	removeable	printf ("removable");
.DE
The regular expression ``removeable''
matches all occurrences of \fIremoveable\fR that appear in the input text.
.PP
Certain characters have special meaning to \fBlex\fR patterns.
To match a special character literally, you must \fIquote\fR it.
For example, \fB*\fR has special meaning.
To match the asterisk literally (that is to match any `*'s that
appear in the input), surround it with quotation marks:
.DM
	"*"
.DE
Another way to quote characters is to precede it
with the backslash character '\fB\e\fR'.
.DM
	\e*
.DE
The following characters each have special meaning and must be quoted
to be matched as text characters:
.DM
	" \e ( ) < > { } % * + ? [ ] - ^ / $ . |
.DE
However, within \fB"\fR, the \fB\e\fR still has its meaning, so to match
the string \fB\e*\fR use the regular expression:
.DM
	"\e\e*"
.DE
Also, to match a quote character, use:
.DM
	\e"
.DE
.Sh "Classes of Characters"
.PP
.II "lex^patterns"
The power of patterns comes from special characters that match more than
one character.
The following examines each special character in detail.
.PP
.II "lex^dot"
The
.I period
or
.I dot
matches any character except newline.
The following regular expression matches any pair of characters that
begins with \fBJ\fR:
.DM
	J.
.DE
The following example prints in square brackets any sequence of five
characters that precedes a blank.
Type the following into the file \fBfive.lex\fR:
.DM
	%%
	....." "	printf ("[%s]", yytext);
.DE
Compile the program with the following commands:
.DM
	lex five.lex
	cc lex.yy.c -ll -o five
.DE
Now, type the following to create a test file for
.BR five :
.DM
	cat > work
	how well does this work?
	no match
	<ctrl-D>
.DE
Now, test
.B five
by typing:
.DM
	five < work
.DE
The result is:
.DM
	how[ well ]does[ this ]work?
	no match
.DE
The second line of the input does not have any matches.
Because the \fBdot\fR
pattern character does not match the end-of-line character,
all five characters that precede the blank must be on the same line.
.PP
.II "lex^character classes"
Another way to match many characters, but selectively, is with the
\fIcharacter\fR \fIclass\fR operation.
Enclose in square brackets
the set of characters to be matched.
Any of the characters listed there will match one character of the input.
For example,
.DM
	[0123456789]
.DE
matches any decimal digit in the input.
Characters may be in any order within the brackets.
Thus
.DM
	[0246813579]
.DE
is equivalent to the example above.
.PP
To simplify specifying for character classes, you can specify ranges of
characters.
The beginning and end of the range is separated by a hyphen.
To match all decimal digits as above, use:
.DM
	[0-9]
.DE
To match all alphabetic characters, type:
.DM
	[a-zA-Z]
.DE
.II "lex^match, exception"
.II "lex^exception"
The special character \fB^\fR, when used after the opening bracket `\fB[\fR',
tells
.B lex
to match any character \fIexcept\fR those enclosed.
The following example finds all strings that consist of two digits
followed by a third character that is neither an alphabetic character
nor a period,
and prints them enclosed by \fB{\fR
and \fB}\fR.
Type the following into file \fBtwodig.lex\fR:
.DM
	%%
	[0-9][0-9][^\e.a-zA-Z]	printf ("{%s}", yytext);
.DE
Process and compile the program by typing the following commands:
.DM
	lex twodig.lex
	cc lex.yy.c -ll -o twodig
.DE
Invoke the program by typing
.BR twodig ,
and test it by entering the following text:
.DM
	12. 12 12a 1 12 b
	<ctrl-D>
.DE
.B twodig
prints the following in reply:
.DM
	12. {12 }12a 1 {12 }b
.DE
.Sh "Repetition"
.PP
.II "lex^repetition"
In the patterns shown so far,
each character matches only one character at a time.
However,
many interesting input patterns involve repetition of characters.
The above program
.B twodig.lex
used such repetition, albeit in a primitive way.
.PP
.II "lex^+"
.II "lex^repetion, zero or more"
To match one or more instances of a character, follow it with the pattern
operator \fB+\fR.
Consider the summation example in \fBnsum.lex\fR,
shown earlier,
which recognized strings of input numbers and added them to a total:
.DM
	[0-9]+	{
		sscanf (yytext, "%d", &number);
		sum += number;
		printf ("%s", yytext);
		}
.DE
The pattern
.DM
	[0-9]+
.DE
matches a string of one or more digits.
.PP
.II "lex^*"
The operator \fB*\fR will match \fIzero\fR or more characters of a specified
type.
The following example deletes all characters between square brackets.
Type it into file \fBstar.lex\fR:
.DM
	%%
	\e[.*\e]	printf ("[]");
.DE
Type the following commands to generate and compile the program:
.DM
	lex star.lex
	cc lex.yy.c -ll -o star
.DE
Type the following to create a test file:
.DM
	cat > disappear
	[This should disappear]
	[what happens with two] of them [on a line?]
	<ctrl-D>
.DE
Now, use the test file with
.BR star :
.DM
	star < disappear
.DE
The output is:
.DM
	[]
	[]
.DE
In looking at the example's input,
you might have expected the output to be:
.DM
	[]
	[] of them []
.DE
.II "lex^match, longest"
.B lex
does not produce the latter output because it generates
recognizers that find the longest match if several matches are possible.
Therefore,
.B star
matched the first \fB[\fR, then all characters up to and
including the second \fB]\fR.
When you write a pattern that matches many characters, you should bear
this possibility in mind.
.PP
To change the program to match the first \fB]\fR, rewrite it as follows:
.DM
	%%
	\e[[^\e]]*\e]	printf ("[]");
.DE
The regular expression now matches a string of all characters except a \fB]\fR,
when that string is enclosed in square brackets.
.PP
.II "lex^?"
.II "lex^optional match"
.II "lex^match, optional"
The `\fB?\fR' character signals that the previous character or
regular expression is optional.
In other words, `\fB?\fR'
signals zero or one instance of a character or regular expression.
.PP
To see how this would be used in a program, consider
a text processor that regards a word as being a
strings of alphabetic characters that may or may not be followed by a period.
The following example does this, and encloses the
recognized words in parentheses.
Enter it into file \fBword.lex\fR:
.DM
	%%
	[a-zA-Z]+\e.?	printf ("(%s)", yytext);
.DE
Generate and compile the program with the following commands:
.DM
	lex word.lex
	cc lex.yy.c -ll -o word
.DE
Create a test file:
.DM
	cat > words
	These are words.
	Question mark not included?
	<ctrl-D>
.DE
And test
.B word
with the following command:
.DM
	word < words
.DE
The result is
.DM
	(These) (are) (words.)
	(Question) (mark) (not) (included)?
.DE
.II "lex^?"
.II "lex^repetitions, zero or one"
The question mark, like the \fB*\fR and \fB+\fR operators, can also follow
another specification of a pattern.
If you wanted to end a sentence with a character other than a
period, the following code will do the job for you:
.DM
		[a-zA-Z]+[.?!,]?
.DE
The characters
.DM
	.?!,
.DE
are optional.
.PP
.II "lex^braces, in patterns"
.II "lex^repetition, specific count"
The `+' and `*' operators may match many characters.
If you wish to match a specific number of characters or patterns, follow the
patterns with the repetition within braces \fB{\fR and \fB}\fR.
For example
.DM
	[0-9]{3}
.DE
matches a string of exactly three characters.
With this information, you should be able to rewrite the pattern part of
.BR twodig.lex ,
described above.
.PP
You can also specify a range of counts.
To match from seven to nine occurrences of lower-case alphabetic characters,
use:
.DM
	[a-z]{7,9}
.DE
.Sh "Choices and Grouping"
.PP
.II "lex^alternatives"
.II "lex^|"
To indicate alternate choices of characters or regular expressions,
separate them in the regular expression with a vertical bar operator \fB|\fR.
For example, if you wish
to match either three decimal digits or the character \fBa\fR, use:
.DM
	[0-9]{3}|a
.DE
.II "lex^( and )"
.II "lex^grouping, ()"
Parentheses help to group the parts of the pattern
that are separated by the vertical bar:
.DM
	(abc)|(def)
.DE
This pattern will match either the string \fBabc\fR or the string \fBdef\fR.
.Sh "Matching Non-Graphic Characters"
.PP
.II "lex^non-graphic character"
.II "lex^match, non-graphic characters"
Non-special, graphic characters in patterns match themselves.
Most non-graphic characters, such as space, tab, and control characters,
cannot be matched directly.
\fBlex\fR provides special sequences to match control characters.
The following example removes tabs and
blanks from the beginning and end of input lines.
Type it into file \fBdeblank.lex\fR:
.DM
	%%
	[ \et]+\en	printf ("\en");
	\en[ \et]+	printf ("\en");
.DE
Generate and compile the program with the following commands:
.DM
	lex deblank.lex
	cc lex.yy.c -ll -o deblank
.DE
Type the following to create a test file:
.DM	
	cat > sportab
	begins with no space or tab
		begins with tab
	   begins with three spaces
	<ctrl-D>
.DE
Type the following to test
.BR deblank :
.DM
	deblank < sportab
.DE
The result is:
.DM
	begins with no space or tab
	begins with tab
	begins with three spaces
.DE
.II "lex^non-graphic characters"
The special regular expression \fB\et\fR represents \fItab\fR, and \fB\en\fR
represents \fInewline\fR.
.PP
To match the backspace character, use \fB\eb\fR.
Form feed is matched by \fB\ef\fR.
To match an arbitrary character with a known octal value, use
three octal digits after the backslash;
for example,
.DM
	\e007
.DE
.SH "More Patterns"
.PP
This section discusses more advanced capabilities of patterns.
.Sh "Line Context"
.PP
Like \fBed\fR, \fBlex\fR
patterns can include characters that represent the beginning and end of line.
To match a line that consists of exactly five alphabetic characters, type:
.DM
	^[a-zA-Z]{5}$
.DE
.II "lex^end of line"
.II "lex^beginning of line $"
.II "lex^$"
The character \fB^\fR matches the beginning of the line, and \fB$\fR
matches the end of the line.
.Sh "Context Matching"
.PP
.II "lex^//"
.II "lex^match, in context"
.II "lex^context match"
A slash (virgule) `\fB/\fR'
shows that a following context is necessary to match a string.
For example, the following program
matches the string \fBmatch\fR
only if it is immediately followed by the string \fBing\fR.
Type it into file \fBmatch.lex\fR:
.DM
	%%
	match/ing	printf ("{%s}", yytext);
.DE
To compile the program, type the following commands:
.DM
	lex match.lex
	cc lex.yy.c -ll -o match
.DE
Type the following to create a test file:
.DM
	
	cat > matchtit
	Will this match?
	This is a matching test.
	<ctrl-D>
.DE
And run it against
.B match
by typing:
.DM
	match < matchtit
.DE
The result is:
.DM
	Will this match?
	This is a {match}ing test.
.DE
Notice that the string before the slash is matched.
The program does not match
the part that follows the slash, even though the string must
be there for the first part to be matched.
Thus, the regular expression that follows the slash may also be
matched on its own.
To see how this works, type the following into the file
.BR match2.lex :
.DM
	%%
	match/ing	printf ("{%s}", yytext);
	ing		printf ("ed");
.DE
To compile the program, type the following commands:
.DM
	lex match2.lex
	cc lex.yy.c -ll -o match2
.DE
Once again, create a test file:
.DM
	cat > matching
	Will this match?
	This is a matching test.
	You must now sing for your supper.
	<ctrl-D>
.DE
And run it:
.DM
	match2 < matching
.DE
The result is:
.DM
	Will this match?
	This is a {match}ed test.
	You must now sed for your supper.
.DE
.II "lex^context match"
.II "lex^match, in context"
The context-string that follows the \fB/\fR
may be a regular expression.
The following example
matches the whole-number portion of a decimal fraction.
Type it into the file
.BR wholept.lex :
.DM
	%%
	"-"?[0-9]+/"."[0-9]+	printf ("(%s)", yytext);
.DE
To compile the program, type the following commands:
.DM
	lex wholept.lex
	cc lex.yy.c -ll -o wholept
.DE
Invoke the program by typing
.BR wholept ;
then type the following to test it:
.DM
	123 12345 1234.567
1	<ctrl-D>
.DE
The result will be:
.DM
	123 12345 (1234).567
.DE
As you can see,
the part of the regular expression
.DM
	"-"?
.DE
matches an optional leading minus sign.
Then
.DM
	[0-9]+
.DE
matches a string of at least one decimal digit.
Then, the following context must match the regular expression
.DM
	"."[0-9]+
.DE
which matches the fractional part of the number.
When it finds a number that matches, it prints the number's whole part
enclosed in parentheses.
.Sh "Macro Abbreviations"
.PP
.II "lex^macro"
.II "lex^abbreviations"
.B lex
also provides a macro facility that can substantially simplify
the writing of complex regular expressions.
.PP
A \fImacro\fR
is a named body of text.
A macro processor simply replaces the name of the macro
with the text of the macro.
.PP
To illustrate, type following example into file \fBfloat.lex\fR.
It recognizes integer and floating point constants according to the C format:
.DM
	d [0-9]+
	e [Ee][+-]?[0-9]+
	%%
	{d}\e.		|
	{d}\e.{d}	|
	\e.{d}		|
	{d}\e.{e}	|
	\e.{d}{e}	|
	{d}\e.{d}{e}	|
	{d}{e}		printf ("F:[%s]", yytext);
.DE
.II "lex^macro"
.B lex
replaces the macro name
.B e
with the code that matches a string of digits at least one digit long.
It replaces the macro name \fBd\fR with code that matches the
number's exponent.
These two are invoked in the manner of
.DM
	{d}
.DE
within a pattern.
To compile the program, type the following commands:
.DM
	lex float.lex
	cc lex.yy.c -ll -o float
.DE
Type the following to create a test file:
.DM
	cat > flonumb
	1 1. 1.2 1.e4 1e4
	.1e4 e4 .1 . 0 1.2e3
	<ctrl-D>
.DE
And test it by typing:
.DM
	float < flonumb
.DE
The result is:
.DM
	1 F:[1.] F:[1.2] F:[1.e4] F:[1e4]
	F:[.1e4] e4 F:[.1] . 0 F:[1.2e3]
.DE
.Sh "Context: Start Rules"
.II "lex^context, start"
.II "lex^rules, context start"
.II "lex^start condition"
.PP
Many tasks in lexical processing require the program to be aware of
a token's context.
\fBlex\fR
lets you make processing conditional upon previously processed input.
This is done by using \fBstart conditions\fR.
.PP
.II "lex^definitions"
.II "lex^%S"
Start conditions are named in the definitions section as follows:
.DM
	%S name1 name2
.DE
.II "lex^< >"
.II "lex^angle brackets"
where \fBname1\fR and \fBname2\fR are names of start conditions.
These start conditions are then used by prefixing a pattern with
the start condition's name enclosed in angle brackets.
For example:
.DM
	<name1>
.DE
.II "lex^BEGIN action"
For example, you can use one start condition to control the scanning
of comments in a Pascal-like language.
The start condition is set by the \fBlex\fR statement \fBBEGIN\fR
when the beginning bracket of the comment is found.
The comment is scanned for strings that begin with \fB$\fR
to signal compiler operation.
To see how this works,
type the following into the file \fBcomment.lex\fR:
.DM
.ta 0.5i 1.2i 1.7i
	%S CMNT
	%%
	<CMNT>\e$[ler]	printf ("Option is %s.\en", yytext);
	<CMNT>[^\e}]	;
	<CMNT>\e}		BEGIN 0;
	\e{		BEGIN CMNT;
.DE
To compile, use the following commands:
.DM
	lex comment.lex
	cc lex.yy.c -ll -o comment
.DE
Once again, create a test file:
.DM
	cat > option
	{This is a comment}
	{This comment has options $l $e $r}
	program
	information
	<ctrl-D>
.DE
And run it by typing:
.DM
	comment < option
.DE
The result is:
.DM
	Option is $l.
	Option is $e.
	Option is $r.
.sp \n(pDu
	program
	information
.DE
The context start condition is named following \fBBEGIN\fR
in the action part of the rule.
To return to the normal condition, use \fB0\fR as the context name.
.Sh "Separate Contexts"
.PP
.II "lex^context, separate"
If you wish to perform context-dependent processing that is more
complex than that shown in the example above,
you will find it convenient to use separate contexts.
.PP
The names of the contexts are defined in the definitions sections,
after the definitions of any start conditions:
For example:
.DM
	%C name name ...
.DE
The \fBlex\fR function \fByyswitch\fR switches to a new context.
.PP
The body of the context's rules is preceded in the rules section by:
.DM
	%C name
.DE
To see how this works, type the following into file \fBpre.lex\fR.
It is part of a program that recognizes the preprocessor statements in a
C program:
.DM
	%C PRE
	%%
	^# 	yyswitch (PRE);
	[^#\en]+		printf ("[%s]", yytext);
	%C PRE
	include.+	|
	define.+	{
			printf("{%s}", yytext);
			yyswitch(0);
			}
	.+		{
			printf ("{??%s}", yytext);
			yyswitch (0);
			}
.DE
A \fB#\fR in column 1 signals the beginning of a preprocessor statement.
Upon recognizing this condition, this program uses \fByyswitch\fR
to activate the context \fBPRE\fR.
.PP
Within this separate context, individual rules recognize different
preprocessor statements; this example includes only two.
Each of the rules prints the preprocessor line enclosed in braces
\fB{\fR \fB}\fR.
In addition, the rules switch back to the original (and unnamed) context
by the statement
.DM
	yyswitch (0);
.DE
To compile and test this program, use the following commands:
.DM
	lex pre.lex
	cc lex.yy.c -ll -o pre
	pre <lex.yy.c | more
.DE
.II "lex^context, switch"
This example uses the function \fByyswitch\fR
to return to the original context at the end of each rule in the secondary
context.
Some applications require a return to the context that was previously
in force.
To assist in this, \fByyswitch\fR returns the value of the previous context.
.PP
To modify the example to switch to the previous context,
add a statement to the definitions section to declare a variable to hold
the previous context:
.DM
	int prev;
.DE
Then, when switching, save the current context:
.DM
	prev = yyswitch (NEW);
.DE
To switch back, use:
.DM
	yyswitch (prev);
.DE
To summarize, you can specify a match at the beginning and end of input lines.
You may need a following context for a match.
Macros provide a means of abbreviating elements of patterns.
\fBlex\fR
can qualify some patterns based on a start context, or process entirely
separate contexts.
.SH "More About Writing Actions"
.PP
This section discusses predefined \fBlex\fR actions and how to use them.
It also presents other \fBlex\fR routines that are useful in writing actions.
.Sh "ECHO"
.PP
.II "lex^ECHO"
Many \fBlex\fR actions simply output the matched pattern:
.DM
	[0-9]+	printf ("%s", yytext);
.DE
This form has been used in the examples because many examples
also output additional material, such as enclosing braces,
to illustrate the matched token.
.PP
\fBlex\fR provides a simpler way to echo the exact token matched:
.DM
	[0-9]+		ECHO;
.DE
The following example echoes all strings of digits twice, and everything
else once.
Type it into file \fBdouble.lex\fR:
.DM
	%%
	[0-9]+		{ECHO; ECHO;}
	[^0-9]+		ECHO;
.DE
To compile the program, use the commands:
.DM
	lex double.lex
	cc lex.yy.c -ll -o double
.DE
To invoke the program, type
.BR double ;
and to test it, type the following text:
.DM
	abcdef 123 678 as45 67gh
	<ctrl-D>
.DE
.B double
will reply:
.DM
	abcdef 123123 678678 as4545 6767gh
.DE
.Sh "Processing Overlapping Strings"
.PP
The \fBlex\fR processing illustrated to this point has
been restricted to programs whose rules recognize distinct strings.
That is, once any character of a string is matched by a regular expression,
it cannot be matched by another.
.PP
.II "lex^REJECT"
Some applications require that strings be matched by more than one rule;
such multiply-matched strings are called \fIoverlapping strings\fR.
The \fBlex\fR action word \fBREJECT\fR provides this capability.
When \fBREJECT\fR
appears in a rule, other rules can also match the string.
Remember, however, that \fBlex\fR
programs give precedence to the longest string that matches a regular
expression.
.PP
The following example determines the number of letter pairs, or \fIdigrams\fR,
in its input.
The input is presumed to be lower-case letters.
Enter the following into
\fBdigram.lex\fR:
.DM
		int digram [128] [128];
	%%
	[a-z][a-z]	{
			digram [yytext [0]] [yytext [1]]++;
			REJECT;
			}
	.		;
	\en		;
	%%
	yywrap ()
	{
	    int i1, i2;
	    for (i1 = 'a'; i1 <= 'z'; i1++)
		for (i2 = 'a'; i2 <= 'z'; i2++)
		    if (digram [i1] [i2] != 0)
			printf ("%d\et%c%c\en",
			    digram [i1] [i2], i1, i2);
	}
.DE
To compile the program, type the commands:
.DM
	lex digram.lex
	cc lex.yy.c -ll -o digram
.DE
To invoke the program, type
.BR digram ;
and test it with the following text:
.DM
	this is a test of digrams.
	<ctrl-D>
.DE
The result will be:
.DM
	1	am
	1	di
	1	es
	1	gr
	1	hi
	1	ig
	2	is
	1	ms
	1	of
	1	ra
	1	st
	1	te
	1	th
.DE
.Sh "yylex"
.PP
.II "lex^yylex"
.II "lex^definitions section"
.II "lex^sections, definitions"
.B lex
places the actions you provide for the rules in your \fBlex\fR
program into a C routine named \fByylex\fR.
.PP
If you add variable declarations in the definitions section before the first
\fB%%\fR, \fByylex\fR
can access them, as in the example \fBdigram.lex\fR, shown above.
You can also declare variables that are local to \fByylex\fR,
if you place the declarations after the rules section
delimiter and before the first rule.
A tab or space must precede the declaration, where the
.B %
symbols are at the beginning of the line.
See the example
.BR yacclex.lex ,
below.
.PP
The following program is a different version of \fBdigram.lex\fR,
called \fBdigram2.lex\fR; it uses such a declaration.
.DM
		int digram [128] [128];
	%%
		int t0, t1;
	[a-z][a-z]	{
			t0 = yytext [0];
			t1 = yytext [1];
			digram [t0] [t1]++;
			REJECT;
			}
	%%
	yywrap ()
	{
	    int i1, i2;
	    for (i1 = 'a'; i1 <= 'z'; i1++)
		for (i2 = 'a'; i2 <= 'z'; i2++)
		    if (digram [i1] [i2] != 0)
			printf ("%d\et%c%c\en",
			    digram [i1] [i2], i1, i2);
	}
.DE
.Sh "Header Section"
.PP
.II "lex^header section"
.II "lex^section, header"
You can insert
additional code at the beginning of the generated program by
including such code in the definitions section.
An earlier example, \fBcount.lex\fR, demonstrated how to do this:
.DM
		int count;
	%%
	[^ \et\en]+	count++;
	[ \et\en]+	;
	%%
	yywrap ()
	{
	    printf ("Number of tokens:%d \en ", count);
	    return (1);
	}
.DE
A tab or space character must precede the code you include.
.PP
If you wish to insert \fBinclude\fR
or any other C preprocessor statement at the beginning of the program,
however, a different technique must be used.
This stems from the fact that the preprocessor statements must begin at
the beginning of the line, and the blank or tab precludes this.
.PP
.II "lex^%{ %}"
The alternative method to add code to the beginning is as follows:
.DM
	%{
	... code ...
	%}
.DE
where the \fB%\fR symbols are at the beginning of the line.
.Sh "Additional Routines"
.PP
.II "lex^yywrap"
If your version of \fByywrap\fR
or any of the rules that you write need other routines,
you can include code for them after a second \fB%%\fR.
(This was where \fByywrap\fR was shown in \fBdigram.lex\fR.)
If you wish to provide your own version of \fBinput\fR
or \fBoutput\fR, you must define it there.
.SH "Using lex With yacc"
.PP
.II "lex^yacc"
Although \fBlex\fR can handle many applications by itself,
it is often used with the parser-generator \fByacc\fR.
For example,
programming-language compilers often have parts generated
by both \fBlex\fR and \fByacc\fR.
.PP
.II "lex^tokens"
Like \fBlex\fR, \fByacc\fR is a program generator.
Its programs can recognize input that is
structured according to a grammar fed to the \fByacc\fR program generator.
In most instances,
\fByacc\fR-generated programs require \fItokens\fR as input, instead
of individual characters.
In the context of a programming language, a token is a variable name or
a special character (such as an operator).
\fBlex\fR is often used with \fByacc\fR because \fBlex\fR
is especially well suited for partitioning text input into tokens.
.PP
A \fByacc\fR-generated
program expects a token number as input from the routine \fByylex\fR.
\fByacc\fR assigns a unique number, or constant definition,
to each unique type of token,
and expects \fByylex\fR to return these numbers as input.
.PP
For your \fBlex\fR program
to access these predefined constant definitions for token types,
you must include the generated \fBlex\fR source in the \fByacc\fR specification.
.PP
The following examples process very simple input, to
illustrate how to assemble \fBlex\fR- and \fByacc\fR-generated
programs.
To begin, type the following into the file \fByacclex.yy\fR:
.DM
	%token beginning midtok ending
	%start simplistic
	%%
	simplistic :	beginning middle ending
				{printf ("recognized"); };
	middle	:	midtok;
	middle	:	middle midtok;
	%%
.DE
When \fByacc\fR processes this program, it produces the file \fBy.tab.h\fR
that contains the token-name definitions.
The following \fBlex\fR source reads \fBy.tab.h\fR to learn of the
constant definitions that \fByacc\fR generated;
type it into file \fByacclex.lex\fR:
.DM
	%{
	#include "y.tab.h"
	%}
	%%
	"("	return (beginning);
	")"	return (ending);
	[a-zA-Z]	return (midtok);
.DE
The symbolic definition of the token names are \fBbeginning\fR,
\fBending\fR and \fBmidtok\fR.
.PP
To compile the programs, type the following commands:
.DM
	yacc yacclex.yy
	lex yacclex.lex
	cc y.tab.c lex.yy.c -ly -ll -o yacclex
.DE
Type
.B yacclex
to invoke the new program; and test by typing the following:
.DM
	(abcdef)
	<ctrl-D>
.DE
The result will be:
.DM
	recognized
.DE
.SH Summary
.PP
.B lex
is a utility that generates lexical analyzers according to
a set of specifications that you write.
.I "Lexical analysis"
means to read a mass of text, recognize strings within that mass,
and react appropriately when each type of string is discovered.
With
.BR lex ,
you can write programs to perform complex analysis of text simply
by describing what analysis you want to perform, without worrying
about the messy details of how that analysis is actually performed;
thus, \fBlex\fR is a fine example of what is nowadays called a
``fourth-generation language''.
.PP
.B lex
is especially well suited to work with the parser-generator
.BR yacc .
By using them together, you can efficiently build command processors
and even entire computer languages.
