.ds TL "Running 3rd-Party Software"
.NH "Running Third-Party Software"
.PP
\*(CO runs many packages from third-party vendors that
were designed for use with other
operating systems, in particular SCO \*(UN.
\*(CO will run these packages, but some require some minor
reconfiguration of either the package, of \*(CO, or both, to run smoothly.
.PP
The following sections describe some of the packages that Mark Williams
Company has tested, and gives directions on how to perform these minor
reconfigurations.
Note that we continually test new packages, so the following list probably
is incomplete.
For the latest information on what packages run under \*(CO, and how you
can run them, telephone the company from which you purchased the software,
check the MWC bulletin board, or telephone Mark Williams Company.
.SH "General Directions"
.PP
To install third-party \*(UN software onto \*(CO, be aware of the following
general rules:
.IP \fB1.\fR 0.3i
\*(CO does not support the command
.BR custom .
To install, you should read the product's manual and determine what must
be done to install the product.
Some products come with a shell script that performs all the tasks of
installation; you can use the Bourne shell
.B sh
to invoke that script.
Other products use
.B tar
or
.BR cpio ;
look in your package's installation or release notes for instructions.
Please note that when un\fBtar\fRring or \fBcpio\fRing
floppy disks from other \*(UN systems, you sometimes must
use the complex \*(UN device names that are now supported in this release.
For information on floppy-disk device names, see the Lexicon entries for
.B fd
and
.BR "floppy disk" .
.IP \fB2.\fR
Some third-party packages support many different operating systems, and when
you install the package let you pick the operating system from a menu.
If this is the case, you should pick \*(CO first, if it is one of the options.
If \*(CO is not an option, pick SCO \*(UN; if SCO \*(UN is not available,
then Interactive \*(UN; and if none of those are available, then
USL \*(UN System V release 3.2.
If none of those are available, then select generic
\*(UN system V release 2.
.IP \fB3.\fR
Some packages offer a variety of console devices.
If you have a color console, select
.BR colorcons .
If this is not available, try
.BR at386 .
If neither is an option, or if you have a monochrome console,
then you should select
.BR monochrome ;
in most cases, this works correctly, even with color devices.
.IP \fB4.\fR
If possible, install all available terminal types.
Some packages require that you reinstall the entire package should you
need a terminal type that you have not yet installed.
.IP \fB5.\fR
\*(CO does not come with the utility
.BR unpack ,
which some packages require to install.
To get around this problem, simply link the program
.B gzip
to
.BR unpack .
.IP \fB6.\fR
Some packages require some features of
.B awk
that the \*(CO version does not yet implement.
To get around this problem, move the \*(CO
.B awk
to
.BR awk.old ;
then link the command
.B mawk
(the GNU version of
.BR awk )
to
.BR awk .
If your system does not have
.BR mawk ,
contact Mark Williams Company and we will send you a copy.
.IP \fB7.\fR
Some packages require the \*(UN pager
.B pg
to display the contents of some text files.
\*(CO does not come with
.BR pg ;
however, you can use the \*(CO pager
.B more
as a substitute.
To do so, type the command:
.DM
	ln -f /usr/bin/more /usr/bin/pg
.DE
.IP \fB8.\fR
The SCO impementation of the link command
.B ln
will overwrite an existing file by default.
This behavior does not comply with the \*(PX.
Under \*(CO,
you must use the option \fB\-f\fR to force
.B ln
to overwrite an existing file.
Some installation scripts, however, depending upon the non-standard behavior of
.BR ln .
If this problem arises while you are installing a package, you must edit the
installation script and insert the flag \fB\-f\fR where appropriate.
.IP \fB9.\fR
\*(CO does not support the
.B brand
utility from SCO.
This utility ``brands'' (or serializes) executables using a
serial number and license activation key.
Any package that requires
.B brand
for proper installation and execution will not work under \*(CO.
.SH "Third-Party Packages"
.PP
The following gives information on using some of the more popular third-party
\*(UN packages under \*(CO.
.Sh "Aide de Campe"
.PP
This package installs and runs correctly.
.Sh "BBX Progression/4"
.PP
The installation of this package requires that you use the command
.BR cpio .
Because the installation program does not prompt for the two supplementary
disks, you must re-issue the command
.DM
	cpio -icB
.DE
for each disk in the distribution.
.PP
The newest version from BASIS runs BASIS International's
regression suite without error.
.Sh "Blast"
.PP
The package works correctly; however, some of its graphic displays do not
work correctly because of limitations in the \*(CO console driver.
We are working to correct this problem.
If this is a problem for you, contact Mark Williams Company, and we will send
you a corrected version of the console driver as soon as it is ready.	
.Sh "ccsh"
.PP
As of this writing, the shell-script compiler
.B ccsh
does not run under \*(CO 4.2.
That is due to the fact that its internal library
.B /usr/lib/libccsh.a
is not in the COFF format.
It does run, however, on release 3.2 of \*(CO.
.PP
For details, please contact the publisher of
.BR ccsh .
.Sh "Codebase"
.PP
The Kernighan and Ritchie source compiles.
.Sh "Dataflex"
.PP
This package installs and runs correctly.
.Sh "dBASE III"
.PP
The package works correctly; however, some of its graphic displays do not
work correctly because of limitations in the \*(CO console driver.
We are working to correct this problem.
If this is a problem for you, contact Mark Williams Company, and we will send
you a corrected version of the console driver as soon as it is ready.	
.Sh "dBASE IV"
.PP
To install dBASE IV under \*(CO, do the following:
.IP \fB1.\fR 0.3i
Type the command
.DM
	cd /
.DE
.IP
to change to the root directory.
.IP \fB2.\fR
Insert System Disk 1 into a floppy-disk drive.
.IP \fB3.\fR
Type the command
.DM
	tar -xvf /dev/rfva\fIX\fP ./tmp/install
.DE
where
.I X
is `0' or `1', depending upon whether you have inserted the system disk
into drive 0 (i.e., A:) or 1 (i.e., B:).
.IP \fB4.\fR
When the file is extracted, issue the following command:
.DM
	cp ./tmp/install ./tmp/coh-install
.DE
.IP
Then, use the editor of your choice to edit file
.BR ./tmp/coh-install .
Change line 33 from:
.DM
	label=`dd if=$disk count=1 | dd bs=100 count=1 2>/dev/null | tr`
.DE
.IP
to
.DM
	label=`dd if=$disk count=1 | dd bs=100 count=1 2>/dev/null`
.DE
.IP
Save the edited file.
.IP \fB5.\fR
.I
Make sure you are in the root directory!
.R
This is crucial at this stage to installing correctly.
.IP \fB6.\fR
Execute the new install script by typing:
.DM
	/bin/sh ./tmp/coh-install /dev/rfva\fIX\fP
.DE
.IP
Once again, substitute `0' or `1' for
.I X
depending upon which floppy-disk drive you have inserted the system disk.
.IP \fB7.\fR
Continue doing the installation normally, as described in the documentation
that comes with the dBASE-IV package.
.IP \fB8.\fR
When you have concluded installing dBASE IV, you must execute the following
final commands:
.DM
	cd /usr/local/lib/dbaseiv
	rm dbase1.res
	ln ./dbase1.res.us ./dbase1.res
	rm /tmp/coh-install
.DE
.PP
You should now have a working implementation of dBASE IV!
.PP
Some users have encountered problems with this package's license manager.
They report that the first time they added a license to the
data base, the license manager corrupted the entry, thus prompting some
surly behavior from dBASE when they attempted to use the package.
.PP
To correct this problem, use the dBASE utility
.B editlic
to add a second license entry.
It appears that dBASE stores and processes this second entry correctly.
After you confirm that the new entry functions properly,
re-invoke
.B editlic
and remove the original entry.
After that, all appears to work as it should.
.PP
You may experience some problems with console output and the handling of
function keys.
If this is the case, you need a version of the dBASE terminal-description
file that is corrected for \*(CO.
You can obtain this file from site \fBraven\fR on the Intenet,
or from the Mark Williams bulletin board.
If all else fails, send mail to \fBsupport@mwc.com\fR, and we will mail a copy
to you.
.PP
Some users of dBASE IV have reported that the package, for whatever reason,
does not recognize directory or file names that contain capital letters.
If dBASE IV has trouble saving files, check the name of the directory,
and make sure it is all in lower-case letters.
.Sh "FilePro"
.PP
The package works correctly; however, some of its graphic displays do not
work correctly because of limitations in the \*(CO console driver.
We are working to correct this problem.
If this is a problem for you, contact Mark Williams Company, and we will send
you a corrected version of the console driver as soon as it is ready.	
.Sh "Flexilint"
.PP
The tool
.B Flexilint
does compile and run under \*(CO release 4.2.
.PP
To install the package, you must make one small change:
edit header file
.B custom.h
and change the prototype for
.B void
to
.BR int .
.Sh "Lotus 1-2-3"
.PP
To install Lotus 1-2-3, do the following:
.IP \fB1.\fR 0.3i
Before you begin installation, invoke the following commands:
.DM
	su root rm /bin/awk
	su root ln /usr/gnu/bin/mawk /bin/awk
.DE
.IP \fB2.\fR
Install Lotus by hand with
.B tar
(not with
.BR custom ),
as described in the Lotus documentation.
.IP \fB3.\fR
Before you run the command
.BR setup123 ,
type the following command:
.DM
	echo -n '\033[?8l'
	ln -f /usr/bin/more /usr/bin/pg
	ln -f /usr/bin/gzip /usr/bin/unpack
.DE
The first command set erase mode on the console.
The other two commands links \*(CO utilities to \*(UN utilities that
Lotus requires.
.PP
The sample files as stored in
.B /u/lotus/123.v11/smpfiles/misc
will fail if the current working directory is not
.BR /u/lotus/123.v11/smpfiles/misc .
To enter this directory from within Lotus, use the following command:
.DM
	/WGDD/u/lotus/123.v11/smpfiles/misc
.DE
As of this writing, graphs did not work because of limitations of the \*(CO
console driver.
We are working to correct this problem.
.\"If this is a problem for you, contact Mark Williams Company, and we will send
.\"you a corrected version of the console driver as soon as it is ready.	
.Sh "mc \(em MacroCALC"
.PP
MacroCALC installs and runs correctly under \*(CO 4.0 and 4.2.
Users should have no problems installing this package.
If you are installing MacroCALC onto \*(CO 4.2, do
.I not
copy onto your system the
.B ansipc
termcap entry that comes with the package:
the one included with \*(CO 4.2 already works correctly
.Sh "RMCobol"
.PP
RMCobol works out of the box, without modification.
You may notice some slight discrepancies with regard to colors and
the number of lines on the screen, which are easily corrected by resetting
the \*(CO console device.
.Sh "SlickEdit"
.PP
This editor works properly.
However, it comes with an internal language that it uses for enhanced editing,
macros, etc., that requires a compiled named
.BR st .
This compiler is on the SlickEdit's distribution disks; however, it is in
Microsoft
.B a.out
binary format, which cannot be executed under \*(CO.
.PP
SlickEdit works best with a customized terminal-information file that has
been prepared by Mark Williams Company.
To obtain a copy, contact the Mark Williams BBS as described earlier in these
notes.
.Sh "TERM Communications Package"
.PP
Because the distribution disks are not high density, you must use
device \fB/dev/fqa\fIX\fR (where \fIX\fR is \fB0\fR or \fB1\^\fR)
to load the distribution.
.PP
When you execute the
.B install
script after you have loaded the distribution, you
must to choose the first option in the index:
.DM
	UNIX System V/386 (Including Altos, AT&T and Interactive)
.DE
Once installed, our tests show that the package works well.
.Sh "WordPerfect and WordPerfect Office"
.PP
Before you can print anything on your \*(CO system, you must start up and
configure the MLP print spooler.
Do this before you attempt to install WordPerfect.
For details on MLP, see the Lexicon entry for
.BR printer .
.PP
When you load the initial boot disk,
if you choose to un-\fBtar\fR the disk using the device
.BR /dev/fva0 ,
.B tar
complains:
.DM
	This doesn't look like a tar archive.
	Skipping to next file...
.DE
.PP
This behavior also occurs with devices
.BR /dev/fva0 ,
.BR /dev/dsk/f03h ,
.BR /dev/dsk/f0q18dt ,
and
.BR /dev/fd0135ds18 .
Note that this does not affect the installation:
it is just a curiosity.
.PP
When you perform the actual installation, in some instances the installation
program exhibits strange behavior.
It will say:
.DM
	*Please Wait*    0% complete
.DE
It then prints what appears to be a process id on top of the fragment
.BR *Plea ,
moves the cursor down another line, and prints:
.DM
	*Please Wait*   1% complete
.DE
The
.B 1%
stage continues for a lengthy period of time;
the floppy-disk drive's light glows, to show that the disk is being read,
but otherwise the installation program displays nothing to indicate that
useful activity is occurring.
Do not panic:
the installation program is working; it simply is not updating the screen
properly.
.PP
.\"Once you have gone through the command
.\".BR "wpinstall" ,
.\"you have performed initial installation.
.\"At this point, you must install additional printers.
.\"The installation program by default selects 299 printers for installation.
.\"These include the printers that were installed initially, and others that
.\"appear to have been selected at random from the list of available printers.
.\"Go through the entire list; un-select the printers you do not have, and
.\"select others (if any) that you do have but that were not selected by default.
.\".PP
If your console is monochrome, insert the command
.DM
	export WPTERM51=scocons
.DE
into file
.BR /etc/profile .
If you console is color, insert the command:
.DM
	export WPTERM51=scoconscol
.DE
WordPerfect expects this variable to be set before it can run correctly.
Note that after you insert it, you must log out and log in again before
that variable can become part of your environment.
.PP
If you wish to run either package under X Windows, use the terminal type
.B VT102
and insert the following instructions into the file
.BR $HOME/.Xdefaults :
.DM
	xterm*VT102.Translations: #override \e
		<Key> F1 : string(0x1b) string("OP") \en \e
		<Key> F2 : string(0x1b) string("OQ") \en \e
		<Key> F3 : string(0x1b) string("OR") \en 
.DE
.PP
Some systems have experienced problems with printing under the
.B lp
spooler, when logged in as anyone other than the superuser.
This should not be a problem, but if it is, try switching the WordPerfect
spooler command from
.B lp
to
.B "hpr -B"
or
.BR "lpr -B" ,
respectively, depending upon whether you have a laser printer or a
dot-matrix printer.
If all else fails, try the following workaround:
.PP
.IP \fB1.\fR 0.3i
Set WordPerfect's spool command to:
.DM
	cat -u > /tmp/PRINT.ME
.DE
.IP
When WordPerfect prints, it will dump its output into file
.BR /tmp/PRINT.ME ,
instead of sending it directly to the spooler.
.IP \fB2.\fR
To print WordPerfect's output, open another virtual console and type the
command:
.DM
	cat /tmp/PRINT.ME > /dev/lp
.DE
With WordPerfect Office, you cannot receive mail from within the
application because it expects either SCO's MMDF, or
a version of
.B smail
that \*(CO does not support.
