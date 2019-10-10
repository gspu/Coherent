.II "nroff^tutorial"
.ds PN \\fBnroff\\fR
.ds TL "nroff Text-Formatting Language"
.NH "nroff, The Text-Formatting Language" 1
.PP
.B nroff
is the \*(CO system's text-formatting language.
You write a file that mingles the text you want formatted with commands
to control the formatting.
\fBnroff\fR then uses the commands to format the text, and writes the formatted
text onto the standard-output device.
.PP
This tutorial describes how to work with
.B nroff.
It assumes you are familiar with the basic features of the \*(CO system.
In particular, you should know what a
.I command
is, what a
.I file
is, and how to create and edit a file.
If you are not familiar with these concepts, read
.I "Using the COHERENT System"
before you read this tutorial.
.PP
The Lexicon also contains a number of articles that relate to
.BR nroff .
In particular, you should read the article for
.BR printer ,
which describes how you can print text under the \*(CO system.
.Sh "What is nroff?"
.PP
\*(PN is the text processor for \*(CO.
A \fItext processor\fR is a utility that accepts commands and text,
and uses the commands to format the text on a page.
The commands may call for simple formatting, such as indenting
each new paragraph five spaces, to complex formatting of columns
and entire pages.
.PP
A file that contains text mixed with \*(PN commands is called a
\fIscript\fR.
For example, the following \*(PN script
.DM
.II "Belloc, Hilaire"
	.nr Z 0 5
	.nf
	I tire of love,
	.ti \en+Z
	I sometimes tire of rhyme;
	.ti \en-Z
	But money makes me happy
	.ti \en+Z
	All the time!
	.fi
.DE
produces the following printed text:
.DM
.ta 0.5i 1.0i 1.5i 2.0i
	I tire of love,
		I sometimes tire of rhyme;
	But money makes me happy
		All the time!
.DE
An \*(PN script allows you to change your output very easily.
For example, change the minus sign \*(Ql\fB\-\fR\*(Qr in line 7
of the \*(PN to a plus sign \*(Ql+\*(Qr, and the formatted
text suddenly becomes:
.DM
.ta 0.5i 1.0i 1.5i 2.0i
	I tire of love,
		I sometimes tire of rhyme;
			But money makes me happy
				All the time!
.DE
As you can see, \*(PN is a powerful and versatile formatter.
.PP
In truth, however, \*(PN is both a text formatter and a text
formatting \fIlanguage\fR.
With \*(PN, you can
write your own text-formatting commands to handle automatically
the unique requirements of whatever formatting you need.
.Sh "nroff Input and Output"
.PP
.I Input
is what you give to \*(PN.
.I Output
is what \*(PN returns to you.
If you simply type
.DM
	nroff
.DE
then
.B nroff
accepts input from your keyboard, and prints its output on your screen.
For example, if you want \*(PN to process the contents of a file named
.BR script.r ,
type the command line
.DM
	nroff script.r
.DE
\*(PN then takes the file \fBscript.r\fR, processes it,
and in a few moments it displays the formatted text on your screen.
Note that the suffix
.B .r
is used by convention to indicate that a file contains an unprocessed
.B nroff
script.
.PP
You can save \*(PN's output by \fIredirecting\fR it into another file.
For example, you can redirect \*(PN's processed
output of the file \fBscript.r\fR into the file named
.B target
by using the following command:
.DM
	nroff script.r > target
.DE
.Sh "Printing nroff Output"
.PP
The \*(CO system's implementation of \fBnroff\fR currently
can be used with any variety of printer.
\*(CO, however, fully supports three varieties of printer:
Epson-compatible dot-matrix printers, printers that use the Hewlett-Packard
Page Control Language (PCL) (including the Hewlett-Packard LaserJet and
DeskJet
families of printers), and any printer that has implemented the PostScript
page-control language.
The following descriptions assume that you have plugged your printer into
a parallel port on your computer, and have installed \*(CO correctly
so that it can access your printer.
.PP
To print \fBnroff\fR output on an Epson-compatible printer, use the
commands \fBepson\fR and \fBlpr\fR.
For example, to print the \fBnroff\fR output that you have directed into
file \fBtext.out\fR, use the following command:
.DM
	epson text.out | lpr
.DE
Or, you can pipe the output of \fBnroff\fR directly into \fBepson\fR,
as follows:
.DM
	nroff -ms text.r | epson | lpr
.DE
In the above example, \fBtext.r\fR is your input, and \fB\-ms\fR invokes
the \fBms\fR package of macros.
.PP
To print on a printer that uses PCL, use the commands \fBhp\fR and \fBhpr\fR.
For example, to print the file \fBtext.out\fR on a PCL printer,
use the command:
.DM
	hp text.out | hpr -B
.DE
The option \fB\-B\fR to \fBhpr\fR suppresses 
the printing of a banner page.
If you wish, you can pipe the output of \fBnroff\fR directly into \fBhp\fR,
as follows:
.DM
	nroff -ms text.r | hp | hpr -B
.DE
To access a printer that uses PostScript, use the command \fBhpr\fR, but do
not use the command \fBhp\fR.
Also, you use must the \fB\-p\fR switch to \fBnroff\fR, which tells it to
generate PostScript output.
For example, the following command processes file \fBtext.r\fR into
PostScript output, and passes that output to a PostScript printer:
.DM
	nroff -p -ms text.r | hpr -B
.DE
All of the above commands are described in their respective entries in the
Lexicon.
.PP
You can also print the output of
.B nroff
through the
.B lp
spooler.
For information on that spooler, see its entry in the Lexicon.
For a summary of how the \*(CO system manages printers, see the Lexicon
entry for
.BR printer .
.Sh "nroff Limitations"
.PP
Because \fBnroff\fR is a text-formatting language rather than a text-formatter
\fIper se,\fR it makes no assumptions about how you want to lay out your page.
It does not automatically leave margins at the top and bottom of pages;
it does not automatically number pages;
it does not automatically format paragraphs.
You must use or create a set of formatting commands, called
.I macros,
to generate these features.
This tutorial will teach you how to write macros that can solve
nearly every conceivable formatting problem.
As you have seen, too, your copy of \*(CO
comes with a set of predefined macros, the \fB\-ms\fR macro package.
.Sh "The ms Macro Package"
.PP
.II "nroff^ms macros"
A macro package called
.B \-ms
is included with your copy of \*(PN.
It provides macros to format paragraphs, produce headers
and footers (the areas at the top and bottom of pages, respectively),
and perform most other page-formatting tasks.
\fB\-ms\fR is easy to use.
The command
.DM
	nroff \-ms
.DE
tells \*(PN to accept input from your keyboard, process it using the \fB\-ms\fR
macro package, and print the output on your screen.
The command
.DM
	nroff \-ms script.r
.DE
tells \*(PN to process
.B script.r
with the \fB\-ms\fR package and print the output on your terminal;
while the command
.DM
	nroff \-ms script.r >target
.DE
redirects the output of
.B nroff
into the file
.BR target ;
and
.DM
	nroff -ms script.r | lpr
.DE
prints the output on the line printer.
.PP
.II "nroff^macro definition"
Working with the \fB\-ms\fR macro package is
a good way to gain confidence in working with
.B nroff
commands.
Soon you will learn the correct way to encode \*(PN commands in your scripts.
.Sh "Using this Tutorial"
.PP
The only way to learn about
.B nroff
is to use it.
You should type all the examples in this tutorial into your computer and
observe how they work.
You should also alter the example and examine how
your changes affect what \*(PN produces.
Don't hesitate to experiment!
You can learn more from analyzing why something unexpected happens
than you can from simply copying an example that
works as you were told it would.
.PP
The first section describes how to use
.B nroff
with the
.B \-ms
macro package.
The second section describes how to perform sophisticated formatting.
For most users, this chapter contains all the information they need to know.
.PP
The rest of the tutorial describes how
.B nroff
actually works with the input text to produce its output.
This will teach you how to write your own \*(PN
macros for your special word processing needs.
.SH "The ms Macro Package"
.PP
As explained above,
.B nroff
is the text formatter for \*(CO.
You give \*(PN a \fIscript\fR \(em that is,
text interspersed with commands that control its processing;
\*(PN, in turn, formats your text in the manner dictated by your commands.
.PP
\*(PN's most outstanding feature is its flexibility:
you can control line length, page offset, page length, paragraph format,
beginning-  and end-of-page format,
and every other aspect of formatting a document.
.PP
.II "nroff^.PP macro"
\*(PN has built into it a set of basic commands,
called \fIprimitives\fR, that are used to control formatting.
A basic formatting function might require several primitives.
For example, formatting a new paragraph requires one
primitive to force the printing of the fragment
of a line left at the end of the previous paragraph;
another primitive to skip a blank line;
and a third primitive to indent the first line of the new paragraph.
If you were to type directly into your script all the primitives required to
control every feature of your document, formatting
would be a very difficult task, and mistakes would be common.
.PP
Fortunately, another feature of
.B nroff
makes it easier for you to prepare input:
\*(PN allows you to bundle together a group of primitives
and give the bundle its own name.
Such a bundle is called a
.IR macro .
Whenever you want all the commands in that bundle to be executed,
you simply insert the name of the macro into the text.
For example, you might group the primitives needed
to format a paragraph, and call that bundle \fBPP\fR.
Then, instead of retyping the primitives,
all you need to do is insert the command
.B .PP
before the start of a paragraph.
.PP
\fB\-ms\fR is a package of macros that are ready for you to use.
When you include the option \fB\-ms\fR on the \*(PN command line,
.B nroff
automatically uses the
the macros that have been defined in the \fB\-ms\fR package.
These macros will take
care of setting line length and page length, numbering pages,
formatting paragraphs, and all other formatting tasks.
You do not need to know how \*(PN's primitives
are used in the macros; you only need to know the names of the macros
and what they do, so that you can insert them correctly into your text.
.PP
Using the \fB\-ms\fR
package is a good way to become accustomed to preparing input for
.B nroff,
so that the features of the primitives will not seem so
alien when you eventually choose to work with them.
When you become familiar with \*(PN, you may wish to your own macro packages,
to handle the unique requirements of different types of documents.
For now, however, you will find that the \fB\-ms\fR package
will get you up and running with \*(PN.
.Sh "Text and Commands"
.PP
.B nroff
input includes both
.I text
and
.I commands.
The commands control the processing of the text.
.B nroff
distinguishes between text and commands by looking at the
first character of each input line.
If that character is a period or an apostrophe, the line is a \fIcommand\fR;
otherwise, it is \fItext\fR.
.PP
Earlier in this tutorial, you used the \fB\-ms\fR
package to format a text file that had already been prepared for you.
To become more accustomed to using
.B nroff,
try entering the following text into a file that can be
formatted later.
Use a text editor (either \fBed\fR or \*(ME) to create a file named
.B script2.r
that contains the following text.
It is important for this exercise that you
break up the lines as they are shown here:
.DM
.II "Dickens, Charles"
	London.  Michaelmas Term lately over,
	and the Lord Chancellor sitting in
	Lincoln's Inn Hall.  Implacable November weather.
	As much mud in the streets, as if the waters
	had but newly retired from the face of the
	earth, and it would not be wonderful to meet
	a Megalosaurus, forty feet long or so, waddling
	like an elephantine lizard up Holborn Hill.
.DE
Note that this file contains no commands;
every line is a text line.
Process the file with the command:
.DM
	nroff script2.r | more
.DE
The output is piped to
.B more
so that it will not all rush past your screen.
\*(PN will process the text, and
in a moment you will see the following:
.EX
London.  Michaelmas Term lately over,
and the Lord Chancellor sitting in
Lincoln's Inn Hall.  Implacable November weather.
As much mud in the streets, as if the waters
had but newly retired from the face of the
earth, and it would not be wonderful to meet
a Megalosaurus, forty feet long or so, waddling
like an elephantine lizard up Holborn Hill.
.EE
.II "nroff^justify text"
When you see this example, the spacing will be
different;
the spacing for the examples in this tutorial is adjusted to conform to
the rest of the tutorial text.
Notice that
.B nroff
automatically adjusts the spacing between words to
justify the right margin, even though the input
text has a ragged right margin.
Each output line contains 65 characters,
and each output page contains 66 lines.
.PP
Now try processing
.B script.r
again, this time with the
.B \-ms
macro package.
Type
.DM
	nroff \-ms script.r | more
.DE
.II "nroff^margin, right"
As you can see,
.B nroff
again adjusted the spacing to keep a strict right margin.
Each line was indented with ten leading spaces, followed by
65 characters of text.
The pages output by both the \fBnroff\fR command
and the \fBnroff \-ms\fR command both contain 66 lines, but
the page built with the \fB\-ms\fR package
left blank lines at the top of the page and
printed the page number in a blank space at the bottom of the page.
When \*(PN constructs its output, it assumes that your printer prints
ten characters per inch (Pica, or 10-pitch spacing) and six lines per inch.
Given these assumptions, each page of output from
.B "nroff \-ms"
fits onto an 8.5 by 11 inch page, with an inch of blank
space at the top, at the bottom, and on each side.
.PP
As this example shows,
.B nroff
adjusts the spacing between words to keep a strict right margin.
When you type in the text, don't worry about the right margin.
You must, however, keep a strict left margin, because when
.B nroff
encounters a line of text that begins with blank spaces,
it breaks the line it was working on and begins a new, indented line.
.II "nroff^breaking line"
.PP
.II "nroff^hyphenation"
Also, do not hyphenate words; if you do,
.B nroff
treats each part as a separate \*(QLword\*(QR
(the first ending with the hyphen character),
rather than keeping them joined, as you want.
.PP
.B nroff
normally interprets as a command every line
that begins with a \fIperiod\fR or an \fIapostrophe\fR.
However, to include an initial apostrophe or period as a literal part
of your document, you must place
the characters \fB\e&\fR before the period or apostrophe.
.PP
The remainder of this will show you how to use commands in input text
to change the appearance of the output.
You can control many aspects of the printed document
simply by including the appropriate commands within your text.
.PP
.II "nroff^.sp primitive"
.II "nroff^space, vertical"
.Sh "Command Names"
.PP
The name of every
.B nroff
primitive consists of two lower-case letters.
Some commands can also include additional information, or \fIarguments\fR.
For example,
.B .sp
is the command to leave vertical space between output lines.
The command line
.DM
	.sp
.DE
leaves one space, whereas
.DM
	.sp 2
.DE
leaves two spaces.
.II "nroff^command, argument"
The information that follows the command name on the command line is an
argument.
.II "nroff^macro, name"
Each macro defined in the
.B \-ms
macro package is named with one or two upper-case letters.
For example,
.B .PP
is the name of the macro that begins a new paragraph.
.Sh "Paragraphs"
.PP
.II "nroff^paragraph"
.II "nroff^.PP macro"
Every time you want to begin a new paragraph, enter the
.I paragraph
command
.B .PP;
that is, place the command line
.B .PP
in the text.
To test this macro, enter the following text under the name \fBscript3.r\fR:
.DM
.II "Austen, Jane"
	.PP
	It is a truth universally acknowledged,
	that a single man in possession of a good fortune,
	must be in want of a wife.
	.PP
	However little known the feelings or views of such
	a man may be on first entering a neighbourhood, the
	truth is so well fixed in the minds of the surrounding
	families, that he is considered as the rightful
	property of some one or the other of their daughters.
.DE
When you process this text with
the command
.DM
	nroff \-ms script3.r | more
.DE
the result resembles the following:
.EX
.ti 1.3i
It is a truth universally acknowledged,
that a single man in possession of a good fortune,
must be in want of a wife.
.sp
.ti 1.3i
However little known the feelings or views of such
a man may be on first entering a neighbourhood, the
truth is so well fixed in the minds of the surrounding
families, that he is considered as the rightful
property of some one or the other of their daughters.
.EE
As the output shows, the
.B .PP
command inserts a blank line before beginning a new
paragraph, and indents the first line of the new paragraph
by half an inch.
.PP
.II "nroff^.IP macro"
.II "nroff^paragraph, indented"
The
.B \-ms
package also provides another paragraph format:
the
.B .IP
command.
This macro creates an
.I "indented paragraph."
The
.B .PP
macro indents only the first line of each paragraph;
however,
.B .IP
indents every line \fIexcept\fR the first.
For example,
.DM
	.IP
	This is an indented paragraph.
	All the lines are indented by
	the same amount.
	.PP
	This is a normal paragraph.
	nroff indents the first line
	but does not indent the following lines.
.DE
gives the output
.EX
.in +0.5i
This is an indented paragraph.
All the lines are indented by
the same amount.
.in -0.5i
.sp
.ti +0.5i
This is a normal paragraph.
nroff indents the first line
but does not indent the following lines.
.EE
.II "nroff^tag on paragraph"
.II "nroff^paragraph tag"
Several options are available for the basic
.B .IP
macro.
You can add two
.I arguments
to it.
.B nroff
interprets the first argument after the
.B .IP
as a
.I tag
to the paragraph, and it interprets the second argument as the amount
of indentation you want.
For example,
.DM
	.IP A. 8
	This is the first line of text.
	nroff indents the following lines by the same
	amount as the first.
	The indent is eight spaces.
	The paragraph includes a tag in the indent.
.DE
produces
.EX
.IP \fLA.\fL 0.5i
This is the first line of text.
nroff indents the following lines by the same
amount as the first.
The indent is eight spaces.
The paragraph includes a tag in the indent.
.EE
.PP
You must make sure the indent leaves enough spaces for the tag.
If the tag contains blank spaces, enclose it within quotation marks.
To see how this works, enter the following script under the title
\fBscript4.r\fR:
.II "King Lear"
.II "Shakespeare, William"
.DM
	.IP "King Lear:" 16
	Is man no more than this?
	Consider him well.
	Thou owest the worm no silk,
	the beast no hide,
	the sheep no wool,
	the cat no perfume...
	Unaccommodated man is no more
	but such a poor, bare, forked
	animal as thou art.
.DE
When processed with
the command
.DM
	nroff \-ms script4.r >script4.p
.DE
you see:
.EX
.IP "\fLKing Lear:\fL" 1.0i
Is man no more than this?
Consider him well.
Thou owest the worm no silk,
the beast no hide,
the sheep no wool,
the cat no perfume...
Unaccommodated man is no more
but such a poor, bare, forked
animal as thou art.\fR
.EE
.PP
As this example shows, this form of the
.B .IP
macro can be used to format the script for a play.
.PP
If you do not want a tag, but merely wish to set the indentation to
something other than the default setting of five spaces, then use
a pair of quotation marks with nothing between them for the first field:
.DM
	.IP "" 8
.DE
If you forget the quotation marks, you will not get what you expect:
.B nroff
will interpret \*(Ql8\*(Qr as a tag and use the normal indentation
of five spaces.
.PP
Once you set the amount of indentation, the new indentation
stays in effect until you change it again.
For example, if you format a paragraph with
.DM
	.IP "" 8
.DE
and follow it with another paragraph that begins with \fB.IP\fR,
.B nroff
will also indent the second paragraph by eight spaces.
The indentation will remain in effect until you explicitly change it \(em for
example, by beginning a paragraph with
.DM
	.IP "" 6
.DE
which resets the indent to six spaces.
.PP
.II "nroff^.RS macro"
.II "nroff^.RE macro"
.II "nroff^relative indent"
.II "nroff^indentation, relative"
Normally,
.B nroff
measures the paragraph indentation from the left margin.
Another variation of
.B IP
allows you to measure the indentation of a new indented paragraph from
the left-hand edge of a previous indented paragraph, thus producing
.IR "relative indentation" .
To do this, enclose the new paragraph between the macros
.B RS
and
.B RE
(for \fBr\fRelative indent \fBs\fRtart and \fBr\fRelative indent \fBe\fRnd).
Copy the following script into the file \fBscript5.r\fR:
.II "Bathsheba"
.II "King David"
.II "Uriah the Hittite"
.DM
	.IP
	And it came to pass in an eveningtide,
	that David arose from off his bed ...
	and from the roof he saw a woman washing
	herself; and the woman was very beautiful
	to look upon.  And David sent and enquired
	after the woman.  And one said,
	.RS
	.IP
	Is not this Bathsheba, the daughter of Eliam,
	the wife of Uriah the Hittite?
	.RE
	.IP
	And David sent messengers and took her; and
	she came in unto him, and he ...
	and she returned unto her house.
.DE
When processed through \*(PN with
the command
.DM
	nroff \-ms script5.r >script5.p
.DE
the output resembles the following:
.EX
And it came to pass in an eveningtide,
that David arose from off his bed ...
and from the roof he saw a woman washing
herself; and the woman was very beautiful
to look upon.  And David sent and enquired
after the woman.  And one said,
.sp
.in +0.5i
Is not this Bathsheba, the daughter of Eliam,
the wife of Uriah the Hittite?
.sp
.in -0.5i
And David sent messengers and took her; and
she came in unto him, and he ...
and she returned unto her house.\fR
.EE
You can include
any number of indented paragraphs between \fB.RS\fR
and \fB.RE\fR.
Also, you can specify tags and different indents just as for
ordinary indented paragraphs.
You can even nest
.B .RS
and
.B .RE
pairs inside each other to produce multiple relative indents.
Just remember that an
.B .RS
must always be balanced by an
.BR .RE .
Type the following into the file \fBscript6.r\fR to see
how \*(PN handles nested flashbacks:
.DM
	.IP
	In England during World War II, a captain tells the
	story of his Free French bomber squadron.
	.RS
	.IP
	In the early days of the war, a French ship picks up
	five men adrift in a small boat.  One tells of their
	life on Devil's Island.
	.RS
	.IP
	A convict tells others of his past.
	.RS
	.IP
	Publication of anti-Nazi material leads to arrest on
	false charges.
	.RE
	.IP
	The convicts escape to help France in the war.
	.RE
	.IP
	When France surrenders, the crew overpowers pro-Vichy
	officers and heads for England instead of Marseilles.
	.RE
	.IP
	The captain concludes his story as the bombers return
	from a mission.
.DE
When you process this file with the \fB\-ms\fR package, the output
file \fBscript6.p\fR should
resemble the following:
.EX
In England during World War II, a captain tells the
story of his Free French bomber squadron.
.sp
.in +0.5i
In the early days of the war, a French ship picks up
five men adrift in a small boat.  One tells of their
life on Devil's Island.
.sp
.in +0.5i
A convict tells others of his past.
.sp
.in +0.5i
Publication of anti-Nazi material leads to arrest on
false charges.
.sp
.in -0.5i
The convicts escape to help France in the war.
.sp
.in -0.5i
When France surrenders, the crew overpowers pro-Vichy
officers and heads for England instead of Marseilles.
.sp
.in -0.5i
The captain concludes his story as the bombers return
from a mission.
.EE
.PP
As you can see, each \fB.RE\fR command peels away the current
layer of indentation and moves you into the previous one.
To return to an even earlier level, you must input
the appropriate number of \fB.RE\fR commands before you begin
a paragraph.
.PP
.II "nroff^quoted paragraph"
.II "nroff^paragraph, quoted"
.II "nroff^.QE macro"
.II "nroff^.QS macro"
A third type of paragraph is the \fIquoted\fR paragraph.
It produces a paragraph that is
indented on both on the right side and on the left side,
in order to set off a quotation from the surrounding text.
To produce such a paragraph, precede it with the
.B .QS
macro
and follow it with the
.B .QE
macro.
To break the quotation into different sections,
insert a blank line in the text before each line that you
want to begin a new section.
For example, type the following example as \fBscript7.r\fR:
.DM
.II "Twain, Mark"
	Form of Tender of Rescue from Strange Young Gentleman
	to Strange Young Lady at a Fire.
	.QS
	Although through the fiat of a cruel fate, I have been
	debarred the gracious privilege of your acquaintance,
	permit me, Miss [here insert name, if known], the
	inestimable honor of offering you the aid of a true
	and loyal arm against the fiery doom which now
	o'ershadows you with its crimson wing.  [This form
	to be memorized, and practiced in private.]
	.QE
	Should she accept, the young gentleman should offer
	his arm - bowing, and observing "Permit me" -
	and so escort her to the fire escape and deposit
	her in it.
.DE
After processing with the \fB\-ms\fR package, the output file
\fBscript7.p\fR should resemble the following:
.EX
Form of Tender of Rescue from Strange Young Gentleman
to Strange Young Lady at a Fire.
.sp
.in +0.5i
.ll -0.5i
Although through the fiat of a cruel fate, I have been
debarred the gracious privilege of your acquaintance,
permit me, Miss [here insert name, if known], the
inestimable honor of offering you the aid of a true
and loyal arm against the fiery doom which now
o'ershadows you with its crimson wing.  [This form
to be memorized, and practiced in private.]
.sp
.in -0.5i
.ll +0.5i
Should she accept, the young gentleman should offer
his arm - bowing, and observing "Permit me" -
and so escort her to the fire escape and deposit
her in it.
.EE
.Sh "Section Headings"
.PP
.II "nroff^.SH macro"
.II "nroff^section heading"
.II "nroff^headings, section"
The
.I "section heading"
macro
.B .SH
prints a heading or title.
For example:
.DM
	.SH
	Section Headings
.DE
The heading may be more than one line long;
consequently, you should follow a section heading with a
.B .PP
or an
.B .IP
macro.
.B nroff
leaves a blank line before the heading and prints the heading
flush with the left margin in
.B boldface
type, as described below in the section on Fonts.
.PP
.II "nroff^.NH macro"
.II "nroff^numbered heading"
The
.I "numbered heading"
macro
.B .NH
produces consecutively numbered section headings.
For example:
.DM
	.NH
	Guess What's Coming to Dinner?
	.NH
	Guess Why I Won't be There?
.DE
produces
.DM
	1.   Guess What's Coming to Dinner?
.sp
	2.   Guess Why I Won't Be There?
.DE
You can number subsection headings by entering a number from two
to five to the
.B .NH
macro.
The number indicates the level
of section headings; for example,
.B ".NH 2"
numbers subsection headings,
.B ".NH 3"
numbers sub\-subsection headings.
For example:
.DM
	.NH
	Guess What's Coming to Dinner?
	.NH 2
	Guess What it Looks Like?
	.NH 3
	Teeth Like That Might Frighten the Children!
	.NH 2
	What Does it Eat?
	.NH
	Guess Why I Won't be There?
.DE
produces:
.DM
	1.   Guess What's Coming to Dinner?
.sp \n(pDu
	1.1   Guess What it Looks Like?
.sp \n(pDu
	1.1.1   Teeth Like That Might Frighten the Children!
.sp \n(pDu
	1.2   What Does it Eat?
.sp \n(pDu
	2.   Guess Why I Won't be There?
.DE
The number on the
.B .NH
command line is
.I not
the number that appears before the heading; instead, it
controls how many \*(QLparts\*(QR appear in the number.
For example,
.B ".NH 3"
produces a three-part number, such as \fB2.5.3\fR,
whereas
.B ".NH 4"
produces a four-part number, such as \fB7.4.5.2\fR.
.PP
You can reset the entire numbering scheme by using the command
\fBNH 0\fR; for example,
.DM
	.NH 0
	Through The Mandelbrot Set With Rod and Gun
.DE
produces
.DM
	1.   Through The Mandelbrot Set With Rod and Gun
.DE
with numbering starting at one.
.Sh "Title Page"
.PP
.II "nroff^.TL macro"
.II "nroff^.AU macro"
.II "nroff^.AB macro"
.II "nroff^.AE macro"
.II "nroff^.AI macro"
If you want your output to begin with a title page, begin the input
with the following.
.DM
	.TL
	Title of Document (may be more than one line)
	.AU
	Name(s) of Author(s) (may be more than one line)
	.AI
	Institution(s) of Author(s)
	.AB
	Abstract (line length 5.5 inches)
	.AE
.DE
.PP
The
.B .TL
macro indicates the
.I title,
the
.B .AU
macro indicates the
.I author,
the
.B .AI
macro indicates the
.I "author's institution,"
and the
.B .AB
macro precedes the
.IR abstract .
The
.B .AE
macro, for \fBa\fRbstract \fBe\fRnd, marks the end of the abstract.
If you do not want some of these headings to appear,
simply omit the relevant macros.
Begin the body of the document immediately after the
.B .AE
macro.
The body must begin with a formatting command, such as
.B .PP
or
.B .SH.
.PP
Note that the \fIend abstract\fR macro \fB.AE\fR also
prints today's date automatically.
To do so, \*(PN reads the date as encoded in \*(CO.
Before you use these macros, be sure that you have set
the correct date in \*(CO.
.PP
To see how these macros work, type the following script
into file \fBscript8.r\fR:
.ID
	.TL
	Tickling in the Therapy of
	von Muenchausen's Syndrome
	.AU
	P. R. Sanserif
	.AI
	The Department of Parapsychology
	The University of Southern North Dakota
	at Hoople
	.AB
	Study of 150 subjects (75 men and 76 women)
	indicated that hard tickling may prove beneficial
	to patients with von Muenchausen's syndrome.
	Applications for a seven-figure grant have been
	made to continue research in this area.
	.AE
	.PP
	Due to complications in our experiment, this paper
	has now been withdrawn.
.IE
After processing with the \fB\-ms\fR macro package,
you will see that in the outputfile \fBscript8.p\fR, \*(PN placed the
text on the same page as the title information.
You may or may not want this to happen.
If you do not, one solution is to insert two
additional commands between the
.B .AE
macro and the body of your text:
.DM
	.PP
	.bp
.DE
.II "nroff^.bp primitive"
.Sh "Headers and Footers"
.PP
.II "nroff^header"
.II "nroff^footer"
The
.I header
macro controls the format of the top of each page.
It automatically skips one inch at the top of the page.
The
.I footer
macro controls the format of the bottom of each page.
It stops printing text one inch above the bottom of the page,
and prints the page number.
.PP
.II "nroff^title"
.II "nroff^LT string"
.II "nroff^CT string"
.II "nroff^RT string"
It is easy to print either a page header or a page footer.
Both the page header and the page footer are three-part titles:
.B nroff
prints the first part on the left side of the page, the second part
in the middle, and the third part on the right side of the page.
The parts of the header title are named:
.DS
	\fBLT\fR:  left, top
	\fBCT\fR:  center, top
	\fBRT\fR:  right, top
.DE
and the parts of the footer title are named:
.DS
	\fBLF\fR:  left, footer
	\fBCF\fR:  center, footer
	\fBRF\fR:  right, footer
.DE
.II "nroff^strings"
.II "nroff^.ds primitive"
These parts are called
.I strings.
A later section of this tutorial describes strings in detail.
Normally, these strings are undefined,
except for \fBCF\fR, which prints the current page number;
therefore, the header macro normally prints nothing, and the footer
macro prints only the page number in the center of the block of space at the
bottom of each page.
However, you can set any portion of the
header or footer to print what you like.
To set the left portion of the header, for example, type
the following:
.DM
	.ds LT "Walnuts in History"
.DE
Note that you do \fInot\fR type a period before the \fBLT\fR.
After you define
.B LT
in this fashion,
.B nroff
will print
.DM
	Walnuts in History
.DE
at the top of each page on the left-hand side.
If you want the date to appear on the right-hand side
of the header, type:
.DM
	.ds RT "\e*(Ds"
.DE
The string \fBDs\fR is automatically initialized to today's date,
as set on your \*(CO system.
A later section of this tutorial will present strings in detail.
For now, all you need to know is that whenever you want \*(PN to insert today's
date into your script automatically, just type the entry \fB\e*(Ds\fR.
This entry does \fInot\fR have to be at the beginning of a line to work.
.PP
Use the same procedure to define the strings in the footer title.
If you want something
other than the page number to appear in the position allocated to \fBCF\fR,
use the
.B .ds
primitive to redefine
.BR CF .
If you want nothing to appear there, type
.DM
	.ds CF ""
.DE
.II "nroff^%, page number"
.II "nroff^page number"
Wherever you want the current page number to appear in the header or
footer, use the symbol \*(Ql%\*(Qr.
For example,
if you want the page number to appear in the upper right-hand corner of each
page, type
.DM
	.ds RT "Page %"
.DE
Be sure to type in all of the macros to define headers and footers
\fIbefore\fR you begin to type in your text.
Otherwise, your headers and footers will not appear on the first
page of the formatted output.
.PP
To see how this works, try editing the file \fBscript1.r\fR.
At the top, insert the macro
.DM
	.ds RT "\e*(Ds"
.DE
and reprocess the file using the \fB\-ms\fR macro package.
Each output page should have today's date written in the
upper right-hand corner.
.Sh "Fonts"
.PP
.II "nroff^fonts"
.II "nroff^boldface"
.II "nroff^italic"
.II "nroff^Roman"
.B nroff
normally prints ordinary,
or \*(QLRoman\*(QR, characters.
In addition,
.B nroff
can print
.B boldface
and
.I italic
characters.
Each of the three styles of type \(em
Roman, boldface, and italic \(em is called a
.IR font ,
in keeping with typesetting terminology.
.PP
.B nroff
prints each boldface and italic character by generating
a special three-character output sequence.
It prints the boldface character
.BR c ,
for example,
by printing a \*(Qlc\*(Qr, then the backspace character
.BR <ctrl-H> ,
and then another \*(Qlc\*(Qr.
This sequence emaphasizes \*(Qlc\*(Qr by forcing your
printer to print it twice.
.B nroff
represents an italic character
.I c
with the underscore character \*(Ql_\*(Qr,
followed by the backspace character
.B <ctrl-H>,
followed by \*(Qlc\*(Qr.
.PP
Because of these special representations, the appearance of
.B nroff
boldface and italic fonts depends on the device on which you see the output.
On your terminal, the
.B <ctrl-H>
backspaces the cursor, and the third character of each
sequence replaces the first;
therefore, boldface and italic characters appear the same as Roman characters.
On a printer, the appearance depends on the characteristics of the printer.
The \*(CO system provides programs to print boldface and italic characters
appropriately on certain devices.
.PP
The
.B \-ms
macro package includes three commands for easy printing in specific fonts:
the
.I boldface
command
.BR .B ,
the
.I italic
command
.BR .I ,
and the
.I Roman
command
.BR .R .
To print a single word in boldface, do the following:
.DM
	The last word is printed in
	.B boldface.
.DE
Likewise for italics:
.DM
	The last word is printed in
	.I italics.
.DE
These example printed a word in a different font.
You can print several words in a different font by
enclosing the words within quotation marks on the command line:
.DM
	This sentence ends with
	.B "three bold words".
.DE
You can also switch fonts by using one of the font commands
with nothing after it on the command line.
For example,
.DM
	.B
	This entire sentence is printed in boldface.
	.R
.DE
or
.DM
	.I
	This entire sentence is printed in italics.
	.R
.DE
In these examples, the Roman font command
.B .R
is needed to return to the
normal font after completing the boldface or italic text.
.PP
On rare occasions, you might want different parts of one word
to be in different fonts.
You cannot use the
.B \-ms
macros to produce mixed-font words directly.
A later section of this tutorial gives additional information about
.B nroff
fonts.
As explained there, the input
.DM
	This manual describes \efBnroff\efR's powerful features.
.DE
produces the output:
.DM
	This manual describes \fBnroff\fP's powerful features.
.DE
The word
.B nroff
is boldface but the following apostrophe and \*(Qls\*(Qr are Roman.
.Sh "Special Characters"
.PP
.II "nroff^characters, special"
A few characters have special meaning to
.B nroff.
You should be aware of these characters if you want
.B nroff
to process your text properly.
.PP
As mentioned earlier, the period and the apostrophe introduce
.B nroff
command lines.
Each is a special character if it is the \fIfirst non-space
character\fR on an input line.
If you wish to use a period or an apostrophe at the start of an input line
simply as part of your text,
you must precede it with a backslash and ampersand \*(QL\fB\e&\fR\*(QR.
For example, the input
.DM
	The footnote command
	.DS
	\e&.FT
	.DE
	generates footnotes for you automatically.
.DE
produces the output
.DM
.ta 0.5i
	The footnote command
.sp \n(pDu
	.FT
.sp \n(pDu
	generates footnotes for you automatically.
.DE
Neither the period nor the apostrophe is a special character unless it is
the first non-space character on a line.
.PP
The most important special character for
.B nroff
is the backslash \*(Ql\e\*(Qr.
It changes the meaning of the following character or characters.
If you simply want a backslash to appear as part of your text,
you must follow it with the letter \*(Qle\*(Qr;
that is, use \*(QL\ee\*(QR in your input to
have \*(Ql\e\*(Qr appear in your output.
Later sections of this tutorial describe other special uses for backslash.
.Sh "Footnotes"
.PP
.II "nroff^footnote"
.II "nroff^.FS macro"
.II "nroff^.FE macro"
You can place footnotes between the
.I "footnote start"
command
.B .FS
and
the
.I "footnote end"
command
.B .FE,
as in the following example:
.II "MicroKVETCH Electronic Nag"
.DM
	.FS
	*MicroKVETCH Electronic Nag is a
	copyrighted trademark of Caveat Emptor
	Software, Inc.
	.FE
.DE
You should insert
each footnote into your text where the
reference to it occurs;
.B nroff
will see to it that the footnote
appears at the bottom of the correct page.
Footnotes should be inserted as follows:
.DM
	The notion that we have been visited
	by visitors from outer space may seem
	outlandish(1)
	.FS
	1. Raucus J, O'Hooligan R: "Viruses
	from Venus?" \efIJ Earth Med Assoc\efR,
	1985;36:412-414.
	.FE
	but reason compels us to exclude no ...
.DE
The journal article cited in the footnote will
appear at the bottom of the page, with the journal name in italics.
.Sh "Displays and Keeps"
.PP
.II "nroff^display"
.II "nroff^keep"
A
.I display
is a portion of text, such as a graph or a table, that should appear in the
output exactly as it is typed in the input.
.B nroff
normally alters the spacings between elements
in your text, which, of course, would destroy the appearance
of a display.
Therefore,
.B nroff
has macros to tell it that a portion of text is a display,
and so not to alter spacings between elements or split it between two pages.
.II "nroff^.DS macro"
.II "nroff^.DE macro"
These macros are the
.I "display start"
macro
.B .DS
and the
.I "display end"
macro
.BR .DE .
You should place your display between these macros, as follows:
.DM
	.DS
	The text of the display goes here,
	exactly
	as
	you
	want
	it
	to appear in the output.
	.DE
.DE
The
.B .DS
macro comes in three varieties.
.II "nroff^centered display"
The
.I "display start centered"
macro 
.B ".DS C"
centers every line of your display.
Because
.B nroff
centers each line individually, both right and left margins are ragged.
.II "nroff^display, block-centered"
.II "nroff^block-centered display"
The
.I "display start block-centered"
macro
.B ".DS B"
takes the entire display at once and centers it.
You can think of this as simply shifting the
display to the right by an appropriate amount.
.II "nroff^display indented"
.II "nroff^indented, display"
The
.I "display start indented"
macro 
.B ".DS I"
indents the entire display by half an inch.
.PP
If your display is longer than one page, do not use
.B .DS
or any of its variants.
Instead, begin the display with one of the following.
.PP
.II "nroff^.CD macro"
.II "nroff^centered display"
.II "nroff^display, centered"
The
.I "centered display"
macro
.B ".CD"
centers each line of the display.
.II "nroff^.BD macro"
.II "nroff^block-centered display"
.II "nroff^display, block-centered"
The
.I "block-centered display"
macro
.B ".BD"
considers the entire display as a block and centers it.
.II "nroff^.LD macro"
.II "nroff^display, left"
.II "nroff^left display"
The
.I "left display"
macro
.B ".LD"
performs no indenting or centering, but simply begins each line
at the left margin.
.II "nroff^.ID macro"
.II "nroff^indented display"
.II "nroff^display, indented"
Finally, the
.I "indented display"
macro
.B ".ID"
indents each line by half an inch.
If you begin the display with one of these macros, do \fInot\fR end it
with
.BR .DE ;
rather, just type
.B .PP
or
.B .SH
or whatever other
macro is needed at that point.
.PP
To see how displays work, type the following into the file
\fBscript9.r\fR and process it with the \fB\-ms\fR macro package:
.DM
.II "Blake, William"
.II "Burma Shave"
	.PP
	.DS C
	Tyger! Tyger! burning bright
	In the forests of the night,
	What immortal hand or eye
	Could frame thy fearful symmetry?
	Burma Shave
	.DE
.DE
When the output file \fBscript9.p\fR is read, the results will
appear as follows:
.EX
.ce 5
Tyger! Tyger! burning bright
In the forests of the night,
What immortal hand or eye
Could frame thy fearful symmetry?
Burma Shave
.EE
You must remember one important fact when you use display macros:
the normal length of output lines is 6.5 inches, but if the
display contains lines longer than this
.B nroff
simply prints them as they are.
If a line is too long to fit onto the page,
what occurs afterwards depends upon the output device.
If you are displaying the output on the screen, the text will
be displayed as far as possible to the right, then the remainder
will be wrapped around onto the next line, without indentation.
On most printers, however, the chunk of text that extends past
the right margin will simply be lopped off and thrown away.
In any event, the effect is usually quite unsightly.
The only restriction on what you can safely put in a display,
then, is that lines should be no longer than 6.5 inches.
If you are using an indented display,
lines should be no longer than six inches.
.PP
.II "nroff^.KE macro"
.II "nroff^.KS macro"
.II "nroff^keep"
A
.I keep
is a
display macro:
you put text between the
.I "keep start"
macro
.B .KS
and the
.I "keep end"
macro
.B .KE
when you want it all kept on the same page.
If you put a block of text between these macros that
proves to be longer than one page,
.B nroff
moves the excess text onto a new page.
.PP
The major difference between the keep and the display is that normal
processing occurs in the keep:
.B nroff
adjusts spacings between words, hyphenates words,
justifies lines, and performs all other formatting tasks,
just as it normally does.
.Sh "Other Commands"
.PP
.II "nroff^.bp primitive"
.II "nroff^.sp primitive"
.II "nroff^skip lines"
.II "nroff^begin page"
.II "nroff^new page"
.II "nroff^page, break"
Several of
.BR nroff 's
primitives can be used with the
.B \-ms
macro package.
The primitive
.DM
	.sp \fIN\fP
.DE
skips
.I N
lines on the output page; for example,
.B ".sp 4"
skips four lines.
.PP
The
.I "begin page"
primitive
.B .bp
tells
.B nroff
to begin a new page, no matter where it is on the current page.
.PP
The remaining sections of this tutorial provide more information
about these other
.B nroff
primitives.
.SH "Introducing nroff's Primitives"
.PP
The rest of this tutorial describe \*(PN's
\fIbasic commands\fR \(em the commands that are \*(QLbuilt into\*(QR
\*(PN, and from which macros are assembled.
These basic commands, or \fIprimitives,\fR
form \*(PN's text formatting language.
Once you have mastered the primitives,
you will be able to write macros to control automatically
even the most difficult text formatting tasks.
.PP
The rest of this tutorial includes a number of exercises.
You should type them into your system and execute them as described
in the tutorial; this will greatly increase the rate at which you
master \fBnroff\fR.
None of the following examples should be processed
with the \fB\-ms\fR macro package; the purpose of this portion of
the tutorial is to teach you how to create you own text processing
routines, rather than how to use ones that have already been written.
.Sh "Page Format"
.PP
When deciding how to process text, you must first decide
how to position the text on the printed page.
You must control line length, left and right margins,
page offset (i.e., how far from
the left edge of the page each line begins), and page length.
Controlling these functions is quite easy with the appropriate
.B nroff
commands.
.PP
.II "nroff^line, length"
.II "nroff^.ll primitive"
.II "nroff^.po primitive"
.II "nroff^page, offset"
.II "nroff^command, line length"
.II "nroff^command, page offset"
The
.I "line length"
primitive
.B .ll
controls the line length; and the
.I "page offset"
command
.B .po
controls the page offset.
If you are writing an
.B nroff
script, you should include these commands before the beginning of your
text, so that
.B nroff
can put them into effect immediately.
The following example uses a line length of three inches and a page
offset of two inches.
Type this into your system under the name \fBex1.r\fR.
Note, by the way, that the text to the right of the characters
\*(Ql\e"\*(Qr is a \fIcomment\fR, and there is no need for you
to type it into your system:
.ID
.II "hoop snake"
.II "Aunt Patsy"
.II "Twain, Mark"
.ta 0.5i 1.5i
	.ll 3i	\e" set line length
	.po 2i	\e" set page offset
	Along outside of the front fence ran the country
	road, dusty in the summertime, and a good place for
	snakes -- they liked to lie in it and sun themselves;
	when they were rattlesnakes or puff adders, we killed
	them; when they were black snakes, or racers, or belonged
	to the fabled "hoop" breed, we fled, without shame; when
	they were "house snakes", or "garters", we carried them
	home and put them in Aunt Patsy's work basket for a
	surprise; for she was prejudiced against snakes, and
	always when she took the basket in her lap and they
	began to climb out of it it disordered her mind.
.IE
.II "nroff^specification"
Process this script by
typing the command
.DM
	nroff ex1.r >ex1.p
.DE
From this point on, you should \fInot\fR use the
\fB\-ms\fR macro package with your \*(PN examples.
When you display the output stored in the file \fBex1.p\fR,
you will see that the length of each line is three inches, and
each line begins two inches from the left-hand margin.
.PP
As you noticed, line length and page offset were set in \fIinches\fR.
.B nroff
output can be controlled using a number of different
units of measurements, including inches, number of characters, or
lines, or \fImachine units\fR.
A following section discusses
.B nroff
units of measurement in detail.
.PP
.II "nroff^comments"
As noted above, this example contains two \fIcomments\fR.
.B nroff
ignores any text that appears on a line
after \*(QL\e"\*(QR.
You should use comments, for the benefit of anyone who must
read your
.B nroff
script (including yourself).
The above example used the comments
.DM
	\e" set line length
	\e" set page offset
.DE
to help you understand the \fB.ll\fR and \fB.po\fR commands.
Judicious comments can make a complex
script much easier to understand.
.Sh "Breaks"
.PP
.II "nroff^break"
Before you look at the \fIbreak\fR primitive
.BR .br ,
it is helpful to examine how
.B nroff
constructs a finished line of output.
Suppose, for example, that you tell
.B nroff
that you want each output line to be five inches long.
.B nroff
takes your input one word at a time, and attempts to squeeze that
word into the space that has not yet been taken up in the line.
.II "nroff^margins"
.II "nroff^break"
.II "nroff^.br primitive"
.II "nroff^command, break"
When 
.B nroff
finally picks up a word that is
too large to fit into the amount of space left in the line,
it either puts the word aside entirely, or hyphenates
the word and places the hyphenated portion into the line.
.B nroff
then inserts extra blank spaces between the words to justify the line.
The
.I break
primitive
.BR .br ,
however, tells
.B nroff
to print whatever words have already been put into the line,
even if they do not form a complete line, and without
performing right justification.
.PP
The idea of a break might seem strange at first, but
you are familiar with a simple example:
the end of a paragraph.
You do not want the start of a new
paragraph to be on the same line as the end of the previous
paragraph:
you want to print the end of the previous paragraph
whether or not it fills a complete line;
and you want to begin the new paragraph on a new line.
As you will learn later, some
.B nroff
commands cause breaks automatically;
you should be aware of this when you use them.
.Sh "Fill and Adjust Modes"
.PP
.II "nroff^fill"
.II "nroff^justify"
.II "nroff^adjust"
Two terms describe how
.B nroff
processes your input to create its output:
.IR filling ,
and
\fIadjusting\fR or \fIjustifying\fR.
Unless you order it not to,
.B nroff
operates in the \fIfill\fR and \fIadjust\fR modes.
.II "nroff^command, fill"
.II "nroff^.fi primitive"
.II "nroff^.nf primitive"
.II "nroff^.ad primitive"
.II "nroff^.na primitive"
The
.I "no-fill"
primitive
.B .nf
tells
.B nroff
to stop using fill mode.
The
.I "fill"
primitive
.B .fi
tells it to resume using the fill mode.
In a similar way, the
.I adjust
primitive
.B .ad
tells
.B nroff
to use adjust mode, whereas the
.I "no adjust"
primitive
.B .na
tells it to use no-adjust mode.
.PP
As mentioned above,
.B nroff
by default is in both fill mode and adjust mode, so you do not need
to begin your script with
.B .fi
and
.B .ad
if you want \*(PN to fill and adjust your text.
However, if you turn off filling and adjusting by using the
.B .nf
and
.B .na
commands,
you must use the
.B .fi
and
.B .ad
commands to turn filling and adjusting back on.
.PP
.II "nroff^no-fill"
When you use
.B .nf
to turn off fill mode,
.B nroff
no longer tries to fill lines to a fixed line length.
It prints each line of input text exactly as received.
However, a sufficiently long line of
text would run off the right-hand edge of the page if
.B nroff
were to print it as entered.
If the input line cannot fit on one line,
.B nroff
prints as much as it can fit on one line,
then breaks the line and prints the rest
on the next line with no page offset.
.PP
In adjust mode,
.B nroff
inserts extra spaces between words to justify lines of text, as
described above.
When
.B nroff
is in no-fill mode, it is automatically in no-adjust mode:
with no fixed line length, there is no need to insert extra spaces.
.I Moral:
you can fill without adjusting, but you cannot adjust without filling.
.PP
If you request filling but not adjusting,
.B nroff
fills the output line as described earlier, but
does not insert extra spaces between words;
that is, it does not try to keep an even right margin.
Every output line either is shorter than the line length you
specified, or exactly as long.
.PP
.II "nroff^.ad primitive"
The
.B .ad
primitive includes several options.
If you use the command
.B .ad
without an argument,
.B nroff
keeps strict left and right margins.
The primitive
.B ".ad l"
justifies the left margin only;
.B ".ad r"
justifies the right margin only; and
.B ".ad b"
justifies both margins (this, of course, is the default).
Finally,
.B ".ad c"
centers output lines while keeping their lengths less than
or equal to the line length, as set with the \fB.ll\fR
command.
.PP
Remember that
.B nroff
ignores adjustment requests if you are in no-fill mode.
If
.B nroff
is in fill mode and you request any variety of adjustment, it
adjusts accordingly until you issue either a no-fill or a no-adjust command.
If you give a no-fill command, only a fill command restores
adjustment; any plea for a
different kind of adjustment is ignored while \*(PN is in no-fill mode.
.PP
To see how this works, type the following script
under the name \fBex2.r\fR, and process it as above:
.ID
.II "Boswell, James"
.II "Johnson, Samuel"
.II "Hume, David"
.ta 0.5i 1.5i
	.ll 6.75i
	.sp	\e" space
	When we were alone, I introduced the subject
	of death, and endeavored to maintain that the fear
	of it might be got over.  I told [Johnson] that
	David Hume said to me, he was no more uneasy to
	think that he should not be after this life, than
	that he had not been before he began to exist.
	.sp
	.na	\e"no adjust
	JOHNSON:  "Sir, if he really thinks so,
	his perceptions are disturbed;
	he is mad: if he does not think so, he
	lies .... When he dies, he at
	least gives up all he has."
	.sp
	.ad r	\e"right-adjust
	BOSWELL:  "Foote, sir, told me that
	he was not afraid to die."
	.sp
	.nf	\e"no-fill
	JOHNSON:  "It is not true, sir.
	Hold a pistol to Foote's
	breast or to Hume's breast,
	and threaten to kill them,
	and you'll see how they behave."
	.sp
	.fi	\e"fill
	BOSWELL:  "But may we not fortify our minds for
	the approach of death?"
	.sp
	JOHNSON:  "No, sir, let it alone.  It matters not
	how a man
	dies, but how he lives.  The act of dying is not of
	importance, it lasts so short a time .... A man
	knows it must be so, and submits.
	It will do him no good to whine."
.IE
When you process this input with
.B nroff,
your output should look like this:
.EX
When we were alone, I introduced the subject
of death, and endeavored to maintain that the fear
of it might be got over.  I told [Johnson] that
David Hume said to me, he was no more uneasy to
think that he should not be after this life, than
that he had not been before he began to exist.
.sp
.na
JOHNSON:  "Sir, if he really thinks so,
his perceptions are disturbed;
he is mad: if he does not think so, he
lies .... When he dies, he at
least gives up all he has."
.sp
.ad r
BOSWELL:  "Foote, sir, told me that
he was not afraid to die."
.sp
.nf
JOHNSON:  "It is not true, sir.
Hold a pistol to Foote's
breast or to Hume's breast,
and threaten to kill them,
and you'll see how they behave."
.sp
.fi
BOSWELL:  "But may we not fortify our minds for the approach
of death?"
.sp
JOHNSON:  "No, sir, let it alone.  It matters not
how a man
dies, but how he lives.  The act of dying is not of
importance, it lasts so short a time .... A man
knows it must be so, and submits.
It will do him no good to whine."
.EE
.fi
.ad b
.II "nroff^.ad primitive"
.II "nroff^.na primitive"
.II "nroff^.fi primitive"
After the
.B .na
primitive,
\*(PN fills but does not adjust the second paragraph.
After
.BR ".ad r" ,
it fills and right adjusts the third paragraph.
After
.BR .nf ,
it neither fills nor adjusts the fourth paragraphs.
Finally, after
.BR .fi ,
it fills the fifth and sixth paragraphs and uses the
.B ".ad r"
adjust option that was in effect previously.
.PP
Under certain extreme conditions,
.B nroff
cannot adjust a line even though it is in adjust mode.
If, for example, you specified a line length of one inch,
a seven-letter or eight-letter word would then take up most of a line.
When such a word was then followed by
a word that could not fit into the line after it,
\*(PN would begin a new line with the second word
rather than violate the right margin by inserting the into the line.
When a line has only one word in it,
.B nroff
obviously cannot adjust the line by inserting extra spaces between words;
therefore, the right margin is left uneven,
as though \*(PN were in no-adjust mode.
.Sh "Defining Paragraphs"
.PP
.II "nroff^paragraph"
What happens if you copy text from several pages of a book into a file
without adding any formatting commands, and then process the file with
.BR nroff ?
There is no page offset, because
.BR nroff 's
default page-offset setting is zero;
and the processed lines are set to the default length of 6.5
inches (65 Pica characters).
.PP
More interesting things happen with paragraphs.
Suppose you skip a line between paragraphs and
begin each paragraph by indenting five spaces.
The blank line in the input text causes a break, and forces
.B nroff
to print a blank line.
The last line of each paragraph is unadjusted,
and a blank line appears before the next paragraph.
.II "nroff^break"
Initial blank spaces in a line of input also cause a break.
In this example, the breaks caused by initial
blank spaces at the beginning of each paragraph do
nothing, because the preceding blank line forces out
the last line of the preceding paragraph.
.B nroff
always considers initial blank
spaces in a line to be significant, and preserves them
in the output.
.PP
To see how blank lines and initial spaces affect \*(PN's output,
copy the following example and run it through
.B nroff:
.DM
.ta 0.8i
	     Here is a little text so you can see
	whether nroff will ignore the initial
	indentation
	            in this very very long sentence.
	Here is a little bit more text.

	     And here is something to mimic
	the beginning of a new paragraph.
.DE
The output should look like this:
.EX
     Here is a little text so you can see
whether nroff will ignore the initial
indentation
            in this very very long sentence.
Here is a little bit more text.

     And here is something to mimic
the beginning of a new paragraph.
.EE
.II "nroff^.sp primitive"
Instead of leaving a blank line in the text, you could use the
.I space
primitive
.B ".sp 1,"
which causes a break and inserts one blank line into the output.
In a similar way,
.B ".sp 5"
causes a break and inserts five blank lines in the output.
Edit the example and replace the blank line with the
command line:
.DM
	.sp 1
.DE
You will see that it has the same effect.
You can also use the form
.BR .sp ;
.B nroff
assumes you want one space if you omit the argument.
.PP
Most
.B nroff
input consists of many paragraphs that contain text, and
you probably want each paragraph to have the same format in the output.
Rather than formatting each paragraph explicitly, as in this example,
you can use the
.I macro
facility of
.B nroff
to define a sequence of commands to format a paragraph.
Macros are described in detail later in this tutorial.
.Sh "Centering"
.PP
.II "nroff^.ce primitive"
The
.I center
primitive
.B .ce
centers one or more lines of text.
For example, you can center a two-line heading as follows:
.DM
	.ce 2
	Heading Printed
	In Center of Page
.DE
If you use the
.B .ce
command with no argument,
.B nroff
assumes a default argument of one, and centers only the next line of input.
The command \fBce 0\fR cancels any earlier centering command that
is in operation.
.Sh "Tabs"
.PP
.II "nroff^tab"
If your
.B nroff
input includes tables, you may find it convenient to use tabs to
separate items in a line of the table.
.B nroff
recognizes the
.B <tab>
character
and expands it into spaces.
If you use tabs to format a table, remember to use no-fill mode;
otherwise,
.B nroff
tries to fill and adjust your output lines.
.PP
.II "nroff^.ta primitive"
By default,
.B nroff
sets a tab stops after every eight characters.
You can use the
.I tab
primitive
.B .ta
to change the positions of the tab stops.
For example,
.DM
	.ta 10 20 30 40 50 60
.DE
sets tab stops ten characters apart rather than eight.
.B .ta
can also be used to fix tab stops in inches
rather than after a number of characters;
for example,
.DM
	.ta 0.8i 2.0i
.DE
sets tab stops after 0.8 inches and 2.0 inches on the output line.
This is quite helpful when you are designing a
table.
.PP
.II "nroff^.tc primitive"
You can use the
.I "tab character"
command
.B .tc
to change the character
.B nroff
prints between its current position and the next tab stop.
Enter the following text to see how this primitive works:
.DM
	.ta 9 19 29 39
	.tc *
	.nf
	<tab>1<tab>2<tab>3<tab>4
.DE
The output file, \fBex3.p\fR, should appear as follows:
.DM
	*********1*********2*********3*********4
.DE
.Sh "Page Breaks" 
.PP
.II "nroff^.bp primitive"
.II "nroff^.pl primitive"
The
.I "begin page"
primitive
.B .bp
causes a break and forces
.B nroff
to the next output page.
By default,
.B nroff
assumes a page length of 11 inches (66 lines).
You can change the page length with the
.I "page length"
command
.B .pl.
For example,
.DM
	.pl 2i
.DE
specifies a two-inch page length.
.PP
At this point, the question arises about how \*(PN
handles top and bottom page margins, number pages,
and other aspects of page layout.
The answer is it merely keeps track of the current output page number and
the current line number on the current output page;
designing top and bottom margins, page headers and footers,
and other aspects of page layout is up to you.
.PP
.II "nroff^traps"
Can
.B nroff
execute a set of commands whenever it reaches a certain position on the page?
This would
solve the problem of producing top and bottom margins, and you would not
have to guess where to insert the commands in your script.
In fact, you can tell \*(PN to do this, by using
.I traps.
The next section of this tutorial
describes macros and traps and how to use them to format a page.
.SH "Macros and Traps"
.PP
.II "nroff^macro"
This section presents \*(PN macros:
how to write them, how to tell \*(PN to execute them
at a give point on every output page, and how to install
a macro file under the \*(CO system
.PP
As with previous sections, this one uses a number of exercises.
Working the exercises will help you master \*(PN quickly.
When you format the exercise scripts, do not use the \fB\-ms\fR option.
Also, it is not necessary for you to copy the comments into your
system; they are here to help you understand what each \*(PN
command does, but they have no effect on how the script executes.
.Sh "What Is a Macro?"
.PP
To become familiar with the idea of a
.IR macro ,
consider the problem of formatting a paragraph.
Whenever you come to a new paragraph, you want
\*(PN to skip a line and indent the first line five spaces.
Because
.B nroff
preserves blank lines and initial indentations, you could force
.B nroff
to break your text into paragraphs
simply by inserting a blank line and spaces directly into your text.
The same effect, however, can be achieved by inserting
following set of \*(PN commands
.DM
.ta 0.8i 2.0i
	.br	\e" break
	.sp	\e" skip a line
	.ti 5	\e" indent next line 5 spaces
.DE
between the end of each paragraph and the start of the next paragraph.
.II "nroff^.br primitive"
.II "nroff^.ti primitive"
.II "nroff^.sp primitive"
You should recognize the first two commands:
.B .br
causes a break, so that
.B nroff
prints the last line of the previous paragraph even
though it might not be a complete line;
.B .sp
skips a line before the next paragraph begins.
The third command is the
.I "temporary indent"
command
.BR .ti ,
which tells \*(PN to indent the next line;
the number indicates how many spaces to indent.
The following exercise, \fBex4.r\fR, demonstrates
how this works:
.ID
.II "Twain, Mark"
.II Adam
.II peach
.II almond
.II cabbage
.II cauliflower
.ta 0.8i 2.0i
	.ll 3i	\e" line length
	.po 3i	\e" page offset
	.ti 5	\e" indent next line
	Adam was human--this explains it all.  He did
	not want the apple for the apple's sake, he
	wanted it because it was forbidden.  The mistake
	was in not forbidding the serpent; then he would
	have eaten the serpent.
	.br	\e" break
	.sp	\e" skip a line
	.ti 5	\e" indent next line
	Training is everything.  The peach was once a bitter
	almond; cauliflower is nothing but cabbage with a
	college education.
	.br
	.sp
	.ti 5
	Habit is habit, and not to be flung out of the window
	by any man, but coaxed downstairs a step at a time.
.IE
After you have processed this file, the output file \fBex4.p\fR
should resemble the following:
.EX
.ti 10
Adam was human--this explains it all.  He did
not want the apple for the apple's sake, he
wanted it because it was forbidden.  The mistake
was in not forbidding the serpent; then he would
have eaten the serpent.
.br
.sp
.ti 10
Training is everything.  The peach was once a bitter
almond; cauliflower is nothing but cabbage with a
college education.
.br
.sp
.ti 10
Habit is habit, and not to be flung out of the window
by any man, but coaxed downstairs a step at a time.
.EE
Now, in a small file it would be easy to type all of the \*(PN primitives
directly into your input text;
however, what if
your file is very long, with hundreds of paragraphs?
Every time you wanted to begin a paragraph, you would have to include that
set of commands within the text.
You would save considerable agony if you could
bundle these commands together under a common
.IR name ;
then you could simply put that \fIname\fR into
your text whenever you wanted
.B nroff
to perform these commands, rather than typing the commands themselves over
and over again.
.PP
.II "nroff^.PP macro"
As you probably have guessed by now, you can do just that;
the set of commands is called a
.I macro.
.II "Puddnhead Wilson"
The following shows the selections from Pudd'nhead Wilson's
calendar set with a macro called
.B .PP
that takes care of formatting each paragraph.
The following exercise, \fBex5.r\fR, shows how to bundle together
the \*(PN primitives for formatting paragraphs into the \fB.PP\fR macro:
.ID
.ta 0.5i 1.8i
	.de PP	\e" define the PP macro
	.br	\e" break the line
	.sp	\e" insert a blank line
	.ti 5	\e" indent next line 5 spaces
	..	\e" two periods ends the macro definition
	.PP	\e" execute PP macro
	Adam was human--this explains it all.  He did
	not want the apple for the apple's sake, he
	wanted it because it was forbidden.  The mistake
	was in not forbidding the serpent; then he would
	have eaten the serpent.
	.PP
	Training is everything.  The peach was once a bitter
	almond; cauliflower is nothing but cabbage with a
	college education.
	.PP
	Habit is habit, and not to be flung out of the window
	by any man, but coaxed downstairs a step at a time.
.IE
As you can see, using a macro can save you a considerable amount of work
when you prepare your script.
.Sh "Introducing Traps"
.PP
Now, consider the problem of formatting the beginning and ending of each page
of output.
You could define what are traditionally called
.I header
and
.I footer
macros, which contain the commands you want performed at the top and
bottom of each page.
However, how can you tell \*(PN when to execute these macros?
You cannot possibly know where
to call these macros in the input text, because you cannot know
where any given text line will appear in the output until you
have processed it through \*(PN.
This problem is solved by using
.I traps.
.PP
.B nroff
keeps track of its vertical position on each output page.
You can set a
.I trap
that tells
.B nroff
to execute a macro at a particular vertical position on every page.
When a line of output reaches or extends past the position that is specified
in your
.IR trap ,
.B nroff
then executes the commands named in the trap command
before processing any more input text.
.PP
.II "nroff^.wh primitive"
.II "nroff^.hd primitive"
.II "nroff^command, when"
You can set a trap by using the
.I when
command
.BR .wh .
For example, if you want
.B nroff
to call your header macro
.B .HD
at the very top of each page, the command
.DM
.ta 0.5i 1.8i
	.wh 0 HD	\e" set header trap
.DE
sets a trap for the macro
.B .HD
at vertical position 0 (the very top of the page) of every output page.
The macro
.B .HD
will then be executed every time \*(PN begins a new page.
.II "nroff^.FO macro"
.II "nroff^measurement, units"
To have your footer macro
.B .FO
execute one inch from the bottom of each page, use the command
.DM
.ta 0.5i 1.8i
	.wh -1i FO	\e" set footer trap
.DE
The negative number tells
.B nroff
to measure distance from the \fIbottom\fR
of the page rather than from the top;
the \fBi\fR is an abbreviation for inches.
(\*(PN recognizes various units of measurement; this will be
described in more detail later.)
.Sh "Headers and Footers"
.PP
.II "nroff^% number register"
Suppose you want to design the output page by defining the header and footer
macros.
A simple header macro merely skips an inch of space at the
top of each page; a simple footer macro forces printing to stop an
inch from the bottom of each page and prints the page number.
.B nroff
does not print page numbers automatically, but
it does automatically keep track of which output page it is on.
It stores the page number internally in a
.I "number register"
that you can access with the symbol \*(Ql%\*(Qr.
(A later section gives more information about number
registers and how to use them.)
.PP
.II "nroff^footer"
The following gives a simple footer macro that prints the page number:
.DM
.ta 0.5i 1.8i
	.de FO	\e" define footer macro FO
	'sp 4v	\e" skip four vertical lines (no break)
	.tl ''- % -''	\e" print hyphen, page number, hyphen
	'bp	\e" jump to new page
	..	\e" end macro definition
.DE
There are several points of interest raised by this macro.
.PP
First, notice that some
commands are preceded with an apostrophe rather than with a period.
The use of the apostrophe instead of
the period tells \*(PN to suppress the break these commands normally cause.
You might run into problems if you define your header macro
as follows:
.DM
.ta 0.8i 2.0i
	.de HD	\e" header macro
	.sp 1i	\e" skip an inch (break)
	..
.DE
You want this to leave a blank space of one inch
at the top of each page; however, the
.B .sp
command causes a break, so that if a word were left over
from the last line on the preceding page,
.B nroff
would print it at the very top of the next page.
The effect would be quite unsightly.
However, if you use
.B 'sp
instead of
.B .sp
in the macro,
.B nroff
suppresses the break and does not print the partial word
until \fIafter\fR it performs the macro commands.
The same is true for the footer macro:
you do not want anything
unplanned to be printed in the blank space at the bottom of the page.
You should always be conscious of these considerations when you use
commands that cause breaks.
.PP
.II "nroff^.tl primitive"
Another new item in the above example is the
.I title
command
.B .tl,
which prints a three-part title.
A three-part title contains a left part
(aligned to the left margin of the page),
a center part (centered),
and a right part (aligned to the right margin).
The command name
.B .tl
is followed by four apostrophes:
.B nroff
prints the characters
between the first two apostrophes
as the left part of the
title line, those between the second and third apostrophes
as the center part,
and those
between the third and fourth apostrophes
as the right part of the three-part title.
If you do not want
.B nroff
to print anything in one of these positions, simply put nothing between the
appropriate pair of quotes.
In the above example, the
.B .tl
primitive tells
.B nroff
to print nothing in the left and right portions of the footer title line, but
to print the page number in the center.
If you want an apostrophe to appear as a part of the title,
precede it with the backslash character \*(Ql\e\*(Qr.
.PP
.II "nroff^.lt primitive"
.II "nroff^command, title length"
\*(PN considers the length of the title line to be independent of the length of
normal output lines; therefore, you must set it with the
.I "length of title"
primitive
.B .lt
unless you want
.B nroff
to use the default title length of 6.5 inches.
For example, to set the length of the title to five inches, use the command
.DM
	.lt 5i
.DE
In light of all you now know, you should give Pudd'nhead Wilson's
calendar the treatment it deserves:
.ID
.ta 0.5i 1.8i
	.ll 3i	\e" set line length to 3 inches
	.po 2i	\e" set page offset to 3 inches
	.pl 9i	\e" set page length to 9 inches
	.wh 0 HD	\e" set the header trap
	.wh -1i FO	\e" set the footer trap
	.de HD	\e" define header macro HD
	'sp 1i	\e" skip 1 inches of space
	..	\e" end macro definition
	.de FO	\e" define footer macro
	'sp 2	\e" skip 2 lines
	.tl ''- % -''	\e" define footer title
	'bp	\e" begin new page
	..	\e" end macro definition
	.de PP	\e" define paragraph macro
	.sp 1	\e" skip 1 line of space
	.ti 5	\e" indent the first line 5 characters
	..	\e" end macro definition
	.PP
	Adam was human--this explains it all.  He did
	not want the apple for the apple's sake, he
	wanted it because it was forbidden.  The mistake
	was in not forbidding the serpent; then he would
	have eaten the serpent.
	.PP
	Training is everything.  The peach was once a bitter
	almond; cauliflower is nothing but cabbage with a
	college education.
	.PP
	Habit is habit, and not to be flung out of the window
	by any man, but coaxed downstairs a step at a time.
.IE
.PP
.II "nroff^traps"
As a point of technique, always set header and footer traps
early in your input script; otherwise, \*(PN may not
print the header on the first page.
.Sh "Macro Arguments"
.PP
.II "nroff^macro, arguments"
You can affect how macros function by passing
them modifiers, called \fIarguments\fR.
An argument may be a bit of text that is arranged in a special
way by the macro, or it may be a number or other parameter that
dictates exactly what the macro does.
.PP
As an example of how a macro can handle arguments,
consider a macro to format the list of ingredients for a recipe.
You want the ingredients to be printed as follows:
.DM
	3 cups of pumpkin
	1 cup of milk
	1 cup of sugar
	1 tsp of ground ginger
	1 tbl of cinnamon
.DE
.PP
.II "nroff^macro, definition"
Each of these lines has the same format: the amount of
ingredient, the unit of
measurement, the word \*(QLof\*(QR, and the name of the ingredient.
You can create a macro (call it
.BR .RE ,
for \fBre\fRcipe)
that encodes the format of these lines and
contains three \*(QLslots\*(QR:
one slot for the amount, one for the unit of
measurement, and one for the name of the ingredient.
Each time you use the macro, you
indicate what you want to go into each slot, and
.B nroff
substitutes it for you.
The macro \fB.RE\fR can be constructed as follows:
.ID
.ta 0.5i 1.8i
	.de RE	\e" define macro RE
	\e\e$1 \e\e$2 of \e\e$3	\e" set RE's arguments
	..	\e" end definition
	Here is some text.
	.nf	\e" don't fill the recipe
	.RE 3 cups pumpkin
	.RE 1 cup milk
	.RE 1 cup sugar
	.RE 1 tsp "ground ginger"
	.RE 1 tbl cinnamon
	.fi	\e" resume filling
	Here is some more text.
	.bp	\e" begin a new page, to force printing
.IE
.II "nroff^macro, arguments"
When you call a macro that takes arguments, the arguments
must appear on the same line as the macro command itself.
A macro may have up to nine arguments; they are denoted by \fB\e$1\fR,
through \fB\e$9\fR, respectively:
the first field after the macro name
is called \fB\e$1\fR, the second \fB\e$2\fR, and so on.
.PP
If you want to use as an argument a string of characters that includes
blank spaces, you must enclose the string within quotation marks, as
with the words \*(QLground ginger\*(QR, in the example above.
If you forget to include the quotation marks,
.B nroff
regards each word in the string
as a separate argument, and treats them accordingly.
.PP
Note that
macros that are called by traps cannot accept arguments.
.Sh "Double vs. Single Backslashes"
.PP
If you carefully examine the definition of
.BR RE ,
you will see that it identifies
each argument with two backslashes:
.DM
	\e\e$1 \e\e$2 of \e\e$3
.DE
Whenever you identify an argument within a macro,
always preface it with two backslashes, rather than one.
The reason is that \*(PN in effect processes a macro \fItwice\fR:
when it first reads it, and later when you call it within your text.
Prefacing an argument with \fIone\fR backslash tells \*(PN that you
want to expand that argument when the macro is first read; prefacing
it with two backslashes tells \*(PN that you want to expand it when
the macro is called in your text.
In nearly every circumstance, you want to expand the arguments
in your text, so you should use two backslashes.
As you will see, this rule also applies to the use of \fIstrings\fR
and \fInumber registers\fR within macros.
.PP
To see how this works, consider again the \fB.RE\fR macro:
.ID
	.de RE
	\e\e$1 \e\e$2 of \e\e$3
	..
	Here is some text.
	.nf
	.RE 3 cups pumpkin
	.RE 1 cup milk
	.RE 1 cup sugar
	.RE 1 tsp "ground ginger"
	.RE 1 tbl cinnamon
	.fi
	Here is some more text.
	.bp
.IE
Using two backslashes, as above, allows you to redefine
what \fB$1\fR, \fB$2\fR, and \fR$3\fR mean many times
throughout your text, to generate the following output:
.DM
	Here is some text.
	3 cups of pumpkin
	1 cup of milk
	1 cup of sugar
	1 tsp of ground ginger
	1 tbl of cinnamon
	Here is some more text.
.DE
If you used only one backslash, however, your output
would appear as follows:
.DM
	Here is some text.
	  of
	  of
	  of
	  of
	  of
	Here is some more text.
.DE
\*(PN could not expand the argument calls (\fB\e$1\fR etc.), because
you had not yet defined them; therefore, it threw them away;
and because all of the argument calls
had been thrown away, \*(PN then threw all the arguments away.
All that was left was word \fBof\fR.
.SH "Designing and Installing Macros"
.PP
Now that you have been shown how to write a macro, the next step
is to design some macros and install them, so you can call them
over and over again.
.PP
The first step in designing a macro is to analyze the problem that
you want to solve.
Suppose that in this instance you want to print a list of names.
Each name will consist of a first name, a last name, and the
department with which he is associated, and the list will
be printed in columns; for example:
.DM
.ta 0.5i 1.5i 2.5i
	Firstname	Lastname	Department
.DE
Moreover, you want to be able to switch the order in which the
columns appear without having to retype your list; for example:
.DM
.ta 0.5i 1.5i 2.5i
	Lastname	Firstname	Department
.DE
or
.DM
.ta 0.5i 1.5i 2.5i
	Department	Lastname	Firstname
.DE
In effect, then, you want three macros:
one for each of the three orders of columns shown above.
.PP
When you have finished designing your macros, they should
look something the following.
Type the following into the file \fBtmac.lst\fR; note that
the symbol \fB<tab>\fR represents a \fItab\fR character, and
should not be entered literally:
.ID
	.\e" List macros. $1 represents first name,
	.\e" $2 last name, $3 department
	.de LA
	.nf
	.ta 1.5i 2.75i
	\e\e$1<tab>\e\e$2:<tab>\e\e$3
	.Rt
	..
	.de LB
	.nf
	.ta 1.5i 2.75i
	\e\e$2,<tab>\e\e$1:<tab>\e\e$3
	.Rt
	..
	.de LC
	.nf
	.ta 1.5i 2.75i
	\e\e$3:<tab>\e\e$2,<tab>\e\e$1
	.Rt
	..
.IE
The first lines are \fIcomments\fR, so that
anyone who looks at these macros will know
what they do.
The first command line, introduced with the \fB.de\fR
command, names each macro.
These names were selected after checking the file \fBtmac.s\fR,
which is where the \fB\-ms\fR macro package is kept, to confirm
that they are not used elsewhere.
Naturally, using the same macro name in two different
places can lead to a great deal of trouble.
.PP
The next command, \fB.nf\fR, turns off the \*(PN's normal right
justification, which otherwise would smear a table.
The \fB.ta\fR command sets the tab characters at certain
points on the page, measured from the left margin.
.PP
The next line gives the order in which the arguments appear.
The arguments are separated by tab characters, and punctuation is inserted.
The last command, \fB.Rt\fR, calls a macro in the file \fBtmac.s\fR;
this macro resets \fBnroff\fR to its normal fill mode and returns
the tab settings to normal.
Note that these macros can be used only when you also use
the \fB\-ms\fR macro package.
.PP
After you have typed the macros into \fBtmac.lst\fR,
carefully read over what you type to ensure that no there are
no errors; if you find any, be sure to correct them.
The final step is to move \fBtmac.lst\fR into
the directory \fB/usr/lib\fR, which is where \fBtmac.s\fR is also kept.
.PP
To test your new macros, type the following text into the file
\fBex6.r\fR:
.ID
	The following lists give the personnel who are involved in
	this project: 
	.sp
	.LA Ivan Morgan Engineering
	.LA Marian Gusman Design
	.LA George Meyer Electrical
	.LA Catherine Scanlon "Metal Shop"
	.LA Fred McElroy Carpentry
	.LA Anne Assenmacher "Machine Shop"
	.sp
	.LB Ivan Morgan Engineering
	.LB Marian Gusman Design
	.LB George Meyer Electrical
	.LB Catherine Scanlon "Metal Shop"
	.LB Fred McElroy Carpentry
	.LB Anne Assenmacher "Machine Shop"
	.sp
	.LC Ivan Morgan Engineering
	.LC Marian Gusman Design
	.LC George Meyer Electrical
	.LC Catherine Scanlon "Metal Shop"
	.LC Fred McElroy Carpentry
	.LC Anne Assenmacher "Machine Shop"
	.sp
	We expect that they will receive your full cooperation.
.IE
The same set of names is used three times; the only difference
is the macro call employed.
.PP
Now, process this file with the following command:
.DM
	nroff -ms -mlst ex6.r >ex6.p
.DE
As you can see, when you installed \fBtmac.list\fR into \fB/usr/lib\fR,
you could invoke it in the same way that you invoke
\fBtmac.s\fR with \fB\-ms\fR.
.PP
When you look at the output file \fBex6.p\fR, you should see
something that resembles the following:
.EX
.de LA
.nf
.ta 1.25i 2.5i
\\$1	\\$2:	\\$3
.fi
..
.de LB
.nf
.ta 1.25i 2.5i
\\$2,	\\$1:	\\$3
.fi
..
.de LC
.nf
.ta 1.25i 2.5i
\\$3:	\\$2,	\\$1
.fi
..
The following lists give the personnel who are involved in
this project: 
.sp
.LA Ivan Morgan Engineering
.LA Marian Gusman Design
.LA George Meyer Electrical
.LA Catherine Scanlon "Metal Shop"
.LA Fred McElroy Carpentry
.LA Anne Assenmacher "Machine Shop"
.sp
.LB Ivan Morgan Engineering
.LB Marian Gusman Design
.LB George Meyer Electrical
.LB Catherine Scanlon "Metal Shop"
.LB Fred McElroy Carpentry
.LB Anne Assenmacher "Machine Shop"
.sp
.LC Ivan Morgan Engineering
.LC Marian Gusman Design
.LC George Meyer Electrical
.LC Catherine Scanlon "Metal Shop"
.LC Fred McElroy Carpentry
.LC Anne Assenmacher "Machine Shop"
.sp
We expect that they will receive your full cooperation.
.EE
As you grow proficient in writing \*(PN macros,
you will probably find it most efficient to keep
special macros in their own files; this will save
time by ensuring that \*(PN does not have
to process macros that are never called.
.SH "Strings"
.PP
.II "nroff^string"
Suppose you are writing a script for
.B nroff
and, to relieve the tedium, decide to punctuate the text
occasionally with a rousing cry of \*(QLFOOD FIGHT!!\*(QR.
If you plan to interject this
phrase more than a few times in your script, you can
take advantage of another labor-saving device, called a
.I string.
You can use a string name as an abbreviation for
a long string of characters you use frequently.
Like a macro, a string is a
.I name
that
.B nroff
associates with a
.I definition
that you supply.
Wherever you put the name in your text,
.B nroff
prints the definition.
Although macros refer to sets of \fIcommands\fR that you define, strings refer
to strings of \fIcharacters\fR that you define.
.PP
.II "nroff^.ds primitive"
You define a string with the
.I "define string"
primitive
.B .ds:
.DM
	.ds FF "FOOD FIGHT!!"
.DE
The first field after the
.B .ds
gives the name of the sting, in this case
.B FF.
Like a macro name, a string name may be either one or two characters.
The second field after the
.B .ds
gives the definition of the string, in this case
.DM
	"FOOD FIGHT!!"
.DE
As in this example, you must enclose the definition within
quotation marks if it contains spaces.
.PP
Be careful whenever you define a macro or a string.
If you already have a macro or a string named
.B X
and you define a new macro or string named
.B X,
.B nroff
forgets the previous meaning of
.B X.
.PP
Once you have defined a string, you can refer to it
anywhere in your text.
The string itself appears in the output text wherever a reference to it
appears in the input text.
You refer to the string
.B FF
in the following fashion:
.DM
        \e*(FF
.DE
Use the left parenthesis \*(Ql(\*(Qr only when the name of the string
is two characters long.
If the string name is only a single character,
such as
.B S,
refer to it as follows:
.DM
	\e*S
.DE
As an example, type the following script into \fBex7.r\fR,
and process it through
.BR nroff ;
do \fInot\fR use the \fB\-ms\fR macro package:
.DM
.II Aristotle
	.ds FF "FOOD FIGHT!!"
	.ds W "WHOOPEE!!"
	.ce
	From Aristotle's "Poetics"
	.br
	.sp
	A tragedy is the imitation of an action \e*(FF
	that is serious and also, \e*W as having magnitude,
	complete in itself, with incidents \e*(FF
	arousing pity and fear, wherewith to accomplish \e*W
	\e*(FF its purgation of such emotions \e*(FF \e*(FF.
	.bp
.DE
.B nroff
adjusts the spacings between words in a string but does not
hyphenate any word that is within a string.
If you use a very short line length, such as two inches, and define a
string that includes a three-inch long word, that word would
not be hyphenated but would extend past the right-hand margin.
.PP
You cannot include a newline character in a string.
However, you can
spread the definition of a string out over more than one line with the
aid of \fIconcealed\fR newlines (preceded by the backslash character
\*(Ql\e\*(Qr).
.B nroff
ignores each concealed newline.
For example, add the following string to the previous example:
.DM
	.ds PR "GO TEAM \e
	GO!!!"
.DE
As you can see,
.B nroff
ignores concealed newlines anywhere in its input.
.Sh "Strings Within Strings"
.PP
.II "nroff^string, within strings"
You can define a string that has embedded within it a
reference to another string.
Whenever you refer to the bigger string in your text,
.B nroff
substitutes the definition of the smaller string for any reference
to the smaller string.
When you embed strings, though, you should use \fItwo\fR backslashes
to refer to the embedded string, for the same reason
that you should use two backslashes to refer to
an argument within a macro:
.DM
	.ds S "This string \e\e*x has embedded \e\e*y strings"
.DE
To help understand this better, type following three scripts
into your computer and format them with \*(PN.
The first script contains proper references
to embedded strings (using double backslashes); it works as
expected:
.DM
	.ds S "strings \e\e*X, strings \e\e*Y, strings \e\e*Z"
	.ds X "here"
	.ds Y "there"
	.ds Z "everywhere"
	\e*S
.DE
The next script contains embedded references that use only
single backslashes.
Because the embedded strings are defined
after the larger string, they are not available when
.B nroff
defines the larger string, and so the references are ignored:
.DM
	.ds S "strings \e*X, strings \e*Y, strings \e*Z"
	.ds X "here"
	.ds Y "there"
	.ds Z "everywhere"
	\e*S
.DE
The third
script again contains embedded references using single backslashes.
This time, the embedded strings are defined \fIbefore\fR the
larger string, and so are available when the larger
string is defined:
.DM
	.ds X "here"
	.ds Y "there"
	.ds Z "everywhere"
	.ds S "strings \e*X, strings \e*Y, strings \e*Z"
	\e*S
.DE
To avoid unnecessary worry, you should always play it safe and use
double backslashes to refer to embedded strings.
.SH "Number Registers"
.PP
.II "nroff^register, number"
You learned in previous sections that
.B nroff
keeps track of the output page number while it prints its output.
You made use of this fact when you created a footer macro that printed page
numbers.
.B nroff
also keeps track of other housekeeping information,
such as the current line length, page offset,
page length, and vertical position of the last output line.
It keeps this information in
storage locations called \fInumber registers\fR.
.PP
You can use the \fIname\fR of a number register to refer to the number that is
stored in it.
When you place a reference to a number register in your text,
.B nroff
substitutes for the name whatever number is currently in the register.
.PP
Number register names are one or two characters long, just like
macro and string names.
You can have a number register with the same name as a string
or a macro without confusing
.BR nroff ,
even though you cannot give a macro and a string the same name.
However,
.I you
might become confused;
.B nroff
scripts usually are easier to understand
if you keep all macro names, string names, and register names distinct.
.PP
Another difference between number registers, macros, and strings is that
.B nroff
itself does not define any macros or strings (although the
.B \-ms
macro package does), but it does automatically
define and update quite a few number registers.
You can use these predefined number registers
in much the same way that you use registers you define yourself,
except that you cannot change their values.
.PP
.II "nroff^.nr primitive"
To define a number register, you must specify the
.I "register name"
and the
.I "initial value"
for the register.
The
.I "number register"
primitive
.B .nr
looks like this:
.DM
	.nr X 5
.DE
Here
.B X
is the name of the register and
.B 5
is the initial value to store in it.
To refer to number register
.B X
in your text, use \fB\enX\fR; if the name is two characters long
(for example, \fBXY)\fR, use \fB\en(XY\fR.
This is exactly like the way you refer to a string,
except that you use the letter \*(Qln\*(Qr instead of
an asterisk \*(Ql*\*(Qr.
When
.B nroff
sees a reference to number register
.BR X ,
it automatically substitutes the value stored in
.BR X .
As you will see shortly,
.B nroff
can do arithmetic, and learning to use number registers is an
important part of learning to take advantage of \fBnroff\fR's
arithmetic abilities.
.PP
A reference to a number register can occur anywhere a number would
normally occur.
For example, if you set register
.B X
to 5, as above,
you can set the line length to five inches as follows:
.DM
	.ll \enXi
.DE
This command is essentially the same as
.DM
	.ll 5i
.DE
if the current value of register
.B X
is 5.
.PP
A familiar problem arises when you refer
to a number register inside a macro or a string definition.
If you use just one backslash,
.B nroff
substitutes the value in the register
for the reference when it first processes the macro or string.
If you have not yet defined the number register in your script,
.B nroff
inserts
.B 0
into the macro or string.
Normally, you should use a
a double backslash, such as \fB\e\enX\fR or
\fB\e\en(XY\fR, when referring to a number register
within a macro or string.
Using the double backslash is
particularly important if you \fIchange\fR the value of the register
throughout your script, and want the current value to
appear in the macro or string each time you call it.
.PP
Try typing the following examples into your computer,
and processing them with \*(PN.
See if you can describe why
.B nroff
prints what it does in each case.
The first example defines a string with a register reference
preceded by a single backslash.
.DM
	.ds S "Here is a number \enX"
	.nr X 55
	\e*S
	\enX
.DE
You should see the following output:
.DM
	Here is number 0
	55
.DE
\*(PN printed what it did because number register X had not
yet been defined when it was called in string S;
\*(PN
therefore erased the reference to X and substituted zero for it.
Number register X was then set to 55, which was
printed when the register was specifically called later in the script.
.PP
The second example is similar, but now the number register is set
\fIbefore\fR the string is called:
.DM
	.nr Y 56
	.ds T "Here is a number \enY"
	\e*T
	\enY
.DE
Now the output is
.DM
	Here is a number 56
	56
.DE 
The third example uses a double backslash for the register reference.
.DM
	.ds U "Here is a number \e\enZ"
	.nr Z 57
	\e*U
	.nr Z 58
	\e*U
.DE
This script produces the following:
.DM
	Here is a number 57
	Here is a number 58
.DE
The final example uses a single backslash again.
.DM
	.nr W 59
	.ds V "Here is a number \enW"
	\e*V
	.nr W 60
	\e*V
.DE
The following is produced:
.DM
	Here is a number 59
	Here is a number 59
.DE
The last example illustrates the danger of using a single backslash
to refer to a number register within a string definition.
You defined the number register
.B W
before you defined the string
.BR V ,
so the value for
.B W
was available when
.B nroff
read the definition of
.BR V .
.B nroff
substituted the value when it reads the definition; the reference to the
number register
.B W
is no longer there.
You then change the value of
.BR W ,
but as you see in the next call of
.BR V ,
the change does not affect the number that appears in
.BR V .
In contrast to this, notice in the third example that the double
backslash in the definition of
.B U
allows the reference to number register
.B Z
to remain within the definition of string
.BR U .
Whenever you change the value of
.B Z
and then call
.BR U ,
.B nroff
substitutes the new value of
.B Z
for the reference to
.B Z
within
.BR U .
.PP
You can also use the
.B .nr
primitive to increase or decrease the value in a number register.
For example, suppose you initially store the value
five in \fBX\fR:
.DM
	.nr X 5
.DE
.Sh "Incrementing and Decrementing"
.PP
You can change the value of
.B X
to 9 by adding 4, as follows:
.DM
	.nr X +4
.DE
You can then change the value of
.B X
to 7 by subtracting 2:
.DM
	.nr X -2
.DE
A plus or minus sign before a number on the
.B .nr
command line tells
.B nroff
to add or subtract the given amount to or from the value in the register.
Because a negative number is always preceded by a minus sign
whereas a positive number usually is not preceded by a plus sign,
you can use
.B .nr
to set a register to a positive value in
a way that cannot be imitated for negative values.
For example, suppose you again start out with number register \fBX\fR set to
a value of 5:
.DM
	.nr X 5
.DE
If you immediately follow this with
.DM
	.nr X 7
.DE
then
.B nroff
replaces the value of 5 with 7.
The second
.B .nr
does not increase the value of
.B X
by 7 to produce 12;
rather, it wipes out the previous value of 5 and replaces it by the value 7.
The command line to increase
.B X
by 7 is
.DM
	.nr X +7
.DE
If you again start with a value of 5 in
.B X
and want to change the value to -4, you cannot use the following command line:
.DM
	.nr X -4
.DE
.B nroff
interprets this as a command to \fIdecrease\fR the current value of
.B X
by 4,
which is not what you intended.
This command places the value 1 in \fBX\fR, since 5-4=1.
If
.B X
initially has a value of 5 and you want to change the value to
\-4, you could use the command
.DM
	.nr X \-9
.DE
You can also
increase or decrease the value of a number register without using
.BR .nr .
If number register
.B X
currently has the value 10, the reference \fB\en+X\fR increases the value in
.B X
by 1 to 11 and substitutes the new value for the reference.
The value in
.B X
becomes 11;
.B nroff
replaces the next reference \fB\enX\fR by 11, whereas another
reference \fB\\n+X\fR increments the value in
.B X
to 12 and replaces the reference by 12.
Similarly, if number register
.B XY
currently has the value 15, the reference \fB\\n+(XY\fR increases the
value in
.B XY
to 16 and replaces the reference by 16.
.PP
You can also decrease a register's value.
The reference \fB\en-X\fR decreases the current value in
.B X
by 1 and substitutes the new value for the reference.
Likewise, the reference \fB\en-(XY\fR decreases the current value in
.B XY
by 1 and substitutes the new value for the reference.
.PP
You can change the size of the increment or decrement by means
of another option to the
.B nr
command.
If you define
.B X
with
.DM
	.nr X 1 5
.DE
then
.B nroff
sets the value of
.B X
to 1 and sets the increment value for
.B X
to 5.
The next reference \fB\en+X\fR increments the value in
.B X
from 1 to 6 (the \*(Ql+\*(Qr now causes
.B nroff
to add 5 to the current value of
.B X
rather than adding 1) and substitutes 6 for the reference.
In the
same manner, \fB\en-X\fR subtracts 5 from the current value of
.B X
and substitutes the new value for the reference.
This is convenient if you plan to repeatedly increment or decrement
.B X
by the same fixed amount.
If you wish to change the size of the increment, simply redefine
.B X
with another
.B .nr
that specifies the new initial and
increment values.
If you define a number register but do not specify an increment value,
.B nroff
assumes the increment value to be 1.
.PP
The following example of a macro illustrates a typical use of a number
register and incrementing.
.ID
.ta 0.5i 3.5i
	.nr W 1	\e" set W to 1, inc by 1
	.ds X "Here's Wrestler No. \e\enW,"	\e" define string X
	.de B	\e" define macro B
	.br
	\e\e*X \e\e$1!!!	\e" define arg to macro B
	.nr b \e\en+W	\e" increment W
	..	\e" end definition
	.B "Alex 'Killer' Bovine"	\e" call B with arguments
	.B "William 'Crusher' Risible"
	.B "Vlad 'the Impaler' Acephalous"
	.bp	\e" force printing of page
.IE
to produce the following output:
.DM
	Here's Wrestler No. 1, Alex 'Killer' Bovine!!!
	Here's Wrestler No. 2, William 'Crusher' Risible!!!
	Here's Wrestler No. 3, Vlad 'the Impaler' Acephalous!!!
.DE
.PP
A reference to a number register may appear any place a number
can normally appear.
For example:
.DM
	.nr X \enY \enZ
.DE
sets register
.B X
to the value of register
.B Y
and sets the increment for
.B X
to the value of register
.B Z.
.PP
As mentioned before,
.B nroff
performs arithmetic.
It understands and evaluates properly formed
arithmetic expressions involving numbers,
references to number registers,
the arithmetic operators \*(Ql+\*(Qr, \*(Ql-\*(Qr,
\*(Ql*\*(Qr, \*(Ql/\*(Qr, \*(Ql%\*(Qr, and parentheses.
The first four operators represent addition, subtraction, multiplication,
and division.
The \*(Ql%\*(Qr is the \fImodulus\fR or \fIremainder\fR operator:
the value of 7%3 is 1, which is the remainder when 7 is divided by 3.
.PP
.II "nroff^expression"
One word of caution:
.B nroff
evaluates expressions from left to right without
any preference for performing some operations before others.
For example,
.DM
	.nr X 5+4*3/9
.DE
stores 3 in
.B X.
.B nroff
does not perform the multiplication and division before the addition,
as you might expect.
.PP
Another important fact is that number registers hold only integers.
If you write
.DM
	.nr X 3.6
.DE
.B nroff
truncates the value 3.6 and stores 3 in
.BR X .
Also, an assignment such as
.DM
	.nr X 3.9*3.9
.DE
stores 9 in
.BR X ;
.B nroff
truncates each factor before it performs the multiplication.
The assignment
.DM
	.nr X 0.4*8
.DE
stores 0 in
.B X
rather than 3:
truncation occurs before
.B nroff
performs the multiplication rather than after.
.PP
.II "nroff^units"
.II "nroff^measurement, units"
A final word of caution:
when you use numbers with commands other than
.B .nr,
the results may
.I not
be what you expect.
.B nroff
understands several different units of measurement
and converts between units automatically.
The next section explains units and conversion in detail.
.Sh "Units of Measurement"
.PP
.II "nroff^.ll primitive"
As mentioned above,
.B nroff
maintains many number registers during processing.
For example, it stores the current page length in the
register
.B .l
(Note that the period \*(Ql.\*(Qr
is actually part of the name of this register.)
If you set the line length to five inches with the command
.DM
	.ll 5i
.DE
.B nroff
stores the length in register
.B .l
automatically; however, if you print the value in register
.B .l
by entering \fB\en(.l\fR, you find the value is 600.
What does this mean?
.PP
.II "nroff^measurement"
Many
.B nroff
commands require that you specify lengths or measurements as arguments.
You are already familiar with many of these commands: for example,
.B .ll,
.B .po,
.B .pl,
and
.BR .lt .
.B nroff
accepts various units of measurement, but
for purposes of calculation, it converts each into a basic unit called a
.IR "machine unit" ,
which is abbreviated
.BR u .
A machine unit is 1/120 of an inch long.
Because one inch is 120 machine units, the length of a
five-inch line is 5 times 120, or 600 machine units.
.PP
The conversion table for units of measurement is as follows:
.DS
.ta 0.8i 2.5i
	inch:	1\fBi\fR = 120\fBu\fR
	vertical line space:	1\fBv\fR = 20\fBu\fR
	centimeter:	1\fBc\fR = 47\fBu\fR
	em:	1\fBm\fR = 12\fBu\fR
	en:	1\fBn\fR = 6\fBu\fR
	pica:	1\fBP\fR = 20\fBu\fR
	point:	1\fBp\fR = 1\fBu\fR
.DE
Most of these are traditional typesetting terms.
.PP
As noted briefly earlier,
.BR nroff 's
output actually consists of a sequence of characters.
It is useful, though, to
think of the output as being \*(QLprinted\*(QR
at ten characters per inch (Pica or 10-pitch spacing) and six lines per inch.
Many output devices use this spacing.
With these assumptions, \fB5i\fR is equivalent to five inches of printed output.
.PP
.II "nroff^unit, default"
Every
.B nroff
command has a default unit of measurement.
For example, the default unit for
.B .ll
is
.BR m ,
whereas the default unit for
.B .sp
is
.BR v .
If you type
.DM
        .ll 5
.DE
.B nroff
interprets it not as five inches or five centimeters, but as 5\fBm\fR,
which it converts to 5 times 12, or 60 machine units (60\fBu\fR).
.PP
.B nroff
always assumes
a unit specification as part of each number and automatically converts each
number and its unit specification into machine units.
If you append an explicit unit specification to the number,
.B nroff
uses it; if you do not,
.B nroff
uses the default unit for the command.
.PP
For example, suppose you write the following commands:
.DM
        .nr X 2i
        .ll \enX
.DE
What line length results?
The first command stores the number 2 times 120, or 240, in register
.B X.
The second command is therefore equivalent to typing
.DM
	.ll 240
.DE
However, the default unit for
.B .ll
is
.B m.
Because 1\fBm\fR equals 12\fBu\fR,
.B nroff
sets the line length to 12 times 240, or 2,880 machine units.
If you wanted a line length of two inches
to result from the above commands, you will be unpleasantly surprised,
because 2\fBi\fR equals only 240\fBu\fR.
Instead, you should write:
.DM
        .nr X 2i
        .ll \enXu
.DE
By including the
.B u
in the
.B .ll
primitive, you do not accidentally multiply your results by 12,
as happened earlier.
.PP
You should think of the unit specification as a part of a number.
Because
.B nroff
accepts so many different units of measurement,
a number without a unit specification is ambiguous.
What does \*(Ql5\*(Qr mean?
Five inches?
Centimeters?
Ems?
.B nroff
must know what unit of measurement you are using.
If you think of the unit specification as a part of the number,
you will have less trouble with
potentially mystifying situations like the following.
As mentioned, number registers store only integers and
.B nroff
truncates each number in an arithmetic
expression to an integer before evaluating the expression.
Therefore, the following stores 0 in register
.BR X :
.DM
	.nr X 0.4*9
.DE
But now try the following:
.DM
	.nr X 0.4i
	\enX
.DE
This does not store 0 in
.B X
like the previous command; it stores 0.4 times 120, or 48 in
.BR X .
The 0.4 is not truncated to 0 here!
Truncation occurs
.I after
conversion to machine units, so
.B nroff
truncates 0.4\fBu\fR in the first example.
But the number in the second example is given in inches
.B i
instead of machine units
.BR u .
.B nroff
converts it to
.B u
before truncating to get an integer.
.PP
As another example, the following stores 1 in
.BR X :
.DM
	.nr X 0.01i
.DE
.B nroff
converts 0.01 inches to 0.01 times 120, or 1.2\fBu\fR, and then truncates 1.2
to 1.
.PP
The following command illustrates that
.B nroff
understands
.I each
number in an arithmetic expression
to have an attached unit specification, whether you supply one or not.
.DM
	.ll 2*8
.DE
Recall that
.B nroff
stores the current line length in the register
.B .l;
if you type
.DM
	\en(.l
.DE
you will receive the number 2,304.
.B nroff
interprets the 2 as
2\fBm\fR and the 8 as 8\fBm\fR, because the default unit for
.B .ll
is
.B m.
Then it converts each to machine units and multiplies
to give the result: (2*12)*(8*12), or 2,304.
.PP
.II "nroff^.po primitive"
Consider one final example that
illustrates the unusual consequences of seemingly innocent assignments.
Suppose you set the page offset as follows:
.DM
	.po 8/3
.DE
.B nroff
stores the current page offset in register
.BR .o .
To see what number it stores there, type
.DM
	\en(.o
.DE
You see that the page offset is 2.
Because the default unit for
.B .po
is
.B m,
the calculation is (8*12)/(3*12)=8/3, which
.B nroff
truncates to 2.
Two machine units is equivalent to only 1/60 of an inch.
This is not a physically reasonable value for most typewriter-like
devices, so a page offset of 0 characters results.
On the other hand,
.DM
	.po 8/3u
.DE
produces a page offset of approximately 1/4 of an inch.
.SH "Conditional Input"
.PP
.II "nroff^command, conditional"
.II "nroff^conditional input"
Now that you have been introduced to number registers, you can use them in
conjunction with powerful
.I "conditional commands"
to create more elaborate
.B nroff
scripts.
.PP
To see how conditional statements help you construct
an \*(PN script,
consider again the problem of creating header and footer macros.
Earlier, you constructed macros
that skipped space at the top of the page and
printed the page number at the bottom of each page.
.PP
Suppose, however, that you are formatting a paper that has a title.
You want to print the page
number for page 1 at the bottom of the page, and to print the rest of the
page numbers at the top of the page.
Both the header and the footer need some kind of conditional mechanism
to perform differently on the first page than on subsequent pages.
On page 1, the header should skip to where the
title will be printed; on other pages, the header should
print the page number.
On page 1, the footer should print the page number; on other pages,
the footer should leave a block of blank space at the bottom of the page.
.PP
.II "nroff^.ie primitive"
.II "nroff^.el primitive"
To execute commands conditionally, use the
.I "if/else"
commands
.B .ie
and
.BR .el ,
which are demonstrated in the following example.
Note that the formation \fL''\fR, which is used with
the \fB.tl\fR command, represents two apostrophes, 
\fInot\fR a quotation mark.
.ID
.ta 0.5i 1.75i
	.de HD	\e" define header
	.ie \e\en%=1 .A	\" if page no. is 1 then do A
	.el .B	\e" else do B
	..
	.de A	\e" define macro A
	.sp |1.0i	\e" space down to 1 inches from top of page
	..
	.de B	\e" define macro B
	'sp 2v	\e" skip 2 spaces
	.tl ''- % -''	\e" print page no.
	'sp |1.0i	\e" skip to 1 inch from top of page
	..
	.de FO	\e" define footer
	.ie \e\en%=1 .C	\e" if page no. is 1 then do C
	.el .D	\e" else do D
	..
	.de C	\e" define macro C
	'sp |-4v	\e" move to 4 in. above bottom of page
	.tl ''- % -''	\e" print page no.
	'bp	\e" begin new page
	..
	.de D	\e" define macro D
	'bp	\e" begin new page
	..
.IE
As you can see, the
.B .ie
and
.B .el
commands always occur in pairs.
.B .ie
consists of three parts:
the command name
.BR .ie ,
then a
.I condition
that
.B nroff
tests, followed by a
.I command
.B nroff
performs if the condition is true.
If the condition on the
.B .ie
command line is not true,
.B nroff
performs the command on the
.B .el
line instead.
.PP
In the example, each conditional invokes a macro on the command line.
Actually, the conditional can specify \fIinput\fR text rather than the
command after the condition.
If you want to execute several commands
or include several text lines conditionally,
enclose the lines with the special sequences \*(Ql\e{\*(Qr and
\*(Ql\e}\*(Qr.
.PP
Note, too, that one other new element was introduced in
the construction of these macros.
Some of the
.B .sp
commands have a vertical bar immediately in front of the measurement; for
example,
.DM
	.sp |1.0i
.DE
.II "nroff^measurement, absolute"
Normally, when
.B nroff
sees a command like
.B ".sp 1.0i,"
it moves down one inch on the output page.
The movement is relative to where
.B nroff
happens to be on the output page when it received the request.
The vertical bar tells
.B nroff
that the following measurement is an
.I absolute
measurement, measuring either from the top of the page (if
positive) or from the bottom of the page (if negative).
Therefore,
.DM
	.sp |1.0i
.DE
tells
.B nroff
to move to one inch from the top of the page;
.DM
	.sp |(-4v)
.DE
tells it to move to four vertical spaces from the bottom of the page.
.PP
The
.B .if
primitive
is formed exactly like
.BR .ie .
Unlike
.BR .ie ,
which must always be used with
.BR .el ,
the
.B .if
command may be used by itself.
If the condition on the
.B .if
line is true,
.B nroff
performs the command that follows the condition;
if the condition is false, it ignores the command altogether.
.PP
This chapter ends with two substantial examples
that incorporate most of what you have studied so far.
To illustrate the use
of conditionals, the first example begins each even paragraph of
output with the phrase
.B "Even Paragraph:"
and begins each odd paragraph
with the phrase \fBOdd Paragraph:\fR.
Type this into the file \fBex8.r\fR, and process it through
\*(PN without using the \fB\-ms\fR macro package, and as before,
there is no need to copy the comments:
.ID
.II "Bierce, Ambrose"
.ta 0.5i 1.75i
	.wh 0 HD	\e" set header trap
	.wh -2i FO	\e" set footer trap
	.nr EO 1	\e" set EO register to 1
	.po 2i	\e" page offset 2 inches
	.pl 6i	\e" page length 6 inches
	.lt 4i	\e" title length 4 inches
	.ll 4i	\e" line length 4 inches
	.de HD	\e" define header
	'sp |(1i-1v)	\e" space down to 1 inch minus 1 line
	.tl ''\e\e*(WS''	\e" set WS macro in title
	'sp |1.5i	\e" space down to 1.5 inches
	..
	.de FO	\e" define footer
	'sp |(3i+3v)	\e" space down to 3 inches plus 3 lines
	.tl ''- % -''	\e" set page number in footer
	'bp	\e" begin new page
	..
	.ds WS "From the Devil\e's Dictionary"
	.	\e" define string WS
	.de PP	\e" define paragraph macro
	.ie \e\en(EO=0 .EP	\e" if EO = 0 (even) then do EP
	.el .OP	\e" else do OP
	..
	.de EP	\e" define EP (even paragraph)
	.br
	.nr EO 1	\e" set register EO to 1
	.sp 1v	\e" skip 1 line
	.ll 4i	\e" set line length to 4 inches
	.lt 4i	\e" set title length to 4 inches
	\e\e*E	\e" insert string E
	..
	.ds E "Even Paragraph:"
	.	\e" define string E
	.de OP	\e" define macro OP (odd paragraph)
	.br
	.nr EO 0	\e" set register EO to 0
	.sp 1v
	.ll 3i	\e" set line length to 3 inches
	.lt 3i	\e" set title length to 3 inches
	\e\e*O	\e" insert string O
	..
	.ds O "Odd Paragraph:"
	.	\e" define string O
	.PP
	Debt, n. An ingenious substitute for the whip
	and chain of the slave-driver.
	.PP
	Bore, n. One who talks when you wish him to listen.
	.PP
	Brandy, n. A cordial composed of one part
	thunder-and lightning, one part remorse, two parts
	bloody murder, one part death-hell-and-the-grave,
	and four parts clarified Satan.
	.PP
	Responsibility, n. A detachable burden easily
	shifted onto the shoulders of God, Fate, Fortune,
	Luck, or one's neighbor.
.IE
This example
uses an \*(QLeven/odd\*(QR register called \fBEO\fR
to determine whether you are beginning an even or an odd paragraph.
To distinguish between even and odd paragraphs, it uses a line length
of four inches for even paragraphs and one of of three inches for
odd paragraphs.
It changes the title length with each paragraph, so
.B nroff
centers the page number with respect to whichever kind of
paragraph happens to occur at the bottom of a page.
.PP
The final example illustrates a loop constructed with
the \fBif\fR/\fBelse\fR commands.
The first paragraph is six inches long with no
page offset; each succeeding paragraph is one inch shorter with a
page offset one inch longer.
The line length of the sixth paragraph is one inch;
the next paragraph renews the cycle with a
six-inch line length.
Type this into file \fBex9.r\fR, and process it as you did
the above example:
.II guillotine
.II gallows
.ID
.ta 0.5i 1.75i
	.nr PO 0 1	\e" set register PO to 0, increment by 1
	.de PP	\e" define paragraph macro
	.ie \e\en(PO=6 .A	\e" if register PO=6 then do A
	.el .B	\e" else do B
	..
	.de A	\e" define macro A
	.br
	.nr PO 0	\e" set register PO to 0
	.nr LL 6-\e\en(PO	\e" set register LL to 6 minus PO
	.ll \e\en(LLi	\e" set line length to LL inches
	.po \e\en(POi	\e" set page offset to PO inches
	.nr PO \e\en+(PO	\e" increment register PO
	.sp	\e" skip a space
	..
	.de B	\e" define macro B
	.br
	.nr LL 6-\e\en(PO	\e" set LL to 6 minus PO
	.ll \e\en(LLi	\e" set line length to LL inches
	.po \e\en(POi	\e" set page offset to PO inches
	.nr PO \e\en+(PO	\e" increment register PO
	.sp	\e" skip a space
	..
	.PP
	Future, n. That period of time in which our affairs prosper,
	our friends are true, and our happiness is assured.
	.PP
	Gallows, n. A stage for the performance of miracle plays, in
	which the leading actor is translated into heaven.
	.PP
	Geneaology, n. An account of one's descent from an ancestor
	who did not particularly care to trace his own.
	.PP
	Guillotine, n. A machine which makes a Frenchman shrug
	his shoulders with good reason.
	.PP
	History, n. An account most false, of events
	most unimportant, which are brought about by
	rulers mostly knaves, and soldiers mostly fools.
	.PP
	Idiot, n. A member of a large and powerful tribe
	whose influence in human affairs has always been
	dominant and controlling.
	.PP
	Kiss, n. A word invented by the poets as a rhyme
	for "bliss".
.IE
You should try this example
to see verify that \*(QLloop\*(QR works as advertised.
.SH "Environments and Diversions"
.PP
Another aspect of \*(PN's power is the ability to shift from one
.I environment
to another.
.PP
The
\*(PN
.I environment
is the overall manner in which
.B nroff
processes your input text.
The environment's definition includes such aspects as
line length, fill and adjust modes, and indentation.
.PP
.II "nroff^.ev primitive"
.II "nroff^command, environment"
.B nroff
allows you to define three independent environments, called \fB0\fR,
\fB1\fR, and \fB2\fR.
In each, you can set as you wish such parameters as line length,
filling, adjustment, and indentation.
You can call a different environment with the
.B .ev
command; the parameters you define for the new environment control
text processing until you change them within the present
environment or shift to another environment.
.PP
.II "nroff^command, page offset"
.II "nroff^.po primitive"
Not all
.B nroff
parameters change when you switch to a new environment.
For example, different environments do
.I not
have independent page offsets; the
.B .po
command affects all environments.
Parameters that may be set to different values in different environments are
.IR "environmental parameters" ;
parameters that cannot be switched according to environment, like page offset,
are
.IR "global parameters" .
Macro and string definitions are global.
.PP
When you first call
.B nroff,
you are by default in environment 0.
In all the examples used in this tutorial thus far,
everything happened in environment 0.
The following example illustrates how to switch back and forth
between environments.
Type in the following \fBex10.r\fR and process it to
see the output as you go along.
.ID
.II "Book of Proverbs"
.ta 0.5i 1.75i
	.po 1i	\e" set global page offset to 1 inch
	.ll 4i	\e" set line length in ev 0 to 4 inches
	.de PP	\e" define paragraph macro
	.sp
	.ti 0.5i	\e" indent first line 1/2 inch
	..
	.PP
	The heart of the righteous studieth to answer,
	but the mouth of the wicked poureth out evil things.
	.br
	.ev 1	\e" switch to environment 1
	.ll 3i	\e" set line length to 3 inches
	.ls 2	\e" line spacing now double space
	.PP
	A froward man soweth strife, and a whisperer
	separateth chief friends.
	.br
	.ev	\e" return to previous ev (0)
	.PP
	It is naught, it is naught, sayeth the buyer;
	but when he is gone his way, then he boasteth.	
	.br
	.ev 1	\e" switch to ev 1
	.PP
	Wealth maketh many friends; but the poor is separated
	from his neighbors.
	.br
	.ev	\e" return to ev 0
.IE
The first
.B .ll
command sets a line length of four inches in environment 0.
After defining the paragraph macro
.B .PP
and an initial paragraph in environment 0, you switched to environment 1
with the command
.DM
	.ev 1
.DE
You now enter a new environment.
If you do not explicitly set environmental parameters, such as line length,
.B nroff
automatically uses default values for them.
.B nroff
assigns the same default values in environments 1 and 2 as it does in
environment 0.
.PP
.II "nroff^.ls primitive"
.II "nroff^command, line space"
The line length in environment 1 is set to three inches with
the output text double-spaced.
The
.I "line space"
primitive
.DM
	.ls 2
.DE
leaves one blank line between each output line.
Thus, paragraphs processed in environment 0 have four-inch single-spaced lines,
whereas paragraphs processed in environment 1 have three-inch
double-spaced lines.
.PP
The example used the command line
.DM
	.ev
.DE
without an argument to \fIleave\fR environment 1.
This leaves environment 1 and restores (or \*(QLpops\*(QR) the previous
environment \(em in this case, environment 0.
The next time you enter environment 1, you will not need to
set the line length to three inches again:
the value stays in effect in environment 1 until you specifically change it.
The same is true of all environmental parameters.
.PP
To understand how
.B nroff
switches between environments, imagine that you
have a set of plates, each marked with either a \fB0\fR, a \fB1\fR,
or a \fB2\fR.
You have as many plates of each type as you wish.
You stack the plates on a table;
the top plate represents your current environment.
You begin with a \*(Ql0\*(Qr plate on the table, to
represent the initial environment when you enter
.BR nroff .
.PP
.II "nroff^stack, environment"
Switching to environment 1 with the command
.B ".ev 1"
corresponds to placing a \*(Ql1\*(Qr plate on top of the \*(Ql0\*(Qr plate.
You can again change the stack of two plates either by
placing a new plate on top of the stack,
or by removing the top plate from the stack:
the former corresponds to calling a new environment, whereas the latter
corresponds to restoring the previous environment with the command line
.BR .ev .
.PP
Because you can have as many plates of each type as you wish,
you can call environment 1, then call environment 2, then
restore environment 1, then call environment 0, and so on.
The command \fB.ev\fI N\fR, where
.I "N"
is 0, 1, or 2, places (or \*(QLpushes\*(QR) a plate
onto the stack; the command
.B ".ev"
removes (or \*(QLpops\*(QR) the top plate from the stack.
.PP
To illustrate this, add the following text to the end of the previous example.
Use a piece of paper and pencil to keep track of how the
.B .ev
commands add or remove environments.
Because the line lengths are different in each environment,
it should be easy to tell in which environment
.B nroff
has processed each paragraph:
.ID
.II "Book of Proverbs"
.ta 0.5i 1.75i
	.ev 2	\e" introduce environment 2
	.ll 5i	\e" set line length
	.in 1i	\e" set indentation
	.PP	\e" paragraph in ev 2
	A poor man that oppresseth the poor is like
	a sweeping rain which leaveth no food.
	.br
	.ev 0	\e" push ev 0
	.PP
	As a roaring lion, and a ranging bear; so is
	a wicked ruler over the poor people.
	.br
	.ev 1	\e" push ev 1
	.PP
	Wrath is cruel, and anger is outrageous;
	but who is able to stand before envy?
	.br
	.ev 2	\e" push ev 2
	.PP
	A good name is rather to be chosen than
	great riches; and loving favour rather than
	silver and gold.
	.br
	.ev 0	\e" push ev 0
	.PP
	Pride goeth before destruction, and an haughty
	spirit before a fall.
	.br
	.ev	\e" return to ev 2
	.ev	\e" return to ev 1
	.PP
	He that answereth a matter before he heareth it,
	it is folly and shame unto him.
	.br
	.ev	\e" return to ev 0
	.ev	\e" return to ev 2
	.PP
	A merry heart doeth good like a medicine, but a
	broken spirit drieth the bones.
	.br
.IE
.SH "Buffers"
.PP
Earlier, it was shown that
.B nroff
uses a buffer to assemble words from its input into output lines.
Actually, each environment has its own buffer.
Switching to a new environment does
.I not
cause a break.
Suppose you are currently in environment 1 with an
unfinished line in the buffer.
When you give the command \fB.ev 2\fR, the unfinished line
remains undisturbed in the environment 1 buffer until you return to
environment 1.
Text you process in the meantime in environment 2 or in
environment 0 has no effect on the partial line in the environment 1 buffer,
because
.B nroff
assembles text processed in other environments in different buffers.
.PP
In the following example, you process some text in environment 0 and then
switch to environment 2.
Any partial line collected in
environment 0 when you switch to environment 2 waits patiently in
the buffer until you return to environment 0 and issue the break command
to flush the buffer.
You then return to environment 2 and flush any partially filled
line left when you restored environment 0.
Enter the following into the file \fBex11.r\fR and process it
through \*(PN:
.DM
.ta 0.5i 1.75i
	.ll 3i	\e" set line length in ev 0
	.po 2i	\e" set page offset in ev 0
	This is environment 0.
	.ev 2	\e" introduce ev 2
	This is environment 2
	.br	\e" flush ev 2 buffer
	.ev	\e" return to ev 0
	.br	\e" flush ev 0 buffer
.DE
As you can see, the order of the two sentences is reversed
from the way you entered them.
If you were to delete the \fB.br\fR commands after the
texts in \fBex10.r\fR, the output would be very badly affected.
.Sh "Headers and Footers"
.PP
.II "nroff^.lt primitive"
A common use of environment switching is for the creation of header and
footer macros.
As the following example demonstrates, the length of title set by the
\fB.lt\fR command
is an environmental parameter.
The following constructs
header and footer macros that print strings of asterisks in the margins
above and below the text; type it into your computer as \fBex12.r\fR:
.ID
.ta 0.5i 1.75i
	.wh 0 HD	\e" set header trap 
	.wh |2.5i FO	\e" set footer trap
	.de HD	\e" define header macro
	.ev 1	\e" define ev 1
	.lt 5i	\e" set title length to 5 inches
	'sp 3v	\e" move down three spaces
	.tl '****''****'	\e" define header title
	'sp 2v	\e" skip two more spaces
	.ev	\e" pop environment
	..
	.de FO	\e" define footer macro
	'sp 2
	.ev 1	\e" push ev 1
	.tl '****'%'****'	\e" define footer title
	.ev	\e" pop environment
	'bp	\e" begin new page
	..
	.ll 4i	\e" set line length in ev 0
	.pl 3i	\e" set page length
	.in 1i	\e" set indentation
	.po 2i	\e" set page offset
	.de PP	\e" define paragraph macro
	.sp 1
	.ti 0.5i	\e" indent 1st line 1/2 inch
	..
	.PP
	When in the course of human events ...
.IE
The following section explains why header and footer
macros often use a different environment.
.Sh "More About Fonts"
.PP
As earlier described in some detail,
.B nroff
output includes representations for
.B boldface
and
.I italic
characters,
in addition to the normal Roman characters.
The visual appearance of boldface and italic characters depends
on the device you use to print your
.B nroff
output.
.PP
If you want a word or a phrase to appear in boldface,
enclose the word or phrase between \fB\efB\fR and \fB\efR\fR:
.DM
	The last word of this sentence appears in \efBboldface\efR.
.DE
The sequence \fB\efB\fR tells
.B nroff
to print in boldface, whereas the sequence \fB\efR\fR tells
.B nroff
to return to the Roman font.
Italics are used in a similar manner:
.DM
	An entire phrase \efIappears in italics\efR.
.DE
.II "nroff^.ft primitive"
To print more than a few words in a different font, you should use the
.I font
command
.B .ft:
.DM
	.ft I
	Here is text you want to
	appear in italics.
	.ft R
.DE
The initial
.B ".ft I"
switches the print to italic font, and the concluding
.B ".ft R"
returns it to Roman font.
As you might have suspected, the command
.B ".ft B"
switches to boldface.
.PP
You have two additional options when you use the
.B .ft
primitive.
The command
.B ".ft P"
returns to the
.I previous
font.
You can use
.B ".ft P"
within a macro or a string to return to the previous output font,
even though you do not know which font was previously in effect.
You can also use the sequence \fB\efP\fR to return to the previous font.
The
.B .ft
primitive without an argument tells
.B nroff
to return to the Roman font.
.PP
In scripts that frequently change fonts,
you should switch to a new environment for header and footer macros,
in order to protect their font settings.
.Sh "Diversions"
.PP
.II "nroff^diversion"
The
.I diversion
is a powerful feature that allows you to suspend outputting lines
until \*(PN has collected all of a block of text.
For example, suppose you use
.B nroff
to format a chapter of a book.
The chapter includes footnotes at various places in the text; you want
.B nroff
to collect these footnotes and print at the end of the chapter.
To do this, \*(PN must store the processed footnote text somewhere
until the end of the chapter, when you want it printed.
Where do you store the text until the time comes for it to appear?
To handle situations like this,
.B nroff
provides a
.I diversion
mechanism:
you can
.I divert
text into temporary storage within a macro.
.PP
Diversion normally involves passing to a new environment to process the
footnote without causing a break in the main environment.
When the text of the diversion ends,
.B nroff
returns to the main environment,
again without causing a break, and continues processing
just as if the text of the note had never been in the input.
.PP
.II "nroff^.di primitive"
However, before you attempt to write a footnote macro,
type the following text into the file \fBex13.r\fR, and process it with \*(PN.
This example illustrates the basic features of diversion.
The example exchanges two paragraphs of text, so that
.B nroff
prints the second before the first.
.ID
.II "Book of Proverbs"
.ta 0.5i 1.75i
	.di X	\e" divert the following to macro X
	.sp
	A soft answer turneth away wrath:
	but grievous words stir up anger.
	.br	\e" send last line of paragraph to X
	.di	\e" end diversion
	.sp
	He that is slow to anger is better than the
	mighty; and he that ruleth his spirit than he
	that taketh a city.
	.br
	.sp
	.X	\e" print the paragraph diverted to X
.IE
.II "nroff^command, divert"
The new command here is the
.I divert
primitive
.BR .di .
The command
.B ".di X"
tells
.B nroff
to divert the text that follows into macro
.BR X ;
the matching
.B .di
with no argument marks the end of the diversion.
.PP
The break is necessary before the end of the diversion because
.B nroff
diverts
.I processed
text into the macro.
Without the break,
.B nroff
would not divert any partially filled line in its buffer to
.BR X ;
the last few words of diverted text might not form a complete line in the
buffer, so
.B nroff
might not divert them.
However, if you break the input before you end the diversion,
.B nroff
will also divert those last few words.
.PP
As you saw earlier, the \fB.br\fR command
must be used to flush that environment's buffer
before switching environments.
.PP
The next example, \fBex14.r\fR, illustrates a similar point.
.DM
.ta 0.5i 1.75i
	.br	\e" clear buffer
	testword	\e" put 'testword' into buffer
	.di X	\e" divert to X
	Piracy, n. Commerce without its folly-swaddles,
	just as God made it.
	.br	\e" divert last line
	.di	\e" end diversion
	.X	\e" print text in X
.DE
.II "nroff^.di primitive"
Here
.B nroff
diverts
.B testword
into
.B X
along with the text between
.B ".di X"
and \fB.di\fR.
Why did this happen?
The command
.B ".di X"
does \fInot\fR cause a break.
Because you did not pass to a new environment in this example before
you diverted,
.B nroff
formed the diversion text in the same buffer in which it stored
.BR testword .
You did not break the input, so
.B nroff
appended the diverted text to
.BR testword .
.PP
To make sure
.B nroff
diverts only text between
.B ".di X"
and
.B .di
into
.BR X ,
do one of the following:
If you want to process the diverted
text within the current environment, empty the buffer by inserting the
.B .br
command before you start the diversion.
If you switch to a new environment before starting the diversion, flush
the buffer for the new environment before you begin to process
diverted text.
.PP
.II "nroff^.da primitive"
Diverting processed text into a macro that already holds material
will erase whatever had already been stored there.
In some cases, such as the footnote example, you need to append
information into the same macro.
The
.I "divert and append"
variation
.B .da
of the diversion construction allows you to do so.
The following example, \fBex15.r\fR, demonstrates this command:
.ID
.II "Bierce, Ambrose"
.ta 0.5i 1.75i
	.ll 3i	\e" set line length
	.po 2i	\e" set page offset
	.de PP	\e" define paragraph macro
	.br
	.sp 1
	.ti 0.5i	\e" indent first line 1/2 inch
	..
	.di X	\e" divert the following into X
	.PP
	Litigation, n. A machine which you go into as a pig
	and come out of as a sausage.
	.br
	.di	\e" end diversion
	.X	\e" print what is in X
	.br
	.da X	\e" divert and append material into X
	.PP
	Inventor, n. A person who makes an ingenious arrangement
	of wheels, levers and springs, and believes it
	civilization.
	.br
	.di	\e" end diversion
	.X	\e" print what is now in X
.IE
In this example, you first diverted a single paragraph into the macro \fBX\fR.
.B nroff
stored in
.B X
the
.I processed
paragraph; in other words, the command line
.B .PP
is
.I not
stored in
.BR X ;
its
.I output
is.
When you invoke
.B X
with the command line \fB.X\fR,
.B nroff
takes the processed text in
.B X
as input.
To
.B nroff,
there is no difference between processed text and unprocessed text as input:
it processes the contents of
.B X
in the current environment, just like any other text.
Therefore,
.B nroff
processes diverted text \fItwice\fR: first when it
stores the text within the macro, and again when you invoke the macro.
.PP
The fact that
.B nroff
processes diverted text twice can cause problems if you are not careful.
Fortunately, nothing strange happens in the example above.
You store a processed paragraph with lines three inches long in
.BR X .
When you invoke
.BR X ,
the line length is three inches.
Because each line in
.B X
is already exactly three inches long, nothing happens to it when
reprocessed; the layout of the output paragraph is unchanged.
.PP
But now, consider what happens in the following example, \fBex16.r\fR:
.ID
.II "Bierce, Ambrose"
.ta 0.5i 1.75i
	.ll 3i	\e" set line length
	.po 2i	\e" set page offset
	.de PP	\e" define PP macro
	.sp 1
	.ti 0.5i	\e" indent first line 1/2 inch
	..
	.di X	\e" divert following into X
	.ev 2	\e" push environment 2
	.ll 4i	\e" set line length to 4 inches
	.PP
	Justice, n. A commodity which in a more or less
	adulterated condition, the State sells to the
	citizen as a reward for his allegiance, taxes
	and personal service.
	.br
	.ev	\e" pop environment (return to ev 0)
	.di	\e" end diversion
	.X
.IE
A pargraph processed in environment 0 in this example
has three-inch lines; you want your diverted paragraph to have
four-inch lines.
However, when you print the diverted paragraph with the command line
\fB.X\fR, what happened?
.B nroff
did
.I not
print four-inch lines.
The four-line text lines set in environment 2 were
reprocessed into three-inch lines when the diversion
macro is called in environment 0.
.PP
There are two ways to prevent such disasters.
First, if you wish to invoke
.B X
in the main environment, use no-fill mode:
.DM
.ta 0.5i 1.75i
	.nf	\e" begin no-fill mode
	.X
	.fi	\e" return to fill mode
.DE
In no-fill mode,
.B nroff
outputs lines of input exactly as it receives
them, so it keeps four-inch lines four inches long and does not change the
format of the diverted text.
The second strategy is to return to environment 2 and then invoke
.BR X ;
again, the format of the diverted paragraph does not change, because the line
length in environment 2 is four inches.
.DM
.ta 0.5i 1.75i
	.ev 2	\e" push environment 2
	.X
	.ev	\e" restore original environment
.DE
.Sh "A Footnote Macro"
.PP
The footnote macro that follows
does not print notes at the bottom of each page;
rather, it prints everything at the end of the chapter.
In the processed text, number register \fBFn\fR is
used to keep track of the footnote number;
the footnote number will be printed in
square brackets where the footnote originally appeared in the text.
.PP
Type this macro into the file \fBex17.r\fR.
If you wish to use it in your text processing, transfer it to
the directory \fB/usr/lib\fR under the name \fBtmac.fn\fR.
Then, whenever you wish to use this macro, be sure to include the option
.DM
	\-mfn
.DE
when you invoke \*(PN:
.DM
.ta 0.5i 1.75i
	.de FN	\e" define macro FN
	[\e\en+(Fn]	\e" print footnote no. in main text
	.ev 1	\e" push environment 1
	.da Z	\e" divert and append following into Z
	.sp
	\e\en(Fn. \e\e$2, \e\efI\e\e$1\e\efR,
	    \e\e$3, \e\e$4.	\e" format & print footnote in Z
	.br	\e" flush diversion buffer
	.di	\e" end diversion
	.ev	\e" pop environment (return to ev 0)
	..
.DE
Note that requests to change fonts are preceded by double backslashes,
because they are within a macro.
The change to the italic font prints the first
macro argument, which should be the title of the work, in italics.
Number register
.B Fn
contains the number of the last footnote;
you should initialize it with the command
.DM
	.nr Fn 0 1
.DE
As shown above, each footnote entry in your text should have
four arguments.
In your input text, each footnote will look like this:
.DM
.II "Burton, Sir Richard"
	.FN "Personal narrative of a pilgrimage to\e
	El-Medinah and Mecca" "Richard F. Burton"\e
	London 1856.
.DE
When you print the diversion
.B .Z
at the end of the chapter,
each footnote will be laid out as follows:
.nf
.sp \n(pDu
.ft L
.ta 0.5i 1.0i
	8.	Richard F. Burton,
.ul
		Personal narrative of a pilgrimage to
.ul
		El_Medinah and Mecca,
		London, 1856.
.sp \n(pDu
.fi
.R
.SH "Command Line Options"
.PP
In the previous sections, you learned how to control
.B nroff
by including
.I commands
in the input along with the
.I text.
You can also supply information
in another way:  on the command line you type to call
.BR nroff .
Unlike the commands discussed above, this information is
.I not
part of the script you input into \*(PN.
.PP
You already know about some simple
.B nroff
command lines.
For example, the command
.DM
	nroff
.DE
forces \*(PN to accept input from the keyboard (sometimes called the
.I "standard input)"
and print output on the terminal (the
.I "standard output)."
Type
.B <ctrl-D>
(that is, hold down the
.B ctrl
key and type
.BR D )
to exit from
.B nroff
if it is reading input from your terminal.
.PP
The command line
.DM
	nroff script1.r
.DE
forces \*(PN to take accept input from the file
.B script1.r
instead of from your terminal, while
the command
.DM
	nroff \-ms script.r
.DE
processes
.B script1.r
with the
.B \-ms
macro package.
You can also redirect
.B nroff
output to another file
.B target:
.DM
	nroff \-ms script1.r >target
.DE
.PP
The general form of the
.B nroff
command line is:
.DM
	\fBnroff\fI [ option ... ] [ file ... ]\fR
.DE
This means that the command line consists of the \*(PN
command, followed by zero or more \fIoption\fRs, followed by zero
or more \fIfile\fRs.
.B nroff
processes each named
.I file
and prints the result on the standard output
(the terminal, unless redirected).
If no
.I file
argument is given,
as in the first example above,
.B nroff
reads from the standard input.
.PP
Each
.I option
on the command line must begin with a hyphen \*(Ql\-\*(Qr
to distinguish it from a
.I file
specification.
Using
.B nroff
with the
.B \-ms
macro package is one example of entering an option.
.II "nroff^/usr/lib/tmac"
In general, the
.B \-m
option takes the form
.DM
	\-\fBm\fIname\fR
.DE
which means the option consists of the characters
.B \-m
immediately followed by a
.I name.
This tells
.B nroff
to process the macro package found in the
\*(CO file
.DM
        \fB/usr/lib/tmac.\fIname\fR
.DE
For example, the
.B ms
macro package discussed in chapter 2 is in the file
.B /usr/lib/tmac.s,
whereashe
.B man
macro package used
for the
.B man
command and to process manual pages
is in the file
.B /usr/lib/tmac.an.
.PP
Any macro packages that you customize for your own use
should be stored in the directory \fB/usr/lib\fR
under such a name if you wish to use
them with the \fB\-m\fIname\fR option.
.PP
The
.B \-i
option tells
.B nroff
to read input from the standard input
after processing each given
.I file.
This allows you to supply additional input interactively from your terminal.
.PP
The
.B \-x
option tells
.B nroff
not to move to the bottom of the last output page when done.
This is especially useful if you want to see
the output on the screen of a CRT terminal.
.PP
The \-\fBn\fIN\fR option
sets the page number of the first output page to the number
.I N,
rather than starting at page 1.
This is useful for processing large documents
with input text in several files which
.B nroff
processes separately.
.PP
The \-\fBr\fIXN\fR option sets the value of number register
.I X
to
.I N.
This option lets you initialize number registers when you invoke
.BR nroff .
.PP
The \*(CO system provides many useful features which
can be helpful while you are using
.B nroff.
In particular, you can use a number of special characters.
The
.I "stop-output"
and
.I "start-output"
characters, usually
.B <ctrl-S>
and
.B <ctrl-Q>,
stop and restart output on your terminal.
The
.I interrupt
character, usually
.BR <ctrl-C> ,
interrupts program execution;
you can use it to stop an
.B nroff
command if you typed the command line incorrectly.
The
.I kill
character, usually \fB<ctrl-\fR\\>,
also terminates program execution.
Some \*(CO systems use different characters than those
mentioned above;
consult
.I "Using the COHERENT System"
for details.
.SH "For Further Information"
.PP
The Lexicon entry for \fBnroff\fR summarizes its primitives, dedicated
number registers, escape sequences, and command-line options.
.PP
For example, this manual was typeset by \*(CO
.BR troff .
The program \fBtroff\fR also performs text formatting.
Unlike
.BR nroff ,
however,
.B troff
produces proportionally spaced output that can be
printed on printers that support the Hewlett-Packard Page Control Language
(including the LaserJet and DeskJet families of printers) or on any
printer that implements the PostScript page-control language.
This manual (including the positioning of the fancy capital letters and
ornaments) was typeset by
.B troff
under the \*(CO system.
See the Lexicon entry for \fBtroff\fR for details on how to use this
command.
.PP
The Lexicon also has entries for two macros packages that are included
with the \*(CO system:
.B man ,
which produces manual pages similar to those that appear in the Lexicon;
and
.BR ms ,
which performs formatting somewhat similar to that seen in this tutorial.
You will find that these two packages already perform practically all
of the formatting tasks that you will commonly need to do.
.PP
The error messages generated by
.B nroff
are given in the appendix at the rear of this manual.
