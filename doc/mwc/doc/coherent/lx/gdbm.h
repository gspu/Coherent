.TH gdbm.h "" "" "Header File"
.PC "Header file for GDBM routines"
.B "#include <gdbm.h>"
.PP
Header file
.B <gdbm.h>
declares functions, data types, and global variables used by the GDBM
set of routines:
.sp \n(pDu
.nf
.tc . 0.1i
.ta 2.5i
\fBgdbm_close()\fR	Close a GDBM data base
\fBgdbm_delete()\fR	Delete a record from a GDBM data base
\fBgdbm_exists()\fR	Check whether a GDBM data base contains a given record
\fBgdbm_fetch()\fR	Retrieve a record from a GDBM data base
\fBgdbm_firstkey()\fR	Return the first record from a GDBM data base
\fBgdbm_nextkey()\fR	Return the next record from a GDBM data base
\fBgdbm_open()\fR	Open a GDBM data base
\fBgdbm_reorganize()\fR	Reorganize a GDBM data base
\fBgdbm_setopt()\fR	Set GDBM options
\fBgdbm_store()\fR	Add records to a GDBM data base
\fBgdbm_strerror()\fR	Translate a GDBM error code into text
\fBgdbm_sync()\fR	Flush buffered GDBM data into its data base
.tc
.PP
This header file also defines two structures that the GDBM
routines use.
The first,
.BR datum ,
defines the structure of a data element, either a key or its associated
data set:
.DM
	typedef struct {
		char *dptr;
		int dsize;
	} datum;
.DE
.PP
The other structure,
.BR GDBM_FILE ,
holds the information that the GDBM routines use to access a GDBM data base:
.DM
	typedef struct {int dummy[10];} *GDBM_FILE;
.DE
.PP
Error codes are written into global variable
.BR gdbm_errno ,
and are defined in header file
.BR <gdbmerrno.h> .
.SH "See Also"
.XR "dbm.h," dbm.h
.XR "gdbmerrno.h," gdbmerr.h
.XR "header files," header_fi
.XR "libgdbm," libgdbm
.XR "ndbm.h" ndbm.h
.SH Notes
For a statement of copyright and permissions on this header file, see the
Lexicon entry for
.BR libgdbm .
