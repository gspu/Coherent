.TH core "" "" "System Administration"
.PC "Format of a core-dump file"
.B "#include <sys/core.h>"
.PP
.II "core dump file format"
.II "file format^core dump"
When a process terminates abnormally because a it encounters
an unrecoverable error or receives an asynchronous signal from another process,
\*(CO tries to write a copy of its image in memory into a file called
.BR core .
You can examine this file with the debugger
.B db
and other tools
.\" if we had any
to try to determine what went wrong.
.PP
.\".B core
.\"contains an image of the aborted process's code and data segments, and
.\"of the system's description segment for it.
The structure
.B ch_info
appears at the head of a
.B core
file.
The header file
.B core.h
defines it as follows:
.DM
	struct  ch_info {
		unsigned short ch_magic;
		unsigned int ch_info_len;
	};
.DE
.PP
Field
.B ch_magic
is always set to the constant
.BR CORE_MAGIC .
This ``magic'' value signifies to \*(CO that this is a core file.
Field
.B ch_info_len
gives a count of information bytes in the
core file, including the
.B ch_info
structure itself.
.PP
If the value of
.B ch_info_len
exceeds the size of the
.B ch_info
structure, this indicates that data follow the
.B ch_info
structure.
These data follow the
.B ch_info
structure, and are in the form of a
.B core_proc
structure.
Header file
.B <sys/core.h>
defines this structure as follows:
.DM
	struct core_proc {
		gregset_t cp_registers;
		int cp_signal_number;
		struct _fpstate cp_floating_point;
		dregset_t cp_debug_registers;
	};
.DE
.PP
This substitutes for a dump of the
.B u
area, whose information is reserved for the kernel alone.
.PP
This is followed by an image of each process segment.
The data for each segment consists of the following:
a header, which is a structure of type
.BR core_seg ;
.B cs_pathlen
bytes of data that give the path name of the file from the segment data
originated; and
.B cs_dumped
bytes of core-image data.
.PP
.B core.h
defines strucutre
.B core_seg
as follows:
.DM
.ta 0.5i 1.0i 4.0i
	struct core_seg {
		size_t cs_pathlen;	/* length of path name */
		off_t cs_dumped;	/* dumped size in bytes */
		caddr_t cs_base;	/* virtual base address */
		off_t cs_size;	/* full size in bytes */
		unsigned long cs_reserved[8];
	};
.DE
.PP
The order of the segments is
the text segment first (if it is present \(em usually it is omitted),
followed by the data segment, and then the stack segment.
The contents of the text segment
can usually be identified from the program being debugged.
The patchable kernel variable
.B DUMP_TEXT
allows the \*(CO kernel to dump text segments as well as data and stack
segments.
.PP
Patchable kernel variable
.B DUMP_LIM
sets the maximum size of a segment within a
.B core
file.
The system uses this limit to keep keep core files from getting out of hand.
.SH "See Also"
.Xr "Administering COHERENT," administe
.Xr "core.h," core.h
.Xr "signal()," signal
.Xr "wait()" wait.s
.SH Diagnostics
\*(CO will not write
.B core
if that file already exists as a non-ordinary file
or if there is more than one link to it.
The 0200 bit in the status returned to the parent process by
.B wait()
indicates a successful dump.
.PP
For a list of signals that automatically trigger a core dump, see the
Lexicon entry for
.BR signal() .
