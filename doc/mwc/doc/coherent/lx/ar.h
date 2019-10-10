.TH ar.h "" "" "Header File"
.PC "Format for archive files"
.B "#include <ar.h>"
.PP
.II "file format^archive file"
.II "archive file, format"
An
.I archive
is a file that has been built from a number of files.
Archives are maintained by the command
.BR ar .
Usually, an archive is a library of object files used by the linker
.BR ld .
.PP
The header
.B ar.h
describes the format of an archive.
All archives start with a magic number
.BR ARMAG ,
which identifies the file as an archive.
The members of the archive follow the magic number,
each preceded by the structure
.BR ar_hdr :
.DM
#define DIRSIZ  14
#define ARMAG   0177535          /* magic number */
.DE
.DM
struct ar_hdr {
        char    ar_name[DIRSIZ]; /* member name */
        time_t  ar_date;         /* time inserted */
        short   ar_gid;          /* group owner */
        short   ar_uid;          /* user owner */
        short   ar_mode;         /* file mode */
        size_t  ar_size;         /* file size */
};
.DE
.PP
The structure at the head of each member is immediately followed by
.B ar_size
bytes, which are the data of the file.
.PP
To enhance the performance of
.BR ld ,
the command
.B ranlib
provides a random library facility.
.B ranlib
produces archives that contain a special entry named
.B "_\|_.SYMDEF"
at the beginning.
.PP
All integer members of the structure (everything but
.BR ar_name )
are in canonical form to ease portability.
See
.B canon.h
for more information.
.SH "See Also"
.Xr ar, ar
.Xr canon.h, canon.h
.Xr "header files," header_fi
.Xr ld, ld
.Xr ranlib ranlib
