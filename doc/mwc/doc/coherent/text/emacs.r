.II "MicroEMACS^tutorial"
.II "me^tutorial"
.ds TL "MicroEMACS Screen Editor"
.NH "Introduction to MicroEMACS" 1
.PP
This section introduces \*(ME, the interactive screen editor
for \*(PN.
.SH "What is MicroEMACS?"
.PP
\*(ME is an interactive screen editor.
An \fIeditor\fR lets you type text into your computer,
name it, store it, and recall it later for editing.
\fIInteractive\fR means that \*(ME accepts an
editing command, executes it, displays the results for
you immediately, then waits for your next command.
\fIScreen\fR means that you can use nearly the
entire screen of your terminal as a writing surface:
you can move your cursor up, down, and around your
screen to create or change text, much as you move your
pen up, down, and around a piece of paper.
.PP
These features, plus the others that will be described in
the course of this tutorial, make \*(ME powerful yet easy to use.
You can use \*(ME to create or change computer programs
or any type of text file.
.PP
This version of \*(ME was developed by \*(MW from the public-domain
program written by David G. Conroy.
This tutorial is based on the descriptions in his essay
.IR "MicroEMACS: Reasonable Display Editing in Little Computers" .
\*(ME is derived from the mainframe display editor
EMACS, created by Richard Stallman.
.PP
For a summary of \*(ME and its commands, see the entry for
\fBme\fR in the Lexicon.
.SH "Keystrokes: <ctrl>, <esc>"
.PP
The \*(ME commands use \fBcontrol\fR characters and \fBmeta\fR characters.
Control characters use the \fIcontrol\fR key, which is marked \fBControl\fR
or \fBctrl\fR on your keyboard.
Meta characters
use the \fIescape\fR key, which is marked \fBEsc\fR.
.PP
.II "MicroEMACS^<ctrl>"
\fBControl\fR
works like the \fIshift\fR key:
you hold it down \fIwhile\fR you strike the other key.
This tutorial represent it with a hyphen; for
example, pressing the control key and the letter \*(QlX\*(Qr
key simultaneously will be shown as follows:
.DM
	<ctrl-X>
.DE
The \fBesc\fR key, on the other hand, works like an ordinary character.
You strike it first, \fIthen\fR strike the letter character you want.
This tutorial does \fInot\fR represent the
\fIEscape\fR codes with a hyphen; for example, it represents
\fBescape X\fR as:
.DM
	<esc>X
.DE
.SH "Becoming Acquainted with MicroEMACS"
.PP
Now you are ready for a few simple exercises that will help
you get a feel for how \*(ME works.
.PP
To begin, type the following command to \*(OS:
.DM
	me sample
.DE
Within a few seconds, your screen will have been cleared of writing,
the cursor will be positioned in the upper left-hand corner of
the screen, and a command line will appear at the bottom of your
screen.
.PP
Now type the following text.
If you make a mistake, just backspace over it and
retype the text.
Press the carriage return or enter key
after each line:
.DM
	main()
	{
		printf("Hello, world!\en");
	}
.DE
Notice how the text appeared on the screen character by character
as you typed it, much as it would appear on a piece
of paper if you were using a typewriter.
.PP
Now, type
\fB<ctrl-X><ctrl-S>\fR;
that is, type \fB<ctrl-X>\fR,
and then type \fB<ctrl-S>\fR.
It does not matter whether you type capital or lower-case letters.
Notice that this message has appeared at the bottom of your
screen:
.DM
	[Wrote 4 lines]
.DE
This command has permanently stored, or \fIsaved\fR, what you
typed into a file named \fBsample\fR.
.PP
Type the next few commands, which demonstrate some of the tasks that
\*(ME can perform for you.
These commands will be explained in full in the sections
that follow; for now, try them to get a feel
for how \*(ME works.
.PP
Type \fB<esc><\fR.
Be sure that you type a less-than symbol
\&\*(Ql<\*(Qr, which on most keyboards
is located just above the comma.
Notice that the cursor has returned to the upper left-hand corner
of the screen.
Type \fB<esc>F\fR.
The cursor has jumped forward by one word, and is now on the left parenthesis.
.PP
Type \fB<ctrl-N>\fR.
Notice that the cursor has jumped to the next line, and is now
just to the right of the left brace \*(Ql\fB{\fR\*(Qr.
.PP
Type \fB<ctrl-A>\fR.
The cursor has jumped to the \fIbeginning\fR of the second line of your text.
.PP
Type \fB<ctrl-N>\fR again.
Now the cursor is at the beginning of the
third line of the program, the \fBprintf\fR statement.
.PP
Now, type \fB<ctrl-K>\fR.
The third line of text has disappeared, leaving an empty space.
Type \fB<ctrl-K>\fR again.
The empty space where the third line of text had been has now
disappeared.
.PP
Type \fB<esc>>\fR.
Be sure to type a greater-than symbol
\&\*(Ql>\*(Qr, which on most keyboards is just above the period.
The cursor has jumped to the space just below the last line of text.
Now type \fB<ctrl-Y>\fR.
The text that you erased a moment ago has reappeared, but in a
new position on the screen.
.PP
By now, you should be feeling more at ease with typing
\*(ME's \fIcontrol\fR and \fIescape\fR codes.
The following sections will explain what these commands mean.
For now, exit from \*(ME by typing \fB<ctrl-X><ctrl-C>\fR, and
when the message
.DM
	Quit [y/n]?
.DE
appears type \fBy\fR and then \fB<return>\fR.
This will return you to the shell.
.Sh "Beginning a Document"
.PP
This tutorial practices on file \fBexample1.c\fR,
\fBexample2.c\fR, and \fBexample3.c\fR.
They are stored in the directory \fB/usr/src/example\fR.
Before beginning, type the following commands to copy these files
into the current directory and change their permissions:
.DM
	cp /usr/src/sample/example?.c .
	chmod +w example?.c
.DE
Now, type the following command to invoke \*(ME:
.DM
	me example1.c
.DE
In a moment, the following text will appear on your screen:
.B1
.ta 0.4i 0.8i 1.2i 2.5i
	/*
	 * This is a simple C program that computes the results
	 * of three different rates of inflation over the
	 * span of ten years.  Use this text file to learn
	 * how to use MicroEMACS commands
	 * to make creating and editing text files quick,
	 * efficient and easy.
	 */
	#include <stdio.h>
	main()
	{
		int i;		/* count ten years */
		float w1, w2, w3;	/* three inflated quantities */
		char  *msg = " %2d\et%f %f %f\en";	/* printf string */
		i = 0;
		w1 = 1.0;
		w2 = 1.0;
		w3 = 1.0;
		for (i = 1; i<= 10; i++) {
			w1 *= 1.07;	/* apply inflation */
			w2 *= 1.08;
			w3 *= 1.10;
			printf (msg, i, w1, w2, w3);
		}
	}
.B2
.sp \n(pDu
When you invoke \*(ME, it \fIcopies\fR that file into memory.
Your cursor also moved to the upper left-hand corner of the screen.
At the bottom of the screen appears the \fIstatus line\fR, as follows:
.DM

-- Coherent MicroEMACS -- example1.c -- File: example1.c --
.sp \n(pDu
.DE
The word to the left, \*(ME, is the name of the editor.
The word in the center, \fBexample1.c\fR, is the name of the \fIbuffer\fR
that you are using.
(We will describe later just what a buffer is and how you use it.)
The name to the right is the name of the text file that you are editing.
.SH "Moving the Cursor"
.PP
.II "MicroEMACS^backspace key"
.II "MicroEMACS^<return>"
.II "MicroEMACS^arrow keys"
Now that you have read a text file into memory, you are ready to edit it.
The first step is to learn to move the cursor.
.PP
Try these commands for yourself as we described them.
That way, you will quickly acquire a feel for handling \*(ME's commands.
.II "MicroEMACS^movement commands"
.II "MicroEMACS^cursor movement display"
.Sh "Moving the Cursor Forward"
.PP
.II "MicroEMACS^<ctrl-F>"
.II "MicroEMACS^<esc>F"
.II "MicroEMACS^<ctrl-E>"
.II "MicroEMACS^right"
.II "MicroEMACS^forward, one space"
.II "MicroEMACS^forward, one word"
.II "MicroEMACS^forward, end of line"
This first set of commands moves the cursor forward:
.DS
.ta 0.5i 1.5i
	\fB<ctrl-F>\fR	Move forward one space
	\fB<esc>F\fR	Move forward one word
	\fB<ctrl-E>\fR	Move to end of line
.DE
To see how these commands work, do the following:
Type the \fIforward\fR command \fB<ctrl-F>\fR.
As before, it does not matter whether the letter \*(Ql\fBF\fR\*(Qr is
upper case or lower case.
The cursor has moved one space to the right, and now is over
the character \*(Ql\fB*\fR\*(Qr in the first line.
.PP
Type \fB<esc>F\fR.
The cursor has moved one \fIword\fR to the right,
and is now over the space after the word \fBThis\fR.
\*(ME considers only alphanumeric characters when it moves from word to word.
Therefore, the cursor moved from under the \fB*\fR to the space
after the word \fBThis\fR, rather than to the space after the \fB*\fR.
Now type the \fIend of line\fR command \fB<ctrl-E>\fR.
The cursor has jumped to the end of the line and is now
just to the right of the \fBs\fR of the word \fBresults\fR.
.Sh "Moving the Cursor Backwards"
.PP
.II "MicroEMACS^<ctrl-B>"
.II "MicroEMACS^<esc>B"
.II "MicroEMACS^<ctrl-A>"
.II "MicroEMACS^back"
.II "MicroEMACS^left"
.II "MicroEMACS^backward, one space"
.II "MicroEMACS^backward, one word"
.II "MicroEMACS^backward, end of line"
The following summarizes the commands for moving the
cursor backwards:
.DS
.ta .5i 1.5i
	\fB<ctrl-B>\fR	Move back one space
	\fB<esc>B\fR	Move back one word
	\fB<ctrl-A>\fR	Move to beginning of line
.DE
To see how these work, first type the \fIbackward\fR command \fB<ctrl-B>\fR.
As you can see, the cursor has moved one space to the left,
and now is over the letter \fBe\fR of the word \fBthree\fR.
Type \fB<esc>B\fR.
The cursor has moved one \fIword\fR to the left and now
is over the \fBt\fR in \fBthree\fR.
Type \fB<esc>B\fR again, and the cursor will be positioned on the \fBo\fR in
\fBof\fR.
.PP
Type the \fIbeginning of line\fR command \fB<ctrl-A>\fR.
The cursor jumps to the beginnning of the line,
and once again is resting over the \*(Ql\fB/\fR\*(Qr character
in the first line.
.Sh "From Line to Line"
.PP
.II "MicroEMACS^<ctrl-N>"
.II "MicroEMACS^next line"
.DS
.ta .5i 1.5i
	\fB<ctrl-P>\fR	Move to previous line
	\fB<ctrl-N>\fR	Move to next line
.DE
These two commands move the cursor up and down the screen.
Type the \fInext line\fR command \fB<ctrl-N>\fR.
The cursor jumps
to the space before the \*(Ql\fB*\fR\*(Qr in the next line.
Type the \fIend of line\fR command \fB<ctrl-E>\fR, and the cursor moves
to the end of the second line to the right of the period.
.PP
.II "MicroEMACS^line position"
Continue to type \fB<ctrl-N>\fR until the cursor reaches the
bottom of the screen.
As you reached the first line in your text, the
cursor jumped from its position at the right of the period
on the second line to just right of the
brace on the last line of the file.
When you move your cursor up or down the screen, \*(ME
tries to keep it at the same position within each line.
If the line to which you are moving the cursor is not long enough
to have a character at that position,
\*(ME moves the cursor to the end of the line.
.PP
.II "MicroEMACS^previous line"
.II "MicroEMACS^<ctrl-P>"
Now, practice moving the cursor back up the screen.
Type the \fIprevious line\fR command \fB<ctrl-P>\fR.
When the cursor jumped to the previous
line, it retained its position at the end of the line.
\*(ME remembers the cursor's position on the line,
and returns the cursor there when it jumps to a line
long enough to have a character in that position.
.PP
Continue pressing \fB<ctrl-P>\fR.
The cursor will move up the screen until it reaches the top of your text.
.Sh "Repetitive Motion"
.PP
.II "MicroEMACS^repetition"
Some computers repeat a command automatically
if you \fIhold down\fR the control key and the character key.
Try holding down \fB<ctrl-N>\fR for a moment,
and see if it repeats automatically.
If it does, that will speed moving your cursor around the screen,
because you will not have to type the same command repeatedly.
.Sh "Moving Up and Down by a Screenful of Text"
.PP  
.II "MicroEMACS^screen up"
.II "MicroEMACS^screen down"
The next two cursor movement commands allow you to roll forward
or backwards by one screenful of text.
.DS
.ta .5i 1.5i
	\fB<ctrl-V>\fR	Move forward one screen
	\fB<esc>V\fR	Move back one screen
.DE
If you are editing a file with \*(ME that is too big
to be displayed on your screen all at once, \*(ME
displays the file in screen-sized portions
(on most terminals, 22 lines at a time).
The \fIview\fR commands \fB<ctrl-V>\fR and \fB<esc>V\fR allow you
to roll up or down one screenful of text at a time.
.PP
.II "MicroEMACS^<ctrl-V>"
Type \fB<ctrl-V>\fR.
Your screen now contains only the last three lines of the file.
This is because you have rolled forward by the equivalent of one
screenful of text, or 22 lines.
.PP
.II "MicroEMACS^<esc>V"
Now, type \fB<esc>V\fR.
Notice that your text rolls back onto the screen, and your
cursor is positioned in the upper left-hand corner of the
screen, over the character \*(Ql\fB/\fR\*(Qr in the first line.
.Sh "Moving to Beginning or End of Text"
.PP
.II "MicroEMACS^end of text"
.II "MicroEMACS^<esc>>"
These last two cursor movement commands allow you to jump immediately
to the beginning or end of your text.
.DS
.ta .5i 1.5i
	\fB<esc><\fR	Move to beginning of text
	\fB<esc>>\fR	Move to end of text
.DE
The \fIend of text\fR command \fB<esc>>\fR moves the cursor to the
end of your text.
Type \fB<esc>>\fR.
Be sure to type a greater-than symbol \*(Ql>\*(Qr.
Your cursor has jumped to the end of your text.
.PP
.II "MicroEMACS^beginning of text"
.II "MicroEMACS^<esc><"
The \fIbeginning of text\fR command \fB<esc><\fR will move the cursor
back to the beginning of your text.
Type \fB<esc><\fR.
Be sure to type a less-than symbol 
\*(Ql<\*(Qr.
The cursor has jumped back to the upper left-hand corner
of your screen.
.PP
These commands move you immediately to the beginning or the end
of your text, regardless of whether the text is one page or 20
pages long.
.Sh "Saving Text and Quitting"
.PP
.II "MicroEMACS^<ctrl-X><ctrl-C>"
.II "MicroEMACS^quit"
.II "MicroEMACS^saving text"
.II "MicroEMACS^<ctrl-X><ctrl-S>"
If you do not wish to continue working at this time,
you should \fIsave\fR your text, and then \fIquit\fR.
.PP
It is good practice to save your text file every so often while you
are working on it.
If an accident occurs, such as a power failure,
you will not lose all of your work.
You can save your text with the \fIsave\fR command \fB<ctrl-X><ctrl-S>\fR.
Type \fB<ctrl-X><ctrl-S>\fR \*(Em that 
is, first type \fB<ctrl-X>\fR, then type \fB<ctrl-S>\fR.
If you had modified this file, the following message would appear:
.DM
	[Wrote 23 lines]
.DE
The text file would have been saved to your computer's disk.
(\*(ME sends you messages from time to time.
The messages enclosed in square brackets \*(Ql[\*(Qr \*(Ql]\*(Qr are for your
information, and do not necessarily mean that something is wrong.)
To exit from \*(ME, type the \fIquit\fR command \fB<ctrl-X><ctrl-C>\fR.
This will return you to the shell.
.SH "Killing and Deleting"
.PP
.II "MicroEMACS^erase text"
Now that you know how to move the cursor, you are ready to edit your text.
.PP
To return to \*(ME, type the command:
.DM
	me example1.c
.DE
Within a moment, \fBexample1.c\fR will be restored to your screen.
.PP
By now, you probably have noticed that \*(ME is always ready to insert material
into your text.
Unless you use the \fB<ctrl>\fR or \fB<esc>\fR
keys, \*(ME assumes that whatever you type is
text and inserts it onto your screen where your cursor is positioned.
.PP
The simplest way to erase text is simply to position the
cursor to the right of the text you want to erase and backspace over it.
\*(ME, however, also has a set of commands that
allow you to erase text easily.
These commands, \fIkill\fR and \fIdelete\fR,
behave differently; the distinction
is important, and will be explained in a moment.
.II "MicroEMACS^killing and deleting"
.Sh "Deleting Vs. Killing"
.PP
.II "MicroEMACS^kill text, versus deleting"
.II "MicroEMACS^delete text, versus killing"
When \*(ME \fIdeletes\fR text, it is erased completely and
disappears forever.
However, when \*(ME \fIkills\fR text,
the text is copied into a temporary storage area in memory.
This storage area is overwritten when you
move the cursor and then kill additional text.
Until then, however, the killed text is saved.
This aspect of killing allows you to restore text that you
killed accidentally, and it also allows you to move or copy portions
of text from one position to another.
.PP
.II "MicroEMACS^erase text, erasing spaces"
\*(ME is designed so that when it erases text, it does so beginning
at the \fIleft edge\fR of the cursor.
This left edge is called the \fIcurrent position\fR.
.PP
You should imagine that an invisible vertical bar separates the cursor from
the character immediately to its left.
As you enter the various kill and delete commands,
this vertical bar moves to the right or the left with the cursor, and
erases the characters it touches.
.Sh "Erasing Text to the Right"
.PP
.II "MicroEMACS^<ctrl-D>"
.II "MicroEMACS^<del>"
.II "MicroEMACS^text, erase to right"
.II "MicroEMACS^erase text, to the right"
The first two commands to be presented erase text to the \fIright\fR.
.DS
.ta .5i 1.8i
	\fB<ctrl-D>\fR	Delete one character to the right
	\fB<del>\fR	Delete one character to the right
	\fB<esc>D\fR	Kill one word to the right
.DE
The keystrokes
.B <del>
and
\fB<ctrl-D>\fR both
delete one \fIcharacter\fR to the right of the current position.
\fB<esc>D\fR deletes one \fIword\fR to the right of the current position.
.PP
To try these commands, type \fB<ctrl-D>\fR or
.BR <del> .
\*(ME erases the character \*(Ql\fB/\fR\*(Qr in the first line,
and shifted the rest of the line one space to the left.
.PP
.II "MicroEMACS^<esc>D"
Now, type \fB<esc>D\fR.
\*(ME erases
the \*(Ql\fB*\fR\*(Qr character and the word \fBThis\fR,
and shifts the line six spaces to the left.
The cursor is positioned at the \fIspace\fR before the word \fBis\fR.
Type \fB<esc>D\fR again.
The word \fBis\fR vanishes along with the \fIspace\fR that preceded it,
and the line shifts \fIfour\fR spaces to the left.
.PP
Remember that
\fB<ctrl-D>\fR \fIdeletes\fR text, but \fB<esc>D\fR \fIkills\fR text.
.Sh "Erasing Text to the Left"
.PP
.II "MicroEMACS^erase text, to the left"
.II "MicroEMACS^<backspace>"
.II "MicroEMACS^text, erase to left"
You can erase text to the \fIleft\fR with the following commands:
.DS
.ta 0.5i 2.5i
	\fB<backspace>\fR	Delete one character to the left
	\fB<esc><backspace>\fR	Kill one word to the left
.DE
To see how to erase text to the left, first type the
\fIend of line\fR command \fB<ctrl-E>\fR; this will move the
cursor to the right of the word \fBthree\fR on the first line of text.
Now, type
.BR <backspace> .
The second \fBe\fR of the word \fBthree\fR has vanished.
.PP
To erase the \fIword\fR that lies to the left of the cursor, type
.BR <esc><backspace> :
that is, type
.B <esc>
and then type
.BR <backspace> .
\*(ME
defines a word as a string delimited by white space.
For example, if you type
.BR <esc><backspace> ,
the rest of the word \fBthree\fR vanishes, and the cursor moves to the
white space that lies to the left of that word.
If the cursor is at the beginning of a line, then this command kills the
last word on the previous line of text.
.PP
Please note that erasing text with
.B <backspace>
.I deletes
the text; that is, the text is thrown away and gone forever.
Erasing text with
.BR <esc><backspace> ,
however,
.I kills
text; which means that the text is stored internally and can be retrieved.
The distinction between
.I deleting
text and
.I killing
is described in detail below.
.Sh "Erasing Lines of Text"
.PP
.II "MicroEMACS^erase text, by line"
.II "MicroEMACS^text, kill by lines"
Finally, the following command erases a line of text:
.DS
.ta .5i 1.8i
	\fB<ctrl-K>\fR	Kill from cursor to end of line
.DE
This command kills a line of text, from the line beginning
from immediately to the left of the cursor to the end of the line.
.PP
To see how this works, move the cursor to the beginning
of line 2.
Now, strike \fB<ctrl-K>\fR.
All of line 2 has vanished and been replaced with an empty space.
Strike \fB<ctrl-K>\fR again.
The empty space has vanished, and the cursor
is now positioned at the beginning of what used to be
line 3, in the space before \fB* Use\fR.
.Sh "Yanking Back (Restoring) Text"
.PP
.II "MicroEMACS^restore (yank back) killed text"
.II "MicroEMACS^yank back text"
.II "MicroEMACS^text, restore (yank back)"
.II "MicroEMACS^text, yank back (restore)"
.II "MicroEMACS^<ctrl-Y>"
The following command allows you restore material that you have killed:
.DS
.ta .5i 1.5i
	\fB<ctrl-Y>\fR	Yank back (restore) killed text
.DE
Remember that when you kill text, \*(ME
temporarily stores it elsewhere.
You can return this material to the screen
by using the \fIyank back\fR command \fB<ctrl-Y>\fR.
Type \fB<ctrl-Y>\fR.
All of line 2 has returned; the cursor, however,
remains at the beginning of line 3.
.Sh "Quitting"
.PP
.II "MicroEMACS^<ctrl-X><ctrl-C>"
.II "MicroEMACS^quit without saving text"
When you are finished, do not save the text.
If you do so, the undamaged copy of the text that
you made earlier will be replaced with the present mangled copy.
Rather, use the \fIquit\fR command \fB<ctrl-X><ctrl-C>\fR.
Type \fB<ctrl-X><ctrl-C>\fR.
On the bottom of your screen, \*(ME responds:
.DM
	Quit [y/n]?
.DE
Reply by typing \fBy\fR and a carriage return.
If you type \fBn\fR, \*(ME will
return you to where you were in the text.
\*(ME will now return you to the shell.
.SH "Block Killing and Moving Text"
.PP
.II "MicroEMACS^move text"
.II "MicroEMACS^text, move"
.II "MicroEMACS^buffer, for killed text"
As noted above, text that is killed is stored temporarily within memory.
You can yank killed text back onto your screen,
and not necessarily in the spot where it was originally killed.
This feature allows you to move text from one position to another.
.PP
.II "MicroEMACS^block-kill text"
.II "MicroEMACS^move text"
.II "MicroEMACS^kill and move commands"
.Sh "Moving One Line of Text"
.PP
You can kill and move one line of text with the following commands:
.DS
.ta .5i 1.5i
	\fB<ctrl-K>\fR	Kill text to end of line
	\fB<ctrl-Y>\fR	Yank back text
.DE
To test these commands, invoke \*(ME for the file \fBexample1.c\fR
by typing the following command:
.DM
	me example1.c
.DE
When \*(ME appears, the cursor will be positioned in the
upper left-hand corner of the screen.
.PP
To move the first line of text, begin by
typing the \fIkill\fR command \fB<ctrl-K>\fR twice.
Now, press \fB<esc>>\fR to move the cursor to the bottom of text.
Finally, yank back the line by typing \fB<ctrl-Y>\fR.
The line that reads
.DM
	/* This is a simple C program that computes the results
.DE
is now at the bottom of your text.
.PP
Your cursor has moved to the point on your screen that is
\fIafter\fR the line you yanked back.
.Sh "Multiple Copying of Killed Text"
.PP
When text is yanked back onto your screen, it is \fInot\fR deleted
from memory.
Rather, it is simply copied back onto the screen.
This means that killed text can be reinserted into the text more than once.
To see how this is done, return to the top of the text by typing \fB<esc><\fR.
Then type \fB<ctrl-Y>\fR.
The line you just killed now appears as both the first and last line of
the file.
.PP
The killed text will not be erased from its temporary
storage until you move the cursor and then kill additional text.
If you kill several lines or portions of lines in a row, all of the
killed text will be stored in the buffer; if you are not careful,
you may yank back a jumble of accumulated text.
.Sh "Kill and Move a Block of Text"
.PP
.II "MicroEMACS^kill text, block"
.II "MicroEMACS^<ctrl-@>"
.II "MicroEMACS^<ctrl-W>"
.II "MicroEMACS^text, block kill"
If you wish to kill and move more than one line of text at a time,
use the following commands:
.DS
.ta .5i 1.5i
	\fB<ctrl-@>\fR	Set mark
	\fB<esc>.\fR	Set mark
	\fB<ctrl-W>\fR	Kill block of text
	\fB<ctrl-Y>\fR	Yank back text
.DE
If you wish to kill a block of text, you can either
type the \fIkill\fR command \fB<ctrl-K>\fR repeatedly
to kill the block one line at a time, or you can
use the \fIblock kill\fR command \fB<ctrl-W>\fR.
To use this command, you must first set a \fImark\fR on the screen,
an invisible character that acts as a signal to the computer.
The mark can be set with either \fB<esc>.\fR or \fB<ctrl-@>\fR.
.PP
Once the mark is set, you must move your cursor to the
other end of the block of text you wish to kill, and then strike
\fB<ctrl-W>\fR.
The block of text will be erased, and will be ready to be
yanked back elsewhere.
.PP
Try this out on \fBexample1.c\fR.
Type \fB<esc><\fR to move the cursor to the upper left-hand
corner of the screen.
Then type the \fIset mark\fR command \fB<ctrl-@>\fR.
\*(ME will respond with the message
.DM
	[Mark set]
.DE
at the bottom of your screen.
Now, move the cursor down six lines, and type \fB<ctrl-W>\fR.
Note how the block of text you marked out has disappeared.
.PP
Move the cursor to the bottom of your text.
Type \fB<ctrl-Y>\fR.
The killed block of text has now been reinserted.
.PP
When you yank back text, be sure
to position the cursor at the \fIexact\fR point
where you want the text to be yanked back.
This will ensure that the text will be yanked back in the proper place.
To try this out, move your cursor up six lines.
Be careful that the cursor is at the \fIbeginning\fR of the line.
Now, type \fB<ctrl-Y>\fR again.
The text reappeared \fIabove\fR where the cursor
was positioned, and the cursor has not moved from
its position at the beginning of the line \*(Em which is not
what would have happened had you positioned it in the middle
or at the end of a line.
.PP
Although the text you are working with has only 23 lines,
you can move much larger portions of text using only
these three commands.
Remember, too, that you can use this technique to duplicate
large portions of text at several positions to save
yourself considerable time in typing and reduce the number of possible
typographical errors.
.SH "Capitalization and Other Tools"
.PP
The next commands perform a number of tasks to help with your editing.
Before you begin this section, destroy the old text on
your screen with the \fIquit\fR command \fB<ctrl-X><ctrl-C>\fR,
and read into \*(ME a fresh copy of the program,
as you did earlier.
.Sh "Capitalization and Lowercasing"
.PP
The following \*(ME commands automatically
capitalize a word (that is, make the first letter
of a word upper case), or make an entire word upper case or
lower case.
.DS
.ta .5i 1.5i
	\fB<esc>C\fR	Capitalize a word
	\fB<esc>L\fR	Lowercase from cursor to end of word
	\fB<esc>U\fR	Uppercase from cursor to end of word
.DE
To try these commands, do the following:
.II "MicroEMACS^uppercase"
.II "MicroEMACS^lowercase"
.II "MicroEMACS^capitalization"
.II "MicroEMACS^<esc>C"
.II "MicroEMACS^text, capitalize"
First, move the cursor to the letter \fBd\fR of
the word \fIdifferent\fR on line 2.
Type the \fIcapitalize\fR command \fB<esc>C\fR.
The word is now capitalized, and
the cursor is now positioned at the space after the word.
Move the cursor forward so that it is over the letter \fBt\fR in \fBrates\fR.
Press \fB<esc>C\fR again.
The word changes to \fBraTes\fR.
When you press \fB<esc>C\fR, \*(ME capitalizes
the \fIfirst\fR letter the cursor meets.
.PP
\*(ME can also change a word to all upper case or all lower case.
(There is very little need for a command that will
change only the first character of an upper-case word
to lower case, so it is not included.)
.PP
.II "MicroEMACS^<esc>U"
.II "MicroEMACS^text, uppercase"
Type \fB<esc>B\fR to move the cursor so that it is again to the left of the
word \fBDifferent\fR.
It does not matter if the cursor is directly over the \fBD\fR
or at the space to its left; as you will see, this means
that you can capitalize or
lowercase a number of words in a row without having to move the cursor.
.PP
Type the \fIuppercase\fR command \fB<esc>U\fR.
The word is now spelled \fBDIFFERENT\fR, and the
cursor has jumped to the space after the word.
.PP
.II "MicroEMACS^<esc>L"
.II "MicroEMACS^text, lowercase"
Again, move the cursor to the left of the word \fBDIFFERENT\fR.
Type the \fIlowercase\fR command \fB<esc>L\fR.
The word has changed back to \fBdifferent\fR.
Now, move the cursor to the space at the beginning of line 3 by typing
\fB<ctrl-N>\fR then \fB<ctrl-A>\fR.
Type \fB<esc>L\fR once again.
The character \*(QL\fB*\fR\*(Qr is not affected by the command,
but the letter \fBU\fR is now lower case.
\fB<esc>L\fR not only shifts a word that is all upper case to
lower case: it can also un-capitalize a word.
.PP
The \fIuppercase\fR and \fIlowercase\fR commands
stop at the first punctuation mark they meet
\fIafter\fR the first letter they find.
This means that, for example,
to change the case of a word with an apostrophe in it
you must type the appropriate command twice.
.Sh "Transpose Characters"
.PP
.II "MicroEMACS^<ctrl-T>"
.II "MicroEMACS^transpose characters"
\*(ME allows you to reverse the position of two characters,
or \fItranspose\fR them, with the \fItranspose\fR command
\fB<ctrl-T>\fR.
.PP
Type \fB<ctrl-T>\fR.
\*(ME transposes the character that is under the cursor
with the character immediately to its \fIleft\fR.
In this example,
.DM
	* use this
.DE
in line 3 now appears:
.DM
	* us ethis
.DE
The space and the letter \fBe\fR have been transposed.
Type \fB<ctrl-T>\fR again.
The characters have returned to their original order.
.Sh "Screen Redraw"
.PP
.II "MicroEMACS^<ctrl-L>"
.II "MicroEMACS^screen redraw"
.II "MicroEMACS^redraw screen"
.DS
.ta .5i 1.8i
	\fB<ctrl-L>\fR	Redraw screen
.DE
Occasionally, while you are working on a text another \*(CO
user will write or mail you a message.
\*(OS will write the message directly on your
screen, which scrambles your screen.
A message sent from another user or a message from
the \*(CO system is \fInot\fR recorded into your
text; however, you may wish to erase the message and
continue editing.
The \fIredraw screen\fR command \fB<ctrl-L>\fR will redraw your screen to
the way it was before it was scrambled.
.PP
Type \fB<ctrl-L>\fR.
Notice how the screen flickers and the text is rewritten.
Had your screen been spoiled by extraneous material,
that material would have been erased and the original
text rewritten.
.PP
.II "MicroEMACS^center line on screen"
.II "MicroEMACS^<ctrl-U><ctrl-L>"
The \fB<ctrl-L>\fR command also has another use:
it can move the line on which the cursor is positioned to the
center of the screen.
If you have a file that contains more than one screenful
of text and you wish to have that particular line in the center
of the screen, position the cursor on that line and type
\fB<ctrl-U><ctrl-L>\fR.
You will see a prompt that says
.DM
	Arg: 4
.DE
The meaning of this prompt is explained below; for now,
ignore it and press
.BR <return> .
Immediately, \*(ME redraws the screen and places the line you
selected in the center of the screen.
.Sh "Return Indent"
.PP
.II "MicroEMACS^return indent"
.DS
.ta .5i 1.8i
	\fB<ctrl-J>\fR	Return and indent
.DE
.II "MicroEMACS^block indentation"
You may often be faced with a situation in which, for the sake of
programming style, you need to indent many lines of text:
before every line you must tab the correct number of
times before typing the text.
These \fIblock indents\fR can be a time-consuming typing chore.
The \*(ME \fB<ctrl-J>\fR command makes this task easier.
\fB<ctrl-J>\fR moves the cursor to the next line on the screen and
automatically positions the cursor at the previous line's level of indentation.
.PP
To see how this works, first move the cursor to the line that reads
.DM
	w3 *= 1.10:
.DE
Press \fB<ctrl-E>\fR, to move the cursor to the end of the line.
Now, type \fB<ctrl-J>\fR.
.PP
As you can see, a new line opens up and the cursor is indented the same
amount as the previous line.
Type
.DM
	/* Here is an example of auto-indentation */
.DE
This line of text begins directly under the previous line.
.Sh "Word Wrap"
.PP
.II "MicroEMACS^word wrap"
.DS
.ta .5i 1.8i
	\fB<ctrl-X>F\fR	Set word wrap
.DE
Although you have not yet had much opportunity to use it,
\*(ME will automatically wrap text that you are typing.
.II "MicroEMACS^<ctrl-X>F"
Word-wrapping is controlled with the \fIword wrap\fR command
\fB<ctrl-X>F\fR.
To see how the word wrap command works, first exit from \*(ME
by typing \fB<ctrl-X><ctrl-C>\fR; then reinvoke \*(ME by typing
.DM
	me cucumber
.DE
When \*(ME re-appears,
type the following text; however, do \fInot\fR type
any carriage returns:
.DM
	A cucumber should be
	well sliced, and dressed
	with pepper and vinegar,
	and then thrown out, as
	good for nothing.
.DE
When you reached the edge of your screen,
a dollar sign was printed and you were allowed to continue typing.
\*(ME accepted the characters you typed, but it placed
them at a location beyond the right edge of your screen.
.PP
Now, move to the beginning of the next line and type \fB<ctrl-U>\fR.
\*(ME will reply with the message:
.DM
	Arg: 4
.DE
Type \fB30\fR.
The line at the bottom of your screen now appears as follows:
.DM
	Arg: 30
.DE
(The use of the \fIargument\fR command \fB<ctrl-U>\fR will be
explained in a few minutes.)
Now type the \fIword-wrap\fR command \fB<ctrl-X>F\fR.
\*(ME will now say at the bottom of your screen:
.DM
	[Wrap at column 30]
.DE
This sequence of commands has set the word-wrap function, and
told it to wrap to the next line all words that extend beyond
the 30th column on your screen.
.PP
The \fIword wrap\fR feature automatically moves your cursor to the
beginning of the next line once you type past a preset border on your
screen.
When you first enter \*(ME, that limit is automatically set
at the first column, which in effect means that word wrap has
been turned off.
.PP
When you type prose for a report or a letter of some sort, you
probably will want to set the border at the 65th column, so that
the printed text will fit neatly onto a sheet of paper.
If you are using \*(ME to type in a program, however, you probably
will want to leave word wrap off, so you do not
accidentally introduce carriage returns into your code.
.PP
To test word wrapping, type the above text again,
without using the carriage return key.
When you finish, it should appear as follows:
.DM
	A cucumber should be well
	sliced, and dressed with
	pepper and vinegar, and then
	thrown out, as good for
	nothing.
.DE
\*(ME automatically moved your cursor to the next line when you typed
a space character after the 30th column on your screen.
.PP
If you wish to fix the border at some special point on your screen
but do not wish to go through the tedium of figuring out how many
columns from the left it is, simply position the cursor where you
want the border to be, type \fB<ctrl-X>F\fR, and then type a carriage
return.
When \fB<ctrl-X>F\fR is typed without being preceded by a \fB<ctrl-U>\fR
command, it sets the word-wrap border at the point your cursor
happens to be positioned.
When you do this, \*(ME will then print a message at the bottom
of your terminal that tells you where the word-wrap border is now set.
.PP
To re-word wrap the text between the cursor and the mark, type
.BR <ctrl-X>H .
.PP
If you wish to turn off the word wrap feature again, simply set the
word wrap border to one.
.SH "Search and Reverse Search"
.PP
When you edit a large text, you may wish to change particular
words or phrases.
To do this, you can roll through the text and read each
line to find them; or you can have \*(ME find them for you.
.II "MicroEMACS^searching"
.II "MicroEMACS^commands"
Before you continue, close the present file by typing
\fB<ctrl-X> <ctrl-C>\fR;
then reinvoke the editor to edit the file \fBexample1.c\fR,
as you did before.
The following sections perform some exercises with this file.
.Sh "Search Forward"
.PP
.II "MicroEMACS^search, forward"
.II "MicroEMACS^<esc>S"
.DS
.ta .5i 1.5i
	\fB<ctrl-S>\fR	Search forward incrementally
	\fB<esc>S\fR	Search forward with prompt
.DE
As you can see from the display, \*(ME has two ways to search
forward:
incrementally, and with a prompt.
.PP
An
.I incremental
search is one in which the search is performed as you type the characters.
To see how this works, first type the \fIbeginning of text\fR
command \fB<esc><\fR to move the cursor to the
upper left-hand corner of your screen.
Now, type the \fIincremental search\fR command \fB<ctrl-S>\fR.
\*(ME will respond by prompting with the message
.DM
	i-search forward []
.DE
at the bottom of the screen.
.PP
We will now search for the pointer
.BR *msg .
Type the letters \fB*msg\fR one at a time, starting with \fB*\fR.
The cursor has jumped to the first place that a \fB*\fR was found:
at the second character of the first line.
The cursor moves forward in the text file and the message at the bottom
of the screen changes to reflect what you have typed.
.PP
Now type \fBm\fR.
The cursor has jumped ahead to the letter \fBs\fR in \fB*msg\fR.
Type \fBs\fR.
The cursor has jumped ahead to the letter \fBg\fR in \fB*msg\fR.
Finally, type \fBg\fR.
The cursor is over the space after the token \fB*msg\fR.
Finally, type \fB<esc>\fR to end the string.
\*(ME replies with the message
.DM
	[Done]
.DE
which indicates that the search is finished.
.PP
If you attempt an incremental search for a word that is not in the file,
\*(ME will find as many of the letters as it can, and then give you
an error message.
For example, if you tried to search incrementally for the word
\fB*msgs\fR, \*(ME would move the cursor to the phrase \fB*msg\fR;
when you typed \*(Qls\*(Qr, it would tell you
.DM
	failing i-search forward: *msgs
.DE
.II "MicroEMACS^<return>"
With the
.IR "prompt search" ,
however, you type in the word all at once.
To see how this works, type
.BR <esc>< ,
to return to the top of the file.
Now, type the
.I "prompt search"
command
.BR <esc>S .
\*(ME responds by prompting with the message
.DM
	Search [*msgs]:
.DE
at the bottom of the screen.
The word \fB*msgs\fR is shown because that was
the last word for which you searched, and so it is kept in the search buffer.
.PP
Type in the words \fBediting text\fR, then press the carriage return.
Notice that the cursor has jumped to the period after the
word \fBtext\fR in the next to last line of your text.
\*(ME searched for the words \fBediting text\fR, found them, and moved
the cursor to them.
.PP
If the word you were searching for was not in your text,
or at least was not in the portion that lies between
your cursor and the end of the text, \*(ME
would not have moved the cursor, and would have displayed the message
.DM
	Not found
.DE
at the bottom of your screen.
.Sh "Reverse Search"
.PP
.II "MicroEMACS^search, reverse"
.II "MicroEMACS^reverse search"
.II "MicroEMACS^<esc>R"
.DS
.ta .5i 1.5i
	\fB<ctrl-R>\fR	Search backwards incrementally
	\fB<esc>R\fR	Search backwards with prompt
.DE
The search commands, useful as they are,
can only search forward through your text.
To search backwards, use the reverse search commands
.B <ctrl-R>
and \fB<esc>R\fR.
These work exactly the same as their forward-searching counterparts,
except that they search toward the beginning of the file rather than
toward the end.
.PP
For example, type \fB<esc>R\fR.
\*(ME replies with the message
.DM
	Reverse search [editing text]:
.DE
at the bottom of your screen.
The words in square brackets are the words you
entered earlier for the \fIsearch\fR command;
\*(ME remembered them.
If you wanted to search for \fBediting text\fR again, you would just
press the carriage return.
For now, however, type the word \fBprogram\fR and press
the carriage return.
.PP
Notice that the cursor has jumped so that it is under the
letter \fBp\fR of the word \fBprogram\fR in line 1.
When you search forward, the cursor moves to the
\fIspace after\fR the word for which you are searching, whereas
when you reverse search the cursor moves to the
\fIfirst letter\fR of the word for which you are searching.
.Sh "Cancel a Command"
.PP
.II "MicroEMACS^cancel a command"
.II "MicroEMACS^<ctrl-G>"
.DS
.ta .5i 1.5i
	\fB<ctrl-G>\fR	Cancel a search command
.DE
As you have noticed, the commands to move the cursor
or to delete or kill text all execute immediately.
Although this speeds your editing, it also means that
if you type a command by mistake, it executes before you can stop it.
.PP
The
.I search
and
.I "reverse search"
commands, however, wait for you to respond to their prompts before they execute.
If you type \fB<esc>S\fR or \fB<esc>R\fR by accident,
\*(ME will interrupt your editing and
wait for you to initate a search that you do not want to perform.
You can evade this problem, however, with the \fIcancel\fR
command \fB<ctrl-G>\fR.
This command tells \*(ME to ignore the previous command.
.PP
To see how this command works, type \fB<esc>R\fR.
When the prompt appears at the bottom of your screen, type \fB<ctrl-G>\fR.
Three things happen:
your terminal beeps,
the characters \fB^G\fR appear at the bottom of your screen,
and the cursor returns to where it was before you first typed \fB<esc>R\fR.
The \fB<esc>R\fR command has been cancelled, and you
are free to continue editing.
.PP
If you cancel an
.I "incremental search"
command,
.B <ctrl-S>
or
.BR <esc-S> ,
the cursor returns to where it was before you began the search.
For example, type
.B <esc><
to return to the top of the file.
Now type
.B <ctrl-S>
to begin an incremental search, and type
.BR m .
When the cursor moves to the
.B m
in
.BR simple ,
type
.BR <ctrl-G> .
Your cursor returns to the top of the
file, where you began the search.
.Sh "Search and Replace"
.PP
.DS
.ta .5i 1.5i
	\fB<esc>%\fR	Search and replace
.DE
\*(ME also gives you a powerful function that allows you
to search for a string and replace it with a keystroke.
You can do this by executing
the \fIsearch and replace\fR command \fB<esc>%\fR.
.II "MicroEMACS^<esc>%"
.II "MicroEMACS^search and replace"
.PP
To see how this works, move to the top of the
text file by typing \fB<esc><\fR; then type \fB<esc>%\fR.
You will see the following message at the bottom of your screen:
.DM
	Old string [m]
.DE
As an exercise, type \fBmsg\fR, and then press
.BR <return> .
\*(ME will then ask:
.DM
	New string:
.DE
Type \fBmessage\fR, and press the carriage return.
As you can see, the cursor jumps to the first occurrence of
the string \fBmsg\fR, and prints the following message
at the bottom of your screen:
.DM
	Query replace: [msg] -> [message]
.DE
\*(ME is asking if it should proceed with the replacement.
Type a carriage return:
this displays the options that are available to you at the
bottom of your screen:
.DM
.in 0
.ta 0.4i
	<SP>[,] replace, [.] rep-end, [n] dont, [!] repl rest <C-G> quit
.DE
The options are as follows:
.PP
Typing a space or a comma executes the replacement, and moves the cursor
to the next occurrence of the old string; in this case, it replaces
\fBmsg\fR with \fBmessage\fR, and moves the cursor to the next occurrence
of \fBmsg\fR.
.PP
Typing a period \*(Ql.\*(Qr replaces this one occurrence of the old
string and ends the search and replace procedure.
In this example, typing a period replaces this one occurrence of \fBmsg\fR
with \fBmessage\fR and ends the procedure.
.PP
Typing the letter \*(Qln\*(Qr tells \*(ME \fInot\fR to replace
this instance of the old string, but move to the next occurrence of
the old string.
In this case, typing \*(Qln\*(Qr does \fInot\fR
replace \fBmsg\fR with \fBmessage\fR, and the cursor jumps
to the next place where \fBmsg\fR occurs.
.PP
Typing an exclamation point \*(Ql!\*(Qr tells \*(ME to replace
all instances of the old string with the new string automatically,
without checking with you any further.
In this example, typing \*(Ql!\*(Qr replaces all instances
of \fBmsg\fR with \fBmessage\fR without further queries from \*(ME.
When you finish searching and replacing,
\*(ME displays a message that tells how many replacements it made.
.PP
Typing \fB<ctrl-G>\fR aborts the search and replace procedure.
.SH "Saving Text and Exiting"
.PP
.II "MicroEMACS^text, saving"
.II "MicroEMACS^exit"
.II "MicroEMACS^saving text"
This set of basic editing commands allows you to save your
text and exit from the \*(ME program.
They are as follows:  
.DS
.ta .5i 2.3i
	\fB<ctrl-X><ctrl-S>\fR	Save text
	\fB<ctrl-X><ctrl-W>\fR	Write text to a new file
.sp \n(PDu
	\fB<ctrl-Z>\fR	Save text and exit
	\fB<ctrl-X><ctrl-C>\fR	Exit without saving text
.DE
.II "MicroEMACS^extended commands"
.II "MicroEMACS^<ctrl-X>"
You have used two of these commands already:
the \fIsave\fR command \fB<ctrl-X><ctrl-S>\fR
and the \fIquit\fR command \fB<ctrl-X><ctrl-C>\fR,
which respectively allow you to save text or to exit
from \*(ME without saving text.
(Commands that begin with \fB<ctrl-X>\fR are called \fIextended\fR
commands; they are used frequently in the commands described later
in this tutorial.)
.Sh "Write Text to a New File"
.PP
.II "MicroEMACS^text, write to new file"
.II "MicroEMACS^<ctrl-X><ctrl-W>"
.DS
.ta .5i 2.3i
	\fB<ctrl-X> <ctrl-W>\fR	Write text to a new file
.DE
If you wish, you can copy the text you are currently
editing to a text file other than the one from which you originally
read the text.
Do this with the \fIwrite\fR command \fB<ctrl-X><ctrl-W>\fR.
.PP
To test this command, type \fB<ctrl-X><ctrl-W>\fR.
\*(ME displays the following message on the bottom of your screen:  
.DM
	Write file:  
.DE
\*(ME is asking for the name of the file into which you wish to write the text.
Type \fBsample\fR.
\*(ME replies:
.DM
	[Wrote 23 lines]
.DE
The 23 lines of your text have been copied to a new file called \fBsample\fR.
The status line at the bottom of your screen has changed to read as follows:
.DM

-- Coherent MicroEMACS -- example1.c -- File: sample --------------

.DE
The significance of the change in file name will be discussed in the
second half of this tutorial.
.PP
Before you copy text into a new file, be sure that you have not
selected a file name that is already being used.
If you do, \*(ME will erase whatever is stored under that file name,
and the text created with \*(ME will be stored in its place.
.Sh "Save Text and Exit"
.PP
.II "MicroEMACS^<ctrl-Z>"
.II "MicroEMACS^store command"
Finally, the \fIstore\fR command
\fB<ctrl-Z>\fR will save your text \fIand\fR move you out of
the \*(ME editor.
To see how this works, watch the bottom line of your terminal
carefully and type \fB<ctrl-Z>\fR.
\*(ME
has saved your text, and now you can issue commands directly to the shell.
.SH "Advanced Editing"
.PP 
The second half of this tutorial introduces
the advanced features of \*(ME.
.PP
The techniques described here will help you
execute complex editing tasks with minimal trouble.
You will be able to edit more than one text at a time,
display more than one file on your screen at a time,
enter a long or complicated phrase repeatedly with only
one keystroke, and give commands to \*(OS
without having to exit from \*(ME.
.PP
Before beginning, however, you must prepare a new text file.
Type the following command:
.DM
	me example2.c
.DE
In a moment, \fBexample2.c\fR will appear on your screen,
as follows:
.B1
	/* Use this program to get better acquainted
	 * with the MicroEMACS interactive screen editor.
	 * You can use this text to learn some of the
	 * more advanced editing features of MicroEMACS.
	 */
.sp \n(pDu
	#include <stdio.h>
	main()
	{
		FILE *fp;
		int ch;
		int filename[20];
.sp \n(pDu
		printf("Enter file name: ");
		gets(filename);
.sp \n(pDu
		if ((fp =fopen(filename,"r")) !=NULL) {
			while ((ch = fgetc(fp)) != EOF)
				fputc(ch, stdout);
		} else
			printf("Cannot open %s.\en", filename);
		fclose(fp);
	}	
.B2
.sp \n(pDu
.SH "Arguments"
.PP
.II "MicroEMACS^arguments"
.II "MicroEMACS^arguments"
.II "MicroEMACS^<ctrl-U>"
Most of the commands already described in this
tutorial can be used with \fIarguments\fR.
An argument is a subcommand that tells \*(ME to
execute a command a given number of times.
With \*(ME, arguments are introduced by typing \fB<ctrl-U>\fR.
.Sh "Arguments: Default Values"
.PP
.II "MicroEMACS^arguments, default value"
By itself, \fB<ctrl-U>\fR sets the argument at \fIfour\fR.
To illustrate this, first type the \fInext line\fR command \fB<ctrl-N>\fR.
By itself, this command moves the cursor down one
line, from being over the \*(Ql\fB/\fR\*(Qr at the beginning
of line 1, to being over the \fIspace\fR at the beginning of line 2.
.PP
Now, type \fB<ctrl-U>\fR.
\*(ME replies with the message:
.DM
	Arg: 4
.DE
Now type \fB<ctrl-N>\fR.
The cursor jumps down \fIfour\fR
lines, from the beginning of line 2 to the 
letter \fBm\fR of the word \fBmain\fR at the beginning of line 6.
.PP
.II "MicroEMACS^arguments, increasing or decreasing"
Type \fB<ctrl-U>\fR.
The line at the bottom of the screen again shows that
the value of the argument is four.
Type \fB<ctrl-U>\fR again.
Now the line at the bottom of the screen reads:
.DM
	Arg: 16
.DE
Type \fB<ctrl-U>\fR once more.
The line at the bottom of the screen now reads:
.DM
	Arg: 64
.DE
Each time you type \fB<ctrl-U>\fR, the value of the argument
is \fImultiplied\fR by four.
Type the \fIforward\fR command \fB<ctrl-F>\fR.
The cursor has jumped ahead 64 characters, and is now
over the \fBi\fR of the word \fBfile\fR in the \fIprintf\fR statement in
line 11.
.Sh "Selecting Values"
.PP
.II "MicroEMACS^arguments, selecting values"
Naturally, an argument does not have to be a power of four.
You can set the argument to whatever number you wish,
simply by typing \fB<ctrl-U>\fR and then typing the number you want.
.PP
For example, type \fB<ctrl-U>\fR, and then type \fB3\fR.
The line at the bottom of the screen now reads:
.DM
	Arg: 3
.DE
Type the \fIdelete\fR command \fB<esc>D\fR.
\*(ME has deleted three words to the right.
.PP
You can use
arguments to increase the power of any
\fIcursor movement\fR command, or any \fIkill\fR or \fIdelete\fR command.
The sole exception is \fB<ctrl-W>\fR, the \fIblock kill\fR command.
.Sh "Deleting With Arguments: An Exception"
.PP
.II "MicroEMACS^yank back text"
.II "MicroEMACS^arguments, deleting"
\fIKilling\fR and \fIdeleting\fR were described in the first
part of this tutorial.
They were said to differ in that text that was killed was
stored in a special area of the computer and could be
yanked back, whereas text that was deleted was erased outright.
However, there is one exception to this rule:
any text that is deleted using an argument can also be yanked back.
.PP
To see how this works, first
type the \fIbegin text\fR command \fB<esc><\fR to
move the cursor to the upper left-hand corner of the screen.
Then, type \fB<ctrl-U> 5 <ctrl-D>\fR.
The word \fBUse\fR has disappeared.
Move the cursor to the right until it is between
the words \fBbetter\fR and \fBacquainted\fR, then
type \fB<ctrl-Y>\fR.
The word \fBUse\fR has been moved within the line
(although the spaces around it have not been moved).
This function is very handy, and should greatly speed your editing.
.PP
Remember, too, that unless you move the cursor between
one set of deletions and another,
the computer's storage area will not be erased, and
you may yank back a jumble of text.
.SH "Buffers and Files"
.PP
Before beginning this section, replace the edited copy
of the text on your screen with a fresh copy.
Type the \fIquit\fR command 
\fB<ctrl-X><ctrl-C>\fR to exit from \*(ME without
saving the text; then return to \*(ME to edit the file
\fBexample2.c\fR, as you did earlier.
.PP
Now, look at the status line at the bottom of your screen.
It should appear as follows:  
.DM

-- Coherent MicroEMACS -- example2.c -- File: example2.c --------------

.DE
As noted in the first half of this tutorial, 
the name on the left of the command line is that of the program.
The name in the middle is the name of the \fIbuffer\fR with
which you are now working, and the name to the right is
the name of the \fIfile\fR from which you read the text.
.Sh "Definitions"
.PP
.II "MicroEMACS^buffer, definition"
.II "MicroEMACS^file, definition"
.II "MicroEMACS^buffer, how differs from file"
.II "MicroEMACS^file, how differs from buffer"
A \fIfile\fR is a mass of text that has been given a name and has been
permanently stored on your disk.
A \fIbuffer\fR is a portion of the computer's
memory that has been set aside for you to use, which may be given a name,
and into which you can put text temporarily.
You can place text into the buffer either by typing it at
your keyboard or by \fIcopying\fR it from a file.
.PP
Unlike a file, a buffer is not permanent:
if your computer were to stop working
(because you turned the power off, for example),
a file would not be affected, but a buffer would be erased.
.PP
.II "MicroEMACS^file, naming"
.II "MicroEMACS^buffer, naming"
You must \fIname\fR your files because you work with many
different files, and you must have some way to tell them apart.
Likewise, \*(ME allows you to \fIname\fR your buffers,
because \*(ME allows you to work with more than one buffer at a time.
.Sh "File and Buffer Commands"
.PP
\*(ME gives you a number of commands for handling files and buffers.
These include the following:
.II "MicroEMACS^file and buffer commands"
.DS
.ta 0.5i 2.3i
	\fB<ctrl-X><ctrl-W>\fR	Write text to file
	\fB<ctrl-X><ctrl-F>\fR	Rename file
.sp \n(PDu
	\fB<ctrl-X><ctrl-R>\fR	Replace buffer with named file
	\fB<ctrl-X><ctrl-V>\fR	Switch buffer or create a new buffer
.sp \n(PDu
	\fB<ctrl-X>K\fR	Delete a buffer
	\fB<ctrl-X><ctrl-B>\fR	Display the status of each buffer
.DE
.Sh "Write and Rename Commands"
.PP
.II "MicroEMACS^<ctrl-X><ctrl-W>"
.II "MicroEMACS^write text to new file"
The \fIwrite\fR command \fB<ctrl-X><ctrl-W>\fR was introduced earlier when
the commands for saving text and exiting were discussed.
To review, \fB<ctrl-X><ctrl-W>\fR changes the name of the
file into which the text is saved, and then copies the text into that file.
.PP
Type \fB<ctrl-X><ctrl-W>\fR.
\*(ME responds by printing
.DM
	Write file:
.DE
on the last line of your screen.
.PP
Type \fBjunkfile\fR, then \fB<return>\fR.
Two things happen:
First, \*(ME writes the message
.DM
	[Wrote 21 lines]
.DE
at the bottom of your screen.
Second, the name of the file shown on the status line changes from
\fBexample2.c\fR to \fBjunkfile\fR.
\*(ME is reminding you that your text is now being saved
into the file \fBjunkfile\fR.
.PP
.II "MicroEMACS^<ctrl-X><ctrl-F>"
.II "MicroEMACS^rename file"
.II "MicroEMACS^file, rename"
The \fIfile rename\fR command \fB<ctrl-X><ctrl-F>\fR allows you rename the
file to which you are saving text, \fIwithout\fR automatically
writing the text to it.
Type \fB<ctrl-X><ctrl-F>\fR.
\*(ME will reply with the prompt:
.DM
	Name:
.DE
Type \fBexample2.c\fR and \fB<return>\fR.
\*(ME does \fInot\fR send you a message that
lines were written to the file; however, the name of the
file shown on the status line has changed from
\fBjunkfile\fR back to \fBexample2.c\fR.
.Sh "Replace Text in a Buffer"
.PP
.II "MicroEMACS^<ctrl-X><ctrl-R>"
.II "MicroEMACS^replace buffer with named file"
.II "MicroEMACS^buffer, replace with named file"
.II "MicroEMACS^file, replace buffer with named f."
The \fIreplace\fR command \fB<ctrl-X><ctrl-R>\fR allows
you to replace the text in your buffer with the text from another file.
.PP
Suppose, for example, that you had edited \fBexample2.c\fR
and saved it, and now wished to edit \fBexample1.c\fR.
You could exit from \*(ME, then re-invoke \*(ME
for the file \fBexample1.c\fR, but this is cumbersome.
A more efficient way is to simply replace the \fBexample2.c\fR
in your buffer with \fBexample1.c\fR.
.PP
Type \fB<ctrl-X><ctrl-R>\fR.
\*(ME replies with the prompt:  
.DM
	Read file:  
.DE
Type \fBexample1.c\fR.
Notice that \fBexample2.c\fR has rolled away and been replaced with
\fBexample1.c\fR.
Now, check the status line.
Notice that although the name of the \fIbuffer\fR is still
\fBexample2.c\fR, the name of the \fIfile\fR has changed to \fBexample1.c\fR.
You can now edit \fBexample1.c\fR; when you save the edited
text, \*(ME will copy it back into the file \fBexample1.c\fR \*(Em unless,
of course, you again choose to rename the file.
.Sh "Visiting Another Buffer"
.PP
.II "MicroEMACS^<ctrl-X><ctrl-V>"
.II "MicroEMACS^switch buffers"
.II "MicroEMACS^visit command"
.II "MicroEMACS^buffer, switch b."
The last command of this set, the \fIvisit\fR command 
\fB<ctrl-X><ctrl-V>\fR, allows you to create more than
one buffer at a time, to jump from one buffer to another,
and move text between buffers.
This powerful command has numerous features.
.PP
Before beginning, however, straighten up your buffer
by replacing \fBexample1.c\fR with \fBexample2.c\fR.
Type the \fIreplace\fR command 
\fB<ctrl-X><ctrl-R>\fR; when \*(ME replies
by asking
.DM
	Read file:
.DE
at the bottom of your screen, type \fBexample2.c\fR.
.PP
You should now have the file \fBexample2.c\fR read into
the buffer named \fBexample2.c\fR.
.PP
Now, type the \fIvisit\fR command \fB<ctrl-X><ctrl-V>\fR.
\*(ME replies with the prompt
.DM
	Visit file:
.DE
at the bottom of the screen.
Now type \fBexample1.c\fR.
Several things happen.
\fBexample2.c\fR rolls off the screen and is replaced
with \fBexample1.c\fR; the status line changes to show
that both the buffer name and the file name are now
\fBexample1.c\fR; and the message 
.DM
	[Read 23 lines]
.DE
appears at the bottom of the screen.
.PP
This does \fInot\fR mean that your previous buffer
has been erased, as it would have been had you used the
\fIreplace\fR command \fB<ctrl-X><ctrl-R>\fR.
\*(ME is still keeping \fBexample2.c\fR \*(QLalive\*(QR in a buffer
and it is available for editing;
however, it is not being shown on your screen at the present moment.
.PP
Type \fB<ctrl-X><ctrl-V>\fR again, and when the prompt
appears, type \fBexample2.c\fR.
\fBexample1.c\fR scrolls off your screen and is replaced
by \fBexample2.c\fR, and the message
.DM
	[Old buffer]
.DE
appears at the bottom of your screen.
You have just jumped from one buffer to another.
.Sh "Move Text From One Buffer to Another"
.PP
.II "MicroEMACS^text, move from one buffer to another"
The \fIvisit\fR command \fB<ctrl-X><ctrl-V>\fR not only allows you to jump
from one buffer to another:
it allows you to \fImove text\fR from one buffer to another as well.
The following example shows how you can do this.
.PP
First, kill the first line of \fBexample2.c\fR by typing the \fIkill\fR command
\fB<ctrl-K>\fR twice.
This removes both the line of text \fIand\fR the space that it occupied.
If you did not remove the space as well the line itself, no new line would be
created for the text when you yank it back.
Next, type \fB<ctrl-X><ctrl-V>\fR.
When the prompt
.DM
	Visit file:
.DE
appears at the bottom of your screen, type \fBexample1.c\fR.
When \fBexample1.c\fR has rolled onto your screen,
type the \fIyank back\fR command \fB<ctrl-Y>\fR.
The line you killed in \fBexample2.c\fR has now been moved
into \fBexample1.c\fR.
.Sh "Checking Buffer Status"
.PP
.II "MicroEMACS^number of buffers allowed"
.II "MicroEMACS^buffers, number allowed"
The number of buffers you can use at any one time is limited
only by the size of your computer.
You should create only as many buffers as you need
to use immediately; this will help the computer run efficiently.
.PP
.II "MicroEMACS^buffer status command"
.II "MicroEMACS^buffer status"
.II "MicroEMACS^<ctrl-X><ctrl-B>"
.II "MicroEMACS^buffer status window"
To help you keep track of your buffers, \*(ME has the
\fIbuffer status\fR command \fB<ctrl-X><ctrl-B>\fR.
Type \fB<ctrl-X><ctrl-B>\fR.
The status line moves up to the middle
of the screen, and the bottom half of your screen
is replaced with the following display:  
.DM
        C    Size Lines   Buffer         File
        -    ---- -----   ------         ----
        *     655    24   example1.c     example1.c
        *     403    20   example2.c     example2.c
.DE
This display is called the \fIbuffer status window\fR.
The use of windows will be discussed more fully in the following section.
.PP
The letter \fBC\fR over the leftmost column stands for \fBChanged\fR.
An asterisk indicates that that buffer has been changed
since it was last saved, whereas
a space means that the buffer has not been changed.
\fBSize\fR indicates the buffer's size, in number of characters;
\fBBuffer\fR lists the buffer name, and \fBFile\fR lists the file name.
.PP
Now, kill the second line of \fBexample1.c\fR by typing the \fIkill\fR command
\fB<ctrl-K>\fR.
Then type \fB<ctrl-X><ctrl-B>\fR once again.
The size of the buffer \fBexample1.c\fR shrinks from
657 characters to 595 to reflect the decrease in the size of the buffer.
.PP
.II "MicroEMACS^<ctrl-X>1"
To make this display disappear, type the \fIone window\fR command
\fB<ctrl-X>1\fR.
This command will be discussed in full in the next section.
.Sh "Renaming a Buffer"
.PP
.II "MicroEMACS^buffer, prompting for new name"
.II "MicroEMACS^buffer, need unique names"
One more point must be covered with the \fIvisit\fR command.
\*(OS does not allow you to have more than one file with the same name.
For the same reason, \*(ME does not allow you to have more than one
\fIbuffer\fR with the same name.
.PP
Ordinarily, when you visit a file that is not already
in a buffer, \*(ME creates a new buffer and gives it the
same name as the file you are visiting.
However, if for some reason you already have a buffer
with the same name as the file you wish to visit,
\*(ME stops and asks you to give a new, different name to
the buffer it is creating.
.PP
For example, suppose that you wanted to visit a new
\fIfile\fR named \fBsample\fR,
but you already had a \fIbuffer\fR named \fBsample\fR.
\*(ME would stop and give you this prompt at the bottom of the screen:
.DM
	Buffer name:
.DE
You would type in a name for this new buffer.
This name could not duplicate the name of any existing buffer.
\*(ME would then read the file \fBsample\fR into the newly named buffer.
.Sh "Delete a Buffer"
.PP
.II "MicroEMACS^<ctrl-X>K"
.II "MicroEMACS^delete buffer command"
.II "MicroEMACS^buffer, delete"
If you wish to delete a buffer, simply type the \fIdelete buffer\fR command
\fB<ctrl-X>K\fR.
This command allows you to delete only a buffer that
is hidden, not one that is being displayed.
.PP
Type \fB<ctrl-X>K\fR.
\*(ME will give you the prompt:
.DM
	Kill buffer:
.DE
Type \fBexample2.c\fR.
Because you have changed the buffer, \*(ME asks:
.DM
	Discard changes [y/n]?
.DE
Type \fBy\fR, then
.BR <return> .
Now, type the \fIbuffer status\fR command \fB<ctrl-X><ctrl-B>\fR.
The buffer status window no longer shows the buffer \fBexample2.c\fR.
Although the prompt refers to \fIkilling\fR
a buffer, the buffer is in fact \fIdeleted\fR and cannot be yanked back.
.SH "Windows"
.PP
Before beginning this section, it will be necessary to create a new text file.
Exit from \*(ME by typing the \fIquit\fR command \fB<ctrl-X><ctrl-C>\fR;
then reinvoke \*(ME for the text file \fBexample1.c\fR as you did earlier.
.PP
Now, copy \fBexample2.c\fR into a buffer by typing
the \fBvisit\fR command \fB<ctrl-X><ctrl-V>\fR.
When the message
.DM
	Visit file:
.DE
appears at the bottom of your screen, type \fBexample2.c\fR.
\*(ME reads \fBexample2.c\fR into a buffer, and shows the message
.DM
	[Read 21 lines]
.DE
at the bottom of your screen.
.PP
Finally, copy a new text, called \fBexample3.c\fR, into a buffer.
(You copied this file into the current directory when you began this
tutorial.)
Type \fB<ctrl-X><ctrl-V>\fR again.
When \*(ME asks which file to visit, type \fBexample3.c\fR.
The message
.DM
	[Read 123 lines]
.DE
appears at the bottom of your screen.
.PP
The first screenful of text appears as follows:
.B1
.ta .3i 
	/*
	 * Factor prints out the prime factorization of numbers.
	 * If there are any arguments, then it factors these.  If
	 * there are no arguments, then it reads stdin until
	 * either EOF or the number zero or a non-numeric
	 * non-white-space character.  Since factor does all of
	 * its calculations in double format, the largest number
	 * which can be handled is quite large.
	 */
	#include <stdio.h>
	#include <math.h>
	#include <ctype.h>
.sp \n(pDu
	#define NUL '\e0'
	#define ERROR 0x10 /* largest input base */
	#define MAXNUM 200 /* max number of chars in number */
.sp \n(pDu
	main(argc, argv)
	int argc;
	register char *argv[];
.sp \n(pDu
	-- Coherent MicroEMACS -- example3.c -- File: example3.c --------------
.B2
.PP
At this point, \fBexample3.c\fR is on your screen,
and \fBexample1.c\fR and \fBexample2.c\fR are hidden.
.PP
You could edit first one text and then another, while remembering
just how things stood with the texts that were hidden; but
it would be much easier if you could display all three texts on your
screen simultaneously.
\*(ME allows you to do just that by using \fIwindows\fR.
.Sh "Creating Windows and Moving Between Them"
.PP
.II "MicroEMACS^<ctrl-X>2"
A \fIwindow\fR is a portion of your screen that can
be manipulated independent of the rest of the screen.
The following commands let you create windows and move between them:
.II "MicroEMACS^window manipulation"
.DS
.ta .5i 2.3i
	\fB<ctrl-X>2\fR	Create a window
	\fB<ctrl-X>1\fR	Delete extra windows
.sp \n(PDu
	\fB<ctrl-X>N\fR	Move to next window
	\fB<ctrl-X>P\fR	Move to previous window
.DE
The best way to grasp how a window works is to create one and work with it.
To begin, type the \fIcreate a window\fR command \fB<ctrl-X>2\fR.
.PP
Your screen is now divided into two parts, an upper and a lower.
The same text is in each part, and the command lines
give \fBexample3.c\fR for the buffer and file names.
Also, note that you still have only one cursor, which is in
the upper left-hand corner of the screen.
.PP
.II "MicroEMACS^<ctrl-X>N"
.II "MicroEMACS^<ctrl-X>P"
.II "MicroEMACS^window, shifting between"
The next step is to move from one window to another.
Type the \fInext window\fR command \fB<ctrl-X>N\fR.
Your cursor has now jumped to the upper left-hand corner
of the \fIlower\fR window.
.PP
Type the \fIprevious window\fR command \fB<ctrl-X>P\fR.
Your cursor has returned to the upper left-hand corner of the top window.
.PP
.II "MicroEMACS^window, number possible"
Now, type \fB<ctrl-X>2\fR again.
The window on the top of your screen is
now divided into two windows, for a total of three on your screen.
Type \fB<ctrl-X>2\fR again.
The window at the top of your screen has again
divided into two windows, for a total of four.
.PP	
.II "MicroEMACS^arguments, with create window"
It is possible to have as many as 11 windows
on your screen at one time, although each window will
show only the control line and one or two lines of text.
Neither \fB<ctrl-X>2\fR nor \fB<ctrl-X>1\fR can
be used with arguments.
.PP
.II "MicroEMACS^<ctrl-X>1"
Now, type the \fIone window\fR command \fB<ctrl-X>1\fR.
All of the extra windows have been eliminated, or \fIclosed\fR.
.Sh "Enlarging and Shrinking Windows"
.PP
.II "MicroEMACS^<ctrl-X>Z"
.II "MicroEMACS^window, enlarge"
When \*(ME creates a window, it divides into half the window in which the
cursor is positioned.
You do not have to leave the windows at the size \*(ME creates them, however.
If you wish, you may adjust the relative size of each window on
your screen, using the \fIenlarge window\fR and \fIshrink window\fR
commands:
.DS
.ta 0.5i 2.0i
	\fB<ctrl-X>Z\fR	Enlarge window
	\fB<ctrl-X><ctrl-Z>\fR	Shrink window
.DE
To see how these work, first type \fB<ctrl-X>2\fR twice.
Your screen is now divided into three windows:
two in the top half of your screen, and the third in the bottom half.
.PP
Now, type the \fIenlarge window\fR command \fB<ctrl-X>Z\fR.
The window at the top of your screen is now one line bigger:
it has borrowed a line from the window below it.
Type \fB<ctrl-X>Z\fR again.
Once again, the top window has borrowed a line from the middle window.
.PP
Now, type the \fInext window\fR command
\fB<ctrl-X>N\fR to move your cursor into the middle window.
Again, type the \fIenlarge window\fR command \fB<ctrl-X>Z\fR.
The middle window has borrowed a line from the bottom
window, and is now one line larger.
.PP
The \fIenlarge window\fR command \fB<ctrl-X>Z\fR
allows you to enlarge the window your cursor is
in by borrowing lines from another window,
provided that you do not shrink that other window out of existence.
Every window must have at least two lines in it:
one command line and one line of text.
.PP
.II "MicroEMACS^<ctrl-X><ctrl-Z>"
.II "MicroEMACS^window, shrink"
The \fIshrink window\fR command
\fB<ctrl-X><ctrl-Z>\fR allows you to decrease the size of a window.
Type \fB<ctrl-X><ctrl-Z>\fR.
The present window is now one line smaller, and
the lower window is one line larger because the line
borrowed earlier has been returned.
.PP
The \fIenlarge window\fR and \fIshrink window\fR commands can also be used
with arguments introduced with \fB<ctrl-U>\fR.
However, remember that \*(ME will not accept
an argument that would shrink another window out of existence.
.Sh "Displaying Text Within a Window"
.PP
Displaying text within the limited area of a window can
present special problems.
The \fIview\fR commands \fB<ctrl-V>\fR and \fB<esc>V\fR roll
window-sized portions of text up or down, but
you may become disoriented when a window shows only
four or five lines of text at a time.
Therefore, three special commands are available for displaying
text within a window:
.DS
.ta 0.5i 2.0i
	\fB<ctrl-X><ctrl-N>\fR	Scroll down
	\fB<ctrl-X><ctrl-P>\fR	Scroll up
.sp \n(PDu
	\fB<esc>!\fR	Move within window
.DE
.II "MicroEMACS^<ctrl-X><ctrl-N>"
.II "MicroEMACS^window, scroll down"
.II "MicroEMACS^<ctrl-X><ctrl-P>"
.II "MicroEMACS^scroll up"
.II "MicroEMACS^scroll down"
Two commands allow you to move your text by one line at a time, or
\fIscroll\fR it:
the \fIscroll up\fR command \fB<ctrl-X><ctrl-N>\fR,
and the \fIscroll down\fR command \fB<ctrl-X><ctrl-P>\fR.
.PP
Type \fB<ctrl-X><ctrl-N>\fR.
The line at the top of your window has vanished,
a new line has appeared at the bottom of your window, and
the cursor is now at the beginning of what had been the
second line of your window.
.PP
Now type \fB<ctrl-X><ctrl-P>\fR.
The line at the top that had vanished earlier has now returned, the
cursor is at the beginning of it, and
the line at the bottom of the window has vanished.
These commands allow you to move forward in your text
slowly so that you do not become disoriented.
.PP
Both of these commands can be used with arguments introduced by \fB<ctrl-U>\fR.
.PP
.II "MicroEMACS^<esc>!"
.II "MicroEMACS^window, move within"
The third special movement command is the
\fImove within window\fR command \fB<esc>!\fR.
This command moves the line your cursor is on to the top of the window.
.PP
To try this out, move the cursor down three lines by
typing \fB<ctrl-U>3<ctrl-N>\fR;
now type \fB<esc>!\fR.
(Be sure to type an exclamation point \*(Ql!\*(Qr, not a
numeral one \*(Ql1\*(Qr, or your window will vanish.
The command \fB<esc>1\fR is explained below.)
The line to which you had moved the cursor
is now the first line in the window, and three new
lines have scrolled up from the bottom of the window.
You will find this command to be very useful as you
become more experienced at using windows.
.PP
All three special movement commands can also be used when your
screen has no extra windows, although you will not need them as much.
.Sh "One Buffer"
.PP
.II "MicroEMACS^window, use with editing"
.II "MicroEMACS^window, using multiple buffers"
Now that you have been introduced to the commands
for manipulating windows, you can begin
to use windows to speed your editing.
.PP
To begin with, scroll up the window you are in
until you reach the top line of your text.
You can do this either by typing the \fIscroll up\fR command
\fB<ctrl-X><ctrl-P>\fR several times, or by typing \fB<esc><\fR.
.PP
Kill the first line of text with the \fIkill\fR command \fB<ctrl-K>\fR.
The first line of text has vanished from all three windows.
Now, type \fB<ctrl-Y>\fR to yank back the text you just killed.
The line has reappeared in all three windows.
.PP
.II "MicroEMACS^buffer, with windows"
.II "MicroEMACS^file, with windows"
.II "MicroEMACS^switch buffer command"
.II "MicroEMACS^<ctrl-X>B"
The main advantage to displaying one buffer with more than one window
is that each window can display a different portion of the text.
This can be quite helpful if you are editing or moving a large text.
.PP
To demonstrate this, do the following:
First, move to the end of the text in your
present window by typing the \fIend of text\fR command \fB<esc>>\fR,
then typing the \fIprevious line\fR command \fB<ctrl-P>\fR four times.
Now, kill the last four lines.
.PP
You could move the killed lines to the beginning of your text
by typing the \fIbeginning of text\fR command \fB<esc><\fR;
however, it is more convenient simply to type the \fInext
window\fR command \fB<ctrl-X>N\fR, which moves you to
the beginning of the text as displayed in the next window.
\*(ME remembers a different cursor position for each window.
.PP
Now yank back the four killed lines by typing \fB<ctrl-Y>\fR.
You can simultaneously observe that the lines have been
removed from the end of your text and that they have been
restored at the beginning.
.Sh "Multiple Buffers" 
.PP
Windows are especially helpful when they display more than one text.
Remember that at present you are working with \fIthree\fR buffers, named
\fBexample1.c\fR,
\fBexample2.c\fR, and \fBexample3.c\fR, although your screen
is displaying only the text \fBexample3.c\fR.
To display a different text in a window, use the
\fIswitch buffer\fR command \fB<ctrl-X>B\fR.
.PP
Type \fB<ctrl-X>B\fR.
When \*(ME asks 
.DM
	Use buffer:
.DE
at the bottom of the screen, type
\fBexample1.c\fR.
The text in your present window is replaced with \fBexample1.c\fR.
The command line in that window
changes, too, to reflect the fact that the buffer and the file names are now
\fBexample1.c\fR.
.Sh "Moving and Copying Text Among Buffers"
.PP
.II "MicroEMACS^window, moving text among"
.II "MicroEMACS^window, copying text among"
.II "MicroEMACS^copying text"
It is now very easy to copy text among buffers.
To see how this is done, first kill the first line of \fBexample1.c\fR
by typing the \fB<ctrl-K>\fR command twice.
Yank back the line immediately by typing \fB<ctrl-Y>\fR.
Remember, the line you killed has \fInot\fR been
erased from its special storage area, and may be
yanked back any number of times.
.PP
Now, move to the previous window by typing \fB<ctrl-X>P\fR,
then yank back the killed line by typing \fB<ctrl-Y>\fR.
This technique can also be used with the \fIblock kill\fR command
\fB<ctrl-W>\fR to move large amounts of text from one buffer to another.
.Sh "Checking Buffer Status"
.PP
.II "MicroEMACS^window, buffer status command use"
.II "MicroEMACS^buffer status command, with windows"
The \fIbuffer status\fI command \fB<ctrl-X><ctrl-B>\fR can
be used when you are already displaying more than one window on your screen.
.PP
When you want to remove the buffer status window, use either
the \fIone window\fR command \fB<ctrl-X>1\fR, or move your
cursor into the buffer status window using the \fInext window\fR
command \fB<ctrl-X>N\fR and replace it with another buffer
by typing the \fIswitch buffer\fR command \fB<ctrl-X>B\fR.
.Sh "Saving Text From Windows"
.PP
The final step is to save the text from your windows and buffers.
Close the lower two windows with the \fIone window\fR
command \fB<ctrl-X>1\fR.
Remember, when you close a window, the text that it displayed
is still kept in a buffer that is \fIhidden\fR from your 
screen.
For now, do \fInot\fR save any of these altered texts.
.PP
.II "MicroEMACS^window, saving text"
When you use the \fIsave\fR command \fB<ctrl-X><ctrl-S>\fR, 
only the text in the window in which the cursor
is positioned is written to its file.
If only one window is displayed on the screen, the \fIsave\fR
command will save only its text.
.PP
If you made changes to the text in another buffer,
such as moving portions of it to another buffer, \*(ME would ask
.DM
	Quit [y/n]:
.DE
If you answer \*(Ql\fBn\fR\*(Qr, \*(ME will \fIsave\fR
the contents of the buffer you are currently displaying
by writing them to your disk, but it will ignore the
contents of other buffers, and your cursor will be returned
to its previous position in the text.
If you answer \*(Ql\fBy\fR\*(Qr, \*(ME again will save the
contents of the current buffer and ignore the other buffers,
but you will exit from \*(ME and return to the shell.
Exit from \*(ME by typing the \fIquit\fR command \fB<ctrl-X><ctrl-C>\fR.
.SH "Keyboard Macros"
.PP
A \fIkeyboard macro\fR is a set of \*(ME commands that are stored
in memory and given a name.
After you create a keyboard macro, you can execute it again and again
just by typing its name.
If you are revising a big file, you will find that keyboard macros are
one of the most useful features in \*(ME, and one that you will use
often.
.PP
The following table summarizes \*(ME's keyboard-macro commands:
.DS
.ta 0.5i 2.0i
	\fB<ctrl-X>(\fR	Open a keyboard macro
	\fB<ctrl-X>)\fR	Close a keyboard macro
	\fB<ctrl-X>E\fR	Execute a keyboard macro
.sp \n(pDu
	\fB<ctrl-X>M\fR	Rename a keyboard macro
	\fB<ctrl-X>I\fR	Bind current macro as initialization macro
.DE
.Sh "Creating a Keyboard Macro"
.PP
.II "MicroEMACS^end macro command"
.II "MicroEMACS^<ctrl-X>E"
.II "MicroEMACS^execute macro command"
To begin to create a macro, type the \fIbegin macro\fR
command \fB<ctrl-X>(\fR.
Be sure to type an open parenthesis \*(Ql(\*(Qr, not a numeral \*(Ql9\*(Qr.
\*(ME will reply with the message
.DM
	[Start macro]
.DE
Type the following phrase:
.DM
	MAXNUM
.DE
Then type the \fIend macro\fR command \fB<ctrl-X>)\fR.
Be sure you type a close parenthesis \*(Ql)\*(Qr, not a numeral \*(Ql0\*(Qr.
\*(ME will reply with the message
.DM
	[End macro]
.DE
Move your cursor down two lines and execute the macro
by typing the \fIexecute macro\fR command \fB<ctrl-X>E\fR.
The phrase you typed into the macro has been inserted
into your text.
.PP
If you give these commands in the wrong order, \*(ME
warns you that you are making a mistake.
For example, if you open a keyboard macro by typing \fB<ctrl-X>(\fR,
and then attempt to open another keyboard macro by again typing
\fB<ctrl-X>(\fR, \*(ME will say:
.DM
	Not now
.DE
Should you accidentally open a keyboard macro, or enter the
wrong commands into it, you can cancel the entire macro simply
by typing \fB<ctrl-G>\fR.
.Sh "Execute a Macro Repeatedly"
.PP
As with most \*(ME commands,
you can use a keyboard macro with an argument to execute it repeatedly.
For example, if you have defined a keyboard macro, then typing
.DS
	\fB<ctrl-U><ctrl-X>E\fR
.DE
executes that macro four times.
(Remember, four is the default value for
\fB<ctrl-U>\fR.)
.PP
As described above, \fB<ctrl-U>\fR normally is used with a positive number,
to indicate how often \*(ME should execute a given command or macro.
With keyboard macros, however, you can use a special value for \fB<ctrl-U>\fR:
\-1.
This tells \*(ME to repeatedly execute a keyboard macro until it fails.
.PP
For example, consider that you define the following keyboard macro:
.DS
	\fB<ctrl-S>\fR foo \fB<ctrl-K>\fR
.DE
This macro searches for the string ``foo'', then kills the rest of line
that that string is on.
Now, when you type the command
.DS
	\fB<ctrl-U>\fR \-1 \fB<ctrl-X>E\fR
.DE
executes this macro until \*(ME can no longer find the string ``foo'';
it then quits.
.PP
Obviously, you should define your macro carefully before you execute it
with this \-1 option to \fB<ctrl-U>\fR; otherwise, you can commit tremendous
mayhem on your file with one keystroke.
.Sh "Replacing a Macro"
.PP
To replace this macro with another, go through the same
process.
Type \fB<ctrl-X>(\fR.
Then type the \fIbuffer status\fR command
\fB<ctrl-X><ctrl-B>\fR, and type \fB<ctrl-X>)\fR.
Remove the buffer status window by typing the \fIone window\fR
command \fB<ctrl-X>1\fR. 
.PP
Now execute your keyboard macro
by typing the \fBexecute macro\fR command \fB<ctrl-X>E\fR.
The \fIbuffer status\fR command has executed once more.
.Sh "Renaming a Macro"
.PP
Many times during a long editing session, you will find that you
use one keyboard macro, then use a second keyboard macro, then find that
you need the first macro again.
In previous releases of \*(ME, the only way to do this work was to
type the first macro, replace it with the second macro, then retype
the first macro when you need it again.
The present edition of \*(ME, however, lets you define any number of
keyboard macros, and save them by giving each one a unique ``name'' \*(Em
that is, its own unique keyboard binding.
.PP
To rename a keyboard macro that you have already created, use the
\fIrename macro\fR command \fB<ctrl-X>M\fR.
To see how this works, do the following:
(1) Type \fB<ctl-X>(\fR to open the keyboard macro.
(2) Type \fB<esc>S\fR xyz \fB<ctrl-U> <ctrl-D>\fR
to fill the macro with something.
(3) Finally, type \fB<ctrl-X>)\fR to close the macro.
.PP
Now, type \fB<ctrl-X>M\fR, to rename the macro.
\*(ME will reply with the prompt:
.DM
	enter keybinding for macro
.DE
Type \fB<esc>L\fR.
This tells \*(ME to take the keyboard macro you created and link it to the
keystrokes \fB<esc>L\fR.
.PP
Now, whenever you type \fB<esc>L\fR, \*(ME will execute
\fB<esc>\fRs xyz \fB<ctrl-U> <ctrl-D>\fR.
You can now define another keyboard macro without wiping out the one
you have renamed.
There is no theoretical limit to the number of keyboard macros you can
create, although there are practical limits imposed by the amount of
memory available to \*(ME.
.Sh "Renaming Macros:  A Few Caveats"
.PP
Please note that if you name a keyboard macro with a keystroke that is
already defined, \*(ME will no longer be able to access
that keystroke's functionality.
.PP
For example, if instead of naming your new macro \fB<esc>L\fR you named it
\fB<ctrl-A>\fR, then every time you typed \fB<ctrl-A>\fR \*(ME would execute
\fB<esc>S\fR xyz \fB<ctrl-U> <ctrl-D>\fR and you would no longer be able
to jump to the beginning of a line (which \fB<ctrl-A>\fR normally does).
.PP
The only exceptions are \fB<ctrl-X>\fR, \fB<esc>\fR, and the
\fB<ctrl-X>R\fR command (described below),
which \*(ME will not let you reassign.
Obviously, you should be very careful when you assign a name to a keyboard
macro, or you could easily clobber much of the editor's functionality.
.PP
Note, too, that \*(ME lets you define reflexive keybindings, but these
never work.
For example, if you named the above example macro
.B <ctrl-D>
instead of
.BR <esc>L ,
then every time you typed \fB<ctrl-D>\fR \*(ME would try to execute a macro
that included \fB<ctrl-D>\fR in it.
Obviously, this can tie \*(ME into knots in no time.
Again, please be very careful when you assign names to keyboard macros.
.PP
The commands \fB<ctrl-X>S\fR and \fB<ctrl-X>L\fR let you save
all named keyboard macros into a file, and restore them during a later
editing session.
These commands are described in the next section.
.Sh "Setting the Initialization Macro"
.PP
\*(ME allows one macro to be specified which will be executed every time
\*(ME is invoked.
This ``initialization macro'' can be set using the key sequence
\fB<ctrl-X>I\fP and causes \*(ME to ``bind'' the currently defined macro
to the initialization macro.
.SH "Flexible Key Bindings"
.PP
As you have noticed by now, \*(ME works through standard \fIkey bindings:\fR
that is, one keystroke or combination of keystrokes tells \*(ME to perform
a particular task.
For example, typing \fB<ctrl-A>\fR tells \*(ME to move the cursor to the
beginning of the line; typing \fB<ctrl-E>\fR tells \*(ME to move the cursor
to the end of the line; and so on.
.PP
\*(ME
allows you to change its key bindings, so you can bind a given keystroke or
combination of keystrokes to a task other than the default one documented
in this tutorial.
In this way, you can reconfigure \*(ME so that it resembles another editor
with which you are more familiar.
.PP
To perform this magic, \*(ME
uses two tables for keybindings:
a \fIdefault table\fR that is loaded at compile time and never changes,
and a \fIdynamic table\fR
that you can modify with \*(ME's keybinding commands.
.PP
The following table summarizes \*(ME's commands for flexible keybindings:
.DS
.ta 0.5i 2.0i
	\fB<ctrl-X>R\fR	Replace one binding with another
	\fB<ctrl-X>X\fR	Rebind metakeys
.sp \n(pDu
	\fB<ctrl-X>S\fR	Save flexible bindings and macros into file
	\fB<ctrl-X>L\fR	Load flexible bindings and macros from file
.DE
.Sh "Changing a Keybinding"
.PP
The \fIreplace binding\fR command \fB<ctrl-X>R\fR replaces one binding with
another.
For example, if you wished to replace the \fIbeginning of line\fR command
\fB<ctrl-A>\fR with \fB<esc>Z\fR, you would do the following:
.RS
.IP \fB1.\fR
Type \fB<ctrl-X>R\fR to invoke the rebinding command.
.IP \fB2.\fR
When you see the prompt
.DM
	Enter old keybinding
.DE
type the keybinding you wish to change \*(Em in this case, \fB<ctrl-A>\fR.
.IP \fB3\fR
When you then see the prompt
.DM
	Enter new keybinding
.DE
type the keybinding to which you wish to change it \*(Em in this case,
\fB<esc>Z\fR.
.PP
Note that you cannot rebind the command \fB<ctrl-X>R\fR itself; otherwise,
you would paint yourself into a corner.
Also, note that if you rebind a command to itself (that is, if you type the
same keybinding in response to both prompts), then that keybinding is
bound to the old meaning of the keybindings, should there be any.
.II "MicroEMACS^metakey"
.Sh "Rebinding Metakeys"
.PP
\*(ME's keybindings depend on several pre-defined metakeys.
A \fImetakey\fR is a keystroke that introduces a further set of commands.
\*(ME's default keybindings use two metakeys:
.B <ctrl-X>
and
.BR <esc> .
Other editors use other keystrokes as metakeys.
If you wish to rebind a metakey, use the \fIrebind metakey\fR
command \fB<ctrl-X>X\fR.
This command prompts you to bind up to three metakeys, and the
argument key \fB<ctrl-U>\fR.
.PP
For example, suppose that you wish to change the metakey \fB<ctrl-X>\fR
to \fB<ctrl-Q>\fR.
To begin, type the command \fB<ctrl-X>X\fR.
You will see the prompt
.DM
	Enter prefix character 1 or space
.DE
``Prefix character 1'' is \fB<ctrl-X>\fR in the default bindings.
Type \fB<ctrl-Q>\fR.
You will then see the prompt:
.DM
	Enter prefix character 2 or space
.DE
``Prefix character 2'' is \fB<esc>\fR in the default bindings.
Since you do not want to change it, type \fB<space>\fR.
You will then see the prompt:
.DM
	Enter prefix character 3 or space
.DE
There is no ``prefix character 3'' in the default bindings, but you can
set a third one for your keybindings if you wish.
Since (for the sake of this example) you do not wish to set one, type
\fB<space>\fR.
Finally, you will see the prompt:
.DM
	Enter repeat code or space
.DE
The ``repeat code'' executes a command repeatedly; in this tutorial, it is
often called the ``argument key'' or ``argument command''.
Since (in this example) you do not wish to change it, type \fB<space>\fR.
.PP
Now that you have reset the \fB<ctrl-X>\fR metakey, you must now type
\fB<ctrl-Q>\fR every time in place of \fB<ctrl-X>\fR throughout all of
the \*(ME commands.
For example, if you wished to change the metakey back from
\fB<ctrl-Q>\fR to \fB<ctrl-X>\fR, you would have to type \fB<ctrl-Q>X\fR
to invoke the \fIrebind metakey\fR command.
.PP
Note that because \fB<ctrl-Q>\fR already is bound in the \*(ME keybindings,
when you rebind it the command
to which it was bound is no longer available to you.
However, if you un-rebind the key, then it automatically is rebound to its
old command.
In the above example, \fB<ctrl-Q>\fR is bound to the \fIinsert literal
character\fR command, which lets you insert control characters into your
file.
When you rebound the \fB<ctrl-X>\fR metakey to \fB<ctrl-Q>\fR, then the
\fIinsert literal character\fR command was no longer available to you.
However, when you re-rebound the \fB<ctrl-Q>\fR metakey to \fB<ctrl-X>\fR,
then \fB<ctrl-Q>\fR was automatically rebound to the \fIinsert literal
character\fR command.
.\" Got that?
.PP
To change the first prefix character back to
.BR <ctrl-X> ,
type
.BR <ctrl-Q>X ,
then enter
.B <ctrl-X>
when you see the prompt for prefix character 1.
This restores the original metacharacter.
Note, however, that the original function of
.B <ctrl-Q>
(which is to let you embed control characters within a file)
is no longer available to you:
\*(ME ``forgot'' its original function when you made
.B <ctrl-Q>
into a prefix character.
.Sh "Save and Restore Keybindings"
.PP
\*(ME lets you save your rebound keybindings into a file, and reload
them during another editing session.
To save your keybindings into a file, type the \fIsave keybindings\fR command
\fB<ctrl-X>S\fR.
Try it.
You will see the prompt:
.DM
	Store bindings file:
.DE
Type the name of a file.
\*(ME then writes its keybindings into that file.
This command saves all named keyboard macros that you
have created.
It also saves other aspects of the \*(ME environment that you have set;
for example, if you have turn on word-wrapping, that fact will be saved.
.PP
To restore a set of keybindings, use the \fIrestore keybindings\fR command
\fB<ctrl-X>L\fR.
Try it.
You will see the prompt:
.DM
	Load bindings file:
.DE
Type the name of the file in which you saved \*(ME' keybindings
and all named keyboard macros.
\*(ME will then load them into memory for you.
.PP
These commands let you prepare several sets of customized keybindings
and macros.
You can customize keybindings to suit your preference, or create custom
sets of macros to suit any
number of specialized editing tasks.
.PP
.II .emacs.rc
.II "MicroEMACS^.emacs.rc"
By default, \*(ME checks for the existence of file
.B "$HOME/.emacs.rc"
and executes it if found.
You can generate a copy of
.B .emacs.rc
using the save-keybindings command \fB<ctrl-X>S\fR.
The \*(ME command-line option \fB\-f\fR lets you specify an alternate file of
keybindings macros.
After it load \fB.emacs.rc\fR,
\*(ME then executes the initialization macro, if one exists.
For example, if you wish to use the set of keybindings saved in file
\fBkeybind\fR to edit file \fBtextfile\fR, then you would type the following:
.DM
	me -f keybind textfile
.DE
As you can see, \*(ME's system of keyboard macros and flexible key bindings
help make it an extremely flexible and powerful editor.
.SH "Sending Commands to COHERENT"
.PP
.II "MicroEMACS^program interrupt"
.II "MicroEMACS^<ctrl-X>!"
The only remaining commands you need to learn are
the \fIprogram interrupt\fR commands \fB<ctrl-X>! and \fB<ctrl-C>\fR.
These commands allow you to interrupt your editing, give a command
directly to the shell,
and then resume editing without affecting your text in any way.
.PP
The command \fB<ctrl-X>!\fR allows you to send \fIone\fR
command line (one command, or several commands plus separators)
to the operating system.
To see how this command works, type \fB<ctrl-X>!\fR.
The prompt
\fB!\fR
has appeared at the bottom of your screen.
Type
\fBlc\fR.
Observe that the directory's table of contents scrolls across your
screen, followed by the message \fB[end]\fR.
To return to your editing, simply type a carriage return.
.II "MicroEMACS^<ctrl-C>"
The \fIinterrupt\fR command \fB<ctrl-C>\fR suspends editing
indefinitely, and allows you to send an unlimited number
of commands to the operating system.
To see how this works, type \fB<ctrl-C>\fR.
After a moment, the \*(CO system's prompt will
appear at the bottom of your screen.
Type \fBtime\fR.
The \*(CO system replies by printing the time and date.
To resume editing, then simply type \fB<ctrl-D>\fR. 
.PP
If you wish, you can suspend \*(ME's operation, tell the
\*(CO system to invoke another copy of the \*(ME program,
edit a file, then return to your previous editing.
To see how this is done, type \fB<ctrl-C>\fR.
When the prompt appears at the bottom of your screen, type
.DM
	me example1.c
.DE
It doesn't matter that you are already editing \fBexample1.c\fR.
\*(ME will simply copy the \fBexample1.c\fR file into a new
buffer and let you work as if the other \*(ME program
you just interrupted never existed.
.PP
Exit from this second \*(ME program by typing the \fIquit\fR command 
\fB<ctrl-X><ctrl-C>\fR.
Then type \fB<ctrl-D>\fR.
Your original \*(ME program has now been resumed.
However, none of the changes you made in the
secondary \*(ME program will be seen here.
.PP
It is not a good idea to use multiple \*(ME programs to edit
the same program:  it is too easy to become confused as to
which edits were made to which version.
.PP 
The only time this is advisable is if you wish to test to see
how a certain edit would affect your text:
you can create a new \*(ME program,
test the command, and then destroy the altered buffer and return
to your original editing program without having to worry that you
might make errors that are difficult to correct.
.PP
Now type \fB<ctrl-X><ctrl-C>\fR to exit.
.Sh "Compiling and Debugging Through MicroEMACS"
.II "MicroEMACS^compiling and debugging"
.II "cc^MicroEMACS mode"
.II "MicroEMACS^automatic mode"
.PP
\*(ME can be used with the compilation command
.B cc
to give you a reliable system for debugging new programs.
.PP
Often, when you're writing a new program, you face the situation in which
you try to compile, but the compiler produces error messages
and aborts the compilation.
You must then invoke your editor, change the program, close the editor,
and try the compilation over again.
This cycle of compilation \*(Em editing \*(Em recompilation can be quite
bothersome.
.PP
To remove some of the drudgery from compiling, the
.B cc
command has the
.IR automatic ,
or
\*(ME option,
.BR \-A .
When you compile with this option, the \*(ME screen editor will
be invoked automatically if any errors occur.
The error or errors generated during compilation will be displayed in
one window, and your text in the other, with the cursor set at the
number of the line that the compiler indicated had the error.
.PP
Try the following example.
Use \*(ME to enter the following program, which you should call
.BR error.c :
.DM
	main() {
		printf("Hello, world!\en")
	}
.DE
The semicolon was left off of the
.B printf()
statement, which is an error.
Now, save the file and exit from \*(ME; then
try compiling
.B error.c
with the following
.B cc
command:
.DM
	cc -A error.c
.DE
You should see no messages from the compiler because they are all being
diverted into a buffer to be used by \*(ME.
Then \*(ME will appear automatically.
In one window you should see the message:
.DM
	3: error.c : missing ';'
.DE
and in the other you should see your source code for
.BR error.c ,
with the cursor set on line 3.
.PP
If you had more than one error, typing
.B <ctrl-X>>
.II "MicroEMACS^<ctrl-X>>"
.II "MicroEMACS^next error"
would move you to the next line with an error in it; typing
.B <ctrl-X><
.II "MicroEMACS^<ctrl-X><"
.II "MicroEMACS^previous error"
would return you to the previous error.
With some errors, such as those for missing braces or
semicolons, the compiler cannot always tell exactly which line the
error occurred on, but it will almost always point to a line that
is near the source of the error.
.PP
Now, correct the error by typing a semicolon at the end of line 2.
Close the file by typing
.BR <ctrl-Z> .
.B cc
will be invoked again automatically.
.PP
.B cc
will continue to compile your program either until the program compiles
without error,
or until you exit from \*(ME by typing
.B <ctrl-U>
followed by
.BR <ctrl-X><ctrl-C> .
.Sh "The MicroEMACS Help Facility"
.II "MicroEMACS^help, in MicroEMACS"
.II "MicroEMACS^help window"
.PP
\*(ME has a built-in help function.
With it, you can ask for information either for a word that you
type in, or for a word over which the cursor is positioned.
The \*(ME help file contains the bindings for all library
functions and macros included with \*(PN.
.PP
For example, consider that you are preparing a C program and want more
information about the function
.BR fopen .
.II "MicroEMACS^<ctrl-X>"
Type \fB<ctrl-X>?\fR.
At the bottom of the screen will appear the prompt
.DM
	Topic:
.DE
Type \fBfopen\fR.
\*(ME will search its help file, find its entry for
.BR fopen ,
then open a window and print the following:
.DM
	fopen - Open a stream for standard I/O
	#include <stdio.h>
	FILE *fopen (name, type) char *name, *type;
.DE
If you wish, you can kill the information in the help window
and yank it into your program to ensure that you prepare the
function call correctly.
.PP
Consider, however, that you are checking a program written earlier,
and you wish to check the call to \fBfopen\fR.
Simply move the cursor until it is positioned over one of the letters in
\fBfopen\fR, then type \fB<esc>?\fR.
.II "MicroEMACS^<esc>?"
\*(ME will open its help window, and show the same information it
did above.
.PP
.II "MicroEMACS^<esc>2"
To erase the help window, type \fB<esc>1\fR.
.SH "Where To Go From Here"
.PP
For a complete summary of \*(ME's commands, see
the entry for
.B me
in the Lexicon.
The \*(CO system includes three other editors: the stream editor \fBsed\fR,
the popular screen editor \fBvi\fP,
and the interactive line editor \fBed\fR.
Each can help you accomplish editing tasks that may not be well suited for
\*(ME.
For more information on these editors, see their tutorials or check their
entries in the Lexicon.
