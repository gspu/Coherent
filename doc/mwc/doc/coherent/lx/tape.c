.TH tape "" "" Command
.PC "Manipulate a tape device"
\fBtape command [\fIcount\^\fB] [\fIdevice\^\fB]\fR
.PP
.HS
.SH "Commands:"
.IC \fBerase\fR
Erase the tape.
SCSI tape only.
.\".IC \fBformat\fR
.\"Format the tape.
.\"This applies only to floppy-tape devices.
.\".IC \fBposition\fR
.\"Skip
.\".I count
.\"files forward on the tape (default, one).
.\".IC \fBreset\fR
.\"Reset the tape device.
.IC \fBretension\fR
Retension the tape.
.IC \fBrewind\fR
Rewind the tape.
.IC \fBrfm\fR
Move the tape forward to the next file mark;
in effect, skip the current file.
SCSI tape only.
.IC "\fBseek \fIposition\fR"
Move the tape device's seek point to byte \fIposition\fR.
Floppy tape only.
.IC \fBstatus\fR
Read the status of the tape device.
Floppy tape only.
.IC \fBtell\fR
Display the byte position at which the next read or write operation will
be performed.
Floppy tape only.
.HE
The command
.B tape
lets you manipulate a tape device.
.I device
names the tape device to manipulate.
If you name no
.I device
on the command line,
.B tape
uses the device
.BR T_DEFAULT ;
header file
.B <tape.h>
defines this constant to be device
.BR /dev/tape .
For a list of tape devices, see the overview article for
.BR tape .
.PP
.I command
names the task that you want
.B tape
to execute, as follows:
.IP \fBerase\fR 1.25i
Erase the tape.
SCSI tape only.
.\".IP \fBformat\fR
.\"Format the tape.
.\"This applies only to floppy-tape devices.
.\".IP \fBposition\fR
.\"Skip
.\".I count
.\"files forward on the tape (default, one).
.\".IP \fBreset\fR
.\"Reset the tape device.
.IP \fBretension\fR 1.25i
Retension the tape.
This rewinds the tape, then performs a full forward wind, then another rewind.
The seek offset is set to zero.
.IP \fBrewind\fR
Rewind the tape.
This command positions the tape at the beginning of track 0.
It resets seek offset (see
.B seek
and
.BR tell ,
below) to zero.
If tape is already rewound, this command has no effect.
.IP \fBrfm\fR
Move the tape forward to the next file mark;
in effect, skip the current file.
SCSI tape only.
.IP "\fBseek \fIlocation\fR"
This command has the same effect as if the tape had just been used
with no-rewind-on-close, leaving the tape at byte
.IR location .
No tape motion occurs at the time of the command, but the next read or
write begins at byte
.I location
on the tape.
Floppy tape only.
.IP "\fBstatus\fR"
Display various parameters for the tape drive,
and for the cartridge being used.
Not every tape drive supports every status option.
Unsupported features appear as ``unavailable''.
The following gives an example of output from this command:
.DM
	Floppy Tape Status:
		Drive Configuration  = 0x90
			500 Kbits/sec
			Non-Extra-Length Tape
			QIC-80 Mode.
		ROM Version  = 0x85
		Vendor ID  = 0x0146, Make=5, Model=6
		Tape Status Unavailable.
		Drive Status  = 0x65
			drive ready or idle
			cartridge present
			cartridge referenced
			at physical BOT
		Drive Error Status - No Error.
.DE
.IP
Floppy tape only.
.IP \fBtell\fR
Display the byte offset that will be in effect the next
time the tape is read or written.
Floppy tape only.
.PP
.II ftbad
The related command
.B ftbad
lets you read and modify the list of bad blocks on a floppy-tape cartridge.
.SH "See Also"
.Xr "commands," commands
.Xr "ftbad," ftbad
.Xr "hai," hai
.Xr "tape" tape
