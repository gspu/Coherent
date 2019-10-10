.TH ndbm.h "" "" "Header File"
.PC "Header file for NDBM routines"
.B "#include <ndbm.h>"
.PP
Header file
.B <ndbm.h>
declares the functions used to manipulate NDBM data bases:
.LB
\fBdbm_clearerr()\fR	Clear an error condition on an NDBM data base
\fBdbm_close()\fR	Close an NDBM data base
\fBdbm_delete()\fR	Delete records from an NDBM data base
\fBdbm_dirfno()\fR	Return the file descriptor for an NDBM .dir file
\fBdbm_error()\fR	Check a NDBM data base for an error
\fBdbm_fetch()\fR	Fetch a record from an NDBM data base
\fBdbm_firstkey()\fR	Retrieve the first key from an NDBM data base
\fBdbm_nextkey()\fR	Retrieve the next key from an NDBM data base
\fBdbm_open()\fR	Open an NDBM data base
\fBdbm_pagfno()\fR	Return the file descriptor for an NDBM .pag file
\fBdbm_rdonly()\fR	Set an NDBM data base into read-only mode
\fBdbm_store()\fR	Store a record into an NDBM data base
.PP
Routines
.B dbm_error()
and
.B dbm_clearerr()
are macros that, in fact, do nothing.
.PP
This header file also defines two structures that the NDBM routines use.
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
This structure lets you have a key and a data element of unlimited length.
.PP
The other structure,
.BR DBM ,
holds the information that the NDBM routines use to access a NDBM data base:
.DM
	typedef struct {int dummy[10];} DBM;
.DE
.SH "See Also"
.XR "dbm.h," dbm.h
.XR "gdbm.h," gdbm.h
.XR "header files," header_fi
.XR "libgdbm" libgdbm
.SH Notes
For a statement of copyright and permissions on this header file, see the
Lexicon entry for
.BR libgdbm .
