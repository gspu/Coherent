.TH UNLOCK() "" "" "DDI/DKI Kernel Routine"
.PC "Release a basic lock"
.B "#include <sys/ksynch.h>"
.B "#include <sys/types.h>"
\fBvoid UNLOCK(\fIlock\^\fB, \fIpriority\^\fB)\fR
\fBlock_t *\fIlock\^\fB; pl_t \fIpriority\^\fB;\fR
.PP
.B UNLOCK()
releases
.IR lock ,
which must be a basic lock.
.I priority
gives the level of interrupt priority that the calling process wants
once the lock is released.
Normally, this is the value that had been returned by the call
that set the lock, that is,
the level of interrupt priority the calling process had had before
it set the lock.
However, the calling function can set a different level of interrupt priority
should it need to.
See entry for
.B LOCK_ALLOC()
in this manual for a list of all legal values for this argument.
.SH "See Also"
.B
DDI/DKI kernel routines,
LOCK(),
LOCK_ALLOC(),
LOCK_DEALLOC(),
TRYLOCK()
.R
.SH Notes
.B UNLOCK()
has base or interrupt level.
It does not sleep.
.PP
A driver can hold a
driver-defined basic lock, read/write lock, or sleep lock
across a call to this function.
