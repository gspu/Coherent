
                        Running Third-Party Software


COHERENT runs many packages from third-party vendors that were designed for
use with  other operating systems,  in particular SCO  UNIX.  COHERENT runs
these packages,  but some require some minor  reconfiguration of either the
package, of COHERENT, or both, to run smoothly.

The following  sections describe  some of  the packages that  Mark Williams
Company  has tested,  and gives  directions on how  to perform  these minor
reconfigurations.   Note that  we  continually test  new  packages, so  the
following list probably is  incomplete.  For the latest information on what
packages  run under  COHERENT,  and how  you  can run  them, telephone  the
company  from which  you  purchased the  software, check  the MWC  bulletin
board, or telephone Mark Williams Company.

                             GGeenneerraall DDiirreeccttiioonnss

To  install  third-party  UNIX software  onto  COHERENT,  be  aware of  the
following general rules:

11. COHERENT does  not support  the command  ccuussttoomm. To install,  you should
   read the product's manual and determine what must be done to install the
   product.  Some  products come with a shell script  that performs all the
   tasks of  installation; you can use  the Bourne shell sshh  to invoke that
   script.   Other  products  use  ttaarr  or  ccppiioo; look  in  your  package's
   installation or  release notes for instructions.   Please note that when
   unttaarrring or ccppiiooing floppy disks from other UNIX systems, you sometimes
   must use  the complex UNIX device  names that are now  supported in this
   release.  For  information on floppy-disk device  names, see the Lexicon
   entries for ffdd and ffllooppppyy ddiisskk.

22. Some third-party packages  support many different operating systems, and
   when you  install the package let  you pick the operating  system from a
   menu.  If this is the case, you should pick COHERENT first, if it is one
   of the  options.  If COHERENT  is not an  option, pick SCO  UNIX; if SCO
   UNIX is not  available, then Interactive UNIX; and if  none of those are
   available, then  USL UNIX System  V release 3.2.   If none of  those are
   available, then select generic UNIX system V release 2.

33. Some packages offer  a variety of console devices.  If  you have a color
   console,  select ccoolloorrccoonnss.  If  this is  not available,  try aatt338866.  If
   neither is  an option,  or if  you have a  monochrome console,  then you
   should select mmoonnoocchhrroommee; in most cases, this works correctly, even with
   color devices.

44. If  possible,  install  all  available  terminal types.   Some  packages
   require that you reinstall the entire package should you need a terminal
   type that you have not yet installed.

55. The SCO impementation of the  link command llnn will overwrite an existing
   file by default.  This behavior does not comply with the POSIX Standard.
   Under COHERENT, you  must use the option -ff to  force llnn to overwrite an
   existing file.   Some installation scripts, however,  depending upon the
   non-standard  behavior  of llnn.  If  this problem  arises  while you  are
   installing a  package, you must edit the  installation script and insert
   the flag -ff where appropriate.

66. COHERENT  does not  support the  bbrraanndd utility  from SCO.   This utility
   ``brands'' (or serializes) executables using a serial number and license
   activation key.  Any package that requires bbrraanndd for proper installation
   and execution will not work under COHERENT.

                            TThhiirrdd-PPaarrttyy PPaacckkaaggeess

The following  gives information on  using some of the  more popular third-
party UNIX packages under COHERENT.

AAiiddee ddee CCaammppee

This package installs and runs correctly.

BBBBXX PPrrooggrreessssiioonn/44

The installation  of this package  requires that you use  the command ccppiioo.
Because the installation program  does not prompt for the two supplementary
disks, you must re-issue the command

     cpio -icB

for each disk in the distribution.

The newest  version from BASIS runs  BASIS International's regression suite
without error.

BBllaasstt

The package works correctly.

ccccsshh

As  of this  writing, the  shell-script  compiler ccccsshh  does not  run under
COHERENT  4.2.   That  is  due  to  the  fact  that  its  internal  library
/uussrr/lliibb/lliibbccccsshh.aa is  not in  the COFF format.   It does run,  however, on
release 3.2 of COHERENT.

For details, please contact the publisher of ccccsshh.

CCooddeebbaassee

The Kernighan-and-Ritchie source compiles.

DDaattaafflleexx

This package installs and runs correctly.

ddBBAASSEE IIIIII

The package works correctly.

ddBBAASSEE IIVV

To install dBASE IV under COHERENT, do the following:

11. Type the command

        cd /


   to change to the root directory.

22. Insert System Disk 1 into a floppy-disk drive.

33. Type the command

        tar -xvf /dev/rfva_X ./tmp/install

   where _X  is `0'  or `1',  depending upon whether  you have  inserted the
   system disk into drive 0 (i.e., A:) or 1 (i.e., B:).

44. When the file is extracted, issue the following command:

        cp ./tmp/install ./tmp/coh-install


   Then,  use the  editor of  your choice  to edit  file ./ttmmpp/ccoohh-iinnssttaallll.
   Change line 33 from:

        label=`dd if=$disk count=1 | dd bs=100 count=1 2>/dev/null | tr`


   to

        label=`dd if=$disk count=1 | dd bs=100 count=1 2>/dev/null`


   Save the edited file.

55. _M_a_k_e _s_u_r_e _y_o_u _a_r_e _i_n _t_h_e  _r_o_o_t _d_i_r_e_c_t_o_r_y!  This is crucial at this stage
   to installing correctly.

66. Execute the new install script by typing:

        /bin/sh ./tmp/coh-install /dev/rfva_X


   Once again, substitute `0' or `1' for _X depending upon which floppy-disk
   drive you have inserted the system disk.

77. Continue  doing   the  installation   normally,  as  described   in  the
   documentation that comes with the dBASE-IV package.

88. When  you  have concluded  installing  dBASE IV,  you  must execute  the
   following final commands:

        cd /usr/local/lib/dbaseiv
        rm dbase1.res
        ln ./dbase1.res.us ./dbase1.res
        rm /tmp/coh-install


You should now have a working implementation of dBASE IV!

Some users  have encountered problems with  this package's license manager.
They report that the first time  they added a license to the data base, the
license  manager corrupted  the entry, thus  prompting some  surly behavior
from dBASE when they attempted to use the package.

To correct  this problem,  use the  dBASE utility eeddiittlliicc  to add  a second
license  entry.  It  appears that  dBASE stores  and processes  this second
entry correctly.  After you  confirm that the new entry functions properly,
re-invoke eeddiittlliicc  and remove the original entry.   After that, all appears
to work as it should.

You may  experience some problems  with console output and  the handling of
function  keys.  If  this is  the  case, you  need a  version of  the dBASE
terminal-description file  that is corrected for  COHERENT.  You can obtain
this  file from  site  rraavveenn on  the  Intenet, or  from  the Mark  Williams
bulletin board.   If all else  fails, send mail to  ssuuppppoorrtt@mmwwcc.ccoomm, and we
will mail a copy to you.

Some users of dBASE IV have reported that the package, for whatever reason,
does not  recognize directory or  file names that  contain capital letters.
If dBASE IV has trouble saving  files, check the name of the directory, and
make sure it is all in lower-case letters.

FFiilleePPrroo

The package works correctly.

FFlleexxiilliinntt

The tool FFlleexxiilliinntt compiles and runs under COHERENT release 4.2.

To install  the package, you must  make one small change:  edit header file
ccuussttoomm.hh and change the prototype for vvooiidd to iinntt.

LLoottuuss 11-22-33

To install Lotus 1-2-3, do the following:

11. Install Lotus  by hand with ttaarr  (not with ccuussttoomm), as  described in the
   Lotus documentation.

22. Before you run the command sseettuupp112233, type the following command:

        echo -n ' 33[?8l'

   The first command set erase mode on the console.

The sample  files as stored in  /uu/lloottuuss/112233.vv1111/ssmmppffiilleess/mmiisscc will fail if
the  current working  directory is  not  /uu/lloottuuss/112233.vv1111/ssmmppffiilleess/mmiisscc. To
enter this directory from within Lotus, use the following command:

     /WGDD/u/lotus/123.v11/smpfiles/misc

As  of this  writing, graphs  did not  work because  of limitations  of the
COHERENT console driver.  We are working to correct this problem.

mmcc -- MMaaccrrooCCAALLCC

MacroCALC installs  and runs correctly  under COHERENT 4.0  and 4.2.  Users
should have  no problems  installing this  package.  If you  are installing
MacroCALC  onto COHERENT  4.2,  do _n_o_t  copy  onto your  system the  aannssiippcc
termcap entry  that comes with the package: the  one included with COHERENT
4.2 already works correctly

RRMMCCoobbooll

RMCobol works  out of the  box, without modification.  You  may notice some
slight discrepancies with  regard to colors and the number  of lines on the
screen,  which  are  easily corrected  by  resetting  the COHERENT  console
device.

SSlliicckkEEddiitt

This editor  works properly.  However,  it comes with  an internal language
that it  uses for enhanced editing, macros, etc.,  that requires a compiler
named sstt. This compiler  is on the SlickEdit's distribution disks; however,
it  is in  Microsoft aa.oouutt  binary format, which  cannot be  executed under
COHERENT.

SlickEdit works  best with a customized  terminal-information file that has
been prepared by Mark Williams Company.  To obtain a copy, contact the Mark
Williams BBS as described earlier in these notes.

TTEERRMM CCoommmmuunniiccaattiioonnss PPaacckkaaggee

Because the  distribution disks are  not high density, you  must use device
/ddeevv/ffqqaa_X (where _X is 00 or 11) to load the distribution.

When you execute the iinnssttaallll script after you have loaded the distribution,
you must to choose the first option in the index:

     UNIX System V/386 (Including Altos, AT&T and Interactive)

Once installed, our tests show that the package works well.

WWoorrddPPeerrffeecctt aanndd WWoorrddPPeerrffeecctt OOffffiiccee

Before you  can print anything on  your COHERENT system, you  must start up
and configure the MLP print spooler.  Do this before you attempt to install
WordPerfect.  For details on MLP, see the Lexicon entry for pprriinntteerr.

When you load the initial boot disk, if you choose to un-ttaarr the disk using
the device /ddeevv/ffvvaa00, ttaarr complains:

     This doesn't look like a tar archive.
     Skipping to next file...


This   behavior  also   occurs  with   devices   /ddeevv/ffvvaa00,  /ddeevv/ddsskk/ff0033hh,
/ddeevv/ddsskk/ff00qq1188ddtt, and  /ddeevv/ffdd00113355ddss1188. Note that this  does not affect the
installation: it is just a curiosity.

When  you   perform  the  actual   installation,  in  some   instances  the
installation program exhibits strange behavior.  It will say:

     *Please Wait*    0% complete

It  then prints what  appears to  be a  process id on  top of  the fragment
*PPlleeaa, moves the cursor down another line, and prints:

     *Please Wait*   1% complete

The  11% stage  continues  for a  lengthy  period of  time; the  floppy-disk
drive's light glows, to show that the disk is being read, but otherwise the
installation program  displays nothing to indicate  that useful activity is
occurring.  Do not panic: the installation program is working; it simply is
not updating the screen properly.

If your console is monochrome, insert the command

     export WPTERM51=scocons

into file /eettcc/pprrooffiillee. If you console is color, insert the command:

     export WPTERM51=scoconscol

WordPerfect expects  this variable to  be set before it  can run correctly.
Note that  after you insert  it, you must  log out and log  in again before
that variable can become part of your environment.

If you  wish to run either  package under X Windows,  use the terminal type
VVTT110022 and insert the following instructions into the file $HHOOMMEE/.XXddeeffaauullttss:

     xterm*VT102.Translations: #override \
          <Key> F1 : string(0x1b) string("OP") \n \
          <Key> F2 : string(0x1b) string("OQ") \n \
          <Key> F3 : string(0x1b) string("OR") \n


Some systems have experienced  problems with printing under the llpp spooler,
when logged  in as anyone other  than the superuser.  This  should not be a
problem, but  if it is, try switching the  WordPerfect spooler command from
llpp to  hhpprr -BB or  llpprr -BB, respectively,  depending upon whether  you have a
laser  printer  or  a dot-matrix  printer.   If  all  else  fails, try  the
following workaround:


11. Set WordPerfect's spool command to:

        cat -u > /tmp/PRINT.ME


   When   WordPerfect  prints,   it  will   dump   its  output   into  file
   /ttmmpp/PPRRIINNTT.MMEE, instead of sending it directly to the spooler.

22. To print WordPerfect's output, open another virtual console and type the
   command:

        cat /tmp/PRINT.ME > /dev/lp

   With  WordPerfect  Office,  you  cannot  receive  mail from  within  the
   application because it expects either  SCO's MMDF, or a version of ssmmaaiill
   that COHERENT does not yet support.
