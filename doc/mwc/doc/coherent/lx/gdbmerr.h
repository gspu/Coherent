.TH gdbmerrno.h "" "" "Header File"
.PC "Define error messages used by GDBM routines"
.B "#include <gdbmerrno.h>"
.PP
Header file
.B <gdbmerrno.h>
defines the error codes that the GDBM routines can write into global variable
.BR gdbm_errno ,
as follows:
.IP \fBGDBM_NO_ERROR\fR
All is well.
.IS \fBGDBM_MALLOC_ERROR\fR
The GDBM routines call
.B malloc()
to allocate memory for each record that they retrieve from a data base.
This message indicates that a call to
.B malloc()
failed.
.IS \fBGDBM_BLOCK_SIZE_ERROR\fR
You tried to set an illegal block size when you created a new data base.
.IS \fBGDBM_FILE_OPEN_ERROR\fR
A data-base file could not be opened, for whatever reason.
.IS \fBGDBM_FILE_WRITE_ERROR\fR
A process could not write into a data-base file.
This probably indicates a problem with permissions.
.IS \fBGDBM_FILE_SEEK_ERROR\fR
A GDBM routine could not move a data-base file's seek pointer to a place
where the data base's hash table indicates a given record was stored.
The data base may well be corrupt; check this error seriously.
.IS \fBGDBM_FILE_READ_ERROR\fR
A process could not read a data-base file.
This probably indicates a problem with permissions.
.IS \fBGDBM_BAD_MAGIC_NUMBER\fR
When the GDBM function
.B gdbm_open()
create a new data base, it stamps the file with a ``magic number,'' which
indicates that that file is, in fact, a GDBM data base.
This error indicates that the file you're attempting to read is not
a GDBM a data base.
.IS \fBGDBM_EMPTY_DATABASE\fR
The GDBM data base contains no data.
.IS \fBGDBM_CANT_BE_READER\fR
You failed in an attempt to open a GDBM data base into read mode.
The data base may have already been opened into write mode.
.IS \fBGDBM_CANT_BE_WRITER\fR
You failed in an attempt to open a GDBM data base into write mode.
The data base may have already been opened into write mode by another
process.
.IS \fBGDBM_READER_CANT_DELETE\fR
You opened a GDBM data base into read mode, but then attempted to delete
a record.
This is illegal.
.IS \fBGDBM_READER_CANT_STORE\fR
You opened a GDBM data base into read mode, but then attempted to write
a record into it.
This is illegal.
.IS \fBGDBM_READER_CANT_REORGANIZE\fR
You opened a GDBM data base into read mode, but then attempted to reorganize it.
This is illegal.
.IS \fBGDBM_UNKNOWN_UPDATE\fR
You attempted to update a record within a data base, but the data base
does not contain a record with the given key.
.IS \fBGDBM_ITEM_NOT_FOUND\fR
You attempted to read a record from a data base, but the data base does not
contain a record with the given key.
.IS \fBGDBM_REORGANIZE_FAILED\fR
An attempted reorganization of a file failed.
The data base may be corrupt.
.IS \fBGDBM_CANNOT_REPLACE\fR
You attempted to write a new record into a data base, but the data base
already contains a record with the given key.
.IS \fBGDBM_ILLEGAL_DATA\fR
You attempted to write a record into a data base, but the record
contains illegal data (e.g., the field
.B dptr
is NULL).
.IS \fBGDBM_OPT_ALREADY_SET\fR
You called
.B gdbm_setopt()
to set an option on a data base, but that option is already set.
.IS \fBGDBM_OPT_ILLEGAL\fR
You called
.B gdbm_setopt()
to set an option on a data base, but the requested option is illegal
or unrecognized.
.PP
Function
.B gdbm_strerror()
translates a GDBM error code into a string that you can display.
.SH "See Also"
.XR "gdbm.h," gdbm.h
.XR "header files," header_fi
.XR "libgdba" libgdba
.SH Notes
For a statement of copyright and permissions on this header file, see the
Lexicon entry for
.BR libgdbm .
