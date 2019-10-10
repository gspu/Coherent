.TH errno.h "" "" "Header File"
.PC "Error numbers used by errno()"
.B "#include <errno.h>"
.PP
.B errno.h
is the header file that defines and describes the error numbers
returned in the external variable
.BR errno .
The following lists the error numbers defined in
.BR errno.h :
.IP "\fBEPERM\fR: Permission denied"
.II EPERM
You lack permission to perform the operation you have requested.
.IP "\fBENOENT\fR: No such file or directory"
.II ENOENT
A program could not find a required file or directory.
.IP "\fBESRCH\fR: No such process"
.II ESRCH
You are attempting to communicate with a process that does not exist.
.IP "\fBEINTR\fR: Interrupted system call"
.II EINTR
A \*(CO system call failed because it received a signal or an alarm expired.
.IP "\fBEIO\fR: I/O error"
.II EIO
A physical I/O error occurred on a device driver.
This could be a tape error, a CRC error on a disk,
or a framing error on a synchronous HDLC link.
.IP "\fBENXIO\fR: No such device or address"
.II ENXIO
You attempted to access a device that does not exist.
It may be that a specified minor device is invalid, or the unit is powered off.
This error can also indicate that a block number given to a minor device
is out of range.
If you attempt to open a pipe in write-only mode, if
.B O_NDELAY
is set, and if there are currently no readers on this pipe,
.B open()
returns immediately and sets
.B errno
to
.BR ENXIO .
.IP "\fBE2BIG\fR: Argument list too long"
.II E2BIG
The number of bytes of arguments passed in an \fBexec\fR is too large.
.IP "\fBENOEXEC\fR: exec() format error"
.II ENOEXEC
The file given to
.B exec
is not a valid executable module (probably because it does not have the
magic number at the beginning),
even though its mode indicates that it is executable.
.IP "\fBEBADF\fR: Bad file descriptor"
.II EBADF
You passed a file descriptor to a system call for a file that was not open
or was opened in a manner inappropriate to the call.
For example, a file descriptor opened only for reading may not
be accessed for writing.
.IP "\fBECHILD\fR: No child processes"
.II ECHILD
A process issued a
.B wait()
call when it had no outstanding children.
.IP "\fBEAGAIN\fR: No more processes"
.II EAGAIN
The system cannot create any more processes,
either because it is out of table space
or because the invoking process has reached its quota or processes.
.IP "\fBENOMEM\fR: not enough memory"
.II ENOMEM
The system does not have enough memory available to map a process into memory.
This occurs in response to a the system calls
.B exec()
or
.BR brk() .
.IP "\fBEACCES\fR: Permission denied"
.II EACCES
You do not have permission to perform the requested operation upon a
given file.
.IP "\fBEFAULT\fR: Bad address"
.II EFAULT
You requested an address that does not lie within the address space.
Normally, this generates signal
.BR SIGSYS ,
which terminates the process.
.IP "\fBENOTBLK\fR: Block device required"
.II ENOTBLK
You passed to system calls
.B mount()
and
.B umount()
the descriptor of file that is not a block-special device.
.IP "\fBEBUSY\fR: Mount device busy"
.II EBUSY
You passed to the system call
.B mount()
the file descriptor of a device that is already mounted;
or you passed to the system call
.B umount()
the descriptor of a device that has open files or active working directories.
.IP "\fBEEXIST\fR: File exists"
.II EEXIST
An attempt was made to
.B link
to a file that already exists.
.IP "\fBEXDEV\fR: Cross-device link"
.II EXDEV
You attempted to link a file on one file system with a file on another.
This is not permitted.
.IP "\fBENODEV\fR: No such device"
.II ENODEV
You attempted to manipulate a device that does not exist.
.IP "\fBENOTDIR\fR: Not a directory"
.II ENOTDIR
You attempted to perform a directory operation upon a file that is not
a directory.
For example, you passed the file descriptor of a character-special
device to system calls
.B chdir()
or
.BR chroot() .
.IP "\fBEISDIR\fR: Is a directory"
.II EISDIR
You attempted to perform an inappropriate operation upon a directory.
For example, you passed the file descriptor of a directory to
.BR write() .
.IP "\fBEINVAL\fR: Invalid argument"
.II EINVAL
An argument to a system call is out of range.
For example, you passed to
.B kill()
or
.B umount()
the file descriptor of a device that is not mounted.
.IP "\fBENFILE\fR: File table overflow"
.II ENFILE
The \*(CO kernel uses a static table to record which files are open.
This error indicates that this table is full.
Until a file is closed, thus freeing space on this table, no more files
can be opened on your system.
.IP "\fBEMFILE\fR: Too many open files"
.II EMFILE
The \*(CO kernel limits the number of files that any one process can have
open at any given time; this error indicates that you have exceeded this
number.
The system call
.B sysconf()
returns the number of files that a process can open
(among other items of information).
For details, see its entry in the Lexicon.
.IP "\fBENOTTY\fR: Not a teletypewriter (tty)"
.II ENOTTY
You attempted to perform a terminal-specific operation upon a device
which is not a terminal.
.IP "\fBETXTBSY\fR: Text file busy"
.II ETXTBSY
The text segment of a shared load module is unwritable.
Therefore, an attempt to execute it while it is being written
or an attempt to open it for writing while it is being executed will fail.
.IP "\fBEFBIG\fR: File too large"
.II EFBIG
The block-mapping algorithm for a file fails above 1,082,201,088 bytes.
Attempting to write a file larger than this will generate this error.
.IP "\fBENOSPC\fR: No space left on device"
.II ENOSPC
You attempt to write onto a device that is full.
If the attemped write was onto a file system, either the file system's
supply of blocks was exhausted, or its supply of i-nodes was exhausted.
.IP "\fBESPIPE\fR: Tried to seek on a pipe"
.II ESPIPE
It is illegal to invoke the system call
.B lseek()
on a pipe.
.IP "\fBEROFS\fR: Read-only file system"
.II EROFS
You attempted to write onto a file system mounted read-only.
.IP "\fBEMLINK\fR: Too many links"
.II EMLINK
A file can have no more than 32,767 links.
The attempted link operation would exceed this value.
.IP "\fBEPIPE\fR: Broken pipe"
.II EPIPE
You attempted to invoke the system call
.B write()
on a pipe for which there are no readers.
This condition is accompanied by the signal
.B SIGPIPE,
so the error will be seen only if the signal is ignored or caught.
.IP "\fBEDOM\fR: Mathematics library domain error"
.II EDOM
An argument to a mathematical routine falls outside that function's domain.
.IP "\fBERANGE\fR: Mathematics library result too large"
.II ERANGE
The result of a mathematical function is too large to be represented.
.IP "\fBENOMSG\fR: No message of desired type"
You invoked
.B msgrcv()
to read a message of a given type, but none was waiting to be read.
.IP "\fBEIDRM\fR: Identifier removed"
.II EIDRM
.IP "\fBEDEADLK\fR: Deadlock condition"
.II EDEADLK
A process is deadlocked for some reason.
.IP "\fBENOLCK\fR: No record locks available"
.II ENOLCK
The maximum number of record locks has been exceeded.
.IP "\fBENOSTR\fR: Device not a stream"
.II ENOSTR
You attempted to perform a \*(ST operation on a file that is not a stream.
.IP "\fBENODATA\fR: No data available"
.II ENODATA
.IP "\fBETIME\fR: Timer expired"
.II ETIME
.IP "\fBENOSR\fR: Out of \*(ST resources"
.II ENOSR
.IP "\fBENOPKG\fR: Package not installed"
.II ENOPKG
.IP "\fBEPROTO\fR: Protocol error"
.II EPROTO
.IP "\fBEBADMSG\fR: Not a data message"
.II EBADMSG
.IP "\fBENAMETOOLONG\fR: File name too long"
.II ENAMETOOLONG
.IP "\fBEOVERFLOW\fR: Value too large for defined data type"
.II EOVERFLOW
.IP "\fBENOTUNIQ\fR: Name not unique on network"
.II ENOTUNIQ
.IP "\fBEBADFD\fR: File descriptor in bad state"
.II EBADFD
.IP "\fBEREMCHG\fR: Remote address changed"
.II EREMCHG
.IP "\fBELIBACC\fR: Cannot access a needed shared library"
.II ELIBACC
\*(CO does not yet support shared libraries.
.IP "\fBELIBBAD\fR: Accessing a corrupted shared library"
.II ELIBBAD
\*(CO does not yet support shared libraries.
.IP "\fBELIBSCN\fR: \fB.lib\fR section in \fBa.out\fR corrupted"
.II ELIBSCN
.IP "\fBELIBMAX\fR: Maximum number of shared libraries exceeded"
.II ELIBMAX
\*(CO does not yet support shared libraries.
.IP "\fBELIBEXEC\fR: Cannot \fBexec()\fR a shared library directly"
.II ELIBEXEC
\*(CO does not yet support shared libraries.
.IP "\fBEILSEQ\fR: Illegal byte sequence"
.II EILSEQ
.IP "\fBENOSYS\fR: Operation not applicable"
.II ENOSYS
.IP "\fBELOOP\fR: Symbolic links error.
.II ELOOP
Number of symbolic links encountered during
path name traversal exceeds
.BR MAXSYMLINKS .
\*(CO does not yet support symbolic links.
.IP "\fBEUSERS\fR: Too many users"
.II EUSERS
.IP "\fBENOTSOCK\fR: Socket operation on non-socket"
.II ENOTSOCK
.IP "\fBEDESTADDRREQ\fR: Destination address required"
.II EDESTADDRREQ
.IP "\fBEMSGSIZE\fR: Message too long"
.II EMSGSIZE
.IP "\fBEPROTOTYPE\fR: Protocol wrong type for socket"
.II EPROTOTYPE
.IP "\fBENOPROTOOPT\fR: Protocol not available"
.II ENOPROTOOPT
.IP "\fBEPROTONOSUPPORT\fR: Protocol not supported"
.II EPROTONOSUPPORT
.IP "\fBESOCKTNOSUPPORT\fR: Socket type not supported"
.II ESOCKTNOSUPPORT
.IP "\fBEOPNOTSUPP\fR: Operation not supported on transport endpoint"
.II EOPNOTSUPP
.IP "\fBEPFNOSUPPORT\fR: Protocol family not supported"
.II EPFNOSUPPORT
.IP "\fBEAFNOSUPPORT\fR: Address family not supported by protocol family"
.II EAFNOSUPPORT
.IP "\fBEADDRINUSE\fR: Address already in use"
.II EADDRINUSE
.IP "\fBEADDRNOTAVAIL\fR: Cannot assign requested address"
.II EADDRNOTAVAIL
.IP "\fBENETDOWN\fR: Network is down"
.II ENETDOWN
.IP "\fBENETUNREACH\fR: Network is unreachable"
.II ENETUNREACH
.IP "\fBENETRESET\fR: Network dropped connection because of reset"
.II ENETRESET
.IP "\fBECONNABORTED\fR: Software-caused connection abort"
.II ECONNABORTED
.IP "\fBECONNRESET\fR: Connection reset by peer"
.II ECONNRESET
.IP "\fBENOBUFS\fR: No buffer space available"
.II ENOBUFS
.IP "\fBEISCONN\fR: Transport endpoint is already connected"
.II EISCONN
.IP "\fBENOTCONN\fR: Transport endpoint is not connected"
.II ENOTCONN
.IP "\fBESHUTDOWN\fR: Cannot send after transport endpoint shutdown"
.II ESHUTDOWN
.IP "\fBETIMEDOUT\fR: Connection timed out"
.II ETIMEDOUT
.IP "\fBECONNREFUSED\fR: Connection refused"
.II ECONNREFUSED
.IP "\fBEHOSTDOWN\fR: Host is down"
.II EHOSTDOWN
.IP "\fBEHOSTUNREACH\fR: No route to host"
.II EHOSTUNREACH
.IP "\fBEALREADY\fR: Operation already in progress"
.II EALREADY
.IP "\fBEINPROGRESS\fR: Operation now in progress"
.II EINPROGRESS
.IP "\fBESTALE\fR: Stale NFS file handle"
.II ESTALE
\*(CO does not yet support nonproprietary file systems.
.\".IP "\fBEKSPACE\fR: out of kernel space"
.\".II EKSPACE
.\"No more space is available for tables inside the \*(CO system.
.\"Table space is dynamically allocated from a fixed area of memory;
.\"it may be possible to increase the size of the area by reconfiguring the system.
.\".IP "\fBENOLOAD\fR: driver not loaded"
.\".II ENOLOAD
.\"Not used.
.\".IP "\fBEBADFMT\fR: bad exec format"
.\".II EBADFMT
.\"An attempt was made to \fBexec()\fR a file on the wrong type of processor.
.\".IP "\fBEDATTN\fR: device needs attention"
.\".II EDATTN
.\"The device being referenced needs operator attention.
.\"For example, a line printer might need paper.
.\".IP "\fBEDBUSY\fR: device busy"
.\".II EDBUSY
.\"The indicated device is busy.
.\"For
.\".BR load() ,
.\"this implies that the given major device number is already in use.
.SH "See Also"
.Xr "errno," errno
.Xr "header files," header_fi
.Xr "perror()," perror
.Xr "signal()" signal
.br
\*(AS, \(sc7.1.3
.br
\*(PX Standard, \(sc2.4
