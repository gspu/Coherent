eerrrrnnoo.hh -- Header File

Error numbers used by errno()
#iinncclluuddee <eerrrrnnoo.hh>

eerrrrnnoo.hh is  the header  file that defines  and describes the  error numbers
returned  in the  external variable  eerrrrnnoo. The  following lists  the error
numbers defined in eerrrrnnoo.hh:

EEPPEERRMM: Not superuser
     You  are  not the  superuser  rroooott, but  attempted  an operation  that
     requires rroooott privileges.

EENNOOEENNTT: No such file or directory
     A program could not find a required file or directory.

EESSRRCCHH: Process not found
     A program attempt to communicate with a process that did not exist.

EEIINNTTRR: Interrupted system call
     A COHERENT  system call failed  due to a  signal being received  or an
     alarm expiring.

EEIIOO: I/O error
     A physical  I/O error occurred  on a device  driver.  This could  be a
     tape error, a CRC error on a disk, or a framing error on a synchronous
     HDLC link.

EENNXXIIOO: no such device or address
     A specified minor device is invalid  or the unit is powered off.  This
     error might also indicate that a  block number given to a minor device
     is  out of  range.   If you  attempt  to open  a pipe  write only,  if
     OO_NNDDEELLAAYY is set,  and if there are currently no  readers on this pipe,
     ooppeenn() returns immediately and sets eerrrrnnoo to EENNXXIIOO.

EE22BBIIGG: argument list too long
     The number of bytes of arguments passed in an eexxeecc is too large.

EENNOOEEXXEECC: exec format error
     The file  given to eexxeecc or  llooaadd is not a  valid load module (probably
     because  it does  not have  the magic number  at the  beginning), even
     though its mode indicates that it is executable.

EEBBAADDFF: bad file descriptor
     A  file  descriptor  passed  to  a  system call  is  not  open  or  is
     inappropriate to the call.  For example, a file descriptor opened only
     for reading may not be accessed for writing.

EECCHHIILLDD: no children
     A process issued a wwaaiitt() call when it had no outstanding children.

EEAAGGAAIINN: no more processes
     The system cannot create any  more processes, either because it is out
     of table space or because the invoking process has reached its process
     quota.

EENNOOMMEEMM: not enough memory
     The system  cannot accomodate the memory size  requested (by eexxeecc() or
     bbrrkk(), for example).

EEAACCCCEESS: permission denied
     The user is denied access to a file.

EEFFAAUULLTT: bad address
     An  address in  a  system call  does  not lie  in  the address  space.
     Normally,  this  generates  a  SSIIGGSSYYSS  signal,  which  terminates  the
     process.

EENNOOTTBBLLKK: block device required
     The calls mmoouunntt() and uummoouunntt() require block devices as arguments.

EEBBUUSSYY: mount device busy
     The special file passed to the call mmoouunntt() is already mounted, or the
     file  system given  to  the call  uummoouunntt()  has open  files or  active
     working directories.

EEEEXXIISSTT: file exists
     An attempt was made to lliinnkk to a file that already exists.

EEXXDDEEVV: cross-device link
     A link to a file must be on the same logical device as the file.

EENNOODDEEVV: no such device
     An unsuitable I/O call was made  to a device; e.g., an attempt to read
     a line printer.

EENNOOTTDDIIRR: not a directory
     A  component in  a path  name  exists but  is not  a directory,  or an
     argument to the commands cchhddiirr or cchhrroooott is not a directory.

EEIISSDDIIRR: is a directory
     Directories cannot be opened for writing.

EEIINNVVAALL: invalid argument
     An  argument to  a system  call is  out of range,  e.g., a  bad signal
     number to kkiillll or uummoouunntt of a device that is not mounted.

EENNFFIILLEE: file table overflow
     A table  inside the COHERENT  system has run out  of space, preventing
     further ooppeenn() calls and related requests.

EEMMFFIILLEE: too many open files
     A process is limited to 20 open files at any time.

EENNOOTTTTYY: not a tty
     An iiooccttll() call was made to a file which is not a terminal device.

EETTXXTTBBSSYY: text file busy
     The text segment of a shared load module is unwritable.  Therefore, an
     attempt to execute it while it  is being written or an attempt to open
     it for writing while it is being executed will fail.

EEFFBBIIGG: file too large
     The block mapping algorithm for files fails above 1,082,201,088 bytes.

EENNOOSSPPCC: no space left on device
     Indicates an attempt to write on  a file when no free blocks remain on
     the associated device.  This error  may also indicate that a device is
     out of i-nodes, so a file cannot be created.

EESSPPIIPPEE: illegal seek
     It is illegal to llsseeeekk() on a pipe.

EERROOFFSS: read-only file system
     Indicates  an attempt  to  write on  a file  system mounted  read-only
     (e.g., with ccrreeaatt() or uunnlliinnkk()).

EEMMLLIINNKK: too many links
     A new  link to a file  cannot be created because  the link count would
     exceed 32,767.

EEPPIIPPEE: broken pipe
     A  write occurred  on a  pipe for  which there  are no  readers.  This
     condition is accompanied by the signal SSIIGGPPIIPPEE, so the error will only
     be seen if the signal is ignored or caught.

EEDDOOMM: mathematics library domain error
     An argument to a mathematical  routine falls outside the domain of the
     function.

EERRAANNGGEE: mathematics library result too large
     The result of a mathematical function is too large to be represented.

EEKKSSPPAACCEE: out of kernel space
     No  more space  is available  for tables  inside the  COHERENT system.
     Table space  is dynamically allocated from a fixed  area of memory; it
     may be possible to increase the  size of the area by reconfiguring the
     system.

EENNOOLLOOAADD: driver not loaded
     Not used.

EEBBAADDFFMMTT: bad exec format
     An attempt was made to eexxeecc() a file on the wrong type of processor.

EEDDAATTTTNN: device needs attention
     The device being  referenced needs operator attention.  For example, a
     line printer might need paper.

EEDDBBUUSSYY: device busy
     The indicated device is busy.  For llooaadd(), this implies that the given
     major device number is already in use.

_S_e_e _A_l_s_o
eerrrrnnoo, hheeaaddeerr ffiilleess, ppeerrrroorr(), ssiiggnnaall()
ANSI Standard, section 7.1.3
POSIX Standard, section 2.4
