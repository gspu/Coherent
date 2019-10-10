ssttttyy -- Command

Set/print terminal modes
ssttttyy
ssttttyy -aa
ssttttyy -gg
ssttttyy _x:_x: ... :_x
ssttttyy _a_r_g_l_i_s_t ...

The command  ssttttyy lets you change  or display the settings  of the standard
input device.  The device is  usually a terminal, although tapes, disks and
other special files may be applicable.

_D_e_f_a_u_l_t _S_e_t_t_i_n_g_s
The following describes how COHERENT  sets up a terminal device by default.
This normal processing is often  called ``cooked'' mode.  Note that on some
machines, the default characters differ from those given below.

The  _e_r_a_s_e and  _k_i_l_l  characters (normally  <ccrrttll-HH>  and <ccttrrll-UU>)  erase,
respectively, one typed character and an entire line of typing.

The _s_t_o_p-_o_u_t_p_u_t  and _s_t_a_r_t-_o_u_t_p_u_t characters (normally  <ccttrrll-SS> and <ccttrrll-
QQ>) respectively stop and restart output.

The _i_n_t_e_r_r_u_p_t character  (normally <ccttrrll-CC>) sends the signal SSIIGGIINNTT, which
usually terminates program execution.

The  _q_u_i_t character  (normally <ccttrrll-\>)  sends  the signal  SSIIGGQQUUIITT, which
usually terminates program execution with a core dump.

The  _e_n_d _o_f  _f_i_l_e character  (normally  <ccttrrll-DD>) generates  an end-of-file
signal from the terminal.

You can change the setting of  each special character by invoking ssttttyy with
the appropriate option.

_O_p_t_i_o_n_s
When called  without any arguments, ssttttyy gives a  brief listing of settings
for the standard-input device.

ssttttyy can read the settings  of devices other than the standard-input device
by redirecting that device to it.  For example, the command

    stty < /dev/com1l

prints a brief summary of the settings for serial device ccoomm11ll.

ssttttyy's command-line  arguments can take a number, as  indicated below by _n;
or they  can take a character,  as indicated below by _c.  Argument _c can be
one of the following:

-> A single character.
-> A  caret `^'  followed  by a  single  character (to  indicate a  control
   character, e.g., ^XX for <ccttrrll-XX>).
-> An ^?, which denotes the <ddeell> character.
-> An `0x' followed by two hexadecimal digits.
-> An ^-, which indicates that that option is not used.

ssttttyy recognizes the following command-line arguments:

-aa      Give a complete listing of settings for the standard-input device.
-gg      Give a complete list of settings for the standard-input device, but
        in hexadecimal.  This is  a dump of the  tteerrmmiioo structure in effect
        at the moment.  For  more information on the  tteerrmmiioo structure, see
        the Lexicon entry for tteerrmmiioo.
_x:_x:...:_x
        Establish new settings for the standard-input device.  The settings
        are hexadecimal values that are separated by colons.  This form can
        be combined with -gg option to copy ssttttyy settings from one device to
        another.  For example, to set device ccoomm22ll so that it mimics device
        ccoomm11ll, use the following command:

            stty `stty -g < /dev/com1l` < /dev/com2l

00       Hang up the telephone.
5500      Set line speed to 50 bps.
7755      Set line speed to 75 bps.
111100     Set line speed to 110 bps.
113344     Set line speed to 110 bps.
115500     Set line speed to 150 bps.
220000     Set line speed to 200 bps.
330000     Set line speed to 300 bps.
660000     Set line speed to 600 bps.
11220000    Set line speed to 1200 bps.
11880000    Set line speed to 1800 bps.
22440000    Set line speed to 2400 bps.
44880000    Set line speed to 4800 bps.
99660000    Set line speed to 9600 bps.
1199220000   Set line speed to 19200 bps.
3388440000   Set line speed to 38400 bps.
bbrrkkiinntt  Send interrupt on break.
-bbrrkkiinntt
        Do not send interrupt on break.
bbss00     No delay on backspace.
bbss11     Delay briefly on backspace.
ccllooccaall  Turn on modem control.
-ccllooccaall
        Turn off modem control.
ccooookkeedd  Set the device  into cooked mode.   This is a  composite of options
        ppaarreennbb, -ppaarroodddd, ccss77,  bbrrkkiinntt, iiggnnppaarr, iissttrriipp,  iiccrrnnll, iixxoonn, ooppoosstt,
        oonnllccrr, iissiigg, and iiccaannoonn.
ccrr00     No delay on carriage returns.
ccrr11     Carriage-return delay depends upon column position.
ccrr22     Delay approximately 0.10 seconds on carriage return.
ccrr33     Delay appoximately 0.15 seconds on carriage return.
ccrreeaadd   Enable the receiver.
-ccrreeaadd  Disable the receiver.
ccss55     Character size is five bits.
ccss66     Character size is six bits.
ccss77     Character size is seven bits.
ccss88     Character size is eight bits.
ccssttooppbb  Use two stop bits per character.
-ccssttooppbb
        Use one stop bit per character.
eecchhoo    Echo every character.
-eecchhoo   Do not echo characters.
eecchhooee   Echo the erase character as backspace-space-backspace.
-eecchhooee  Do not echo the erase character as backspace-space-backspace.
eecchhookk   Echo newline after the kill character.
-eecchhookk  Do not echo newline after the kill character.
eecchhoonnll  Echo newline.
-eecchhoonnll
        Do not echo newline.
eekk      Set the  kill  and  erase characters  to  printable characters.   A
        composite of eerraassee '#' and kkiillll '@'.
eeooff _c   Set the end-of-file character to _c.
eeooll _c   Set the end-of-line character to _c.
eerraassee _c
        Set the erase character to _c.
eevveennpp   Set the port  to even parity.   This is a composite  of the options
        ppaarreennbb, -ppaarroodddd, and ccss77.
-eevveennpp  Turn off  even parity  -- in  effect,  turn off  parity altogether.
        This is a composite of the options -ppaarreennbb and ccss88.
ffff00     No delay on formfeeds.
ffff11     Delay approximately two seconds on formfeeds.
hhuupp     Hang up the telephone on logging out.
-hhuupp    Do not hang up the telephone on logging out.
hhuuppccll   Same as hhuupp.
-hhuuppccll  Same as -hhuupp.
iiccaannoonn  Enable canonical input.
-iiccaannoonn
        Disable canonical input.
iiccrrnnll   Map carriage-return to newline on input.
-iiccrrnnll  Do not map carriage-return to newline on input.
iiggnnbbrrkk  Ignore break on input.
-iiggnnbbrrkk
        Do not ignore break on input.
iiggnnccrr   Ignore carriage return on input.
-iiggnnccrr  Do not ignore carriage return on input.
iiggnnppaarr  Ignore parity errors on input.
-iiggnnppaarr
        Do not ignore parity errors on input.
iinnllccrr   Map newline to carriage return on input.
-iinnllccrr  Do not map newline to carriage return on input.
iinnppcckk   Enable parity checking on input.
-iinnppcckk  Do not enable parity checking on input.
iinnttrr _c  Set the interrupt character to _c.
iissiigg    Check input against interrupt and quit characters.
-iissiigg   Do not check input against interrupt and quit characters.
iiuuccllcc   Map input's upper-case characters to lower case.
-iiuuccllcc  Do not map input's upper-case characters to lower case.
iissttrriipp  Strip input to seven bits.
-iissttrriipp
        Do not strip input to seven bits.
iixxaannyy   Allow any on input character to restart output.
-iixxaannyy  Do not allow any input character to restart output.
iixxooffff   Request that system  send start or  stop characters when  the input
        queue is, respectively, nearly full or nearly empty.
-iixxooffff  Do not request that system send  start or stop characters to manage
        input queue.
iixxoonn    Use start/stop characters to control output queue.
-iixxoonn   Do not use start/stop characters to control output queue
kkiillll _c  Set the kill character to _c.
llccaassee   Map upper-case characters  to lower  case.  A composite  of options
        xxccaassee, iiuuccllcc, and oollccuucc.
-llccaassee  Turn  off  mapping  of  upper-case  character  to  lower  case.   A
        composite of options -xxccaassee, -iiuuccllcc, and -oollccuucc.
LLCCAASSEE   A synonym for llccaassee.
-LLCCAASSEE  A synonym for -llccaassee.
mmiinn _n   Set the constant VVMMIINN to decimal  value _n. For more about VVMMIINN, see
        the Lexicon entry for tteerrmmiioo.
nnll      A composite of options -iiccrrnnll and -oonnllccrr.
-nnll     A composite of options iiccrrnnll, -iinnllccrr,  -iiggnnccrr, oonnllccrr, -ooccrrnnll, and -
        oonnllrreett.
nnll00     No delay on newline.
nnll11     Delay approximately 0.10 seconds on newline.
nnooffllsshh  Flush buffer on interrupt or quit.
-nnooffllsshh
        Do not flush buffer on interrupt or quit.
ooccrrnnll   In output, map carriage return to newline.
-ooccrrnnll  In output, do not map carriage return to newline.
ooddddpp    Set device  to  odd parity.   This  option is  a  composite of  the
        options ppaarreennbb, ppaarroodddd, and ccss77.
-ooddddpp   Turn off odd parity -- in effect, turn off parity altogether.  This
        is a composite of the options -ppaarreennbb and ccss88.
ooffddeell   Use delete characters as fill characters.
-ooffddeell  Do not use delete characters as fill characters.
ooffiillll   Use fill characters for delays.
-ooffiillll  Do not use fill characters for delays.
oollccuucc   Map lower-case characters to upper case on output.
-oollccuucc  Do not map lower-case characters to upper case on output.
oonnllccrr   Map newline to carriage return/newline on output.
-oonnllccrr  Do not map newline to carriage return/newline on output.
oonnllrreett  A newline character executes a carriage return.
-oonnllrreett
        A newline character does not execute a carriage return.
oonnooccrr   Do not output carriage returns at column 0.
-oonnooccrr  Output carriage returns at column 0.
ooppoosstt   Post-process output.
-ooppoosstt  Do not post-process output.
ppaarreennbb  Enable parity generation and detection.
-ppaarreennbb
        Disable parity generation and detection.
ppaarriittyy  Synonym for option eevveennpp.
-ppaarriittyy
        Synonym for option -eevveennpp.
ppaarrmmrrkk  Mark parity errors.
-ppaarrmmrrkk
        Do not mark parity errors.
ppaarroodddd  Odd parity.
-ppaarroodddd
        Turn off odd parity; i.e., use even parity.
qquuiitt _c  Set the quit character to _c.
rraaww     Set the device into raw mode.  This is a composite of the options -
        ppaarreennbb, -ppaarroodddd,  -hhuuppccll, ccss88,  -ooppoosstt, -oollccuucc,  -ooccrrnnll,  -oonnooccrr, -
        oonnllrreett, -ooffiillll,  -ooffddeell, nnll00,  ccrr00, ttaabb00, bbss00,  vvtt00, and  ffff00. This
        turns off most character processing, including all input processing
        (see cc_iiffllaagg  fields in <tteerrmmiioo.hh>),  canonical input  buffering (-
        iiccaannoonn), and  output  processing (-ooppoosstt).   It does  not turn  off
        echo.
-rraaww    Turn off raw mode -- in  effect, restore the device to cooked mode.
        Same as ccooookkeedd.
ssaannee    Restore the device to ``sanity'' -- for example, after an editor or
        communications program has died unexpectedly.   This is a composite
        of options iiccrrnnll, ooppoosstt, oonnllccrr, iissiigg,  iiccaannoonn, -xxccaassee, eecchhoo, eecchhooee,
        eecchhookk, and eerraassee ^hh.
ttaabb00    No delay for horizontal-tab character.
ttaabb11    Delay for horizontal-tab character depends on column position.
ttaabb22    Delay approximately 0.10 seconds on horizontal tab.
ttaabb33    Expand horizontal-tab characters into spaces.
ttaabbss    A synonym for ttaabb00.
-ttaabbss   A synonym for ttaabb33.
ttiimmee _n  Set the constant  VVTTIIMMEE to decimal  value _n. For  more about VVTTIIMMEE,
        see the Lexicon entry for tteerrmmiioo.
vvtt00     No delay on vertical-tab characters.
vvtt11     Delay approximately two seconds on vertical-tab characters.
xxccaassee   Canonical presentation of upper-case and lower-case characters.
-xxccaassee  Do not process upper-case and lower-case characters.

_S_e_e _A_l_s_o
AASSCCIIII, ccoommmmaannddss, ggeettttyy, iinniitt, iiooccttll(), ssiiggnnaall()

_N_o_t_e_s
Executing ssttttyy with input redirected  from another device does not have the
desired  effect unless  the device  being read is  already open.   The last
close  of  any terminal  device  resets  all tteerrmmiioo  values  to the  system
defaults.  To change the settings of  a device, have the device open first.
For example

    enable com1l

or

    sleep 32000 > /dev/com1l &

might precede:

    stty evenp < /dev/com1l

Note, too, that ssttttyy does not check its arguments for consistency.

ssttttyy  provides  complete access  to  the  System-V-style tteerrmmiioo  structure.
Note, however,  that the settings  of tteerrmmiioo are processed  by the kernel's
in-line  discipline and  device-driver  modules.  Under  COHERENT, none  of
these modules pays  attention to delay settings.  Therefore, setting delays
with ssttttyy does not, at present, affect the behavior of the terminal device.
