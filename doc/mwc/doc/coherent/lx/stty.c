.TH "stty" "" "" Command
.PC "Set/print terminal modes"
.B stty
\fBstty \-a\fR
\fBstty \-g\fR
\fBstty \fIx\fB:\fIx\fB: ... :\fIx\fR
\fBstty \fIarglist ...\fR
.PP
.HS
.SH "Common Options:"
.IC \fBcooked\fR
Insert device into cooked mode
.IC "\fBeof \fIc\fR"
Set the end-of-file character to \fIc\fR
.IC "\fBeol \fIc\fR"
Set the end-of-line character to \fIc\fR
.IC "\fBerase \fIc\fR"
Set the erase character to \fIc\fR
.IC \fBevenp\fR
Turn on even parity
.IC "\fBintr \fIc\fR"
Set the interrupt character to \fIc\fR
.IC "\fBkill \fIc\fR"
Set the kill character to \fIc\fR
.IC \fBlcase\fR
Map upper-case characters to lower case
.IC \fB\-lcase\fR
Turn off mapping of upper-case characters to lower case
.IC \fBoddp\fR
Turn on odd parity.
.IC \fBparity\fR
Turn on even parity
.IC \fB\-parity\fR
Turn off parity
.IC "\fBquit \fIc\fR"
Set the quit character to \fIc\fR
.IC \fBraw\fR
Insert device into raw mode
.IC \fBsane\fR
Restore a device to sanity, e.g., after an editor or communications
program died unexpectedly
.Pp
If no \fIoption\fR is specified,
\fBstty\fR prints the modes of the standard-input device on stderr.
.HE
The command
.B stty
lets you change or display the settings of the standard input device.
The device is usually a terminal, although tapes, disks and other
special files may be applicable.
.SH "Default Settings"
.II "cooked devices"
The following describes how \*(CO sets up a terminal device by default.
This normal processing is often called \*(QLcooked\*(QR mode.
Note that on some machines,
the default characters differ from those given below.
.PP
The
.I erase
and
.I kill
characters (normally
.B <crtl-H>
and
.BR <ctrl-U> )
erase, respectively, one typed character and an entire line of typing.
.PP
The
.I stop-output
and
.I start-output
characters (normally
.B <ctrl-S>
and
.BR <ctrl-Q> )
respectively stop and restart output.
.PP
The
.I interrupt
character (normally
.BR <ctrl-C> )
sends the signal
.BR SIGINT ,
which usually terminates program execution.
.PP
The
.I quit
character
(normally \fB<ctrl-\e>\fR)
sends the signal
.BR SIGQUIT ,
which usually terminates program execution with a core dump.
.PP
The
.I "end of file"
character (normally
.BR <ctrl-D> )
generates an end-of-file signal from the terminal.
.PP
You can change the setting of each special character by invoking
.B stty
with the appropriate option.
.SH Options
When called without any arguments,
.B stty
gives a brief listing of settings for the standard-input device.
.PP
.B stty
can read the settings of devices other than the standard-input device
by redirecting that device to it.
For example, the command
.DM
	stty < /dev/com1l
.DE
.PP
prints a brief summary of the settings for serial device
.BR com1l .
.PP
.BR stty 's
command-line arguments can take a number, as indicated
below by
.IR n ;
or they can take a character, as indicated below by
.IR c .
Argument
.I c
can be one of the following:
.IP \(bu 0.3i
A single character.
.IS \(bu
A caret `^' followed by a single character (to indicate a control character,
e.g., \fB^X\fR for \fB<ctrl-X>\fR).
.IS \(bu
An \fB^?\fR, which denotes the \fB<del>\fR character.
.IS \(bu
An `0x' followed by two hexadecimal digits.
.IS \(bu
An \fB^-\fR, which indicates that that option is not used.
.PP
.B stty
recognizes the following command-line arguments:
.IP \fB\-a\fR 0.75i
Give a complete listing of settings for the standard-input device.
.IS \fB\-g\fR
Give a complete list of settings for the standard-input device, but
in hexadecimal.
This is a dump of the
.B termio
structure in effect at the moment.
For more information on the
.B termio
structure, see the Lexicon entry for
.BR termio .
.IS \fIx\fB:\fIx\fB:...:\fIx\fR
Establish new settings for the standard-input device.
The settings are hexadecimal values that are separated by colons.
This form can be combined with \fB\-g\fR option to copy
.B stty
settings from one device to another.
For example, to set device
.B com2l
so that it mimics device
.BR com1l ,
use the following command:
.DM
	stty \(gastty -g < /dev/com1l\(ga < /dev/com2l
.DE
.IP \fB0\fR
Hang up the telephone.
.IS \fB50\fR
Set line speed to 50 bps.
.IS \fB75\fR
Set line speed to 75 bps.
.IS \fB110\fR
Set line speed to 110 bps.
.IS \fB134\fR
Set line speed to 110 bps.
.IS \fB150\fR
Set line speed to 150 bps.
.IS \fB200\fR
Set line speed to 200 bps.
.IS \fB300\fR
Set line speed to 300 bps.
.IS \fB600\fR
Set line speed to 600 bps.
.IS \fB1200\fR
Set line speed to 1200 bps.
.IS \fB1800\fR
Set line speed to 1800 bps.
.IS \fB2400\fR
Set line speed to 2400 bps.
.IS \fB4800\fR
Set line speed to 4800 bps.
.IS \fB9600\fR
Set line speed to 9600 bps.
.IS \fB19200\fR
Set line speed to 19200 bps.
.IS \fB38400\fR
Set line speed to 38400 bps.
.IS \fBbrkint\fR
Send interrupt on break.
.IS \fB\-brkint\fR
Do not send interrupt on break.
.IS \fBbs0\fR
No delay on backspace.
.IS \fBbs1\fR
Delay briefly on backspace.
.IS \fBclocal\fR
Turn on modem control.
.IS \fB\-clocal\fR
Turn off modem control.
.IS \fBcooked\fR
Set the device into cooked mode.
This is a composite of options
.BR parenb ,
.BR \-parodd ,
.BR cs7 ,
.BR brkint ,
.BR ignpar ,
.BR istrip ,
.BR icrnl ,
.BR ixon ,
.BR opost ,
.BR onlcr ,
.BR isig ,
and
.BR icanon .
.IS \fBcr0\fR
No delay on carriage returns.
.IS \fBcr1\fR
Carriage-return delay depends upon column position.
.IS \fBcr2\fR
Delay approximately 0.10 seconds on carriage return.
.IS \fBcr3\fR
Delay appoximately 0.15 seconds on carriage return.
.IS \fBcread\fR
Enable the receiver.
.IS \fB\-cread\fR
Disable the receiver.
.IS \fBcs5\fR
Character size is five bits.
.IS \fBcs6\fR
Character size is six bits.
.IS \fBcs7\fR
Character size is seven bits.
.IS \fBcs8\fR
Character size is eight bits.
.IS \fBcstopb\fR
Use two stop bits per character.
.IS \fB\-cstopb\fR
Use one stop bit per character.
.IS \fBecho\fR
Echo every character.
.IS \fB\-echo\fR
Do not echo characters.
.IS \fBechoe\fR
Echo the erase character as backspace-space-backspace.
.IS \fB\-echoe\fR
Do not echo the erase character as backspace-space-backspace.
.IS \fBechok\fR
Echo newline after the kill character.
.IS \fB\-echok\fR
Do not echo newline after the kill character.
.IS \fBechonl\fR
Echo newline.
.IS \fB\-echonl\fR
Do not echo newline.
.IS \fBek\fR
Set the kill and erase characters to printable characters.
A composite of
.B "erase '#'"
and
.BR "kill '@'" .
.IS "\fBeof \fIc\fR"
Set the end-of-file character to
.IR c .
.IS "\fBeol \fIc\fR"
Set the end-of-line character to
.IR c .
.IS "\fBerase \fIc\fR"
Set the erase character to
.IR c .
.IS \fBevenp\fR
Set the port to even parity.
This is a composite of the options
.BR parenb ,
.BR \-parodd ,
and
.BR cs7 .
.IS \fB\-evenp\fR
Turn off even parity \(em in effect, turn off parity altogether.
This is a composite of the options
.B \-parenb
and
.BR cs8 .
.IS \fBff0\fR
No delay on formfeeds.
.IS \fBff1\fR
Delay approximately two seconds on formfeeds.
.IS \fBhup\fR
Hang up the telephone on logging out.
.IS \fB\-hup\fR
Do not hang up the telephone on logging out.
.IS \fBhupcl\fR
Same as \fBhup\fR.
.IS \fB\-hupcl\fR
Same as \fB\-hup\fR.
.IS \fBicanon\fR
Enable canonical input.
.IS \fB\-icanon\fR
Disable canonical input.
.IS \fBicrnl\fR
Map carriage-return to newline on input.
.IS \fB\-icrnl\fR
Do not map carriage-return to newline on input.
.IS \fBignbrk\fR
Ignore break on input.
.IS \fB\-ignbrk\fR
Do not ignore break on input.
.IS \fBigncr\fR
Ignore carriage return on input.
.IS \fB\-igncr\fR
Do not ignore carriage return on input.
.IS \fBignpar\fR
Ignore parity errors on input.
.IS \fB\-ignpar\fR
Do not ignore parity errors on input.
.IS \fBinlcr\fR
Map newline to carriage return on input.
.IS \fB\-inlcr\fR
Do not map newline to carriage return on input.
.IS \fBinpck\fR
Enable parity checking on input.
.IS \fB\-inpck\fR
Do not enable parity checking on input.
.IS "\fBintr \fIc\fR"
Set the interrupt character to
.IR c .
.IS \fBisig\fR
Check input against interrupt and quit characters.
.IS \fB\-isig\fR
Do not check input against interrupt and quit characters.
.IS \fBiuclc\fR
Map input's upper-case characters to lower case.
.IS \fB\-iuclc\fR
Do not map input's upper-case characters to lower case.
.IS \fBistrip\fR
Strip input to seven bits.
.IS \fB\-istrip\fR
Do not strip input to seven bits.
.IS \fBixany\fR
Allow any on input character to restart output.
.IS \fB\-ixany\fR
Do not allow any input character to restart output.
.IS \fBixoff\fR
Request that system send start or stop characters when the input queue is,
respectively, nearly full or nearly empty.
.IS \fB\-ixoff\fR
Do not request that system send start or stop characters to manage input queue.
.IS \fBixon\fR
Use start/stop characters to control output queue.
.IS \fB\-ixon\fR
Do not use start/stop characters to control output queue
.IS "\fBkill \fIc\fR"
Set the kill character to
.IR c .
.IS \fBlcase\fR
Map upper-case characters to lower case.
A composite of options
.BR xcase ,
.BR iuclc ,
and
.BR olcuc .
.IS \fB\-lcase\fR
Turn off mapping of upper-case character to lower case.
A composite of options
.BR \-xcase ,
.BR \-iuclc ,
and
.BR \-olcuc .
.IS \fBLCASE\fR
A synonym for
.BR lcase .
.IS \fB\-LCASE\fR
A synonym for
.BR \-lcase .
.IS "\fBmin \fIn\fR"
.II VMIN
Set the constant
.B VMIN
to decimal value
.IR n .
For more about
.BR VMIN ,
see the Lexicon entry for
.BR termio .
.IS \fBnl\fR
A composite of options
.B \-icrnl
and
.BR \-onlcr .
.IS \fB\-nl\fR
A composite of options
.BR icrnl ,
.BR \-inlcr ,
.BR \-igncr ,
.BR onlcr ,
.BR \-ocrnl ,
and
.BR \-onlret .
.IS \fBnl0\fR
No delay on newline.
.IS \fBnl1\fR
Delay approximately 0.10 seconds on newline.
.IS \fBnoflsh\fR
Flush buffer on interrupt or quit.
.IS \fB\-noflsh\fR
Do not flush buffer on interrupt or quit.
.IS \fBocrnl\fR
In output,
map carriage return to newline.
.IS \fB\-ocrnl\fR
In output,
do not map carriage return to newline.
.IS \fBoddp\fR
Set device to odd parity.
This option is a composite of the options
.BR parenb ,
.BR parodd ,
and
.BR cs7 .
.IS \fB\-oddp\fR
Turn off odd parity \(em in effect, turn off parity altogether.
This is a composite of the options
.B \-parenb
and
.BR cs8 .
.IS \fBofdel\fR
Use delete characters as fill characters.
.IS \fB\-ofdel\fR
Do not use delete characters as fill characters.
.IS \fBofill\fR
Use fill characters for delays.
.IS \fB\-ofill\fR
Do not use fill characters for delays.
.IS \fBolcuc\fR
Map lower-case characters to upper case on output.
.IS \fB\-olcuc\fR
Do not map lower-case characters to upper case on output.
.IS \fBonlcr\fR
Map newline to carriage return/newline on output.
.IS \fB\-onlcr\fR
Do not map newline to carriage return/newline on output.
.IS \fBonlret\fR
A newline character executes a carriage return.
.IS \fB\-onlret\fR
A newline character does not execute a carriage return.
.IS \fBonocr\fR
Do not output carriage returns at column 0.
.IS \fB\-onocr\fR
Output carriage returns at column 0.
.IS \fBopost\fR
Post-process output.
.IS \fB\-opost\fR
Do not post-process output.
.IS \fBparenb\fR
Enable parity generation and detection.
.IS \fB\-parenb\fR
Disable parity generation and detection.
.IS \fBparity\fR
Synonym for option
.BR evenp .
.IS \fB\-parity\fR
Synonym for option
.BR \-evenp .
.IS \fBparmrk\fR
Mark parity errors.
.IS \fB\-parmrk\fR
Do not mark parity errors.
.IS \fBparodd\fR
Odd parity.
.IS \fB\-parodd\fR
Turn off odd parity; i.e., use even parity.
.IS "\fBquit \fIc\fR"
Set the quit character to
.IR c .
.IS \fBraw\fR
Set the device into raw mode.
This is a composite of the options
.BR \-parenb ,
.BR \-parodd ,
.BR \-hupcl ,
.BR cs8 ,
.BR \-opost ,
.BR \-olcuc ,
.BR \-ocrnl ,
.BR \-onocr ,
.BR \-onlret ,
.BR \-ofill ,
.BR \-ofdel ,
.BR nl0 ,
.BR cr0 ,
.BR tab0 ,
.BR bs0 ,
.BR vt0 ,
and
.BR ff0 .
This turns off
most character processing, including all input processing
(see
.B c_iflag
fields in
.BR <termio.h> ),
canonical input buffering (\fB\-icanon\fR),
and output processing (\fB\-opost\fR).
It does not turn off echo.
.IS \fB\-raw\fR
Turn off raw mode \(em in effect, restore the device to cooked mode.
Same as \fBcooked\fR.
.IS \fBsane\fR
Restore the device to ``sanity'' \(em for example, after an editor or
communications program has died unexpectedly.
This is a composite of options
.BR icrnl ,
.BR opost ,
.BR onlcr ,
.BR isig ,
.BR icanon ,
.BR \-xcase ,
.BR echo ,
.BR echoe ,
.BR echok ,
and
.BR "erase ^h" .
.IS \fBtab0\fR
No delay for horizontal-tab character.
.IS \fBtab1\fR
Delay for horizontal-tab character depends on column position.
.IS \fBtab2\fR
Delay approximately 0.10 seconds on horizontal tab.
.IS \fBtab3\fR
Expand horizontal-tab characters into spaces.
.IS \fBtabs\fR
A synonym for
.BR tab0 .
.IS \fB\-tabs\fR
A synonym for
.BR tab3 .
.IS "\fBtime \fIn\fR"
.II VTIME
Set the constant
.B VTIME
to decimal value
.IR n .
For more about
.BR VTIME ,
see the Lexicon entry for
.BR termio .
.IS \fBvt0\fR
No delay on vertical-tab characters.
.IS \fBvt1\fR
Delay approximately two seconds on vertical-tab characters.
.IS \fBxcase\fR
Canonical presentation of upper-case and lower-case characters.
.IS \fB\-xcase\fR
Do not process upper-case and lower-case characters.
.SH "See Also"
.Xr "ASCII," ascii
.Xr "commands," commands
.Xr "getty," getty
.Xr "init," init
.Xr "ioctl()," ioctl
.Xr "signal()" signal
.SH Notes
Executing
.B stty
with input redirected from another device
does not have an effect unless the device being read is open.
The last close of any terminal device resets all
.B termio
values to the system defaults.
Thus, to change the settings of a device, you must first open the device.
.PP
For example,
.DM
	enable com1l
.DE
.PP
or
.DM
	sleep 32000 > /dev/com1l &
.DE
.PP
might precede:
.DM
	stty evenp < /dev/com1l
.DE
.PP
Note, too, that
.B stty
does not check its arguments for consistency.
.PP
.B stty
provides complete access to the System-V-style
.B termio
structure.
Note, however, that
the settings of
.B termio
are processed by the kernel's in-line discipline and device-driver modules.
Under \*(CO, none of these modules pays attention to delay settings.
Therefore, setting delays with
.B stty
does not, at present, affect the behavior of the terminal device.
