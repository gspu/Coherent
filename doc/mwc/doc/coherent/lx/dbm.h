.TH dbm.h "" "" "Header File"
.PC "Header file for DBM routines"
.B "#include <dbm.h>"
.PP
Header file
.B <dbm.h>
declares the functions used to manipulate DBM data bases:
.LB
\fBdbmclose()\fR	Close a DBM data base
\fBdbminit()\fR	Open a DBM data base
\fBdelete()\fR	Delete a record from a DBM data base
\fBfetch()\fR	Fetch a record from a DBM data base
\fBfirstkey()\fR	Retrieve the first record from a DBM data base
\fBnextkey()\fR	Retrieve the next record from a DBM data base
\fBstore()\fR	Write a record into a DBM data base
.PP
It also defines the structure
.BR datum ,
which holds a data element, either a key or its associated data set:
.DM
	typedef struct {
		char *dptr;
		int dsize;
	} datum;
.DE
.SH "See Also"
.Xr "gdbm.h," gdbm.h
.Xr "header files," header_fi
.Xr "libgdbm," libgdbm
.Xr "ndbm.h" ndbm.h
.SH Notes
Please note that function
.B dbmclose()
is non-standard.
A program that uses it cannot be recompiled on an orthodox \*(UN system.
.PP
For a statement of copyright and permissions on this header file, see the
Lexicon entry for
.BR libgdbm .
