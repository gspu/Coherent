.TH l.out.h "" "" "Header File"
.PC "Format for COHERENT 286 objects"
.B "#include <l.out.h>"
.PP
The header file
.B l.out.h
describes the
.B l.out
object format, which is produced by the compiler, assembler,
and the linker under COHERENT 286.
.PP
The assembler outputs an unlinked object module, which must be bound
with any required libraries
(leaving no unresolved symbols) to produce an executable file, or
.IR "load module" .
A call to one of the
.B exec
routines can then execute the load module directly.
.PP
The link module begins with a header, which gives global
and size information about each segment.
Segments of the indicated size follow the header in a fixed order.
The header file
.B l.out.h
defines the header structure as follows:
.DM
.ta 0.75i 1.75i
struct	ldheader {
	short l_magic;
	short l_flag;
	short l_machine;
	unsigned short l_entry;
	fsize_t l_ssize[NLSEG];
};
.DE
.PP
.B l_magic
is the magic number that identifies a link module;
it always contains
.B L_MAGIC.
.B l_flag
contains flags indicating the type of the object.
.B l_machine
is the processor identifier, as defined in the header file
.BR mtype.h .
.B l_tbase
is the start of the text segment.
.B l_entry
contains the machine address where execution of the module commences.
.B l_ssize
gives the size of each segment.
.SH Files
\fBl.out\fR \(em Default load module name
.br
\fB<l.out.h>\fR \(em Define format of COHERENT 286 objects
.br
\fB<mtype.h>\fR \(em Machine identifiers
.SH "See Also"
.Xr "as," as
.Xr "cc," cc
.Xr "core," core
.Xr "exec," exec
.Xr "ld," ld
.Xr "libc," libc
.Xr "mtype," mtype
.Xr "nm" nm
.SH Notes
\*(CO 386 uses the common object file format (COFF) for its executables.
See the Lexicon entry for
.B "coff.h"
for information on this format.
