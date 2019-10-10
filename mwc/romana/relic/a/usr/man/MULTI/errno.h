

errno.h                    Header File                    errno.h




Error numbers used by errno()

#include <errno.h>

errno.h is a header  that defines and describes the error numbers
returned in the external variable errno.  The following lists the
the error numbers defined in errno.h:

EEIIOO: I/O error
     A  physical I/O  error occurred  on  a device  driver.  This
     could be a  tape error, a CRC error on  a disk, or a framing
     error on a synchronous HDLC link.

EENNXXIIOO: no such device or address
     A specified  minor device is invalid or  the unit is powered
     off.   This error  might also indicate  that a  block number
     given to  a minor  device is  out of range.   suload returns
     this error code if the driver was not loaded.

EE22BBIIGG: argument list too long
     The number  of bytes of  arguments passed in an  eexxeecc is too
     large.

EENNOOEEXXEECC: exec format error
     The file  given to exec or  load is not a  valid load module
     (probably because  it does not have the  magic number at the
     beginning), even  though its mode  indicates that it  is ex-
     ecutable.

EEBBAADDFF: bad file descriptor
     A file descriptor passed to a  system call is not open or is
     inappropriate to  the call.  For example,  a file descriptor
     opened only for reading may not be accessed for writing.

EECCHHIILLDD: no children
     A  process issued  a wait  call when  it had  no outstanding
     children.

EEAAGGAAIINN: no more processes
     The system cannot  create any more processes, either because
     it is out of table space or because the invoking process has
     reached its process quota.

EENNOOMMEEMM: not enough memory
     The system  cannot accomodate the memory  size requested (by
     exec or brk, for example).

EEAACCCCEESS: permission denied
     The user is denied access to a file.

EEFFAAUULLTT: bad address
     An  address in  a system  call does not  lie in  the address
     space.  Normally, this generates a SIGSYS signal, which ter-
     minates the process.


COHERENT Lexicon                                           Page 1




errno.h                    Header File                    errno.h




EENNOOTTBBLLKK: block device required
     The  mount and  umount calls  require  block devices  as ar-
     guments.

EEBBUUSSYY: mount device busy
     The special file passed  to mount is already mounted, or the
     file system given to umount has open files or active working
     directories.

EEEEXXIISSTT: file exists
     An attempt was made to link to a file that already exists.

EEXXDDEEVV: cross-device link
     A link to  a file must be on the  same logical device as the
     file.

EENNOODDEEVV: no such device
     An unsuitable I/O call was made to a device; for example, an
     attempts to read a line printer.

EENNOOTTDDIIRR: not a directory
     A component in a path name exists but is not a directory, or
     a chdir or chroot argument is not a directory.

EEIISSDDIIRR: is a directory
     Directories cannot be opened for writing.

EEIINNVVAALL: invalid argument
     An argument  to a system call  is out of range,  e.g., a bad
     signal  number to  kill or  umount of a  device that  is not
     mounted.

EENNFFIILLEE: file table overflow
     A table  inside the  COHERENT system  has run out  of space,
     preventing further open calls and related requests.

EEMMFFIILLEE: too many open files
     A process is limited to  20 open files at any time.

EENNOOTTTTYY: not a tty
     An ioctl  call was made  to a file  which is not  a terminal
     device.

EETTXXTTBBSSYY: text file busy
     The  text segment  of a  shared  load module  is unwritable.
     Therefore, an attempt to  execute it while it is being writ-
     ten or an  attempt to open it for writing  while it is being
     executed will fail.

EEFFBBIIGG: file too large
     The   block  mapping   algorithm  for   files   fails  above
     1,082,201,088 bytes.




COHERENT Lexicon                                           Page 2




errno.h                    Header File                    errno.h



EENNOOSSPPCC: no space left on device
     Indicates an attempt to write  on a file when no free blocks
     remain on  the associated device.   This error may  also in-
     dicate that a device is out  of i-nodes, so a file cannot be
     created.

EESSPPIIPPEE: illegal seek
     It is illegal to lseek on a pipe.

EERROOFFSS: read-only file system
     Indicates an attempt to write on a file system mounted read-
     only (e.g., with creat or unlink).

EEMMLLIINNKK: too many links
     A new  link to  a file cannot  be created, because  the link
     count would exceed 32,767.

EEPPIIPPEE: broken pipe
     A write  occurred on a pipe for which  there are no readers.
     This condition is  accompanied by the signal SIGPIPE, so the
     error will only be seen if the signal is ignored or caught.

EEDDOOMM: mathematics library domain error
     An  argument to  a  mathematical routine  falls outside  the
     domain of the function.

EERRAANNGGEE: mathematics library result too large
     The result  of a  mathematical function  is too large  to be
     represented.

EEKKSSPPAACCEE: out of kernel space
     No more  space is available  for tables inside  the COHERENT
     system.  Table  space is dynamically allocated  from a fixed
     area of  memory; it may be possible to  increase the size of
     the area by reconfiguring the system.

EENNOOLLOOAADD: driver not loaded
     Not used.

EEBBAADDFFMMTT: bad exec format
     An attempt  was made  to eexxeecc  a file on  the wrong  type of
     processor.

EEDDAATTTTNN: device needs attention
     The device  being referenced needs  operator attention.  For
     example, a line printer might need paper.

EEDDBBUUSSYY: device busy
     The indicated  device is busy.  For  load, this implies that
     the given major device number is already in use.

***** See Also *****

errno, header files, perror(), signal()



COHERENT Lexicon                                           Page 3


