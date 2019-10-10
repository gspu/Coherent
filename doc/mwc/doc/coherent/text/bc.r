.II "bc^tutorial"
.ds TL "bc Desk Calculator"
.NH "bc Desk Calculator Language" 1
.PP
This tutorial introduces
.BR bc ,
the calculator language for \*(OS.
If you have not used
.B bc
before, this tutorial will introduce you to its
features and functions.
If you are familiar with
.BR bc ,
you can use it as a reference.
.PP
\fBbc\fR is a language that can calculate
to high precision.
It automatically adjusts
the number of digits in a number to represent it correctly.
It is like having a powerful calculator at your fingertips.
.Sh "Entry and Exit"
.PP
The \fBbc\fR calculator for \*(OS is easy to use.
Whenever you wish to invoke
.B bc ,
all you do is type its name (\fBbc\fR), followed by
a stroke of the carriage return key.
When you are finished using the calculator and wish to exit,
just type the word \*(Qlquit\*(Qr or
.if \nX=0 \fB<ctrl-D>\fR.
.if \nX=1 \fB<ctrl-Z>\fR.
.B bc
exits and returns control to \*(OS.
.Sh "Example of Simple Use"
.PP
\fBbc\fR performs calculations on formulas that you type into it.
The formulas are laid out as you would naturally write them.
For example, to invoke \fBbc\fR, have it add 
2+2, and then exit, type:
.DM
	bc
	2 + 2
.DE
.B bc
replies:
.DM
	4
.DE
Then, leave \fBbc\fR by typing:
.DM
	quit
.DE
.B bc
is an arbitrary precision calculator:
the number of digits carried by \fBbc\fR depends upon 
the requirements of the calculation,
and is automatically expanded by \fBbc\fR.
Thus, \fBbc\fR will never overflow.
The number of digits it carries is 
limited only by the amount of available computer memory.
For example, invoke
.B bc
and then try this calculation:
.DM
	2^500
.DE
The circumflex \*(Ql^\*(Qr character signifies a superscript; thus,
we are asking \fBbc\fR to raise 2 to the 500th power.
After a moment, \fBbc\fR will reply:
.DM
	327339060789614187001318969682759915221664\e
	204604306478948329136809613379640467455488\e
	327009232590415715088668412756007100921725\e
	6545885393053328527589376
.DE
.II "bc^exponentiation operator"
You have probably already noticed one nice thing about this calculator:
you don't have to include a print statement as part of your command, because
.B bc
automatically prints the results onto your terminal screen.
When \fBbc\fR sees any expression, like \*(QL2+2\*(QR or \*(QL37\-7\*(QR,
it prints the result.
.PP
.B bc
provides the
common arithmetic operators
for add, subtract, multiply, and divide, as
illustrated by the following commands:
.DM
	7 + 5
	7 - 5
	7 * 5
	7 / 5
.DE
.PP
\fBbc\fR also provides the
remainder operator
\*(Ql\fB%\fR\*(Qr.
To get a sense of how it works, type:
.DM
	7 % 5
	5 % 7
.DE
Here, \fBbc\fR prints the \fIremainder\fR of the first number
divided by the second; in the case of the first example,
\fBbc\fR prints 2, and in the second prints 5.
As you saw above, \fBbc\fR also includes
the exponentiation operator \*(Ql\fB^\fR\*(Qr.
.PP
With \fBbc\fR, you can also enter numbers with fractional parts.
Type the following to illustrate:
.DM
	9.999 * 9.999
.DE
\fBbc\fR replies:
.DM
	99.980
.DE
You can save temporary calculations or repeated constants in
\fIvariables\fR.
The following example shows you first how to define variables,
and second how to use them:
.DM
	a = 1.1
	b = 2.2
	a
	b
	a * b
.DE
Variable names can be longer than one letter.
.PP
The basic calculations in the above examples show only part of what
.B bc
can do.
The following section describes simple statements \(em the assignment of
variables and abbreviations \(em that allow you to perform
complex calculations easily.
.SH "Simple Statements"
.PP
Although you can use \fBbc\fR as a simple calculator
for manipulating numbers, you 
can take advantage of its greater power by using \fIvariables\fR.
Variables, as noted above, store parts of calculations or constants that 
you will use repeatedly in calculations.
Variable names are simply \*(QLwords\*(QR that you make up.
Here are some examples of possible variable names:
.DM
	a
	b
	totaltaxesdue
	ratio
.DE
To use variables, simply give them a value, use
them in a calculation in place of a number, or print them out.
.PP
To see how a variable can save you repetitive typing, and protect
you from possible errors, invoke \fBbc\fR and type the following:
.DM
	x = 9.999
	x
	x * x
	x = x * x
	x
.DE
The following gives the example with \fBbc\fR's replies \fIin italics\^\fR:
.DM
	x = 9.999
	x
	\fI9.999\fP
	x * x
	\fI99.980\fP
	x = x * x
	x
	\fI99.980\fP
.DE
.II bc^assignment
.B bc
did not reply to the
assignment statements \fBx=9.999\fR and \fBx=x*x\fR.
However, it did print the value of \fBx\fR when requested,
and the results of arithmetic using \fBx\fR.
.PP
Calculations executed with hand-held calculators,
with programming languages like C, or with \fBbc\fR
often use the following formula:
.DM
	x = x + 1
.DE
To decrease the likelihood of error,
\fBbc\fR offers you a shorthand expression for this common phrase:
.DM
	x += 1
.DE
What it means is, \*(QLadd one to \fBx\fR\*(QR.
Type the following example into \fBbc\fR to see how this expression works:
.DM
	x = 1
	x * x
	x += 1
	x * x
	x += 1
.DE
Likewise, \fBbc\fR provides an abbreviation for:
.DM
	x = x - 2
.DE
The form should now be familiar:
.DM
	x \(mi= 2
.DE
The number to the right of the \fB-=\fR or \fB+=\fR operator can be replaced
with a variable or even another calculation.
When you type:
.DM
	i = 4
	x = 48
	x \(mi= i
	x
.DE
\fBbc\fR replies:
.DM
	44
.DE
Alternatively, if you type:
.DM
	i = 4
	x = 48
	x \(mi= i * i
	x
.DE
then \fBbc\fR replies:
.DM
	32
.DE
Similar abbreviations are provided for multiplication,
division, remainder, and exponentiation.
Here is a summary of this class of operation.
.DS
.ta 0.5i 1.5i
	\fIa \fB+=\fR 2	Replace \fIa\fR with a plus 2
	\fIb \fB+= \fIa\fR	Replace \fIb\fR with \fIb\fR plus \fIa\fR
	\fIb \fB\(mi=\fR \fIa\fR	Replace \fIb\fR with \fIb\fR minus \fIa\fR
	\fIc \fB*= \fIb\fR	Replace \fIc\fR with \fIc\fR multiplied by \fIb\fR
	\fIc \fB/= \fIa\fR	Replace \fIc\fR with \fIc\fR divided by \fIa\fR
	\fIc \fB%= \fIb\fR	Replace \fIc\fR with remainder of \fIc\fR divided by \fIb\fR
	\fId \fB^= \fI3\fR	Replace \fId\fR with \fId\fR raised to the third power
.DE
.B bc
also has an operator that increases a variable by one:
\*(Ql\fB++\fR\*(Qr.
When you type:
.DM
	a = 1
	++a
.DE
then \fBbc\fR replies:
.DM
	2
.DE
To use this operator in an expression, combine it
with a variable anywhere that a variable would normally be used.
For example, entering
.DM
	b = 1
	a = 3
	b = ++a
	a
	b
.DE
yields:
.DM
	4
	4
.DE
The \*(Ql\fB++\fR\*(Qr operator can also be put after a name.
The resulting value in the expression is the value of the
name \fIbefore\fR it is incremented.
However, after the expression is evaluated,
the name will have an incremented value.
The following example shows the use of \*(Ql\fB++\fR\*(Qr
both before and after a name:
.DM
	a = 1
	b = 1
	a++
	++b
	a
	b
.DE
\fBbc\fR replies:
.DM
	1
	2
	2
	2
.DE
Operators are used in this manner:
.DM
	a = 1
	b = 2
	c = a++ + ++b
.DE
Similar to \*(Ql\fB++\fR\*(Qr is \*(Ql\fB\(mi\(mi\fR\*(Qr.
It behaves the same way, except that rather than adding one,
it subtracts one.
.SH "Numbers with Fractions"
.PP
Most of the examples presented earlier use whole numbers (integers).
However, \fBbc\fR can use numbers with fractional parts.
This section discusses the use of fractional numbers in \fBbc\fR and their
precision
under different operations.
.Sh "The Scale of Numbers"
.PP
The number of digits to the left of the decimal point carried by
.B bc
depends upon the requirements of the calculation.
If you calculate a large number, as in:
.DM
	2^500
.DE
the result will contain as many digits as needed to express the product.
.PP
The number of digits to the right of a decimal point is
called the
.I scale
of the number.
Scale depends upon the operation that produces the
number of digits, and a variable called
.B scale
that will be described shortly.
.PP
To illustrate simple uses of numbers with fractions, invoke
.B bc
and then type:
.DM
	a = .01
	b = 0.99
	a + b
.DE
\fBbc\fR replies:
.DM
	1.00
.DE
.Sh "Addition and Subtraction"
.PP
\fBbc\fR will dynamically adjust the number of digits in the calculation.
It deals similarly with fractional numbers.
To the following example
.DM
	a = 0.01
	b = 0.001
	a + b
.DE
\fBbc\fR reply:
.DM
	.011
.DE
In addition and subtraction, the scale of the result
is the \fIlarger\fR of the scales of the two numbers involved.
Results are not truncated in addition or subtraction operations.
.Sh "Scale During Multiplication"
.PP
Other arithmetic operations act differently with numbers 
that contain fractions.
In the multiplication of two numbers, the scale of 
the product will at least equal the larger of the scales of the two numbers.
For example, the input:
.DM
	1.1 * 1.11
.DE
results in:
.DM
	1.22
.DE
.Sh "Setting the Scale of Results"
.PP
To increase the number of fractional digits for higher accuracy,
.B bc
provides the built-in variable \fBscale\fR.
The following example illustrates the \fBscale\fR variable:
.DM
	scale = 3
	1.1 * 1.11
.DE
The result from this example is:
.DM
	1.221
.DE
Note, however, the scale of the product of a multiplication procedure
never exceeds the sum of the scales of the two numbers being multiplied.
For example,
.DM
	scale = 10
	1.1 * 1.11
.DE
yields the result:
.DM
	1.221
.DE
If the variable \fBscale\fR is less than the 
sum of the scales of the numbers being multiplied, then the product
will have a scale equal to that of the variable \fBscale\fR.
For example,
.DM
	scale = 4
	1.11 * 2.222
.DE
yields:
.DM
	2.4664
.DE
The scales of the operands are 2 and 3.
The larger scale is 3, so the result of a multiplication 
will have a scale of at least 3, no matter what \fBscale\fR is set to.
Also, the sum of the scales is 5, so the result will never
have more than 5 digits to the right of the decimal point.
In this example, \fBscale\fR has been set to a scale of 4.
Therefore, the result has four digits to the right of the decimal point.
.Sh "Scale for Divisions"
.PP
For division and remainder, the scale of the result is determined 
only by the value of the variable \fBscale\fR.
For example,
.DM
	scale = 13
	14 / 13
	14 % 13
.DE
yields:
.DM
	1.0769230769230
	.0000000000010
.DE
For non-whole numbers, as well as for integers,
the definition of remainder is chosen so that the relationship
.DM
	dividend = (divisor * quotient) + remainder
.DE
is true.
.Sh "Scale From Exponentiation"
.PP
.B bc
sets the \fBscale\fR of a result of exponentiation as
if repeated multiplications had been performed.
Thus, for
.DM
	5.992 ^ 5
.DE
the scale is chosen as if you typed:
.DM
	n = 5.992
	n * n * n * n * n
.DE
That is, the default is the scale of the largest (or, in this case,
the only) number being multiplied; and scale cannot exceed the
sum of the scales of the numbers being multiplied.
Thus, the scale of the product in this example
has a default setting of 3, and can be reset up to 15.
.Sh "What Is the Current Scale?"
.PP
The variable \fBscale\fR is just like other variables:
you can assign values to it, as above.
Because it is like regular variables, you can also use it
in operations, as in this example:
.DM
	scale += 1
.DE
You can also print its value:
.DM
	scale
.DE
The value of the \fBscale\fR variable is zero until you explicitly change it.
.SH "The if Statement"
.PP
The statements shown so far have been either assignment statements, 
giving a new value to a variable; or an expression, which prints 
the resulting value.
Several other kinds of statements are available.
These give you power to write programs that make
decisions and perform iterative computations.
.Sh "Using the if Statement"
.PP
To see the
.B if
statement in action, type the following example into \fBbc\fR:

.DM
	x = 3
	if (x < 5) x
	if (x > 5) -x
.DE
.B bc
replies:
.DM
	3
.DE
If the input is:
.DM
	x = 6
	if (x < 5) x
	if (x > 5) -x
	<return>
.DE
\fBbc\fR replies:
.DM
	-6
.DE
The part of the
.B if
statement in parentheses, such as \fB(x > 5)\fR, determines whether
.B bc
executes the statement that follows it, such as
.BR -x .
If the expression is false, the following statement is not executed.
If the expression is true, the following statement is executed.
.Sh "Comparisons"
.PP
The decision expression in an \fBif\fR statement is enclosed in parentheses.
The decision can be based upon a comparison of two operands, or numbers.
The kinds of comparisons that can be done are:
.DS
	\fB==\fR	First operand equal to second
	\fB!=\fR	First operand not equal to second
	\fB<=\fR	First operand less than or equal to second
	\fB<\fR	First operand less than second
	\fB>=\fR	First operand greater than or equal to second
	\fB>\fR	First operand greater than second
.DE
The \fBif\fR statement can include the sorts of
the simple statements already shown.
You can also include an \fBif\fR statement, as well as the \fBwhile\fR,
\fBdo\fR, and \fBfor\fR statements, 
which will be discussed below.
The following example illustrates the use of an \fBif\fR
statement within an \fBif\fR statement:
.DM
	a = 2
	b = 6
	if (a >= 2) if (b > a) a + b
	<return>
.DE
\fBbc\fR replies, simply:
.DM
	8
.DE
Because both of the \fBif\fR conditions were true,
\fBbc\fR proceeded to add \fBa\fR and \fBb\fR.
Note that nested
.B if
statements must appear on the same line.
Therefore, 
.DM
	if (a == 3) if (b > a) a + b
.DE
does not print the result of
.B "a + b"
because not both conditions were true.
However
.DM
	if (a == 3)
	if (b > a) a + b
.DE
prints the result of
.B "a + b"
because
.B bc
treats
.B if
statements one by one, and the second
.B if
statement's condition is true.
.Sh "Grouped Statements"
.PP
You can place more than one statement after the expression part of the 
\fBif\fR statement by using grouping braces \*(Ql\fB{\fR\*(Qr
and \*(Ql\fB}\fR\*(Qr.
This can be useful if you want to perform several calculations 
based on the result of an \fBif\fR statement comparison.
The following example prints the value of \fBa\fR and 
\fBb\fR if the value of \fBb\fR is less than the value of \fBa\fR:
.DM
	a = 1
	b = .99
	if (a > b) {
	    a
	    b
	}
.DE
\fBbc\fR replies:
.DM
	1
	.99
.DE
Any statement may be enclosed within the group braces, as
the following example shows:
.DM
	a = 1
	b = .99
	if (a > b) {
	    a
	    b
	    if ((a + b) >= 2) a + b
	}
.DE
.Sh "Many Statements Per Line"
.PP
To this point, all of our examples typed each statement on its own line.
This includes the group braces \*(Ql\fB{\fR\*(Qr and \*(Ql\fB}\fR\*(Qr,
the latter of which must appear on a line by itself.
You can, however, place several statements on one line if you separate them
with semicolons.
If you do this, remember that the semicolon rather than the
carriage return separates the statements.
For example, if you type:
.DM
	a = 1;b = 2;c = 3
	a;b;c
.DE
\fBbc\fR replies:
.DM
	1
	2
	3
.DE
You can use this in combination with the group braces:
.DM
	a = 1;b = 2;c = 3
	if ((a + b) >= c) {
	    a; b; c; a + b; }
.DE
The reply from \fBbc\fR is:
.DM
	1
	2
	3
	3
.DE
This example can be compressed even further by putting all of the \fBif\fR
statement on one line:
.DM
	a = 1;b = 2;c = 3
	if ((a + b) >= c) { a; b; c; a + b; }
.DE
You do not need to follow the \*(Ql\fB}\fR\*(Qr with a semicolon.
.SH "The while Statement"
.PP
The \fBwhile\fR statement repeats calculations.
This is useful in successive approximation calculations.
The following example of the \fBwhile\fR loop 
prints the numbers one through ten:
.DM
.ta 1.0i 1.5i
	i = 1
	while (i <= 10) {
		i
		i = i + 1
	}
.DE
.B bc
replies:
.DM
	1
	2
	3
	4
	5
	6
	7
	8
	9
	10
.DE
The statement
.DM
	i = i + 1
.DE
adds 1 to the variable \fBi\fR.
The expression
.DM
	(i <= 10)
.DE
compares \fBi\fR with ten.
While \fBi\fR is less than or equal to ten, the \fBwhile\fR 
loop executes.
When \fBi\fR is increased to greater than ten, the loop stops executing.
.PP
.B bc
checks the comparison expression for the \fBwhile\fR loop
before the loop is entered for the first time.
If the comparison fails, the loop is not executed at all;
otherwise the processing repeats as long as the comparison is true.
For example, the following statements do not print anything:
.DM
	i = 0
	while (i > 1) i
	quit
.DE
.Sh "Abbreviations in the while Statement"
.PP
If we recall the assignment statements from the previous section,
we can shorten
the \fBwhile\fR counting-to-ten example to:
.DM
	i = 1
	while (i <= 10) {
		i
		i += 1
	}
.DE
The result remains the same \(em a list of numbers from one to ten.
.PP
Another abbreviation of the example uses the \*(Ql\fB++\fR\*(Qr operator.
The variable \fBi\fR is incremented, then tested in the \fBwhile\fR
expression, which simplifies the entire example to:
.DM
	i = 0
	while (++i <= 10) i
.DE
Before the \fBwhile\fR is executed, \fBi\fR is set to zero.
Then, the \fBwhile\fR expression increments the
value of \fBi\fR before it is used or compared,
Thus, the first value compared, then printed, is one.
.PP
Finally, the example calculation can be shortened to one line.
If a variable in \fBbc\fR is used before it is initialized, it will
have the value of zero.
For example:
.DM
	zip
.DE
prints:
.DM
	0
.DE
Using this in our counting-to-ten example yields:
.DM
	while (++n <= 10) n
.DE
.SH "The for Statement"
.PP
.B for
is a statement that controls the execution of other
.B bc
statements.
You should use
\fBfor\fR to write a formula 
to control the number of times a value is computed.
.PP
The previous section demonstrated how to print the numbers one to 
ten using a \fBwhile\fR statement.
The following does the same task with a \fBfor\fR statement:
.DM
	for (i=1; i <= 10; ++i) i
.DE
.Sh "Three Parts of the for Statement"
.PP
The \fBfor\fR statement is more complex than the \fBwhile\fR statement;
its controlling expressions have three parts.
.PP
The first part, shown here in italics
.DM
	for (\fIi=1\^\fP; i <= 10; ++i) i
.DE
sets up the initial condition.
The second part
.DM
	for (i=1; \fIi <= 10\^\fP; ++i) i
.DE
tests whether more iterations should be performed.
.B bc
performs this test
.I before
it executes the statements that are subordinate to the
.B for
statement.
If the test fails, no more iterations are performed.
.PP
The third part
.DM
	for (i=1; i <= 10; \fI++i\^\fP) i
.DE
is performed at the end of each iteration.
In practically every instance, this part of the
.B for
statement modifies the value of the variable that the second part tests.
.PP
Taken together, these statements (1) set \fBi\fR to zero; (2) check whether
\fBi\fR is less than or equal to ten; (3) if \fBi\fR proves to be so,
prints \fBi\fR, and then increases it by one.
.PP
The following example of the \fBfor\fR statement adds
the squares of the numbers one through ten, prints each square,
and then prints the sum of the squares at the end.
.DM
	sum = 0
	for (n=1; n <= 10; ++n) {
	    sq = n * n
	    sq
	    sum += sq
	}
	sum
.DE
The result is:
.DM
	1
	4
	9
	16
	25
	36
	49
	64
	81
	100
	385
.DE
.Sh "Similarities Between the for and while Statements"
.PP
To illustrate the similarity between the \fBfor\fR statement and
the simpler \fBwhile\fR statement, the following rewrites the
above example, substituting the \fBwhile\fR for the \fBfor\fR:
.DM
	sum = 0
	n = 0
	while (++n <= 10) {
		sq = n * n
		sq
		sum += sq
	}
	sum
.DE
.SH "Functions in bc"
.PP
.B bc
allows you to name routines that you use repeatedly.
You can then call them by name without having to retype them;
obviously, this can be a great time-saver.
These named routines are called
.IR functions .
This section shows you how to define and use functions
for your \fBbc\fR calculations.
.Sh "Example of Function Use"
.PP
The following example defines a function that calculates the area of
a circle from its radius.
.DM
	scale = 5
	pi = 3.14159
	define area (radius) {
		r2 = radius * radius
		return (pi * r2);
	}
	area (1.00)
	area (2.00)
	area (56)
.DE
The results will be:
.DM
	3.14159
	12.56636
	9852.02624
.DE
The \fBdefine\fR keyword tells \fBbc\fR that you are defining a function.
The name of the function follows.
Then, in parentheses, come the \fIparameters\fR of the function.
In this example, the only parameter, or \fIargument\fR,
of the function is \fBradius\fR.
Most functions have arguments, but they are not mandatory.
.PP
The \fBreturn\fR statement defines the value of the function.
In the
.B area
example, the expression
.DM
	area (1.00)
.DE
references the function \fBarea\fR.
\fBbc\fR then performs the calculation described by your definition
of the function
.BR area .
The number
.DM
	1.00
.DE
is substituted wherever the parameter \fBradius\fR is shown.
.PP
The statement
.DM
	r2 = radius * radius
.DE
is then executed, yielding this result:
.DM
	1.00
.DE
Then, the statement
.DM
	return (pi * r2)
.DE
calculates the area and returns its value.
The statement
.DM
	area (1.00)
.DE
then has the value calculated in the return statement.
.Sh "Functions Using Other Functions"
.PP
Functions in \fBbc\fR perform calculations using the
same expressions as the rest of the \fBbc\fR program.
This includes the use of functions.
The \fBarea\fR program can be written using another function,
\fBsq\fR, to calculate the square of a number:
.DM
	scale = 5
	pi = 3.14159
	define sq (number) {
		return (number * number)
		}
	define area (radius) {
		return (sq (radius) * pi)
		}
	area (1.00)
	area (2.00)
	area (56)
.DE
Again, the results will be identical:
.DM
	3.14159
	12.56636
	9852.02624
.DE
.Sh "Functions That Call Themselves"
.PP
Not only can functions call other functions and perform
regular calculations; a function can use itself in calculations.
An example of this is the Fibonacci calculation:
.DM
	define fib (f) {
		if (f == 0) return (0)
		if (f == 1) return (1)
		if (f > 1) return (fib (f - 1) + fib (f - 2))
	}
	fib (5)
	fib (20)
.DE
Fibonacci numbers are defined in the following way:
Fibonacci number zero is zero; similarly, Fibonacci number one is one.
Any other Fibonacci number is defined as the sum of the two previous
Fibonacci numbers.
Fibonacci numbers are defined only for non-negative integers.
.PP
The defined function \fBfib\fR follows this definition by 
returning zero if the number requested is zero and one if the argument is one.
If the number is neither of these, then the function calls itself to
calculate the previous two numbers of the series and adds them together.
.Sh "The auto Statement"
.PP
Many functions that call other functions, including themselves, may require
variables that are not changeable by the rest of the program.
This is signalled to \fBbc\fR by the \fBauto\fR statement:
.DM
	auto var1, var2
.DE
This declares \fBvar1\fR and \fBvar2\fR as local to
the function that contains them.
.PP
To illustrate the use of \fBauto\fR, the following \fBbc\fR
program calculates the factorial of a number:
.DM
	define factorial (number) {
		auto value, i
		value = 1
		for (i = 1; i <= number; ++i) value *= i
		return (value)
	}
	value = 3
	factorial (value)
	i = 99
	factorial (20)
	value
	i
.DE
The result is:
.DM
	6
	2432902008176640000
	3
	99
.DE
The first number, 6, results from:
.DM
	factorial (value)
.DE
The second number is from:
.DM
	factorial (20)
.DE
The last two numbers are from \fBvalue\fR and \fBi\fR,
and are included to demonstrate that the variables in the function
\fBfactorial\fR appearing in this statement:
.DM
	 auto value, i
.DE
are separate from the variables of the same name in the rest of the program.
.PP
If the function calls itself, as the \fBfib\fR example does
above, any variable names noted in the \fBauto\fR statement are
handled separately for each call of the function.
.SH "Programs in a File"
.PP
Because its programs can be quite complex, 
\fBbc\fR lets you keep them in files.
This lets you build a library of \fBbc\fR programs and 
functions that can be called up easily.
.Sh "Using a Program From a File"
.PP
To illustrate the use of programs stored in a file,
type the following example into file \fBfib.bc\fR using the
editor of your choice.
The program defines the function \fBfib\fR:
.DM
	define fib (f) {
		if (f == 0) return (0)
		if (f == 1) return (1)
		if (f > 1) return (fib (f - 1) + fib (f - 2))
	}
.DE
To use a \fBbc\fR program that has been stored in a file, enter
the file name on the \fBbc\fR command line, like this:
.DM
	bc fib.bc
.DE
The function definition will be read in by \fBbc\fR and ready for your use.
To use the function, simply type the function name with parameters.
.PP
So, if you type:
.DM
	bc fib.bc
	fib (6)
.DE
\fBbc\fR will reply:
.DM
	8
.DE
.Sh "Using Libraries"
.PP
You can enter several useful programs in their own files and call them
into \fBbc\fR at the same time.
The following example creates another function
that calculates the sum of the squares of integers up to a given number.
Use an editor to type the following into a file named \fBsumsq.bc\fR:
.DM
	define sumsq (number) {
		auto i, sum
		sum = 0
		for (i = number; i > 0; --i) sum += i ^ 2
		return (sum)
	}
.DE
Now, you can use the \fBsumsq\fR function to print the sum of
the squares for each number from one to ten:
.DM
	bc sumsq.bc
	for (i = 1; i <= 10; ++i) sumsq (i)
.DE
The result is:
.DM
	1
	5
	14
	30
	55
	91
	140
	204
	285
	385
	quit
.DE
You can use the two functions stored in a file to print the difference
between the sum of the squares of numbers, and the
Fibonacci number:
.DM
	bc fib.bc sumsq.bc
	for (i = 1; i <= 10; ++i) sumsq (i) - fib (i)
	quit
.DE
The result of this questionable computation is:
.DM
	0
	4
	12
	27
	50
	83
	127
	183
	251
	330
.DE
.Sh "The bc Library"
.II bc^library
.PP
\*(OS provides
an extended library to go with \fBbc\fR.
It includes the following functions:
.DS
	\fBatan\fP(\fIz\fP)	arctangent of \fIz\fP
	\fBcos\fP(\fIz\fP)	cosine of \fIz\fP
	\fBexp\fP(\fIz\fP)	exponential function of \fIz\fP
	\fBj\fP(\fIn,z\fP)	\fIn\fPth order Bessel function of \fIz\fP
	\fBln\fP(\fIz\fP)	natural logarithm of \fIz\fP
	\fBpi\fP	the value of pi to 100 digits
	\fBsin\fP(\fIz\fP)	sine of \fIz\fP
.DE
The library is stored in file
.if \nX=0 \fB/usr/lib/lib.b\fR.
.if \nX=1 \fB\eBIN\eLIB.B\fR.
To use the library, invoke the \fBbc\fR
command with the \fB-l\fR option.
.PP
To show how the library can be used in your work
the following example computes the sine of an angle of
one-third radian with scale set to 20:
.DM
	bc -l
	scale = 20
	sin (1/3)
	quit
.DE
The result is:
.DM
	.32719469679615224418
.DE
.SH Summary
.PP
The Lexicon entry for
.B bc
summarizes its commands, features, and libraries.
It will also refer you to related commands and functions.
