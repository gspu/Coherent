.ds TL "Hardware"
.NH "Compatibility Information"
.PP
It is impossible for Mark Williams Company to test more than
a small fraction of the many computers, controllers, BIOSes, disks,
and other devices that purport to be compatible with the IBM PC.
The \*(CO system has been installed on tens of thousands of computers
throughout the world, and we have received reports from many of
our customers who have successfully installed and run \*(CO on
their systems (as well as from the few who could not do so).
.PP
This section names the machines, add-on cards, and BIOSes
that have been reported either to work or not to work
with the \*(CO operating system.
.PP
Before you continue, please note the following caveats:
.PP
First, this is only a partial list of the hardware on which \*(CO runs.
We receive confirmation of new machine configurations almost daily.
If you believe that you have a machine, BIOS, or add-on board that
is \fBnot\fR compatible with \*(CO but is listed below,
please telephone our technical support department.
.PP
Second, manufacturers change their hardware as part of redesign or
product improvement.
These changes can include logic, timing, firmware, or functionality.
Although we do try to support tested
products, Mark Williams Company cannot guarantee compatibility
with products not under its control.
.PP
If you believe that your computer cannot run \*(CO,
please contact the Mark Williams Company technical support department.
\fIIf you do not find your machine in this section, that does not mean
that it will not run \*(CO; odds are that it will.\fR
Whatever happens, please contact Mark Williams Company and
let us know what happened, so we can make your experience available
to future users of \*(CO.
.SH "Compatible Systems"
.PP
The following 386/486 systems have been tested, and have
been found to have been compatible with \*(CO.
Note that configurations vary, especially with respect to disk controllers,
so not all possible configurations have been tested.
.nf
.sp \n(pDu
.ta 3.5i
ACROS 325SE Notebook	ALR PowerFlex, 386SX, 386/220
ALR VEISA Tower 486/33	AMI 386SX, 386
Arche 386/25	AST NB4/25s
AST Premium 386/33	AST Premium 486/33E (EISA)	
AT&T 6386	Austin 386SX, 386/33
AUVA VIP, 300, 350/25	Bitwise 33-386 Portable
Cheetah International i486/25	Compaq 386, 386 Portable
CompuAdd 320, 325	Compudyne 386
Computer Directions 386SX	Comtex 386/20
Dataworld 386	Dell System 300, 310, 325
Dyna 386/20	EDP 386SX
EPS 386	Five Star 386/20
Gateway 2000 (RLL and ESDI)	Gateway 486, 33MHz (IDE)
GCH 386 AT	Giga-Byte 386-33
Hauppauge 386	Intel 301
Jameco 3550	JDR M386
Leading Edge 386, D3	Leading Technology 386SX
Logix 386-25	MAXAR 386
Micro-1 386	Micro-Designs 386, 25MHz
Micro Express 386	Micronics 386
Micronics 386SX	Mitsubishi 386
Mitsubishi MAXY 386NOTE	MTEK MS-35, MS-37, MS-41
MYLEX MAE486, 33 MHz EISA	MYLEX MNE486, 33 MHz EISA
MYLEX MWS386, 25 MHz ISA	MYLEX MTX386, 16/20 MHz ISA
MYLEX MTI386, 25/33 MHz ISA	NCR 386	NEC 386/25, Powermate 386/20, 386SX
Northgate 386/16, 486	Northgate Elegance 386/25 ISA
Northgate Elegance 433e EISA	Novex 386
Olivetti M380	Omega 386/20
Opti 486 WB, 493 chipset motherboard	Optima 386
Packard Bell 610, Axcel 386SX	PC Brand 386/20, 386/25
PC Pros 486	PC Systems 386-20
Philips P3302	Pulse 386-SX
SEFCO 16 MHz 386SX	Smart Micro 386
Solution 386	Standard Brands 386-25, 386/SX
Sunnytech 386-20	Sys Technologies 386
Tandon 386/20, 386/33	Tandy 4000DX, 4000SX
Tera-Tek 386	Texas Instruments Notebook
Tri-Star 386	Unibit DS316
UTI 386	Victor 386, V386S
Wyse 3216	Zenith Mastersport 386SL
Zenith TurboSport 386, 386/33	ZEOS 386, 386SX, 386 Portable
ZEOS Notebook 386SX
.fi
.SH "Compatible Serial Cards"
.PP
The \*(CO driver
.B asy
supports up to 32 devices (i.e., serial ports) using the same card and driver.
This lets \*(CO support a wealth of serial cards.
The following serial cards, both intelligent and dumb, have been tested with
\*(CO and have been found to work.
.nf
.sp \n(pDu
.ta 3.5i
Applied Digital SIO4
Arnet Multiport 4/8
Arnet TwinPort 2
Bocaboard BB1008
Comtrol Hostess 550 08
Comtrol Hostess 550 16
Connect Tech DFlex-4/8 serial card
Digiboard PC/16 serial card
DigiCHANNEL PC/X	
Gtek PCSS-8TX serial card
Sea Level Systems Turbo Comm+2/4/8 serial card
SEFCO serial adapter
Sritek FastCom
Sritek FastCom Plus
Stargate Plus 8 serial card
.fi
.SH "Compatible Disk Controllers and Drives"
.PP
\*(CO supports a variety of disk controllers and drives,
in a variety of formats.
The following controllers and drives
have been tested with \*(CO and have been found to work.
.nf
.sp \n(pDu
.ta 3.5i
Adaptec AHA-1540A SCSI Host Adapter
Adaptec AHA-1542A SCSI Host Adapter
Adaptec AHA-1540B SCSI Host Adapter
Adaptec AHA-1542B SCSI Host Adapter
Adaptec AHA-1542C SCSI Host Adapter
ADD Adaptec AHA-174X SCSI Host Adapter (when jumpered to 154X mode)
Adaptec 2372B RLL Controller
Adaptec 2372C RLL 1:1 Controller
AMI SCSI FAST Controller (100% Adaptec 154X compatible)
Bustek SCSI Host Adapter (100% Adaptec 154X compatible)
Data Technology DTC7287 1:1 RLL 1:1
DPT Smart Cache Plus SCSI controller #PM2012B/90/95 (Western Digital emulation)
DPT Smart Connex SCSI Host Adapter (Western Digital  emulation)
DTK PTI-217 IDE HD/FD
Future Domain TMC-840/841/880/881 SCSI Host Adapter
Future Domain TMC-845/850/860/875/885 SCSI Host Adapter
Galaxy DC 600B Cache (alternate polling only)
Hedaka HED-827-02 IDE controller
Kalok KL3100 IDE drive
Maxtor 7080AT IDE hard-disk drive
Micropolis 1684-07 SCSI hard-disk drive
MYLEX DCE376DR EISA SCSI Adapter (WD emulation)
National Computer Ltd NDC545 MFM
Perstor PS180-16FN RLL disk
Seagate ST01, ST02 SCSI Host Adapter
Seagate ST-225, ST-4096 MFM disk
Seagate ST-4144 RLL disk
Seagate ST-296N SCSI disk
Ultrastore Ultra 12 ESDI
Western Digital 930xx series IDE hard disks
Western Digital Caviar 2200 IDE hard disks
Western Digital Piranha hard disks
Western Digital WD1006V-MM2 1:1 MFM
Western Digital WD1006V-SR2 1:1 RLL
Western Digital WD1007-WAH ESDI (feature level F001 with on-board BIOS)
Western Digital WD1007-WAH2 ESDI (feature level F003 with on-board BIOS)
.fi
.PP
Please note that for the DPT Caching ESDI Controller, you must use
alternate polling.
For details on alternate polling, see the chapter on installation in these
notes.
.SH "Compatible Tape Devices"
.PP
\*(CO works with the following tape devices:
.PP
Archive Viper 60, 150, 250, and 525 SCSI-tape drives.
Conner CS250Q, CS525Q, CS1350Q, CS2000DAT, and CS4000DAT drives.
Other SCSI-tape drives may work as well, but have not yet been tested.
(Please note that Archive/Maynard was purchased by Conner in 1993.)
.PP
QIC-40 and QIC-80 floppy-tape
drives from Colorado, Archive, Mountain, and Summit.
.SH "Compatible Video Cards"
.PP
\*(CO supports virtually all EGA, VGA, and SVGA video cards
in both standard and Local Bus versions.
Exceptions
are noted in the incompatible list.
Please note that this following applies
.I only
to \*(CO itself:
it does
.I not
applies to the X Window System, which at present supports
a much narrow range of video cards.
.PP
Video cards by the following manufacturers
have been tested with \*(CO, and have been found to work:
.nf
.sp \n(pDu
ATI
BOCA
BTC 1505 Monochrome Graphic Printer Card (does \fInot\fR work with X)
Diamond Speedstar (does \fInot\fR work with X)
DTK Graphicsmith
Genoa
Oak Technologies (does \fInot\fR work with X)
Orchid
Paradise
SEFCO monochrome adapter
Trident
Video 7
Western Digital
.fi
.SH "Compatible Peripherals"
.PP
The following peripheral devices have been tested with \*(CO and have
been found to work.
.nf
.sp \n(pDu
.ta 3.5i
Cherry keyboards
DTK PEI-301 32-bit memory expansion
Honeywell keyboards
IBM monochrome printer card
IBM keyboards
Keytronic KB101 PLUS keyboard
Linksys Parallel Link HLP-100
Linksys Parallel Link HPL-100
MicroSwitch keyboards
NMB Technologies keyboards
Northgate Omnikey 102 keyboard
Syquest SCSI removable cartridge disks
.fi
.SH "Compatible BIOS ROMs"
.PP
The following BIOS ROMs have been tested with \*(CO,
and have been found to be compatible.
.nf \n(pDu
.sp 
.ta 3.5i
AMI 386, 486
DTK 386
Mylex 386, 486
OPTI-Modular
PHOENIX 386, 486
PHOENIX 386SX
.fi
.PP
.I
Please note that the AMI 386 BIOS does not present a problem during
installation.
.R
However, due to a bug in the BIOS, certain versions of the AMI 386 BIOS
fail to reset the system correctly when you reboot your system by typing
.BR "<ctrl><alt><del>" .
If you have this BIOS,
press the \fB<reset>\fR button to reboot your system.
.PP
Certain releases of the AMI BIOS fail to correctly
reset the keyboard controller until after the memory test has completed.
On these systems,
you will not be able to prematurely exit from the memory test by using the
.B <Esc>
key, but you may be able to exit by using the
.B "<Num Lock>"
key.
.PP
Certain releases of the AMI 486 BIOS incorrectly diagnose
external cache memory as being bad after rebooting \*(CO via
.B /etc/reboot
or a
.B <ctrl><alt><del>
key sequence.
If your system exhibits these symptoms, press the
reset button to reboot your system.
.PP
Versions of the Award BIOS have an option to load the BIOS into RAM
at start-up.
Enabling this option causes the \*(CO installation to abort at the point
where it attempts to generate a new system.
You should disable this option and restart your system before you attempt
to install \*(CO.
.SH "Incompatible Hardware"
.PP
The following hardware is known \fInot\fP to work with this
release of \*(CO.
.nf
.sp \n(pDu
All mice that use a bus port rather than a COM port
All 8088-, 8086-, and 80286-based computers
All 80386DX-based computers that have a CPU chip labeled ``16-bit only''
ALR Venture 386SX Laptop \(em boot disk fails to install keyboard driver
American Multi-Source model 1004 MFM/RLL
Chicony 101B IDE adapter
Chicony keyboards (cannot be used with \fBnkb\fR loadable keyboard driver)
Dataworld 386/33 (video incompatibility)
Dataworld Laptop 386 model NM325SX (disk incompatibility)
DTK 386 machines
Fujitsu 2612ET IDE hard disk
Gateway NOMAD 425DXL (486 notebook)
IBM MicroChannel PS/1 and PS/2 computers
JETkey v3.0 Fasttest Keyboard BIOS (on some inexpensive clone motherboards)
Laser computers; they use the VTEK BIOS, which has problems associated with it
Microsoft InPort Mouse
Miniscribe IDE interface
NEC G8BUT ESDI disk controller; uses proprietary, on-board BIOS
NEC VGB video card; requires configuration for eight-bit transfer mode
OMTI 8620 disk controller
Orchid Privilege 386SX-16 motherboard
Western Digital 1004-27X, 1004-WX1, 1002 series
Western Digital XTGEN, XTGEN+, XTGEN-2, XTGEN-R
VTEK BIOS
XT (i.e., all eight-bit) disk controllers
Zenith Z449 video card (older versions cause panics)
.fi
