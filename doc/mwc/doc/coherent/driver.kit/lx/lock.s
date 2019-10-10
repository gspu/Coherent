.TH LOCK() "" "" "DDI/DKI Kernel Routine"
.PC "Acquire a basic lock"
.B "#include <sys/types.h>"
.B "#include <sys/ksynch.h>"
\fBpl_t LOCK(\fIlock\^\fB, \fIlevel\^\fB)\fR
\fBlock_t *\fIlock\^\fB; pl_t \fIlevel\^\fB;\fR
.PP
.B LOCK()
sets the interrupt priority to
.I level
and acquires the lock to which
.I lock
points.
To be portable, a driver must specify a
.I level
high enough to block any interrupt handler that
may attempt to acquire this lock.
If the lock is not available, the caller must wait until it becomes so.
.PP
.B LOCK()
returns the previous interrupt-priority level.
.SH "See Also"
.B
DDI/DKI kernel routines,
LOCK_ALLOC(),
LOCK_DEALLOC(),
TRYLOCK(),
UNLOCK()
.R
.SH Notes
.B LOCK()
has base or interrupt level.
Basic locks are not recursive.
A driver can hold a
driver-defined sleep across a call to this function.
It can also hold a driver-defined basic lock or read/write lock
if it observes the priority restrictions described above.
.PP
A driver
must honor the order of the lock hierarchy when it calls
.BR LOCK() .
If it does not, it can create a deadlock.
When a driver calls
.B LOCK()
from interrupt level, it cannot set
.I level
to a priority below that at which the interrupt handler is running.
