.II "yacc^tutorial"
.ds TL "yacc, Yet Another Compiler-Compiler"
.NH "Introduction to yacc" 1
.PP
The first high-level programming language compiler took a very long time to
write.
Since then, much has been learned about how to design languages and how
to translate programs written in high-level languages into
machine instructions.
With what is known today, the writing of a compiler
takes a fraction of the time it used to require.
.PP
Much of this improvement is due to the use of more powerful software
development methods.
In addition, we know about the mathematical properties of computer
programming languages.
Software tools that apply this mathematical knowledge have played a large part
in this improvement.
.PP
The \*(CO system provides two tools to simplify the generation of
compilers.
These tools are the lexical analyzer generator \fBlex\fR and the parser
generator \fByacc\fR.
The following introduces \fByacc\fR, and gives a basic course in its use.
.PP
Although initially intended for the development of
compilers, \fBlex\fR and \fByacc\fR have proven their utility in other,
simpler, tasks.
Examples of very simple languages are included in this tutorial.
.PP
\fByacc\fR accepts a free-form description of a programming language and
its associated parsing, and generates a C program that, when
compiled, will parse a program written in the described language.
It uses a left-to-right, bottom-up technique,
to detect errors in the input as soon as theoretically possible.
\fByacc\fR generates parsers that handle certain grammatical ambiguities
properly.
.PP
This manual presumes that you are familiar with computer-language parsing and
formal methods of description of languages.
Because \fByacc\fR generates its programs in C and uses many of C's syntactic
conventions, you should have a working knowledge of C.
Related documents include \fIUsing the COHERENT System\fR and 
\fIIntroduction to lex\fR.
.SH "Examples"
.PP
The following presents a few small examples that you can
experiment with to get a feel of how to use \fByacc\fR.
Feel free to experiment with the examples to investigate new ideas.
.Sh "Phrases and Parentheses"
.PP
The first example describes a language we call \fBslang\fR,
or \fIsimple language\fR.
.B slang
consists of sentences.
A sentence, in turn, consists
of strings of letters or groups of letters enclosed in
parentheses, terminated by a period.
A group of letters can also include other groups of letters.
.PP
The simplest ``sentence'' in
.B slang
is:
.DM
	a.
.DE
The following demonstrates a sentence that consists only of
a group:
.DM
	(ab).
.DE
As described above,
a group can have another group inside it:
.DM
	ab(cd(ef)).
.DE
.PP
The following gives the
.B yacc
grammar for
.BR slang .
Type it into the file
.B slang.y.
Note that the lexical-analyzer routine \fByylex\fR is included in the
\fByacc\fR input file.
Note also that, as in C, comments are strings placed between the characters
\fB/*\fR and \fB*/\fR.
.DM
	/* Tokens (terminals) are all caps */
	%token LPAREN RPAREN OTHER PERIOD
	%%
	run	:	sent     /* Input can be a single */
		|	run sent /* sentence or several */
		;
.DE
.DM
	sent	:	phrase PERIOD
				{printf ("sentence\en");}
		;
.DE
.DM
	group	:	LPAREN phrase RPAREN
				{printf ("group\en");}
		;
.DE
.DM
	phrase	:		/* empty */
		|	others
		|	group
		|	others group
		;
.DE
.DM
	others	:	OTHER	/* letters and other chars */
		|	others OTHER
		;
	%%
.DE
.DM
	#include <stdio.h>
	#include <ctype.h>
	/* Called by the parser to get a token  */
	yylex ()
	{
	    int c;
	    c = 0;
.DE
.DM
	    while (c == 0) {
		c = getchar();
		if (c == '.') return (PERIOD);
		else if (c == '(') return (LPAREN);
		else if (c == ')') return (RPAREN);
		else if (c == EOF) return (EOF);
		else if (! isalpha(c)) c = 0; 
	    }
	    return (OTHER);
	}
.DE
.PP
To generate and compile the parser described by this input, issue the commands
.DM
	yacc slang.y
	cc y.tab.c -ly -o slang
.DE
.PP
Now, invoke your new parser by typing
.DM
	slang
.DE
and test it by typing the following input:
.DM
	a
.DE
does not reply, since this is not a sentence.
When you type:
.DM
	a.
.DE
.B slang
replies:
.DM
	sentence
.DE
And if you type:
.DM

	abc(def).
.DE
.B slang
replies:
.DM
	group
.DE
As you can see,
.B slang
recognizes groups and sentences within the input you typed,
and reactes by printing an appropriate message.
Try typing
.DM
	aaa(bbb(ccc)).
	(a).
.DE
and see what you get.
To exit from
.BR slang ,
type
.BR <ctrl-C> .
.Sh "Simple Expression Processing"
.PP
The next example creates a small language that includes two
types of statements.
The first type of statement
resembles a procedure call, and the second is an expression.
Procedure names are in upper-case letters,
whereas the variables in expressions are in lower-case letters.
Both procedures and expressions are terminated by a semicolon.
.PP
The following code generates a parser
that identifies either the procedure being called
or the arithmetic expression being calculated.
The lexical input routine is independently generated by \fBlex\fR.
.PP
Enter the following program into the file \fBcalc.y\fR:
.DM
	%token VARIABLE PROCEDURE
	%%
	prog	:	stmnt
		|	prog stmnt
		;
.DE
.DM
	stmnt	:	stat
		|	stat '\en'
		|	error '\en'
		;
.DE
.DM
	stat	:	PROCEDURE ';'
			    {printf ("PROCEDURE is %c\en", $1);}
		|	expr ';'
			    {printf ("Expression\en");}
		;
.DE
.DM
	expr	:	expr '-' expr
			    {printf
			    ("Subtract %c from %c giving E\en",
			    $3, $1);
			    $$ = 'E';
			    }
		|	VARIABLE
			    {$$ = $1;}
		;
.DE
.PP
Enter the lexical-analyzer part of the program into the file \fBcalc.lex\fR:
.DM
	%{
	#include "y.tab.h"
	%}
	%%
	[A-Z]		{
			yylval = yytext [0];
			return PROCEDURE;
			}
.DE
.DM
	[a-z]		{
			yylval = yytext [0];
			return VARIABLE;
			}
	\en		return ('\en');
	.		return (yytext [0]);
.DE
.PP
Now, generate the programs and compile them by typing:
.DM
	yacc calc.y
	lex calc.lex
	cc y.tab.c lex.yy.c -ly -ll -o calc
.DE
The following messages will appear on your console:
.DM
	1 S/R conflict
	y.tab.c:
	lex.yy.c:
.DE
For now, you can freely ignore the S/R conflict (Shift/Reduce) message from
.BR yacc .
We shall deal with the shift and reduce notions later on.
To invoke the newly generated program, type:
.DM
	calc
.DE
To test it, type the following:
.DM
	A;B;
	C;
	a-b-c;
	a-b-c-d-e;
.DE
.B calc
will reply appropriately to each line of input.
To exit, type
.BR <ctrl-C> .
.SH "Background"
.PP
Now that you have tried
.B yacc,
the following gives some background to it, and how the
parsers that it generates operate.
.Sh "LR Parsing"
.PP
.II "yacc^LR parsing"
.II "yacc^left-to-right parsing"
.II "yacc^LALR"
\fByacc\fR generates a ``bottom up'' parser.
More specifically, \fByacc\fR generates parsers that read LALR(1) languages.
.PP
LR parsers scan the input in a left-to-right fashion.
Unfortunately, LR parsers for interesting languages are unpractically large.
LALR(k) parsers, which are derived from LR parsers,
use a ``look ahead'' technique, in which the next \fBk\fR elements
of the input stream are used to help determine reductions.
LALR(1) parsers are small enough to be practical, are easy to generate, and
are fast.
.Sh "Input Specification"
.PP
.II "yacc^BNF"
.II "yacc^Backus-Naur Form"
.II "Backus-Naur Form"
.II BNF
To generate a language with \fByacc\fR,
you must specify its grammar in Backus-Naur Form (BNF).
(For a good introduction to BNF, see the section on parsing in
\fIApplied C\fR.)
The languages recognized by \fByacc\fR-generated parsers are rich and compare
favorably with modern programming languages.
The time required to generate the parser from the input grammar is very
small \(em less than the time required to compile the generated parsers.
.PP
In addition to generating the parser to recognize the input language,
\fByacc\fR lets you include compiler actions within the
grammar rules that are executed as the constructs are recognized.
This greatly simplifies the entire task of writing your compiler.
When used in combination with \fBlex\fR, \fByacc\fR can make the process of
writing a recognizer for a simple language the task of an afternoon.
.Sh "Parser Operation"
.PP
.II "yacc^yyparse"
.II "yacc^library"
.II "yacc^library, yacc"
\fByacc\fR generates a compilable C program that consists of a routine named
\fByyparse\fR, and the information about the grammar encoded into tables.
Routines in the \fByacc\fR library are also used.
.PP
.II "yacc^push-down list"
.II "yacc^stack"
The basic data structure used by the parser is a \fIstack\fR, or \fIpush
down list\fR.
At any time during the parse, the stack contains
information describing the state of the parse.
The state of the parse is related to parts of grammar rules already recognized
in the input to the parser.
.PP
.II "yacc^parse actions"
At each step of the parse, the parser can take one of four actions.
.PP
.II "yacc^shift"
.II "yacc^action, shift"
The first action is to \fIshift\fR.
Information about the input symbol or
nonterminal is pushed onto the stack, along with the state of the parser.
.PP
.II "yacc^reduce"
.II "yacc^action, reduce"
The second type of action is to \fIreduce\fR.
This occurs when a grammar rule
is completely recognized.
Items describing the component parts of the rule are removed from the stack,
and the new state is pushed onto the stack.
Thus, the stack is \fIreduced\fR, and the symbols corresponding to the grammar
rule are \fIreduced\fR to the left part of the rule.
.PP
.II "yacc^error action"
.II "yacc^action, error"
Third, the parser can execute an \fIerror\fR action.
If the current input symbol is incorrect for the state of the stack, it is not
proper for the parser either to shift or reduce.
As a minimum, this state will result in an error message being issued, usually
.DM
	Syntax error
.DE
\fByacc\fR provides capabilities for using this error state to recover
gracefully from errors in the input.
.PP
.II "yacc^accept action"
.II "yacc^action, accept"
.II "yacc^start symbol"
Finally, the parser can \fIaccept\fR the input.
This means that the \fIstart\fR symbol, such as \fIprogram\fR, has been
properly recognized and that the entire input has been accepted.
.PP
Later sections discuss how you can have the parser describe its parsing
actions step-by-step.
.SH "Form of yacc Programs"
.PP
.II "yacc^%%"
.II "yacc^definition section"
.II "yacc^section, definition"
.II "yacc^rules section"
.II "yacc^section, rules"
A
.B yacc
program can have up to three sections.
Each section is marked by the symbol
\fB%%\fR. 
The first section contains declarations.
The second section contains the rules of the grammar.
User-written routines that are to be part of the generated program can be
included in the third section.
The outline of \fByacc\fR specifications is as follows:
.DM
	definitions
	%%
	rules
	%%
	user code
.DE
If there are no definitions or user code, the input can be abbreviated to
.DM
	%%
	rules
.DE
.Sh "Definitions"
.PP
.II "yacc^definitions section"
The first section in a \fByacc\fR specification is the definitions section.
This section includes information about the elements used in the \fByacc\fR
specification.
Additional items are user-defined C statements, such as \fBinclude\fR
statements, that are referenced by other statements in the generated program.
.PP
.II "yacc^token definition"
Each token, such as
.B VARIABLE
in example program
.BR calc ,
must be predefined in a \fB%token\fR statement in the definitions section:
.DM
	%token VARIABLE
.DE
.II "yacc^terminals"
.II "yacc^nonterminals"
Tokens are also called \fBterminals\fR.
Only nonterminals appear as the left
part of a rule, and terminals can appear only on the right side of a rule.
This helps \fByacc\fR distinguish terminals from nonterminals.
Other types of statements that assist in ambiguity resolution appear here,
and will be discussed in later sections.
.PP
.II "yacc^start symbol"
Each grammar that \fByacc\fR generates a parser for must have a \fIstart\fR
symbol.
Once the start symbol has been recognized by the parser, its input is
recognized and accepted.
For a programming-language grammar, this nonterminal represents the entire
program.
.PP
The start
symbol should be declared in the definitions section as:
.DM
	%start program
.DE
If no
.B %start
symbol is declared, it is taken to be the left side of the first
rule in the rules section.
.Sh "Rules"
.PP
Your language's grammar rules must be entered in a variant of BNF.
The two following rules illustrate how to define an expression:
.DM
	exp	:	VARIABLE;
	exp	:	exp '-' exp;
.DE
.II "yacc^{ }"
Action statements that are enclosed in braces
\fB{ }\fR specify the semantics of
the language, and are embedded within the rules.
More information about how rules are built is given below.
.Sh "User Code"
.PP
.II "yacc^action statements"
.II "yacc^user code"
Action statements may require other routines, such as common code-generating
routines, or symbol table building routines.
Such user code can be included in the generated parser after the rules section
and a \fB%%\fR delimiter.
.PP
The following sections discuss definitions and rules in detail.
.SH "Rules"
.PP
.II "yacc^rule, sections"
.I Rules
describe how programming-language constructs are put together.
Any given language can be described by many configurations of rules.
This frees you to write the rules for clarity and readability.
.PP
.II "yacc^production"
.II "yacc^reduction"
A rule consists of a left part and a right part.
The left part is said to \fIproduce\fR the right part;
or, the right part is said to \fIreduce to\fR the left part.
A rule can also include the action the parser is to perform
once it (the rule) is reduced.
.Sh "General Form of Rules"
.PP
.II "yacc^rule format"
.II "yacc^comments, in rules"
Blanks and tabs are ignored within rules (except in the action parts).
Comments can be enclosed between \fB/*\fR and \fB*/\fR.
The left part of the rule is followed by a colon.
Then come the elements of the right part, followed by a semicolon.
.PP
Rules that have the same left part can be grouped together with the left part
omitted and a vertical bar signifying ``or''.
For example, the grammar
.DM
	exp	:	VARIABLE;
	exp	:	exp '-' exp;
.DE
can be written as:
.DM
	exp	:	VARIABLE
		|	exp '-' exp;
.DE
Note that these are equivalent to the BNF:
.DM
	<exp>	::=	VARIABLE
	<exp>	::=	<exp> - <exp>
.DE
.PP
A rule can also contain C statements that are the
compiler actions themselves.
These actions are enclosed in braces \fB{\fR and \fB}\fR and are executed by
the generated parser when the grammar rule has been recognized.
More will be said about actions in the following section.
.Sh "Suggested Style"
.PP
.II "yacc^rule, format"
.II "yacc^rule, style"
.B yacc
permits you to write rules in completely free form.
For example, the grammar for the above rule can be written:
.DM
	exp:VARIABLE|exp'-'exp;
.DE
However, this form is much less readable.
.PP
Two styles of
.B yacc
grammar are in common use.
The first of these is used throughout this manual.
.PP
First, start the left part at the beginning of the line;
follow it with a tab; then a colon.
The right part should be on the same line, also preceded by a tab.
.PP
Second, group all rules with the same left part together, and use the vertical
bar aligned under the colon for all but the first rule in the group.
.PP
.II "yacc^actions"
Third, place action items on a separate line following the associated rule,
preceded by three tabs.
.PP
Finally, precede the terminating semicolon with a single tab,
to align it with the colon and vertical bar.
.PP
The outline of this style is:
.DM
	left	:	right1 right2
				{action1}
		|	right3 right4
				{action2}
		;
.DE
.PP
This style is compact and works well for languages whose rules and actions
together are simple.
.PP
For somewhat more extensive languages, or for additional flexibility in adding
statements to the action part, use the following modification of the style.
.DM
	left	:	right1 right2 {
				action1
				}
		|	right3 right4 {
				action2
				}
		;
.DE
.PP
For specifications that have larger rules or more complex actions, another
style is recommended.
.PP
As in the first style, group rules with the same left part,
and use the vertical bar.
Place the left part, with its terminating colon, on a line by itself.
Then indent
the right parts of the rule one or more tabs as necessary to
make the rule and actions readable.
Finally,
the vertical bar and the
semicolon should be at the beginning of the line.
.PP
The outline for this style is as follows:
.DM
	left:
		right1 right2 {
			action1
			}
	|	right3 right4 {
			action2
			}
	;
.DE
.PP
Since the input to \fByacc\fR can be entirely free form, there is no
restriction on how to write your rules.
However, if you use a
consistent style throughout, it will make your job easier.
.SH "Actions"
.PP
.II "yacc^rule, actions"
In addition to generating a parser to recognize a specific language, \fByacc\fR
also lets you include parsing action statements.
With this feature, you can include C-language action statements that will be
performed when specified constructs are recognized.
.Sh "Basic Action Statements"
.PP
The example language
.B slang,
described above, the action statements simply
print information on the terminal as productions are recognized:
.DM
	sent	:	phrase PERIOD
				{printf ("sentence\en");}
		;
	group	:	LPAREN phrase RPAREN
				{printf ("group\en");}
		;
.DE
Even if your actions will be more complex, using \fBprintf\fR statements in
this way can help verify your grammar early in the development process.
.Sh "Action Values"
.PP
.II "yacc^rule, values"
If the specification is for the grammar of a programming language, the actions
will normally interface to routines that access symbol tables or
generate code.
.PP
\fByacc\fR lets rules assume a \fIvalue\fR to help
keep track of intermediate results within rules.
These values can contain symbol-table information, code-generation
information, or other semantic information.
.PP
To set a value for a rule, simply use a statement of the form
.DM
	$$ = <value>;
.DE
within an action statement.
The symbol \fB$$\fR is the value of the production.
This value can be used by other rules that use this rule as a non-terminal
part.
.PP
The example program
.B calc,
given above, illustrates the use of the value of productions:
.DM
	expr	:	expr '-' expr {
			    printf
				("Subtract %c from %c giving E\en",
				$3, $1);
			    $$ = 'E';
			    }
		|	VARIABLE
				{$$ = $1;}
		;
.DE
The first rule's action statement sets the value of the production \fBexpr\fR
to \fB'E'\fR: 
.DM
	$$ = 'E';
.DE
.PP
The \fIvalue\fR of a rule is significant in that it can be used in
productions including that rule as a nonterminal part.
.PP
An example is given in the first rule above.
The \fBprintf\fR statement refers to the items \fB$1\fR and \fB$3\fR.
\fByacc\fR interprets these symbols to mean the value of elements one
and three of the right side, respectively;
that is to say, \fB$1\fR refers to the value of the first \fBexpr\fR in the
right side of the first rule, and \fB$3\fR refers to the second \fBexpr\fR,
as illustrated below:
.DM
	expr	:	expr '-' expr
			$1   $2   $3
.DE
.B calc
does not reference \fB$2\fR.
.PP
.II "yacc^token, value"
The value for the tokens is provided by the lexical analyzer.
The second rule for \fBexpr\fR uses this to get the value of the token
\fBVARIABLE\fR.
The value represented by \fB$1\fR is provided by the lexical
analyzer in the statement
.DM
	yylval = yytext [0];
.DE
.PP
To give another example, here is a simple calculator language, called
.B digit,
which performs arithmetic on one-digit numbers and prints the results.
Type the following grammar into the file \fBdigit.y\fR:
.DM
	%token DIGIT
	%%
	session :	calcn
		|	session calcn
		;
.DE
.DM
	calcn	:	expr '\en' /* print results */
				{printf ("%d\en", $1);}
		;
.DE
.DM
	expr	:	term '+' term
				{$$ = $1 + $3;}
		|	term '-' term
				{$$ = $1 - $3;}
		;
.DE
.DM
	term	:	DIGIT
				{$$ = $1;}
		;
.DE
.DM
	%%
	#include <stdio.h>
	yylex ()
	{
	    int c;
	    c = 0;
.DE
.DM
	    while (c == 0) { /* ignore control chars and space */
		c = getchar();
		if (c <= 0) return (c); /* could be EOF */
		if (c == '\en') return (c); /* set c to ignore */
.DE
.DM
		if ((c <= '9') && (c >= '0')) {
		    yylval = c - '0';
		    return (DIGIT);
		}
		if (c <= ' ') c = 0;
	    }
	    return (c);
	}
.DE
This creates the \fByacc\fR specification file.
To turn it into a program, type
.DM
	yacc digit.y
	cc y.tab.c -ly -o digit
.DE
To invoke the compiled progra, type:
.DM
	digit
.DE
And to test it, type the following:
.DM
	1+2
	2+2
	8+9
.DE
\fBdigit\fR will reply, respectively:
.DM
	3
	4
	17
.DE
.PP
To exit from
.BR digit ,
type
.BR <ctrl-C> .
.PP
.B digit
is essentially an interpreter \(em
results are calculated as numbers are typed in.
When you type in
.DM
	1+1
.DE
the parser recognizes the construct
.DM
	term '+' term
.DE
and executes the statement that adds two numbers together.
The two numbers each in turn came from the construct
.DM
	term	:	DIGIT
.DE
and the value of the digit came from \fByylex\fR.
When the statement
\fBcalcn\fR is recognized, the value is printed as the result.
Thus, the calculations are performed at the time that the constructs are
recognized.
If a compiler were being generated, the actions would likely build some form of
intermediate code, or expression tree, as in:
.DM
	expr	:	term '+' term
				{$$=tree (plus, $1, $3);}
.DE
.Sh "Structured Values"
.PP
All the examples thus far have shown action values as simple \fBint\fR types.
This is not sufficient for a large interpreter or compiler, because
at different
points in the language a value can represent a constant values, a pointer to
code generation trees, or symbol table information.
.PP
To solve this problem, \fByacc\fR allows you to
define the values of \fB$$\fR and \fB$n\fR as a \fIunion\fR of several types.
This is done in the definitions section with the \fBunion\fR statement.
For example, to declare action values as
an integer, tree pointer, or a symbol-table pointer, you would use
the following code:
.DM
	%union {
	    int cval;
	    struct tree_t tree;
	    struct sytp_t sytp;
	}
.DE
This says that action values can be a constant value \fBcval\fR, a code tree
pointer \fBtree\fR, or a symbol-table pointer \fBsytp\fR.
.PP
.II "yacc^token, definition"
.II "yacc^%token"
To ensure
that the correct types are used in assignments and calculations in actions
in the generated C program, each token whose value will be used is declared
with the appropriate type:
.DM
	%token <tree> A B
	%token <cval> CONST
.DE
.II "yacc^rule, type"
.II "yacc^%token"
In addition,
the rules themselves can have a type declaration, as they
also can pass action values.
Their type is declared in the \fB%type\fR statement:
.DM
	%type <sytp> variable
.DE
.II "yacc^type, of nonterminal"
This declares the nonterminal \fBvariable\fR to reference the \fBsytp\fR
field of the value union.
.PP
.II "yacc^value, qualification"
The values referenced in the action statements do not need to be qualified
(unless they are referencing a field of one of the union elements).
\fByacc\fR generates the necessary qualification for the references,
based upon
the type information provided in the \fB%type\fR and \fB%token\fR statements.
.PP
.II "yacc^default, action"
Keep in mind that productions that do not have explicit actions will default
to an action of
.DM
	$$ = $1
.DE
which might cause a type clash when compiling the generated parser.
This is more likely to arise during debugging,
when you have defined the types but have not put in the actions.
.SH "Handling Ambiguities"
.PP
.II "yacc^ambiguity"
The ideal grammar for a language is readable and unambiguous.
If the grammar is readable, its users will find it easy to use.
If the language is unambiguous, the parser generator will parse the programs
correctly.
However, many common programming language constructs are ambiguous.
Consider the following definition of an \fBif\fR statement:
.DM
	statement	:	if_statement
			|	others
	if_statement	:	IF cond THEN statement
			|	IF cond THEN statement ELSE statement
.DE
.PP
Consider a program that contains a statement
.DM
	if a > b then if c < d then a = d else b = c;
.DE
The parser does not know by the grammar specification which
\fBif_statement\fR the \fBelse\fR belongs with.
At the point of the \fBelse\fR, the parser could correctly recognize it as
part of the first \fBif\fR or the second \fBif\fR.
The indentations illustrate
the interpretation of the ambiguity associating the \fBelse\fR with the first
\fBif\fR. 
.DM
	if a > b then
	    if c < d then
		a = d;
	else
	    b = c;
.DE
Associating it with the second \fBif\fR: 
.DM
	if a > b then
		if c < d then
			a = d;
		else
			b = c;
.DE
.PP
One solution to this ambiguity is to modify the language and rewrite the
grammar.
Some programming languages (including the \*(CO shell) have a closing
element to the \fBif\fR statement, such as \fBfi\fR.
The grammar for this approach is:
.DM
	statement	:	if_statement
			|	others
	if_statement	:	IF cond THEN statement FI
			|	IF cond THEN statement ELSE statement FI
.DE
Another ambiguity arises from a grammar for common binary arithmetic
expressions.
The following sample specifies binary subtraction:
.DM
	exp	:	TERM
		|	exp '-' exp
		;
.DE
For the program fragment
.DM
	a - b - c
.DE
the parser can correctly interpret the expression as
.DM
	(a - b) - c
.DE
or as
.DM
	a - (b - c)
.DE
While for the \fBif\fR example, the language can be
reasonably modified to remove the ambiguity,
it is unreasonable in the case of expressions.
The grammar can be rewritten for \fBexp\fR but it is less convenient.
.Sh "How yacc Reacts"
.PP
.II "yacc^ambiguity, default handling"
Because some ambiguities, such as the ones detailed above, are common,
\fByacc\fR automatically handles some of them.
.PP
.II "yacc^shift-reduce conflicts"
.II "yacc^shift"
.II "yacc^reduce"
The ambiguity exemplified by the \fBif then else\fR grammar is
called a \fIshift-reduce\fR conflict.
The parser generator can either choose to shift, meaning to add more elements
to the parse stack, or to reduce, meaning to generate the smaller production.
In the terms of \fBif\fR, the shift would match the \fBelse\fR with the first
\fBthen\fR.
Alternatively, the reduce choice will match the \fBelse\fR with
the latest (rightmost) unmatched \fBthen\fR.
.PP
.II "yacc^shift-reduce conflicts"
Unless otherwise specified, \fByacc\fR resolves shift-reduce conflicts in
favor of the shift.
This means that the \fBif\fR ambiguity will be resolved in favor of matching
the \fBelse\fR with the rightmost unmatched \fBthen\fR.
Likewise, the expression
.DM
	a - b - c
.DE
will be interpreted as
.DM
	a - (b - c)
.DE
.Sh "Additional Control"
.PP
.II "yacc^ambiguity, resolution"
.II "yacc^associativity"
\fByacc\fR provides tools to help resolve some of these ambiguities.
When \fByacc\fR detects shift-reduce conflicts,
it consults the precedence and
associativity of the rule and the input symbol to make a decision.
.PP
.II "yacc^definition section"
For the case of binary operators, you can define the associativity of each
of the operators by use of the defining words \fB%left\fR and \fB%right\fR.
These appear in the definition section with \fB%token\fR.
.PP
The usual interpretation of
.DM
	a - b - c
.DE
is
.DM
	(a - b) - c
.DE
.II "yacc^associative, left"
which is called \fIleft\fR associative.
However, the shift/reduce conflict inherent in
.DM
	exp '-' exp
.DE
.II "yacc^associative, right"
is resolved in favor of the reduce, or in a right-associative manner:
.DM
	a - (b - c)
.DE
.PP
.II "yacc^%left"
.II "yacc^%token"
To signal \fByacc\fR that you want the left-associative interpretation,
enter the grammer as:
.DM
	%left '+' '-'
	%token TERM
	%%
	expr	:	TERM
		|	expr '-' expr
		|	expr '+' expr
		;
.DE
Some operators, such as assignment, require right associativity.
The statement
.DM
	a := b + c
.DE
is to be interpreted as
.DM
	a := (b + c)
.DE
.II "yacc^%right"
The \fB%right\fR keyword tells \fByacc\fR that the following terminal is to
right associate.
.Sh "Precedence"
.PP
.II "yacc^precedence"
Most arithmetic operators are left associative.
For example, with the grammar
.DM
	%right =
	%left '-' '+' '*' '/'
	%%
	expr	:	expr '-' expr
		|	expr '*' expr
		|	expr '+' expr
		|	expr '/' expr
		|	expr '=' expr
		;
.DE
The expression
.DM
	a = b + c * d - e
.DE
based on associativity alone will be evaluated
.DM
	a = (((b + c) * d) - e)
.DE
which is not according to custom.
We normally think of \fB*\fR as having higher precedence
than \fB+\fR or \fB-\fR, meaning that it is evaluated before other operators
with the same associativity.
The evaluation preferred is
.DM
	a = (b + (c * d) - e)
.DE
.PP
.II "yacc^%left"
.II "yacc^precedence"
To generate a parser with this evaluation, use several lines of \fB%left\fR,
one line for each level of precedence.
Each line containing \fB%left\fR describes tokens of the same precedence.
The precedence increases with each line.
Thus, to get the common notion of arithmetic precedence, use a grammar of
.DM
	%right =
	%left '-' '+'
	%left '*' '/'
	%%
	expr	:	expr '-' expr
		|	expr '*' expr
		|	expr '+' expr
		|	expr '/' expr
		|	expr '=' expr
		;
.DE
.PP
.II "yacc^%left"
.II "yacc^%right"
This method of \fB%left\fR and \fB%right\fR gives tokens a precedence
and an associativity.
This can eliminate ambiguities where these operators are involved.
But what about the precedence of rules or nonterminals?
.PP
.II "yacc^%prec"
.II "yacc^rules, precedence"
.II "yacc^precedence"
To specify the precedence of rules, the \fB%prec\fR keyword at the end of the
rule sets the precedence of the rule to the token following the keyword.
To add unary minus to the grammar above, and to give it the precedence of
multiply, use \fB%prec *\fR at the end of the unary rule.
.DM
	%right =
	%left '-' '+' '*' '/'
	%%
	expr	:	expr '-' expr
		|	expr '*' expr
		|	expr '+' expr
		|	expr '/' expr
		|	expr '=' expr
		|	'-' expr %prec *
		;
.DE
.II "yacc^%nonassoc"
.II "yacc^nonassociative"
.II "yacc^%left"
.II "yacc^right"
If associativity is not specified, \fByacc\fR will report the number of
shift/reduce conflicts.
When associativity is specified with \fB%left\fR, \fB%right\fR or
\fB%nonassoc\fR, this is considered to reduce the number of conflicts,
and thus the number of conflicts reported will not include the count of these.
.SH "Error Handling"
.PP
.II "yacc^error, recovery"
Parsers generated by \fByacc\fR are designed to parse correct programs.
If an input program contains errors, the LALR(1) parser will detect the error
as soon as is theoretically possible.
The error is identified, and the programmer can correct the error and
recompile.
.PP
However, in most programming environments, it is unacceptable to stop
compiling after the detection of a single error.
\fByacc\fR parsers attempt to go on so that the programmer may find as many
errors as possible.
.PP
.II "yacc^error, token"
.II "yacc^token, error"
When an error is detected, the parser looks for a special token in the input
grammar named \fBerror\fR.
If none is found, the parser simply exits after issuing the message
.DM
	Syntax error
.DE
If the special token \fBerror\fR is present in the input grammar error
recovery is modified.
Upon detection of an error, the parser removes items from the stack until
\fBerror\fR is a legal input token and processes any action associated with
this rule.
\fBerror\fR is the lookahead token at this point.
.PP
.II "yacc^error, recovery"
Processing is resumed with the token causing the error as the lookahead token.
However, the parser attempts to resynchronize by reading and processing three
more tokens before resuming normal processing.
If any of these three are in error, they are deleted and no error message is
given.
Three tokens must be read without error before the parser leaves the error
state.
.PP
.II "yacc^error, token"
.II "yacc^token, error"
A good place to put the \fBerror\fR token is at a statement level.
For example, the \fBcalc.y\fR example in chapter 2 defines a statement as
.DM
	stmnt	:	stat
		|	stat '\en'
		|	error '\en'
		;
.DE
Thus, any error on a line will cause the rest of the line to be ignored.
.PP
.II "yacc^yyerrok"
There is still a chance for trouble, however.
If the next line contains an error in the first two tokens, they will be
deleted with no error message and parsing will resume somewhere in the middle
of the line.
To give a truly fresh start at the beginning of the line,
the function \fByyerrok\fR will cause the parser to resume normal
processing immediately.
Thus, an improved grammar is
.DM
	stmnt	:	stat
		|	stat '\en'
		|	error '\en'
				{yyerrok;}
		;
.DE
will cause normal processing to begin with the start of the next line.
.PP
Error recovery is a complex issue.
This section covers only what the parser can do in recovering from syntax
errors.
Semantic error recovery, such as retracting emitted code, or correcting symbol
table entries, is even more complex, and is not discussed here.
.PP
\fByacc\fR reserves a special token \fBerror\fR to aid in resynchronizing the
parse.
After an error is detected, the stack is readjusted, and processing cautiously
resumes while three error-free tokens are processed.
\fByyerrok\fR will cause normal processing to resume immediately.
The token causing the error is retained as the lookahead token unless
\fBYYCLEARIN\fR is executed.
.SH "Summary"
.PP
\fByacc\fR is an efficient and easy-to-use program to help automate the input
phase of programs that benefit by strict checking of complex input.
Such programs include compilers and interactive command language processors.
.PP
\fByacc\fR generates an LALR(1) parser, that implements
the grammar specifying the structure of the input.
A simple lexical analyser routine can be hand-constructed to fit in among
the rules, or you can use the \*(CO command \fBlex\fR to generate a lexical
analyzer that will fit with the parser.
.PP
As the structured input is analyzed and verified, you assign meaning to the
input by writing semantic \fBactions\fR as part of the grammatical rules
describing the structure of the input.
.PP
\fByacc\fR parsers are capable of handling certain \fIambiguities\fR, such
as that inherent in typical \fBif then else\fR constructs.
This simplifies the construction of many common grammars.
.PP
\fByacc\fR provides a few simple tools to aid in error recovery.
However, the area of error recovery is complex and must be approached with
caution.
.Sh "Helpful Hints"
.PP
Until you have mastered \fByacc\fR, the best way to build your program is to 
do it a piece at a time.
For example, if you are writing a Pascal compiler,
you might start with the grammar
.DM
	%token PROG BEG END OTHER
	program :	PROG tokens BEG END '.'
		;
	tokens	:	OTHER
		|	tokens OTHER
		;
.DE
and with a simple lexical analyzer of:
.DM
	PROGRAM		return (PROG);
	BEGIN		return (BEG);
	END		return (END);
	.		return (yytext [0]);
.DE
With the generated program, you can easily test the grammar by feeding it
simple programs.
Then add items to both the lexical analyzer and \fByacc\fR grammar.
With this approach, you can see the parser working, and if it behaves
differently than you expect, you can more easily pinpoint the cause.
.PP
If you have difficulty understanding what actions your parser is taking,
\fByacc\fR will produce for you a complete description of the generated parser.
To use this, you should be familiar with the way LALR(1) parsers work.
To get this verbose output, specify the \fB-v\fR option on the command line.
The result will appear in the file \fBy.output\fR.
.PP
In addition, you can have the parser give you a token-by-token description
of its actions while it does them, by specifying the debug option \fB-d\fR.
This also generates the file \fBy.output\fR, which is helpful in reading
the debug output.
The debug code is generated when the \fB-d\fR option is used,
but is not activated unless the \fBYYDEBUG\fR identifier is defined.
Include some code in the definitions section to activate it:
.DM
	%{
	    define YYDEBUG
	%}
.DE
Your parser can turn on and off the debugging at execution time by setting the
variable \fBYYDEBUG\fR:
one for on, zero for off.
.PP
A frequent cause of grammar conflicts is the empty statement.
You should use it with caution.
.B yacc
generates
empty statements when you specify actions
in the middle of a rule rather than at the end; for example:
.DM
	def	:	DEFINE {defstart();}
				identifier {defid ($2);}
		;
.DE
\fByacc\fR generates an additional rule:
.DM
	$def	:	/* empty */
				{defstart();}
		;
	def	:	DEFINE $def identifier {defid ($2);}
		;
.DE
The resulting empty statement can cause parser conflicts if there are similar
rules and the empty statement is not sufficient to distinguish between them.
.\".SH "Example"
.\".PP
.\"This tutorial closes with a larger example that incorporates most of the
.\"features of \fByacc\fR discussed here.
.\"You can type it as shown, and modify it to improve its operation.
.\".PP
.\"This example, called
.\".BR nav ,
.\"calculates the great circle
.\"path and bearing from one point on the globe to another.
.\"Each pair of points is input on one line.
.\"The coordinates of the origin and destination are preceeded, respectively,
.\"by the keywords
.\"\fBFROM\fR and \fBTO\fR, and can appear in either order.
.\"Longitude and latitude are followed, respectively,
.\"by the letters \fBE\fR or \fBW\fR, and \fBN\fR and \fBS\fR.
.\"Lower-case may also be used for these letters.
.\".PP
.\"The numeric part of the coordinates may be entered in degrees, minutes, and
.\"optional seconds, or in fractional degrees.
.\"You can use
.\"the symbols \fB^\fR, \fBo\fR, or \fBd\fR to specify degrees
.\"because the raised
.\"circle customarily used for degrees is not available on most terminals.
.\"An apostrophe \fB'\fR follows minutes, and a quotation mark \fB"\fR follows
.\"seconds.
.\".PP
.\"As an example of using \fBnav\fR, calculate the great circle
.\"distance and initial heading from Charlestown, Indiana, to Charlestown,
.\"Australia:
.\".DM
.\"	from 38d27'n 85d40'w to 151d42'e 32d58's;
.\".DE
.\"The result will be:
.\".DM
.\"	From lat 38.450 long 85.667 To lat -32.967 long -151.700
.\"	Distance 8030.623, Init course is 258.417
.\".DE
.\"Here, the coordinates are echoed in decimal degrees.
.\"To exit the program, type
.\".BR <ctrl-C> .
.\".PP
.\"To begin,
.\"type the following \fByacc\fR specification file into the \fBnav.y\fR: 
.\".DM
.\"	%{
.\"	#include "ll.h"
.\"	#define YYTNAMES
.\"		double fromlat, fromlon, tolat, tolon;
.\"		extern calcpath();
.\"	%}
.\".DE
.\".DM
.\"	%union {
.\"		double dgs;
.\"		long dgsi;
.\"		struct ll wh;
.\"		}
.\".DE
.\".DM
.\"	%token NEWLINE FROM TO CIRCLE QUOTE DQUOTE SEP SEMI COMMA
.\"	%token NSYM SSYM WSYM ESYM
.\"	%token <dgs> FNUM
.\"	%token <dgsi> NUM
.\"	%type <dgs> degrees long lat deg
.\"	%type <wh> where from to
.\"	%%
.\".DE
.\".DM
.\"	prob	:	single
.\"		|	prob single
.\"		;
.\".DE
.\".DM
.\"	single	:	sing {
.\"				calcpath();
.\"				}
.\"		|	error NEWLINE {
.\"				yyerrok; YYCLEARIN;
.\"				printf ("Enter line again.\en");
.\"				}
.\"		;
.\".DE
.\".DM
.\"	sing	:	from SEP to SEMI NEWLINE {
.\"				fromlat = $1.lat;
.\"				fromlon = $1.lon;
.\"				tolat = $3.lat;
.\"				tolon = $3.lon;
.\"				}
.\".DE
.\".DM
.\"		|	to SEP from SEMI NEWLINE {
.\"				tolat = $1.lat;
.\"				tolon = $1.lon;
.\"				fromlat = $3.lat;
.\"				fromlon = $3.lon;
.\"				}
.\".DE
.\".DM
.\"		|	to SEMI NEWLINE {
.\"				tolat = $1.lat;
.\"				tolon = $1.lon;
.\"				}
.\"		;
.\".DE
.\".DM
.\"	from	:	FROM SEP where {
.\"				$$ = $3;
.\"				}
.\"		;
.\".DE
.\".DM
.\"	to	:	TO SEP where {
.\"				$$ = $3;
.\"				}
.\"		;
.\".DE
.\".DM
.\"	where	:	lat SEP long {
.\"				$$.lat = $1;
.\"				$$.lon = $3;
.\"				}
.\"		|	long SEP lat {
.\"				$$.lon = $1;
.\"				$$.lat = $3;
.\"				}
.\"		;
.\".DE
.\".DM
.\"	lat	:	degrees NSYM {
.\"				$$ = $1;
.\"				}
.\"		|	degrees SSYM {
.\"				$$ = - $1;
.\"				}
.\"		;
.\".DE
.\".DM
.\"	long	:	degrees WSYM {
.\"				$$ = $1;
.\"				}
.\"		|	degrees ESYM {
.\"				$$ = - $1;
.\"				}
.\"		;
.\".DE
.\".DM
.\"	degrees	:	FNUM	/* e. g. 128.3 */ {
.\"				$$ = $1;
.\"				}
.\"		|	NUM CIRCLE NUM QUOTE /* deg min */ {
.\"				$$=$1 + $3/60.0;
.\"				}
.\"		|	NUM CIRCLE NUM QUOTE NUM DQUOTE
.\"				/* and seconds */ {
.\"				$$=$1 + $3/60.0 + $5/3600.0;
.\"				}
.\"		|	NUM CIRCLE NUM QUOTE FNUM DQUOTE {
.\"				$$=$1 + $3/60.0 + $5/3600.0;
.\"				}
.\"		;
.\"	%%
.\".DE
.\".DM
.\"	#include <stdio.h>
.\"	yyerror (s)
.\".DE
.\".DM
.\"	char *s;
.\"	{
.\"	    struct yytname *p;
.\"	    fprintf (stderr, "%s ", s);
.\".DE
.\".DM
.\"	    for (p = yytnames; p -> tn_name != NULL; ++p)
.\"		if (p->tn_val == yychar) {
.\"		    fprintf (stderr, "at %s", p->tn_name);
.\"		    break;
.\"		}
.\"	    fprintf (stderr, "\en");
.\"	}
.\".DE
.\".PP
.\"Both the lexical analyzer and the parser need the following header file
.\"\fBll.h\fR.
.\"Type the following:
.\".DM
.\"	cat > ll.h
.\"	struct ll {
.\"	    double lat;
.\"	    double lon;
.\"	};
.\"	<ctrl-D>
.\".DE
.\".PP
.\"To turn \fByacc\fR file \fBnav.y\fR into a program, type
.\".DM
.\"	yacc -hdr nav.tab.h -d -v nav.y
.\"	mv y.tab.c nav.y.c
.\".DE
.\".PP
.\"The grammar is straightforward.
.\"The types used in the actions require a
.\".BR union ,
.\"because integer degrees, floating-point
.\"degrees, and pairs of floating point degrees are used as action values.
.\"The lexical analyzer recognizes integer and floating-point numbers,
.\"and passes the value through \fByylval\fR.
.\"The rule for \fBdegrees\fR combines
.\"different degree representations to one double-precision number.
.\".PP
.\"The \fBN\fR, \fBS\fR, \fBE\fR, and \fBW\fR symbols convert a location to a
.\"signed representation:
.\"\fBS\fR and \fBE\fR result in negative degrees,
.\"\fBN\fR and \fBW\fR as positive.
.\".PP
.\"The rule for \fBwhere\fR converts the single-numbered latitude and longitude
.\"into a double number of \fB<wh>\fR type.
.\"Note that it can process the coordinates in either order.
.\".PP
.\"The rule \fBsingle\fR handles the destination and origin in either order.
.\"It takes the pairs of coordinates from \fBfrom\fR and \fBto\fR and stores them
.\"in the global variables that the calculation routine uses.
.\"The error token will halt error recovery at the end of the line,
.\"so that in case of error the user can reenter the correct line.
.\"If many great circles are being computed from the same origin,
.\"you need to enter only the destination after the first time.
.\".PP
.\"Once a set of coordinates has been recognized, the function
.\".B calcpath
.\"calculates the great circle.
.\".PP
.\"The error routine \fByyerror\fR accepts an error message from the parser,
.\"and examines the table of tokens to find the name of the token where the error
.\"is detected.
.\"If it is found, it is printed.
.\"To get these token names in the program, the symbol \fBYYTNAMES\fR
.\"must be defined.
.\".PP
.\"The following code gives the lexical analyzer.
.\"Type it into the file \fBnav.l\fR:
.\".DM
.\"	%{
.\"	#include "ll.h"
.\"	#include "nav.tab.h"
.\"	%}
.\"		int integer;
.\"		double real;
.\"	%%
.\".DE
.\".DM
.\".ta 0.5i 1.5i
.\"	[nN]	return (NSYM);
.\"	[sS]	return (SSYM);
.\"	[eE]	return (ESYM);
.\"	[wW]	return (WSYM);
.\"	o|"^"|d	return (CIRCLE);
.\"	\e"	return (DQUOTE);
.\"	\e'	return (QUOTE);
.\"	\en	return (NEWLINE);
.\"	from	return (FROM);
.\"	FROM	return (FROM);
.\"	to	return (TO);
.\"	TO	return (TO);
.\".DE
.\".DM
.\"	[0-9]+		{
.\"			sscanf (yytext, "%d", &integer);
.\"			yylval.dgsi = (long) integer;
.\"			return (NUM);
.\"			}
.\".DE
.\".DM
.\"	[0-9]+"."([0-9]+)? {
.\"			sscanf (yytext, "%f", &real);
.\"			yylval.dgs = (double) real;
.\"			return (FNUM);
.\"			}
.\"	,		return (COMMA);
.\"	;		return (SEMI);
.\"	[ \et]		return (SEP);
.\"	.		{
.\"			printf ("Illegal character [%s]\en", yytext);
.\"			return (yytext [0]);
.\"			}
.\".DE
.\"The lexical analyzer partitions the input into the
.\"tokens expected by the parser.
.\"For the symbols in the grammar, it returns the token type.
.\"It also recognizes integer and floating-point numbers, and converts them
.\"to integers.
.\".PP
.\"Note that the \fBll.h\fR file is required even though there is no explicit
.\"reference to its contents.
.\"This is needed because the \fB%union\fR in \fBnav.y\fR generates the header
.\"file \fBnav.tab.h\fR, referring to the \fBll\fR structure.
.\".PP
.\"Turn \fBlex\fR file \fBnav.l\fR into program by typing:
.\".DM
.\"	lex nav.l
.\"	mv lex.yy.c nav.l.c
.\".DE
.\"Finally, you should type the following code into
.\"file \fBnavcalc.c\fR.
.\"It is C code that calculates the great circle route:
.\".DM
.\"	#include <stdio.h>
.\"	#include <math.h>
.\"	/*
.\"	 *	Given latitude and longitude of start and finish,
.\"	 *	calculate the great circle path.
.\"	 */
.\"	extern	double fromlon, fromlat, tolon, tolat;
.\".DE
.\".DM
.\"	calcpath ()
.\"	{
.\"		double rad = PI / 180.0;
.\"		double initcourse, arg, dist, d60;
.\"		double rfromlat, rfromlon, rtolat, rtolon;
.\".DE
.\".DM
.\"		printf ("From lat %.3f long %.3f ",
.\"		    fromlat, fromlon);
.\"		printf ("To lat %.3f long %.3f\en",
.\"		    tolat, tolon);
.\".DE
.\".DM
.\"		rfromlat = fromlat * rad;
.\"		rfromlon = fromlon * rad;
.\"		rtolat = tolat * rad;
.\"		rtolon = tolon * rad;
.\".DE
.\".DM
.\"		d60 = acos (
.\"		    sin (rfromlat) * sin (rtolat) +
.\"		    cos (rfromlat) * cos (rtolat) *
.\"			 cos (rfromlon - rtolon)
.\"		 );
.\"		dist = 60 * d60 / rad;
.\".DE
.\".DM
.\"		arg = (sin (rtolat) - cos (d60) * sin (rfromlat))
.\"			/
.\"			(sin (d60) * cos (rfromlat));
.\".DE
.\".DM
.\"		initcourse = acos (arg) / rad;
.\"		if (sin (rfromlon - rtolon) < 0)
.\"		    initcourse = 360 - initcourse;
.\".DE
.\".DM
.\"		printf ("Distance %.3f, Init course is %.3f\en\en",
.\"		    dist, initcourse);
.\"	}
.\".DE
.\"And now compile all three programs together.
.\".DM
.\"	cc nav.y.c nav.l.c navcalc.c -ly -lm -ll -f -o nav
.\".DE
.\"The standard formula is used to calculate great circle path and bearing.
.\"Note that there are several limitations that are not checked for here;
.\"For example,
.\"diametrically opposite points on the globe have no unique great circle
.\"path between them.
.\"In addition, neither of the points should be at either of the poles.
.\"These checks can be added if you wish to use \fBnav\fR program as a general
.\"rather than a tutorial tool.
.SH "Where to Go From Here"
.PP
The Lexicon article for \fByacc\fR summarizes its command syntax and
features.
The tutorial for \fBlex\fR, the \*(CO lexical analyzer, describes
how to combine \fBlex\fR with \fByacc\fR to build applications simply.
