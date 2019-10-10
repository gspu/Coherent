.ds TL "Using X"
.NH "Using the X Window System"
.PP
This chapter introduces you to the using the X Window System.
This and the following chapter are intended for novices to X Windows.
If you are experienced at using the X Window System,
we suggest that you look at the Lexicon articles
.B "X clients"
and
.B "X utiities"
to see what programs are included with this package.
.PP
This chapter first introduces a graphical-user interface (GUI) and
the window manager
.BR twm .
The window manager is responsible for the appearance of the screen,
and manages windows and icons.
This portion of the chapter assumes that you have little or no experience
with a GUI.
.II "Microsoft Windows"
Even if you have worked with other GUIs, such as Microsoft
Windows, you should look over this section to see how
.B twm
differs from what you are used to.
.PP
This chapter then gives a tour of the X Window System, to show you what
files are stored where.
It then discusses the structure of the X Window System
and the elements that comprise it.
Some of this is a little dry for a novice; however, knowing something
of what X can do and how it works will make it much easier for you to learn
how to work with X.
.PP
Finally, this chapter ends with a description of how to modify
the window manager to suit your needs and tastes.
.SH "Introduction to a Graphical Interface"
.PP
To bring up X, log into your system's console, and type
.BR startx .
If you have installed X properly, your screen will clear and within a minute
your console will appear something like this:
.PH 1 1 \*(XD/root1.eps
.PP
The actual amount of screen space that the windows take up depends upon
the mode of your physical screen and the size of the virtual screen,
as described in the installation notes.
.PP
As you can see, there are three windows on the screen:
a window that shows a clock, in the upper left corner of the screen; a
small window at the top of the screen that displays the word
.BR xvt ;
and a large, blank window that has a prompt in it.
The rest of the window is filled with a gray field.
.Sh "Using the Mouse"
.PP
To begin, try working with the mouse.
The mouse should turned so that the wire on the mouse is away from you
and the buttons are under your fingertips.
As you slide the mouse ``up'' on the desk (that is, away from you),
watch the screen.
You'll see a small cursor moving ``up'' on the screen \(em that is, toward
the top of the screen.
Now, slide the mouse toward you.
The cursor moves toward the bottom of the screen.
Slide the mouse to the right \(em the cursor slides to the right;
slide it to the left \(em the cursor moves to the left.
The principle is obvious:
the cursor moves around the screen in a manner analogous with
the movement of the mouse.
.II "mouse cursor^definition"
.II pointer^definition
This cursor is called the
.IR "mouse cursor" ;
in X nomenclature is also called the
.IR pointer .
.PP
Notice that as the cursor moves around the screen, it changes shapes.
When it is against the gray background, it is an \(mu shape; when it is
in the clock window or the little window labelled
.B xvt
it is an \(<-; while when it is the large, blank window its shape resembles
a capital I.
.II xvt
(For the sake of brevity, we will refer to the large, blank window as the
.B xvt
window.
We'll discuss below just what
.B xvt
is.)
.PP
Also, as the cursor enters and leaves windows, things happen:
when the cursor enters the
.B xvt
window, its border changes color,
the text cursor changes from outlined to solid, and the inner
border of the small window at the top of the screen also changes color.
The same things happen when you shift the cursor into the small window,
which suggests that the two windows are somehow linked.
.PP
.II "mouse buttons"
As you've noticed, your mouse has buttons on it.
(Some mice have two buttons, some have three.
For present, we'll ignore the middle-mouse button and work only with the
left and right buttons.)
By pressing the mouse button, you can tell an application to do something;
what happens depends on what programs you are running under X.
.PP
.II mouse^click
.II mouse^drag
.II clicking
.II dragging
There are two kinds of stroke on a mouse buttons:
.I clicking
and
.IR dragging .
Clicking is when you rapidly depress and release the button, just like you
press a key on your keyboard.
You often will use this stroke to press a ``button'' on the screen.
Dragging is when you depress a mouse button, slide the mouse, and then release
the mouse button.
You often use this stroke to select an item from a menu or to move or
resize a window.
(How you can do these actions is described below.)
.PP
To try this, slide the mouse so that the mouse cursor shifts
into the little window at the top of the screen.
The cursor is in the little window when the window's outline brightens.
Click the left-mouse button.
The
.B xvt
window disappears from the screen, and a small, stylized `X' appears
in the little window.
Without moving the cursor, click the left-mouse button again.
The
.B xvt
window reappears, just as it was before, and the stylized `X'
disappears.
.PP
.II icon
To explain what just happened, the small window is an
.I icon
for the
.B xvt
window.
Icons give you a way to run a large number of programs
simultaneously, without cluttering the screen with windows.
You can specifically exclude certain X programs from having an icon.
You would do this because an icon (like every other X object) consumes
memory, and some programs (such as the clock program) always appear on
the screen.
We'll discuss icons further below.
.Sh "Moving and Resizing Windows"
.PP
With the mouse you can move and resize a window.
Both involve dragging the mouse.
.PP
First, try to resize a window.
Slide the mouse so that its cursor is touching the right or bottom border
of the window.
When the cursor touches the frame, it changes shape, from an \(mu to an \(<-.
Now, press the left-mouse button and hold it down.
When you do this, three things happen:
.IP \fB1.\fR 0.3i
The mouse cursor changes to a stylized `+' sign.
.IS \fB2.\fR
The clock window is covered with a grid.
.IS \fB3.\fR
A small window pops up in the upper left corner and displays a
message that gives the X and Y dimensions of the window.
.PP
(If nothing happens, the tip of the \(<- is not touching the window's frame
but is in the inner part of the window.
Release the left-mouse button,
slide the cursor until the tip of the \(<- is touching the frame, and
then press the left button again.)
.PP
The numbers in the small pop-up window give the dimensions of the window,
in pixels.
.II pixel
(A
.I pixel
is one of the tiny dots that comprise your screen.)
While holding down the left-mouse button, slide the mouse to the right and
down until the numbers in the small window are approximately
\fB230\(mu230\fR.
(We say ``approximately'' because a mouse is not a precision instrument,
and trying to position it exactly can be a maddening task.)
Note that as you drag the mouse, the grid that was superimposed on the
window ``stretches'' to match the movement of the mouse cursor.
Now release the left-mouse button.
The clock window is now larger, and your screen looks something like this:
.PH 1 1 \*(XD/root2.eps
The clock window is now larger, the image of the clock has expanded to fill
the larger window, and the clock window now overlaps the
.B xvt
window.
.PP
.II window^background
.II window^foreground
.II background
.II foreground
Now, try moving the
.B xvt
window.
First, however, we need to shift the
.B xvt
window from behind the clock window (in X nomenclature, this is called the
.IR background )
to in front of the clock window
(in X nomenclature, the
.IR foreground ).
Move the mouse cursor into the icon and click the left-mouse button:
the
.B xvt
window vanishes from the screen.
Now, click the icon again:
the
.B xvt
window appears again, but this time in the foreground, superimposed upon
the clock window.
.PP
Now, slide mouse cursor into the
.B xvt
window.
.II "title bar"
When the cursor enters the window, observe the area at the top of the
window that has the word
.B xvt
in it \(em this area is called the
.IR "title bar" .
When the cursor enters the
.B xvt
window, a small, gray patch appears in title bar.
.II "button^screen"
.II "screen button"
The other shapes in the title bar are
.IR "screen buttons" .
.II "screen button"
.II "button^screen"
A screen button is not a physical button, like the button on your mouse;
however, it works like physical buttons in that you can ``press'' it
by moving the mouse cursor to it and then clicking a mouse button.
.PP
Slide the mouse cursor so that it is touching in the title bar, but not
touching any of the buttons.
(The gray patch is part of the title bar, not a button.)
Now, press and hold down the left-mouse button.
Once again, the mouse cursor changes to a stylized `+'.
While holding down the left-mouse button, drag the mouse up and to the
right;
while you slide the mouse, an outline of the window follows.
Keep sliding the mouse until the
.B xvt
window is positioned to the right of the clock window; then release
the left-mouse button.
When you release the mouse button, X redraws the
.B xvt
window in its new position.
Your screen should now appear something like this:
.PH 1 1 \*(XD/root3.eps
.Sh "Screen Buttons"
.PP
.II twm
As we noted earlier, the little drawings in the title bar
are screen buttons.
Moving the mouse cursor to a screen button and clicking the left-mouse
button tells X \(em or, to be more precise, the screen manager \fBtwm\fR \(em
to do something, just like pressing a button on your telephone.
.PP
The window manager displays three buttons on the title bar of
each client's window.
The following describes them in the order they appear in the title bar,
from left to right:
.IP \fB1.\fR 0.3i
.II "iconify window"
.II "de-iconify a window"
.II "window^iconify"
.II "window^de-iconify"
The leftmost button, which has a bullet `\(bu' in it, ``iconifies'' the window.
That is, it erases the window from the screen and draws the stylized `X'
in that window's icon.
The window will not appear on the screen until you ``de-iconify'' it by
clicking on its icon.
.IP
Try this.
Slide the mouse cursor so that it touches the leftmost button in the
.B xvt
window's title bar.
When it touches the button, the cursor changes to a \(rh, except pointing
to the left.
Click the left-mouse button:
this ``presses'' the screen button.
The
.B xvt
window vanishes, as before; it is now iconified.
Now, slide the mouse cursor to the icon, and click the left-mouse button again.
The
.B xvt
window is de-iconified, and reappears on the screen.
.IP \fB2.\fR
The second button from the left, which has a stylized `D' in it,
displays a drop-down menu.
We will discuss this button below, when we introduce menus.
.IP \fB3.\fR
The rightmost button, which has a staggered set of squares in it, lets you
resize the window, just as if you had clicked on the window's frame.
To resize the window, move the mouse cursor to this button; press the
left-mouse button and, while holding down the mouse button,
slide the mouse cursor
to the right and down, until the window is sized to suit your preferences.
.IP
Note that by default
.B twm
does not shrink a window until you have first
moved the mouse cursor past the window's current borders.
Thus, if you want to shrink a window, you have to first stretch the grid
out past the current borders of the window, then slide it back in.
.PP
Many applications use screen buttons to gather information from you, but
you operate each in the same way:
move the mouse cursor to it and click the left-mouse button.
.Sh Sliders
.PP
.II slider
If you look at the following figure, you'll see something different
along the left edge of the
.B xvt
window:
.PH 1 1 \*(XD/root3a.eps
The object to the left of the window is a column, with a gray patch at the
bottom.
This column is called a
.IR slider .
You can use the mouse to move the gray patch up or down.
By moving the slider, you change the value of a variable that is
associated with the slider.
.PP
To change the value of a slider, move the mouse cursor onto the gray
patch, then press the middle-mouse button and hold it down.
(If you have a two-button mouse, click both buttons simultaneously and
hold them down.)
Then drag the mouse up or down on your desk; the gray patch follows the
mouse cursor up or down.
When you release the mouse buttons, the gray patch stays fixed where you
dragged it; the corresponding value will have been raised or lowered by a
proportionate amount.
.PP
Clients often use sliders to let you set a range of integral or ratio values.
For example,
.B xvt
uses the a slider to let you set display lines of text
that had scrolled off the top of the window earlier, up to a maximum of 64.
By dragging the slider up or down, you can review text that had been displayed
in the window, but no longer fits within the window \(em in effect,
you are choosing which line will be the first line in the display.
.PP
Many applications use a slider or sliders to set numeric values of this sort.
Sliders will pop up again and again as you work with X.
.Sh "Menus"
.PP
.II menu
A
.I menu
is a list of entries, like a menu in a restaurant.
You can select one of the items on the menu; the program
then performs the action that corresponds to the item you have
selected.
.PP
Many applications use menus.
The window manager
.B twm
has three menus built into it.
This section describes how to invoke these menus and their contents.
.PP
The first menu that we will describe is the one that appears when you press
the screen button that is second from the left in a window's title bar.
(This is the button with the stylized `D' in it.)
To invoke this menu, slide the mouse cursor to this button in the title bar
for the
.B xvt
window; then press the left-mouse button and hold it down.
The window now appears something like this:
.PH 1 1 \*(XD/root4.eps
As you can see, the menu has 13 entries.
The entry at the top is the menu's title.
If you look carefully, you can see that three of the entries have
a small button on the right.
The button indicates that that entry introduces a sub-menu; a sub-menu
pops up if the mouse cursor simply touches the button \(em you don't have to
press a mouse button (which is fortunate, as you are already holding down
the left-mouse button).
For example, if the mouse cursor touches the button in the menu entry labelled
.K Properties
a three-item sub-menu drops down, and the screen appears something like this:
.PH 1 1 \*(XD/root5.eps
The contents of this sub-menu will be discussed below.
.PP
.II "menu^WINDOW OPS"
.II "WINDOW OPS"
The following describes each entry in the
.B "WINDOW OPS"
menu:
.IP "\fK(Iconify)\fR"
Iconify this window.
This is the same as clicking the leftmost button on the title bar.
.IP "\fK(Lower)\fR"
Lower this window into the background \(em that is,
if this window overlaps with any other window,
draw that window atop this window.
.IP "\fK(Move)\fR"
Move this window.
This works exactly as if you had clicked on the title bar.
.IP "\fK(Raise)\fR"
Raise this window into the foreground \(em that is,
if this window overlaps with any other window,
draw this window atop that window.
.IP "\fK(Refresh)\fR"
Redraw the window.
Do this if any ``junk'' has appeared to ``mess up'' the window.
.IP "\fK(Resize)\fR"
Change the size of the window.
This works exactly the same as if you had clicked on the window's frame.
.IP "\fK(Applications)\fR"
This entry has a button that, when touched by the mouse cursor, drops a
sub-menu that shows a selection of applications that you can run under X.
This menu will be discussed later.
.IP "\fK(Properties)\fR"
This entry has a button that, when touched by the mouse cursor, drops a
sub-menu that lets you change some of
.BR twm 's
properties.
The term
.I property
has a precise meaning under X, and will be discussed later;
for now, consider a
property as being a feature of an object that affects the object's
appearance or behavior.
.II "menu^Properties"
.II "Properties"
The
.B Properties
sub-menu has the following three entries:
.RS
.II autoraise
.IP "\fK(Autoraise)\fR"
If you turn on this property,
the window manager raises this window to the foreground whenever the
mouse cursor enters it.
To turn off this property, simply select this sub-menu entry again.
.II focus
.IP "\fK(Focus)\fR"
Focus the window manager's attention on this window.
This means that the window's frame is always bright, even when the mouse
cursor is not in this window.
Further, whatever you type on the keyboard appears in this window, regardless
of where the mouse cursor is on the screen.
.II unfocus
.IP "\fK(Unfocus)\fR"
Stop focusing the window manager's attention on this window.
This is the default.
.RE
.IP "\fK(Twm_Operations)\fR"
This menu entry drops a long sub-menu that lets you invoke many features of the
window manager
.BR twm .
This menu and its contents are discussed below.
.IP "\fK(Info...)\fR"
This menu entry opens displays information about the current
window:
its name, position on the screen, and other information.
.IP "\fK(Kill_Program)\fR"
This kills the current window dead, just the same as if you had typed the
command
.B "kill -SIGKILL"
for this program.
Do
.I not
use this option unless you absolutely must kill
an application, because a program that dies in this way often leaves memory
and the file system littered with debris.
.IP "\fK(Close_Window)\fR"
This is a kinder, gentler way of killing the program in this window.
It closes this window; in theory, an application perishes gracefully
when its last (or only) window is closed.
Unfortunately, this does not work for every application, and in some cases
you may be compelled to resort to the heavy, blunt object of the previous
menu entry.
.PP
To see how a menu works, try selecting an item from it.
Click the title-bar button that drops this menu; then, while holding
down the left-mouse button, drag your mouse down until the entry
.K Iconify
is highlighted.
Then release the mouse button.
This invokes the ``iconify'' feature, and ``iconifies'' the window.
Now, click the
.B xvt
icon at the top of the screen to redisplay the
.B xvt
window.
.PP
As we mentioned above, the menu entry
.B "Twm_Operations"
drops a menu of things that you can do to, and with,
the window manager
.BR twm .
The same menu appears if you move the mouse cursor so that it is positioned over
the screen's background (when changes shape to an \(mu) and press the
right-mouse button.
Try it; your screen should appear something like this:
.PH 1 1 \*(XD/root6.eps
If the physical screen is smaller the virtual screen, you won't be able to see
all of the menu at once.
To see the bottom of the menu, keeping sliding the mouse cursor down; the
borders of the physical screen will shift downward to keep the mouse cursor in
view.
.PP
.II "menu^TWM Operations"
.II "TWM Operations"
The following describes the entries in the
.B "TWM Operations"
menu:
.IP "\fK(Beep)\fR"
Ring the bell.
.IP "\fK(Close_Window)\fR"
Close the current window.
This is the same as the
.K "Close_Window"
entry in the previous menu.
.IP "\fK(Destroy_Application)\fR"
Kill an application.
This is the same as the
.K "Kill_Program"
entry in the previous window.
.IP "\fK(Focus)\fR"
Focus the window manager's attention on a window.
If you invoked the
.B "TWM Operations"
menu from within the
.B "WINDOW OPS"
menu,
.B twm
fixes its focus on the window from within which
you invoked
.BR "WINDOW OPS" .
If, however, you invoked
.B "TWM Operations"
by clicking the right-mouse button, then
.B twm
lets you pick the window interactively:
it changes the mouse cursor to a bull's-eye; move the mouse cursor to the
window upon which you want
.B twm
to focus, and click the left-mouse button.
.B twm
will focus on this window until you go through this procedure again for
that window, or invoke the
.B "Unfocus"
menu entry, below.
.IP "\fK(Force_Move)\fR"
Move a window, even if the window's properties specifically exclude its
being moved.
.IP "\fK(Full_Zoom)\fR"
Expand a window to fill the entire screen.
The window that
.B twm
``zooms'' is determined just as with the
.K "Focus"
entry, above.
.IP "\fK(Hide_Icon_Manager)\fR"
Erase, or ``hide,'' all icons from the screen.
The icons are kept in memory, waiting for you to ``un-hide'' them.
.IP "\fK(Iconify)\fR"
Iconify a window.
The window that
.B twm
iconifies is determined just as with the
.K "Focus"
entry, above.
.IP "\fK(Info...)\fR"
Display information about a window, such as its name, size, and position
on the screen.
The window about which
.B twm
displays information is determined just as with the
.B "Focus"
entry, above.
.IP "\fK(Lower)\fR"
Lower a window.
Again, the window that
.B twm
lowers depends upon how you invoked this menu.
.IP "\fK(Lower_Current_Window)\fR"
This lowers the window that was raised last.
.IP "\fK(Move)\fR"
Move a window.
This works just as if you had clicked on a window's frame.
.IP "\fK(Open_A_New_Window)\fR"
This invokes the application
.B xvt
to open a new window and run your shell of choice in it.
Yes, this is the same
.B xvt
whose window you have manipulated so far.
This application is introduced below; for detailed information on it,
see its entry in the Lexicon at the back of this manual.
.IP "\fK(Raise)\fR"
Raise a window.
Again, the window that
.B twm
raises depends on how you invoked this menu.
.IP "\fK(Raise_Current_Window)\fR"
Raise the window that was lowered last.
.IP "\fK(Raise_or_Lower)\fR"
Select a window interactively:
if it is lowered, raise it; if it is lowered, raise it.
.IP "\fK(Resize)\fR"
Resize a window.
This works just as if you had clicked on a window's title bar.
Again, the window that
.B twm
resizes depends on how you invoked this menu.
.IP "\fK(Refresh_Window)\fR"
Refresh a window.
.IP "\fK(Refresh_Screen)\fR"
Refresh every window on the screen.
.B twm
blanks the screen briefly, then redraws it.
.IP "\fK(Show_Icon_Manager)\fR"
Redisplay the icon manager, if it is hidden.
.IP "\fK(Unfocus)\fR"
If
.B twm
is focused on a window, unfocus it.
If
.B twm
is not focused on a window, do nothing.
.IP "\fK(Window_List)\fR"
This entry has a button on it.
If the mouse cursor touches this button, a sub-menu appears that lists
every window
.B twm
is handling at this moment, whether or not it is iconified or hidden.
.IP "\fK(Zoom)\fR"
Expand a window to fill the window vertically; do not, however, change its
horizontal dimension.
.IP "\fK(Quit_Window_Mgr.)\fR"
Finally, shut down the
.B twm
window manager.
This also shuts down the X server, and returns control to the
ordinary \*(CO console interface.
.PP
So far, we have discussed
.B twm
and how to manipulate windows:
how to move them, change their size, change their focus, hide them,
redisplay them, and so on.
Now we will discuss just what's going on within the windows themselves \(em
which is, after all, the whole point of the X Window System.
.Sh "Introduction to xvt"
.PP
.II xvt
The window we've called the ``\fBxvt\fR window'' is the window for an
X program named
.BR xvt .
.II VT-100
.II "Tektronix 4014"
This program mimics a VT-100 terminal.
\*(CO ``thinks'' that this window is another terminal,
just as if you had plugged a dumb terminal into a serial port
on your machine.
.PP
.II shell
When
.B xvt
opens its window, by default it invokes a shell through which you can
give commands to \*(CO.
You can invoke other X clients via
.BR xvt ,
dial out to another system, edit files, and play games.
.PP
You can open more than one
.B xvt
window on your screen if you wish.
The number of windows you can manage comfortably depends upon the size of your
screen; if you have a small screen, you may wish to iconify the
.B xvt
windows that you are not using at the moment.
.II Internet
If your system were on a network, you could open a number of different
windows, each logged into a different machine on the network.
In this way, users who are hooked into the Internet can simultaneously view
and manipulate data from machines that are thousands of miles apart.
.PP
To give \*(CO a command via
.BR xvt ,
simply move the mouse cursor into the
.B xvt
window (so that X knows that what you type at the keyboard is intended
for this window), and type the command as you always do.
To spawn another terminal window, simply type:
.DM
	xvt &
.DE
In a moment, X draws the outline of an
.B xvt
window, and fixes it to the mouse cursor.
Slide the mouse until the window outline is where you want the new
window to be, then click the left-mouse button.
The new terminal window appears, with a \*(CO prompt in it.
You can shrink the new terminal window so it and the original terminal
window fit together onto your screen; or you can iconify it, or use the
.B Raise
and
.B Lower
entries from the
.B twm
menus to help you work with both windows.
.PP
Note that when
.B xvt
opens a window, the window is set to 80 characters wide by 25 rows high,
just like an ordinary terminal's screen.
Most \*(CO applications expect that size of a screen, and have no idea that
they are talking to a window that can be resized.
.II MicroEMACS
.II vi
.II chase
Thus, if you shrunk an
.B xvt
window, you can expect that some visually oriented \*(CO applications \(em such
as \*(ME, \fBvsh\fR, or \fBchase\fR \(em will not work correctly.
Note, too, that if you kill an
.B xvt
window, any program that you invoked from it \(em either a \*(CO
application or an X client \(em dies as well.
.II nohup
(The \*(CO command
.B nohup
will help you get around this limitation; for details, see its entry in the
\*(CO Lexicon.)
.PP
.II write
The following screen shows one unusual application of multiple
.B xvt
windows:
the user here is conversing with himself in two different windows
via the \*(CO utility
.BR write :
.PH 1 1 \*(XD/root7.eps
.PP
.Sh "Cutting and Pasting"
.PP
X has a built-in facility with which you can cut and paste text.
This facility is impractical for moving large amounts of text, but is
handy for copying a command or a set of information from
one window into another, or into a file.
Note that this facility only works with text \(em you cannot use it to
copy a bit-mapped image or an icon.
.PP
To cut text, move the mouse cursor to the point where you want to start
cutting, then press the left-mouse button.
Drag the mouse to the end of the block of text you
wish to cut; then release the left-mouse button.
The X server copies the cut text into both its cut buffer and
into its primary selection (that is, the property
.BR PRIMARY ).
.PP
Another way to cut text is to click the left-mouse button at the beginning
of the block of text you wish to cut, then move the mouse cursor to the
end of the block and click the right-mouse button.
All text between the two clicks is highlighted, to show that you have
cut it.
.PP
To un-cut text, move the mouse and click the left-mouse button again.
The text will be un-highlighted, to show that it is no longer cut.
.PP
To paste text, move the mouse cursor to the spot where you want to
``drop'' the text, then press the middle-mouse button.
(If your mouse has only two buttons, press both buttons simultaneously.)
X handles the pasted text just as if you had typed it from the keyboard:
if the text is pasted into a window that cannot accept keyboard input, then
the pasted text goes into the bit bucket.
.II cat
To drop the cut text into a file, invoke an editor or the \*(CO command
.B cat
in an
.B xvt
window, then drop the text into that window.
The program you invoked in the window will save the text into a file, just
as if you had typed it by hand.
.Sh "Introduction to xclock"
.PP
.II xclock
The window that we've called the ``clock window'' is the window for an
X program named
.BR xclock .
This program has many options, some of which you can invoke via command-line
options, like any other \*(CO program.
.PP
For example,
.B xclock
by default displays an analogue clock \(em that is, a clock with a dial and
hands.
You can, however, request a digital clock:
to do so, just type the following command into your
.B xvt
window:
.DM
	xclock -digital -update 1 &
.DE
The option
.B "\-update 1"
tells
.B xclock
to update itself every second.
The default is every 60 seconds.
.PP
Once again, a small outline of a window appears.
Slide the mouse until the window is placed where you want, then click the
left-mouse button.
You now have a small digital clock, which displays the date as well as the
time:
.PH 1 1 \*(XD/root8.eps
To remove your new clock, select the command
.K "Close_Window"
from the menu
.BR "TWM Operations" .
.PP
For a full list of options to
.BR xclock ,
see its entry in this manual's Lexicon.
.Sh "The Applications Menu"
.PP
Earlier, we noted that
.B twm
has three menus available to you.
We have described two:
.B "WINDOW OPS"
and
.BR "TWM Operations" .
.PP
.II "menu^APPLICATIONS"
.II "APPLICATIONS"
The third menu is
.BR APPLICATIONS .
Through it, you can invoke some of the more commonly used X clients,
without having to type a command into an
.B xvt
window.
(This explains, among other things, how you can invoke an
.B xvt
window when the screen has no
.B xvt
window into which you can type the command.)
This menu appears when you position the mouse cursor over the background of
the screen (as shown by the fact that the mouse cursor changes to an \(mu),
and click the left-mouse button.
The following figure shows the menu
.BR APPLICATIONS :
.PH 1 1 \*(XD/root9.eps
.B APPLICATIONS
contains the following entries:
.IP "\fK(Font_Select)\fR" 0.75i
.II xfontsel
Invoke the X client
.BR xfontsel ,
which helps you to select a font interactively.
The description of fonts under X is something of a ``black art''; for
details, see below or see the entry for
.B xfontsel
in the Lexicon.
.IP "\fK(Puzzle)\fR"
.II puzzle
Invoke the X client
.BR puzzle ,
which lets you play a scrambled-tiles game.
.IP "\fK(XBiff)\fR"
.II xbiff
Invoke the X client
.BR xbiff ,
which displays an icon of an old-fashioned roadside mailbox.
When you receive new mail, the flag on the ``mailbox'' pops up.
.IP "\fK(XCalc)\fR"
.II xcalc
.II calculator
.II "Texas Instruments"
.II "Hewlett-Packard"
Invoke the X client
.BR xcalc ,
which displays the image of scientific calculator \(em
either a Hewlett-Packard or
a Texas Instruments model, whichever you prefer.
By clicking on the image's buttons, you can invoke all of the features
of a real scientific calculator.
As you can see from the previous figure,
this entry invokes a sub-menu, from which you can select the type of
calculator you want.
.IP "\fK(XClock)\fR"
.II xclock
Invoke the client
.BR xclock ,
described above.
.IP "\fK(Xeyes)\fR"
.II xeyes
Invoke the X client
.BR xeyes .
This client draws on the screen a pair of eyes whose pupils follow the mouse
cursor around the screen.
This can help you find the mouse cursor on a cluttered screen.
.IP "\fK(XLoad)\fR"
.II xload
Invoke the client
.BR xload .
This client displays a histogram that graphs the load on your system.
You can use this client to gain an idea of how many cycles a given
program consumes.
.IP "\fK(XLogo)\fR"
.II xlogo
Invoke the X client
.BR xlogo ,
which merely displays the X logo.
This logo is the stylized X that appears in an icon when you have iconified a
window.
.IP "\fK(XMag)\fR"
.II xmag
Invoke the X client
.BR xmag .
This client magnifies part of the screen, to help you see how a shape is
built out of individual pixels.
.IP "\fK(Xvt)\fR"
.II xvt
Invoke another
.B xvt
window.
.IP "\fK(XTetris)\fR"
.II xtetris
Invoke the X client
.BR xtetris ,
which plays the popular game Tetris.
.PP
Each of these applications is invoked with selected command-line options;
for example,
.B xclock
is invoked to display an analogue clock rather than a digital one.
Below, we will show you how to change these defaults.
.Sh "Shutting Down X"
.II "X^shut down"
.II "shutting down X"
.PP
Shutting down X is a two-stage operation:
first, you must close all
.B xvt
windows, and then you must kill the server.
.PP
To shut an
.B xvt
window, simply move the mouse pointer into the window (so that its frame
is highlighted), then type the command
.BR exit .
If nothing appears to be happening, wiggle the mouse a bit;
X often waits for you to do something before it redraws the screen to reflect
the effects of any commands you may have issued.
.PP
Once all
.B xvt
windows are closed, you can shut down the X server.
There are two ways to do this.
.PP
The first way is to do so via a menu.
Invoke the menu
.B "TWM Operations"
by moving the mouse cursor over the screen's background, then press the
right-mouse button.
Drag the mouse until the last entry in the menu, which is labelled
.K "Quit_Window_Mgr."
is highlighted.
Then release the right-mouse button.
All windows close, the X server turns itself off, and you are returned to
the standard \*(CO character-based interface.
.PP
The other way is simply to type
.BR <ctrl><alt><backspace> .
No matter when you type this combination of keys, the X server will detect
it and shut itself down.
.PP
The two methods vary in how they flush buffers and tidy up after themselves.
In general, it is better to shut down in a controlled fashion, through the
menu entry, than by pressing the magic combination of keys.
.PP
Note that you can kill the X server without closing your
.B xvt
windows.
However, this is not a good idea because \*(CO may ``think'' that you are
still logged into an
.BR xvt
window, even though that window no longer exists.
This may create problems for you at some future time.
.Sh Conclusion
.PP
This concludes the introduction to the
.B twm
screen interface.
.II GUI
We have introduced the basic ``vocabulary'' of the X GUI, discussed
.B twm
and its menus, and shown how you can invoke and operate some other
X clients under
.BR twm .
.PP
The following section gives you a ``nickel tour'' of the files and
directories that comprise the X Window System.
Subsequent sections introduce the internal structure of X.
Knowing how X is structured internally will help you grasp how to configure X.
The final section in this chapter describes how to configure
.B twm
to suit your preferences.
.SH "A Tour of the X System"
.PP
The X Window System is installed into directory
.BR /usr/X11 .
The following describes the contents of each directory in the X Window System:
.IP \fB/usr/X11/bin\fR
.II /usr/X11/bin
This directory holds the executable programs themselves.
Most are created when you install your system
from the object modules in
.BR /usr/X11/objs .
For an introduction to these program, see the following chapter,
.IR "X Windows Clients" .
.IP \fB/usr/X11/doc\fR
.II /usr/X11/doc
This directory holds public-domain documentation on the X Window System
that you may find useful.
.IP \fB/usr/X11/include/X11\fR
.II /usr/X11/include/X11
This directory holds header files and bit-mapped images that are included
in the source code of an X application.
It holds the following sub-directories:
.RS
.II "Athena widget set"
.IP \fBXaw\fR
.II /usr/X11/include/X11/Xaw
Header files used with the Athena widget set.
.IP \fBXmu\fR
.II /usr/X11/include/X11/Xmu
Header files used with miscellaneous X utilities.
.IP \fBbitmaps\fR
.II /usr/X11/include/X11/bitmaps
This holds the C source code for bit-mapped images that X Windows uses.
You can include these images in programs that you import or write.
.IP \fBextensions\fR
.II /usr/X11/include/X11/extensions
Header files for extensions to the X Window System.
.IP \fBsys\fR
.II /usr/X11/include/X11/sys
System-level header files that are used with X Windows.
.RE
.IP \fB/usr/X11/lib\fR
.II /usr/X11/lib
This directory holds libraries of X Windows routines.
Please note that these libraries do
.I not
include code to use or emulate a mathematics co-processor.
.IP
This directory also holds configuration files for the X server, in particular
.BR Xconfig .
.IP
.B /usr/X11/lib
contains the following sub-directories:
.RS
.IP \fBapp-defaults\fR
.II /usr/X11/lib/app-defaults
Files of settings, or ``resources,'' for selected X programs.
By modifying an program's resource file, you can modify its
appearance and behavior without having to modify its source code.
The next chapter describes how to do this.
.IP \fBconfig\fR
.II /usr/X11/lib/config
This holds files that describe how this implementation of the X Window System
is configured.
.II imake
They are read by the utility
.B imake
to build a correctly configured
.B Makefile
from an
.BR Imakefile .
.II xmkmf
For details, see the Lexicon entries for
.B imake
and
.BR xmkmf .
.IP \fBfonts\fR
.II /usr/X11/lib/fonts
This directory holds the fonts used by the X Window System.
X Windows for \*(CO comes with two directories of fonts:
.BR fonts/misc ,
which holds miscellaneous fonts (such as the font used by the mouse
cursor); and
.BR fonts/75dpi ;
which holds 75 dot-per-inch, proportionally spaced fonts.
For information on how to display a font, see the Lexicon entry for
.BR xfd .
.IP \fBnls\fR
.II /usr/X11/lib/nls
This holds tools used by X developers.
.IP \fBtwm\fR
.II /usr/X11/lib/twm
.II system.twmrc
.II twmrc
This directory holds the file
.BR system.twmrc ,
which is the default resource file for the
.B twm
window manager.
.B twm
reads this file if your home directory does not contain a file named
.BR .twmrc .
If you want to customize
.B twm
to your liking, copy
.B system.twmrc
into your home directory, rename it
.BR .twmrc ,
and modify it.
The following section describes how to modify
.BR $HOME/.twmrc .
.IP \fBx11perfcomp\fR
.II /usr/X11/lib/x11perfcomp
This holds scripts used to help gauge system performance
.IP \fBxinit\fR
.II /usr/X11/lib/xinit
.II xinitrc
This directory holds the file
.BR xinitrc .
The utility
.B xinit
reads this file if your home directory does not contain a file named
.BR .xinitrc .
If you want to customize
.B xinit
to your liking, copy
.B xinitrc
into your home directory, rename it
.BR .xinitrc ,
and modify it to your liking.
The following section describes how to modify
.BR $HOME/.xinitrc .
.RE
.IP \fB/usr/X11/objs\fR
.II /usr/X11/objs
The object modules for the clients and utilities included with this package.
When you install the package onto your system, they are linked with the
libraries in
.BR /usr/X11/lib
to create executable programs.
.PP
This concludes our nickel tour of the X Window System.
The next section discusses the structure of X Windows, in preparation
to discussing how you can modify X.
.SH "The Structure of X"
.PP
.II "X^history
.II MIT
This section introduces the structure of the X Window System.
If you are a beginner, you may find some of this section to be a little dry.
However, if you take a few minutes to read these descriptions,
you probably will find it easier to learn how to customize the X Window System.
.Sh Background
.PP
To begin, a little history.
What we call the
.I "X Window System"
was created at the Massachusetts Institute of Technology.
.II "X^releases"
The first edition, or
.IR release ,
of X appeared in 1984; release 11, which is the
current release of X, appeared in 1987.
.II X^revisions"
Since 1987, release 11 has been revised several times, to fix bugs and
widen its scope; the current edition of X is release 11, revision 5.
(X documentation often uses a kind of shorthand to indicate the release and
revision for which a given program is intended, e.g., ``X11R5'' or
``X11.5''.)
.PP
.II "Sun Microsystems"
MIT performed much of its work on X in partnership with Sun Microsystems;
other computer manufacturers also contributed to X.
The notes at the end of each Lexicon entry usually name the company or
person who contributed the given program to X.
The bulk of the X Window System is copyrighted by MIT; source code, however,
is available for a minimal cost from MIT, and MIT does not demand a royalty
for X.
(Some packages \(em such as X Windows for \*(CO, which you are now using \(em
contain code that is proprietary; therefore, do not assume that you can copy
and give away your X software.)
.PP
.II GUI
Unlike other graphical user interfaces (GUIs),
X makes no assumptions either about the operating
system that is running underneath it, or about the manner by which the user
communicates with the computer.
.II Motif
.II "Open Software Foundation"
.II "OPEN LOOK"
.II AT&T
This approach has a few disadvantages.
In particular, it has led manufacturers to create a number of
different user interfaces, of which the best known are Motif (created by the
Open Software Foundation) and \*(OL (created by Sun and AT&T).
Thus, the appearance and behavior of an X system can vary greatly from one
computer to another.
.PP
.II MS-DOS
However,
the disadvantages of this approach are greatly outweighted by its advantages.
Because X makes no assumptions about its underlying operating system,
X has been adopted as a
.I "de facto"
standard by a number of leading computer manufacturers.
Although it usually runs with some version of the \*(UN operating system
(or a \*(UN-like operating system, such as \*(CO), it has also been ported to
limited operating systems like \*(MD.
Its design also makes X an ideal interface for a network of computers:
even if each computer has a different architecture and operating system,
all can appear the same to the user and run in more or less the same manner,
thanks to X.
.PP
The flexibility of X also means that it is easy to port X programs from one
operating system to another.
Thus, as soon as X is brought up on an operating system,
a wealth of software quickly becomes available to users.
.Sh "X Architecture"
.PP
.II "X^architecture"
The flexibility of the X system results from the fact that X
carefully isolates into a single program
each class of tasks that it must perform.
For example, it isolates into one program, called the \fIserver\fR,
all of the work of driving the hardware.
If an X application needs to draw something onto the screen, it simply
invokes a feature of the server, which does the job.
Thus, to port X to new operating system, one must rewrite the server;
but once that is done, importing an X application
often involves no more than recompiling it.
(There are exceptions to this, of course, but on the whole an X application
that ``plays by the rules'' can be ported with very little trouble.)
.PP
Therefore, X can be compared to a well-designed machine.
Each part of the machine does one thing, and only one thing.
If one part needs to perform a task that falls into the domain of second part,
the first part sends a message to the second part and lets the second
part execute that task for it.
Any one part can be pulled out of the machine and replaced without affecting
the performance of any other part.
.II "object-oriented program design"
.II OOP
This way of fashioning a computer program is called
\fIobject-oriented program design\fR (OOP).
Although the principles of OOP lie beyond the scope of this manual,
you will find it helpful when you work with X to try to think in terms of
objects and how they fit together.
.PP
The manner in which the parts of the X machine fit together is called its
.IR architecture .
The architecture of X has three major parts:
the
.IR server ,
the
.IR "client" ,
and the
.IR "window manager" .
.IP \fBserver\fR
.II "X server^definition"
.II server
The server is the program that actually runs the X system.
It does all the low-level work of drawing images on the screen;
it manages communications among X programs;
and on networked X systems, it manages permissions.
Every other X program registers with the server when they come up;
all of its work with the screen is done via requests to the server.
.IP
.II "VGA"
X Windows for \*(CO comes with two servers:
one that supports color VGA for a set of the most popular VGA chip sets;
and one that supports monochrome images on practically every variety of
VGA or SVGA card.
Most programs run without alteration on either server.
.IP
As a rule of thumb, there is one server for each computer at which
a person works.
.II "X terminal"
.II workstation
Here, the term ``computer'' includes such devices as X terminals,
as well as PCs and workstations.
.IP \fBclients\fR
.II "client^definition"
.II "X client^definition"
A client is a program that registers with the X server,
and uses the server's facilities to work with the user.
As noted above, a client does not contain any code for manipulating
hardware \(em it does not know how to talk to the mouse or the screen.
All it does is send requests to the server and receive information from
the server.
.IP
In this manual, we speak of two types of application:
.I clients
and
.IR utilities .
In this manual, a client
is something that let you do something under X, such as play a game or
edit a file.
.II "utility"
A utility helps you to run X itself.
Both clients and utilities, however, are X clients in the broad sense:
both register with the server and do their work through it.
.IP
On a networked X system, a client can register itself with any server
on the network, provided that the user who launched the client has permission
to register with that server.
For example, a person sitting at one workstation can invoke a client
and tell it to register itself with the server on another person's workstation,
to show that other person a document or graph.
\*(CO does not yet support networking; when it does, you will be able
to tie your machine into a network of computers and work together via X.
.IP "\fBwindow manager\fR"
.II "window manager^definition"
.II "X^window manager"
The
.I "window manager"
is a special client that manages the screen for all other clients.
It allocates screen space to all other clients, and makes sure that no
client intrudes on another client's space.
It provides the tools with which you you can move a window or change its size.
.II "root window"
It also manages all parts of the screen that belong to no individual client;
these include the background of the screen
(also called the screen's
.IR "root window" ),
icons, window frames, title bars, and drop-down menus.
The window manager is largely responsible for the ``look and feel''
of a given implementation of X.
.PP
In your day-to-day work you probably will not need to concern yourself with
the architecture of X.
Its parts fit together to make one smoothly running machine.
However, you will find it helpful to bear the above descriptions in mind,
because they will help you track down the cause of any problems that you
may encounter.
.Sh "Libraries, Widgets, and Resources"
.PP
.II Xlib
Internally, X has a hierarchical design.
The lowest level of the hierarchy are the routines in the library
.BR Xlib .
These routines do all primitive tasks, such as drawing lines and shapes on
the screen, reading input from the keyboard, and polling the mouse.
.B Xlib
underlies all versions of X.
.PP
.II toolkit
On top of
.B Xlib
sit
.IR toolkits .
A toolkit is a set of routines that combine the low-level routines of
.B Xlib
into tools that applications can use directly.
For example, a toolkit routine may combine a set of
.B Xlib
routines that draws shapes into one routine that draws a button.
A toolkit sometimes includes its own low-level routines to cover tasks
that are not addressed in
.BR Xlib ;
for obvious reasons, this is discouraged.
.PP
.II Xt
Numerous toolkits have been written for X.
The tools in this package are built from the MIT toolkit
.BR Xt .
.PP
.II "widget set"
Atop a toolkit sits a
.IR "widget set" .
A widget combines a set of routines with one or more bit-mapped
shapes that can be displayed on the screen.
For example, a button widget can display a bit-mapped shape, superimpose
a button from the toolbox upon the shape, and include routines for polling
the mouse and reacting to the mouse events
(e.g., when a mouse button is pressed or released).
.PP
.II "composite widgets"
.II "widget^composite"
Widgets can be assembled to form ``composite widgets.''
Thus, a widget set has a hierarchy within itself:
whatever affects a widget that is lower in the hierarchy affects all
widgets that are built atop it.
.PP
.II Motif
.II "OPEN LOOK"
.II "Athena widget set"
A screen interface, such as Motif or \*(OL,
consists of a toolkit, a widget set,
and a screen manager that is built from the toolkit and widget set.
The interface embodies a design of how the screen should appear and how the
interface should behave.
.PP
X's widgets are designed so that you can change ``on the fly'' many of
their aspects.
For example, a button widget can let you set the text that appears in it,
its color, its position on the screen, dimensions, whether it has rounded
or square corners, and so on.
.II resource
You can do this by setting a widget's
.IR resources .
.PP
You can set resources in any, or all, of three ways:
.IP \fB1.\fR 0.3i
First, many applications read information from command-line options.
.II Xt
.II geometry
Applications that are built from the
.B Xt
toolkit take a standard set of command-line options, which (among other
things) let you set colors, the window's geometry (that is, its initial
size and position on the screen), and whether it appears in reverse video in
reaction to an event.
The suite of command-line options varies from application to application:
Some have a rich set that lets you fine-tune the application, whereas
others' are sparse.
.IP \fB2.\fR
.II "app-defaults"
.II "defaults"
Some applications read a file from directory
.BR /usr/X11/lib/app-defaults .
In this file, you set resources for that application.
.II xcalc
.II xfontsel
For some applications, such as
.B xcalc
or
.BR xfontsel ,
the application file can be quite large and complex.
The settings in an application's defaults file apply only that application.
By modifying its defaults file, you can change an application's appearance and,
in some instances, its behavior.
.IP \fB3.\fR
.II xrdb
You can use the command
.B xrdb
to build a resource data base within the X server.
Most applications read this data base as they come up.
The contents of this data base override any defaults built into any
application, or any resource settings they may have read from an
application-defaults file.
The settings in the resource data can apply either to an individual
application or to all applications, depending upon how you ``phrase'' them.
.PP
.II "widget class"
As we noted above, widgets most often are built out of other widgets.
If a widget is used to build other widgets, that first widget is said
to be a widget
.IR class .
X has a mechanism by which you can set a widget class;
if you alter a widget class,
every subordinate widget that incorporates the first widget into itself
is altered as well.
.PP
The next chapter, which introduces the X clients,
describes how to set widget classes.
X widgets and widget classes are a large and complex topic of study;
however, it is possible even for novices to make minor modifications to
a defaults file, as we will show below.
.Sh "Bit Maps"
.PP
.II "bit map"
A
.I "bit map"
is a picture that is formed by turning cells in a grid on and off.
X interprets each cell in the grid as a pixel.
The mouse cursor, for example, is a bit-mapped picture.
.PP
You can draw a bit-mapped image with a ordinary text editor, using a
pattern of pound signs `#' and hyphens `-',
or you can use one of the tools that comes with X.
You can then ``compile'' the bit map into C code that can
be included in an application, just like any other header file.
.PP
X includes tools with which you can draw, edit, and compile bit maps.
These are introduced in the next chapter.
.PP
X Windows for \*(CO comes with a selection of bit maps.
These are stored in directory
.BR /usr/X11/include/X11/bitmaps .
.Sh Fonts
.PP
.II font
A
.I font
is a collection of shapes that comprise the letters of the alphabet and
most commonly used punctuation marks.
A font has 14 attributes that describe its
size, weight, style, manufacturer, the character set it encodes, and other
information.
.PP
The characters in a font are bit-mapped images.
.II font^cursor
Thus, a ``font'' does not necessarily have to be the letters of the alphabet;
for example, the mouse cursors are stored in a special font called
.BR cursor .
.II xfd
(For a table of all available mouse cursors, see the entry in this manual's
Lexicon for the X utility
.BR xfd .)
This also means that, if you are so inclined, you can create and use
your own fonts.
.PP
.II "font path"
.II Xconfig
Fonts are stored in the directories named in the server's
.IR "font path" ,
which is set in the file
.BR /usr/X11/lib/Xconfig .
.PP
Because font files can be quite large, it is customary for them to be
compressed so they take up less space on disk.
X can use fonts in their compressed form; you do not have to uncompress
a font for X to use it.
.PP
.II fonts.dir
Each directory named in the font path must contain a file named
.BR fonts.dir .
Each entry in this file gives a font's full, 14-part name and
the file in which it resides.
If you move a new font into a font directory, you must modify this file
or X will not be able to find the font.
.PP
.II mkfontdir
The X utility
.B mkfontdir
reads all of the fonts in a directory and rebuilds that directory's
.B fonts.dir
file to show what fonts that directory actually contains.
.PP
A font's full name describes all 14 of a font's attributes.
For example, the following gives the full name of the font used for on titles:
.DM
	-adobe-times-bold-*-normal-*-*-140-*-*-*-*-*-*
.DE
.II fonts.alias
For the sake
of brevity, you can use an alias for a font's full name.
A font directory should also contain a file named
.BR fonts.alias ;
this file maps aliases to full font names.
.PP
X Windows for \*(CO comes with a selection of fonts for your applications.
It also comes with tools for selecting fonts interactively, and for
modifying a
.B fonts.dir
file.
These are introduced in the next chapter.
.II xfontsel
For more information on selecting fonts, and for more information on
the elements of a font's name, see the entry in this manual's Lexicon for
the X utility
.BR xfontsel .
.Sh Colors
.PP
.II color
X Windows includes tools for describing and managing colors.
The monochrome server also manages colors; however, it only recognizes
two colors \(em black and white.
.PP
.II rgb.txt
The file
.B /usr/X11/lib/rgb.txt
defines and names each color that the X server recognizes.
The following gives a typical entry:
.DM
	65 105 225	RoyalBlue
.DE
The three numbers give the intensity settings on the three electron
guns in your color monitor:
respectively red, green, and blue.
Each intensity can range from 0 through 255.
Setting all three guns to the maximum intensity, 255, creates white;
setting all three to zero creates black.
.PP
When you name a color in a defaults file or command-line option,
the X server looks up the name in this file and translates it into
its numeric settings.
If you wish to add a new name for a color, just edit this file.
Note that it is unwise to modify or delete any color names from this
file; otherwise, if an application requests that color by name, X will
not be able to find it.
.PP
.II palette
.II color^palette
.II VGA^palette
So far, so good.
A problem, however, arises because you can describe up to 16,777,216 colors
(that is, 256 cubed), but most VGA cards can display no more than 16 colors
at any one time.
An application usually requests a set, or
.IR palette ,
of colors from the server; the server, then, has to somehow juggle the
palettes of all of the applications it is handling into one master palette
that it can request from your system's video card.
.PP
X manages its palette automatically, and in most instances you do not have to
do anything.
X Windows for \*(CO does include tools to help you manage colors by hand,
should you wish to do so.
.PP
.II "color^set mode in Xconfig"
If you wish, you can edit
.B Xconfig
to set the mode of color display.
.B X
recognizes any of the following settings:
.DM
	DirectColor
	TrueColor
	PsuedoColor
	StaticColor
	GrayScale
	StaticGray
.DE
Place the value on a line by itself.
The default is
.BR PsuedoColor .
.PP
This concludes our brief introduction to the structure of X.
Although X is large and complex, it never departs from the principles
outlined here \(em well, hardly ever.
The following section describes how to customize the window manager
.BR twm ;
you will see some of these principles in action.
.SH "Customing the Window Manager"
.PP
As noted earlier, the window manager is the master client that manages the
appearance of the entire screen.
.II "root window"
Its window fills the entire screen;
thus, the window of the window manager
is sometimes called the screen's
.IR "root window" .
.PP
All other windows on the screen are sub-windows of the root window;
thus, the window manager positions all other windows on the screen
(or, to be more exact, within the root window),
and for supplying services within the root window such as the drop-down menus,
the window frames, icons, and window title bars.
As a rule of thumb, remember that everything within a window's frame is
defined by the client displayed within that window, whereas everything from
the frame out is the responsibility of the window manager.
You could run more than one application under X without a window manager,
but you would have no way to reposition or resize windows, or perform many
of the other actions that make X Windows so flexible and useful.
.PP
Like many other X clients, the window manager
.B twm
has a defaults file that defines the resources it uses.
We will discuss how to modify this file to change the appearance and
behavior of the window manager.
First, however, we must introduce the program
.BR xinit ,
which invokes the window manager and the other clients that appear
on your screen when you invoke X.
.Sh "Customizing xinitrc"
.PP
.II startx
.II xinit
As you recall, to invoke X you must issue the command
.BR startx .
This is actually a shell script that invokes the X utility
.BR xinit ;
this utility is responsible for invoking the X server, the window manager,
and any other utilities that you want to appear on the screen when you start
up X.
.PP
.II xinitrc
When
.B xinit
is invoked, it reads the file
.BR /usr/X11/lib/xinit/xinitrc ,
which gives the default configuration of X.
This file reads as follows:
.DM
	xclock -geometry 135x141+15+26 -fg blue -chime -update 1 &
	xvt -geometry 80x24+130+146 &
	twm
.DE
.II xsetroot
The first two commands,
.B xclock
and
.BR xvt ,
invoke the applications
.B xclock
and
.BR xvt ,
and display them on the screen.
The argument
.B \-geometry
gives the size of the window and its position on the screen.
The first two numbers, which are separated by an `x', give the size.
This can be either in pixels or, as with the
.B xvt
command, in rows and columns.
The second two numbers give, respectively, the X and Y position of the upper
left corner of the window.
If a number is positive, it is counted from the upper-left corner of the
screen; if negative, it is counted from the lower-right corner.
Note that both
.B xclock
and
.B xvt
are invoked in the background, as shown by the `&' that concludes
the command.
.PP
The last command,
.BR twm ,
invokes the window manager.
This is invoked in the foreground, because you want the X server to die when
you exit from the window manager.
Thus, when you select the command
.K "Quit_Window_Mgr."
from the menu
.BR "TWM Operations" ,
the X server also dies, and the console returns to its normal, character-based
interface.
.PP
Suppose, however, that you want the screen to appear differently.
To do so, you would first enter the command
.DM
	cp /usr/X11/lib/xinit/xinitrc $HOME/.xinitrc
.DE
This gives you a private copy of
.BR xinitrc .
When
.B xinit
is invoked, it first looks for file
.B .xinitrc
in your home directory; only if it cannot find this file does it read file
.BR /usr/X11/lib/xinit/xinitrc .
.PP
Now, you can use \*(ME or any other text editor to modify your
.B $HOME/.xinitrc
as you wish.
.II xsetroot
.II plaid
For example, if you wish to tile the root window with a plaid design
(so that it resembles a picnic tablecloth), use the command
.B xsetroot
as follows:
.DM
	xsetroot -bitmap /usr/X11/include/X11/bitmaps/plaid &
.DE
The next time you invoke X, the root window will appear like this:
.PH 1 1 \*(XD/rootA.eps
For details on the command,
.B xsetroot
see its entry in this manual's Lexicon.
.PP
.II xclock
As another example, suppose that you prefer to have a digital clock rather than
an analogue one.
Just modify the
.B xclock
command to the following:
.DM
	xclock -geometry 135x141+15+26 -digital -fg blue -chime -update 1 &
.DE
You can invoke other X clients, if you wish.
.II xbiff
For example, many users like to invoke the X client
.BR xbiff .
This client displays a picture of a mailbox; when you receive mail, the flag
on the mailbox pops up.
Try adding the command
.DM
	xbiff -geometry 91x81+476+44 &
.DE
after the
.B xsetroot
command.
The mailbox will then be displayed automatically the next time you invoke X.
.PP
.II xeyes
Many users also like to invoke the X client
.B xeyes
when they bring up X.
This command displays a pair of eyes whose pupils follow the mouse cursor
around the screen.
Inserting the command
.DM
	xeyes -geometry 150x100+286+66 &
.DE
into
.B .xinitrc
brings up the eyes near the top center of your screen.
.PP
.II geometry
Note that changing the
.B \-geometry
argument to a command can be rather difficult.
The best approach is invoke the client through
.BR xvt ;
position and size its window as you want it;
then select the
.K Info
option from the menu
.BR "WINDOW OPS" ,
to show you the size and position of the window;
and copy this information into
.BR xinit .
.Sh "Customizing .twmrc"
.PP
As shown above, the window manager
.B twm
is invoked last from within
.BR xinitrc ,
so that when it exits the X session will end as well.
.II system.twmrc
When
.B twm
comes up, it reads the file
.BR /usr/X11/lib/twm/system.twmrc ,
which sets its default behaviors.
.B twm
has many features that you can set or invoke, so
.B twmrc
can be extremely complex.
The following introduces a few of the more commonly used features that you
can set or invoke from within
.BR twmrc .
.PP
To begin, copy the file
.B /usr/X11/lib/twm/system.twm
to
.BR $HOME/.twmrc .
.B twm
reads
.B system.twmrc
only if it cannot find
.B .twmrc
in your home directory.
You can now use \*(ME or any other text editor to edit your private copy of
.B .twmrc
to suit your preferences.
.PP
.B .twmrc
is divided into three sections:
.IP \fBVariables\fR 0.8i
This section sets switches and variables within
.BR twm ,
such as the colors used with various objects, the cursor shapes used in
various circumstances, and how icons are managed.
.IP \fBBindings\fR
This binds function keys, mouse buttons, and title buttons to various actions.
It also lets you define functions.
These functions are much like shell functions, in that they consist of
one or more of
.BR twm 's
built-in functions bundled together.
.IP
Mouse buttons and function keys can be given different bindings,
depending upon the object in which the mouse cursor is positioned.
A binding can tell
.B twm
to display a menu, execute a command under the shell, invoke one of
.BR twm 's
built-in functions, or invoke a user-defined function.
.IP \fBMenus\fR
These define the contents of the menus that
.B twm
displays.
Note that the
.B Bindings
section defines what menus appear when.
.PP
The following sub-sections describe these portions of
.B .twmrc
in more detail.
For a full description of
.BR twm 's
variables, functions, and menus, see the entry for
.B twm
in this manual's Lexicon.
.Sh "Variables"
.PP
As noted above,
.B twm
has many variables that you can set to change its appearance or behavior.
Some variables are Boolean and some take a numeric value,
but most take a string value as an argument.
Some variables also take a
.IR "window list" ,
or a list of windows to which a given argument applies.
.PP
.II DontMoveOff
As an example of a Boolean variable, consider the variable
.BR DontMoveOff .
When this variable is present in
.BR .twmrc ,
the window manager will not let you move a window off the physical screen.
Thus, inserting the variable
.DM
	DontMoveOff	
.DE
into your
.B .twmrc
restricts window movement to the physical screen.
You can override this default setting, as will be described.
.PP
.II NoTitle
The variable
.B NoTitle
is another Boolean variable:
if set,
.B twm
does not display a title bar on its windows.
You can, however, use a window list with this variable to restrict it to
selected windows.
.B system.twmrc
defines
.B NoTitle
as follows:
.DM
	NoTitle
	{
		"Twm Door"
		"TWM Icon Manager"
		"oclock"
		"xbiff"
		"xclock"
		"xload"
	}
.DE
.II xclock
As you can see,
.B xclock
is on this list; and when it comes up on your screen,
it does not have a title bar.
.PP
.II BorderWidth
The variable
.B BorderWidth
is an example of a variable that takes an integer value.
This variable sets the default width, in pixels, of a window's border
(or frame).
(Note that in X, the terms ``border'' and ``frame'' are used almost
interchangeably.)
The default setting is three.
You can change this value.
For example, inserting the instruction
.DM
	BorderWidth 50
.DE
makes the window borders 50 pixels wide; and when your screen comes up,
it will look like this:
.PH 1 1 \*(XD/rootB.eps
Fifty pixels probably is too wide a border; but, if you wish, you can thicken
the border to four or five pixels to make it easier for you to resize and move
the window.
.PP
.II Cursors
Finally, as an example of a variable that takes one or more string arguments,
consider the variable
.BR Cursors .
This variable sets the shape of the cursor in various parts of the screen,
or when certain actions are occurring.
.B system.twmrc
defines this variable as follows:
.DM
.ta 0.5i 1.0i 2.0i
	Cursors 
	{
		Frame	"top_left_arrow"
		Title	"top_left_arrow"
		Icon	"top_left_arrow"
		IconMgr	"top_left_arrow"
		Move	"fleur"
		Resize	"fleur"
		Menu	"sb_left_arrow"
		Button	"hand2"
		Wait	"watch"
		Select	"dot"
		Destroy	"pirate"
	}
.DE
The terms in the left column define a portion of the screen or an action.
For the most part, these are self-explanatory:
for example,
.B Move
and
.B Resize
refer to when you are, respectively, moving and resizing a window;
whereas
.B Frame
refers to when the cursor touches the window border.
.PP
The right column names the cursor to be used.
Because the names are strings, you must enclose them within quotation marks.
.II cursorfont.h
The names of the cursors are defined in file
.BR /usr/X11/include/X11/cursorfont.h ,
with except that the prefix
.B XC_
is dropped from each name.
Read this file for suggestion on other cursors you can use;
for example, you can change the
.B Wait
cursor from
.B watch
to
.BR coffeemug ,
which shows a stylized coffee mug.
.II xfd
For a picture of what cursors are available, see the entry for the command
.B xfd
in this manual's Lexicon.
.PP
As noted above, the X cursors are stored in a font; a font, after all,
is simply a set of bit-mapped images.
A cursor actually consists of two bit-mapped images:
one draws the cursor itself, and the second (which is drawn in a constrasting
color) forms a mask around the cursor.
This is done so that the cursor will never disappear against its background.
If you wish, you can use a bit-mapped image of your own creation as a mouse
cursor.
To do so, simply replace the name of the cursor from the
.B cursor
font with the name of the bit-mapped image you want, plus its mask.
.II Frame
For example, if you replace the definition of the
.B Frame
cursor with the following:
.DM
.ta 0.5i 1.5i
	Frame	"/usr/X11/include/X11/bitmaps/star" \e
		"/usr/X11/include/X11/bitmaps/starMask"
.DE
then, the next time you restart X, the cursor will change to an asterisk
whenever the mouse cursor touches a window frame.
.PP
.II Color
Some string variables let you set specific values for one or more windows.
For example, consider the variable
.BR Color ,
which sets the window manager's color scheme under the color server.
.B system.twmrc
defines it as follows:
.DM
.ta 0.5i 1.0i 3.0i 4.0i
	Color
	{
		DefaultBackground	"#5f9ea0"	# cadet blue
		DefaultForeground	"#ffffff"	# white
		BorderColor	"#00ff00"	# green 
		{
		"xload"	"khaki"
		"xclock"	"khaki"
		"xbiff"	"red"
		}
		BorderTileBackground	"black"		# {winList}
		BorderTileForeground	"white"		# {winList}
		MenuBackground	"firebrick"
		MenuForeground	"white"
		MenuTitleBackground	"green"
		MenuTitleForeground	"black"
		IconBackground	"gray85"	# {winList}
		IconForeground	"brown"	# {winList}
		IconBorderColor	"yellow"	# {winList}
		IconManagerBackground	"blue"	# {winList}
		IconManagerForeground	"white"	# {winList}
		IconManagerHighlight	"#ffff00"	# {winList}
		MenuShadowColor	"grey40"
		TitleBackground	"firebrick3"	# {winList}
		TitleForeground	"white"	# {winList}
	}
.DE
The left column names the section of the screen being defined.
Most of these are self-explanatory.
.PP
The right column names the color to be used in that portion of the
screen.
Because these are strings, you must enclose them within quotation marks.
A color can be defined either by name or as a number.
.II rgb.txt
The color names that
.B twm
recognizes are defined in file
.BR /usr/X11/lib/rgb.txt .
A number consists of a pound sign `#' followed by six hexadecimal digits:
two each for the red, green, and blue video guns.
For example,
.B #00ff00
defines solid green.
.PP
As noted earlier,
.B Color
(like some other string variables)
lets you set colors for individual windows by name.
.II BorderColor
For example, the following sets the colors for the window border:
.DM
.ta 0.5i 2.5i 3.5i
	BorderColor	"#00ff00"	# green 
	{
	"xload"	"khaki"
	"xclock"	"khaki"
	"xbiff"	"red"
	}
.DE
The default border color is green; however, the windows for the X applications
.B xload
and
.B xclock
have a khaki border, and the X application
.B xbiff
has a red border.
.PP
If, for whatever reason, you want
.B xvt
to have a pink border, insert the line
.DM
.ta 0.5i 2.5i 3.5i
	"xvt"	"pink"
.DE
into the above list.
Then, the next time you invoke X,
.B xvt
will come up bordered in pink.
.PP
For a complete list of the variables that you can set or modify your
.BR .twmrc ,
see the entry for
.B twm
in this manual's Lexicon.
.Sh "Bindings"
.PP
The second part of
.B .twmrc
are bindings:
that is, entries that define functions,
and entries that bind functions to mouse-button events, function keys,
and title-button events.
.PP
Function bindings combine one or more existing functions into a new function.
For example, the following defines the function
.BR xrdb-color :
.DM
	Function "xrdb-color"
	{
		f.exec "xrdb -remove"
		f.exec "xrdb -DCOLOR -load $HOME/.Xdefaults"
	}
.DE
This function consists of two calls to the function
.BR f.exec ,
which is built into
.BR twm ;
this function executes its argument under a shell.
.II xrdb
The two calls to
.B f.exec
invoke the X utility
.BR xrdb ;
the first invocation
drops the current resource table, and the second loads the contents of file
.B $HOME/.Xdefaults
into the server.
(For details on what these actions accomplish,
see the Lexicon entry for
.BR xrdb .)
The section on menus, below, will show how to call a function.
.B twm
has a number of functions built into it; for a full list, see the entry for
.B twm
in this manual's Lexicon.
.PP
The binding of a title button consists of the name of the title button,
followed by the name of the bit-mapped image to be drawn on the title bar,
followed by the action to execute when it is invoked.
For example, following entry from
.B system.twmrc
binds the left-title button:
.DM
	LeftTitleButton	 ":menu"     = f.menu "LeftTitleButton" 
.DE
The
.B LeftTitleButton
is the button that appears immediately to the left of the name of the window
in the title bar.
.B :menu
is the bit-mapped image to display for that button;
it is what we have called a stylized `D'.
This menu is built into
.BR twm .
Finally, the binding indicates that when this button is ``pressed,''
.B twm
is to invoke its internal function
.B f.menu
and display the menu called
.BR LeftTitleButton .
This menu, which is described below, is the menu that on the screen has the
title
.BR "WINDOW OPS" .
.PP
A binding for a button or keyboard event consists of three parts:
the name of the key or button in question, the context in which the
binding applies, and the action to take.
The name of the key or mouse button can be combined with metakeys
(i.e., the
.B <shift>
key, the
.B <ctrl>
key, and the
.B <esc>
key) to form other key combinations.
.PP
Context gives the context in which a pressing a given button or key performs
a given action.
The following lists the possible contexts:
.DS
	\fBa\fR	All
	\fBf\fR	Frame
	\fBi\fR	Icon
	\fBm\fR	Icon Manager
	\fBr\fR	Root Window
	\fBt\fR	Title
	\fBw\fR	Window
.DE
A button or key can have more than one context; if so, the context letters
must be separated by a vertical bar `|'.
Note that with regard to the
.B w
context, this simply sets a default action; the application, of course,
governs what goes on within its own window, and can overrule this setting
should it wish.
.PP
The action calls a function, as shown above.
.PP
The following gives the bindings from
.B system.default
for mouse button 1 (that is, the left-mouse button):
.DM
.ta 0.5i 2.0i 3.5i
	Button1 = c|m	: i|f|t|w	: f.raise
	Button1 =	: root	: f.menu "Applications"
	Button1 =	: t|i	: f.move
	Button1 =	: frame	: f.resize
	Button1 =	: m|i	: f.iconify
.DE
The following discusses these lines in order:
.IP \fB1.\fR 0.3i
This first line defines Button1 with a
.B <ctrl>
or
.B <esc> 
(meta) key.
If the mouse cursor is in an icon, on the window frame, on the title bar, or
within a window, then holding down the
.B <ctrl>
key and pressing the left-mouse button invokes the
.B twm
function
.BR f.raise ,
which raises the window to the foreground.
.IP \fB2.\fR
The second and succeeding lines contain no key entries, which means that this
button is not used with metakeys.
The second line instructs
.B twm
to invoke the function
.B f.menu
to display the menu
.B Applications
should you press the left-mouse button while the mouse
cursor is positioned over the background of the root window.
.IP \fB3.\fR
Invoke the
.B twm
function
.BR f.move ,
which moves the current window, should you press the left-mouse button
while the mouse cursor is positioned over the title bar or the window's icon.
.IP \fB4.\fR
Invoke the
.B twm
function
.BR f.resize ,
which resizes a window, should you press the left-mouse button
while the mouse cursor is positioned over the window's frame.
.IP \fB5.\fR
Finally, invoke the
.B twm
function
.BR f.iconify ,
which iconifies a window, should you press the left-mouse button while the
mouse cursor is positioned over the icon manager or the window's icon.
Note that this contradicts the definition given in line 3, which told
.B twm
to raise the window should you click the left-mouse button while the cursor is
positioned over the window's icon.
In the case of contradiction like this,
.B twm
uses the last binding to appear in its resource file.
.PP
You can modify these bindings if you wish.
For example, you can modify the setting for the left-mouse button in the
context of the window's frame to the following:
.DM
	Button1 =	: frame	: f.destroy
.DE
Normally, when you click the left-mouse button while the mouse cursor
is resting on the window's frame,
.B twm
resizes the window;
this change destroys the application and closes its window.
This is an extreme case, but it shows that you can rebind the buttons and
keys to suit your preferences in almost every case.
.PP
For a complete list of the elements that you can bind, and the internal
.B twm
functions to which you can bind them, see the Lexicon entry for
.BR twm .
.Sh "Menus"
.PP
Menus are in many ways the easiest part to define in a the
.B twm
resource file.
A menu begins with the keyword
.BR Menu ,
followed by the menu's name.
Then comes the default color scheme for the menu, foreground first and then
background.
The entries for the menu follow, between braces.
Each entry consists of the stub that appears in the menu, followed by an
optional color scheme, then the action to take when this entry is selected.
.PP
.II menu^APPLICATIONS
The following gives the definition of the menu
.B Applications
from
.BR system.twmrc :
.DM
.ta 0.5i 1.0i 3.5i
	Menu "Applications" ("black":"lightseagreen")
	{
		"APPLICATIONS" ("black":"lightseagreen")	f.title
		"Font Select" ("lightseagreen":"black")	!"xfontsel &"
		"Puzzle"	!"puzzle &"
		"XBiff"	!"xbiff &"
		"XCalc"	f.menu " Calculator " 
		"XClock"	!"xclock -chime -fg blue -update 1 &"
		"Xeyes"	!"xeyes -fg red &"
		"XLoad"	!"xload &"
		"XLogo"	!"xlogo &"
		"XMag"	!"xmag &"
		"XTerm"	!"xvt -ls -cr red &"
		"XTetris" ("lightseagreen":"black")	!"xtetris &"
	}
.DE
This menu is shown in one of the figures reproduced earlier.
Most of the entries are self-explanatory.
Note that the `!' is a synonym for
.BR f.exec ,
which executes its argument under the shell.
.PP
The entry
.B APPLICATIONS
invokes the
.B twm
function
.BR f.title ,
which indicates that this is the title entry for the menu.
You cannot select it.
.PP
The entry for
.B XCalc
.DM
	"XCalc"	f.menu " Calculator " 
.DE
invokes the function
.BR f.menu .
This function displays a button at the right of the menu entry,
and displays a sub-menu when the button is touched.
Its argument is the name of the menu to drop down, in this case the
menu entitled
.BR Calculator ,
which is also defined in
.BR system.twmrc .
.PP
If you wish, you can add commands to this menu:
just insert a line in the appropriate spot.
The entries in the menu are sorted by hand;
.B twm
does not rearrange their order in any way.
.II xgas
For example, to insert into this menu the game
.BR xgas ,
shown above, just add the line
.DM
	"Xgas"	!"xgas -fg red &"
.DE
directly after the entry labelled
.BR Xeyes .
.PP
If you wish, you can also change the arguments to existing commands.
.II xclock
For example, the entry
.DM
	"XClock"	!"xclock -chime -fg blue -update 1 &"
.DE
invokes the X client
.B xclock
invokes an analogue clock with a blue face, which is updated once every second,
and chimes on the hour and half-hour.
If you wish to eliminate chiming and have a digital clock instead,
modify this command to read:
.DM
	"XClock"	!"xclock -digital -fg blue -update 1 &"
.DE
For the options available with each of these commands,
see its entry in this manual's Lexicon.
.PP
The color settings are used only by the color server.
If you wish, you can give a separate color combination to each entry in
the menu.
Note that the first and last selectable entries in this menu both have the
color settings of:
.DM
	("lightseagreen":"black")
.DE
There is a reason for giving the same colors to the first and last entries:
if the first and last color settings in the menu are not exactly the same,
.B twm
generates colors for the menu that fade from the first known pair of colors
to the last known pair.
To see this in action, try removing the first instance of:
.DM
	("lightseagreen":"black")
.DE
.II MenuBackground
.II MenuForeground
.II Monochrome
If a menu does not set any colors, the server uses the settings for
.B MenuBackground
and
.B MenuForeground
in the variable
.BR Color .
The monochrome server ignores all color settings, and uses the settings for
.B MenuBackground
and
.B MenuForeground
in the variable
.BR Monochrome .
.PP
This concludes the discussion of how to customize the window manager.
For more information, see the Lexicon entries for
.BR twm ,
.BR xinit ,
and
.BR xsetroot .
.SH "Where To Go From Here"
.PP
This concludes our introduction to the X Window System.
For more information, see the following chapter, which introduces the
clients and utilities that comes with X Windows for \*(CO.
The Lexicon discusses each client and utility in detail.
If you wish to explore X further, we suggest that you buy the books
named in the introduction to this manual.
.PP
However, there is no better teacher than experience.
We suggest that you work with X, try modifying the defaults files
(or, to be exact,
.I copies
of the defaults files),
and play with the system.
X Windows is well designed and robust.
You will become proficient quickly.
We hope that you enjoy using X Windows!
